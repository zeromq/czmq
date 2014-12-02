/*  =========================================================================
    zloop - event-driven reactor

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    The zloop class provides an event-driven reactor pattern. The reactor
    handles zmq_pollitem_t items (pollers or writers, sockets or fds), and
    once-off or repeated timers. Its resolution is 1 msec. It uses a tickless
    timer to reduce CPU interrupts in inactive processes.
@discuss
@end
*/

#include "../include/czmq.h"

typedef struct _s_reader_t s_reader_t;
typedef struct _s_poller_t s_poller_t;
typedef struct _s_timer_t s_timer_t;
typedef struct _s_ticket_t s_ticket_t;

//  Structure of our class

struct _zloop_t {
    zlistx_t *readers;          //  List of socket readers
    zlistx_t *pollers;          //  List of poll items
    zlistx_t *timers;           //  List of timers
    zlistx_t *tickets;          //  List of tickets
    int last_timer_id;          //  Most recent timer id
    size_t max_timers;          //  Limit on number of timers
    size_t ticket_delay;        //  Ticket delay value
    size_t poll_size;           //  Size of poll set
    zmq_pollitem_t *pollset;    //  zmq_poll set
    s_reader_t *readact;        //  Readers for this poll set
    s_poller_t *pollact;        //  Pollers for this poll set
    bool need_rebuild;          //  True if pollset needs rebuilding
    bool verbose;               //  True if verbose tracing wanted
    bool terminated;            //  True when stopped running
    zlistx_t *zombies;          //  List of timers to kill
};

//  Reactor elements are held as structures of their own

struct _s_reader_t {
    void *list_handle;          //  Handle into list
    zsock_t *sock;              //  Socket to read from
    zloop_reader_fn *handler;   //  Function to execute
    void *arg;                  //  Application argument to poll item
    int errors;                 //  If too many errors, kill reader
    bool tolerant;              //  Unless configured as tolerant
};

struct _s_poller_t {
    void *list_handle;          //  Handle into list
    zmq_pollitem_t item;        //  ZeroMQ socket or file descriptor
    zloop_fn *handler;          //  Function to execute
    void *arg;                  //  Application argument to poll item
    int errors;                 //  If too many errors, kill poller
    bool tolerant;              //  Unless configured as tolerant
};

struct _s_timer_t {
    void *list_handle;          //  Handle into list
    int timer_id;               //  Unique timer id, used to cancel timer
    zloop_timer_fn *handler;    //  Function to execute
    size_t delay;               //  Delay (ms) between executing
    size_t times;               //  Number of times to repeat, 0 for forever
    void *arg;                  //  Application argument to timer
    int64_t when;               //  Clock time when alarm goes off
};

//  As we pass void * to/from the caller for working with tickets, we
//  check validity using an object tag. This value is unique in CZMQ.
#define TICKET_TAG              0x0007cafe

struct _s_ticket_t {
    uint32_t tag;               //  Object tag for runtime detection
    void *list_handle;          //  Handle into list
    size_t delay;               //  Delay (ms) before executing
    int64_t when;               //  Clock time to invoke the ticket
    zloop_timer_fn *handler;    //  Function to execute (use timer fn)
    void *arg;                  //  Application argument to function
};

static int
s_next_timer_id (zloop_t *self)
{
    return ++self->last_timer_id;
}

static s_reader_t *
s_reader_new (zsock_t *sock, zloop_reader_fn handler, void *arg)
{
    s_reader_t *self = (s_reader_t *) zmalloc (sizeof (s_reader_t));
    if (self) {
        self->sock = sock;
        self->handler = handler;
        self->arg = arg;
        self->tolerant = false;     //  By default, errors are bad
    }
    return self;
}

static void
s_reader_destroy (s_reader_t **self_p)
{
    assert (self_p);
    s_reader_t *self = *self_p;
    if (self) {
        free (self);
        *self_p = NULL;
    }
}

static s_poller_t *
s_poller_new (zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    s_poller_t *self = (s_poller_t *) zmalloc (sizeof (s_poller_t));
    if (self) {
        self->item = *item;
        self->handler = handler;
        self->arg = arg;
        self->tolerant = false;     //  By default, errors are bad
    }
    return self;
}

static void
s_poller_destroy (s_poller_t **self_p)
{
    assert (self_p);
    s_poller_t *self = *self_p;
    if (self) {
        free (self);
        *self_p = NULL;
    }
}


static s_timer_t *
s_timer_new (int timer_id, size_t delay, size_t times, zloop_timer_fn handler, void *arg)
{
    s_timer_t *self = (s_timer_t *) zmalloc (sizeof (s_timer_t));
    if (self) {
        self->timer_id = timer_id;
        self->delay = delay;
        self->times = times;
        self->when = zclock_mono () + delay;
        self->handler = handler;
        self->arg = arg;
    }
    return self;
}

static void
s_timer_destroy (s_timer_t **self_p)
{
    assert (self_p);
    s_timer_t *self = *self_p;
    if (self) {
        free (self);
        *self_p = NULL;
    }
}

static int
s_timer_comparator (s_timer_t *timer1, s_timer_t *timer2)
{
    if (timer1->when > timer2->when)
        return 1;
    else
    if (timer1->when < timer2->when)
        return -1;
    else
        return 0;
}

static s_ticket_t *
s_ticket_new (size_t delay, zloop_timer_fn handler, void *arg)
{
    s_ticket_t *self = (s_ticket_t *) zmalloc (sizeof (s_ticket_t));
    if (self) {
        self->tag = TICKET_TAG;
        self->delay = delay;
        self->when = zclock_mono () + delay;
        self->handler = handler;
        self->arg = arg;
    }
    return self;
}

static void
s_ticket_destroy (s_ticket_t **self_p)
{
    assert (self_p);
    s_ticket_t *self = *self_p;
    if (self) {
        self->tag = 0xDeadBeef;
        free (self);
        *self_p = NULL;
    }
}

static int
s_ticket_comparator (s_ticket_t *ticket1, s_ticket_t *ticket2)
{
    if (ticket1->when > ticket2->when)
        return 1;
    else
    if (ticket1->when < ticket2->when)
        return -1;
    else
        return 0;
}

//  Remove timer with specified id, if it exists

static void
s_timer_remove (zloop_t *self, int timer_id)
{
    s_timer_t *timer = (s_timer_t *) zlistx_first (self->timers);
    while (timer) {
        if (timer->timer_id == timer_id) {
            zlistx_delete (self->timers, timer->list_handle);
            break;
        }
        timer = (s_timer_t *) zlistx_next (self->timers);
    }
}


//  We hold an array of pollers that matches the pollset, so we can
//  register/cancel pollers orthogonally to executing the pollset
//  activity on pollers. Returns 0 on success, -1 on failure.

static int
s_rebuild_pollset (zloop_t *self)
{
    free (self->pollset);
    free (self->readact);
    free (self->pollact);
    self->pollset = NULL;
    self->readact = NULL;
    self->pollact = NULL;

    self->poll_size = zlistx_size (self->readers) + zlistx_size (self->pollers);
    self->pollset = (zmq_pollitem_t *) zmalloc (self->poll_size * sizeof (zmq_pollitem_t));
    if (!self->pollset)
        return -1;

    self->readact = (s_reader_t *) zmalloc (self->poll_size * sizeof (s_reader_t));
    if (!self->readact)
        return -1;

    self->pollact = (s_poller_t *) zmalloc (self->poll_size * sizeof (s_poller_t));
    if (!self->pollact)
        return -1;

    s_reader_t *reader = (s_reader_t *) zlistx_first (self->readers);
    uint item_nbr = 0;
    while (reader) {
        zmq_pollitem_t poll_item = { zsock_resolve (reader->sock), 0, ZMQ_POLLIN };
        self->pollset [item_nbr] = poll_item;
        self->readact [item_nbr] = *reader;
        item_nbr++;
        reader = (s_reader_t *) zlistx_next (self->readers);
    }
    s_poller_t *poller = (s_poller_t *) zlistx_first (self->pollers);
    while (poller) {
        self->pollset [item_nbr] = poller->item;
        self->pollact [item_nbr] = *poller;
        item_nbr++;
        poller = (s_poller_t *) zlistx_next (self->pollers);
    }
    self->need_rebuild = false;
    return 0;
}

static long
s_tickless (zloop_t *self)
{
    //  Calculate tickless timer, up to 1 hour
    int64_t tickless = zclock_mono () + 1000 * 3600;
    
    //  Scan timers, which are not sorted
    //  TODO: sort timers properly on insertion
    s_timer_t *timer = (s_timer_t *) zlistx_first (self->timers);
    while (timer) {
        //  Find earliest timer
        if (tickless > timer->when)
            tickless = timer->when;
        timer = (s_timer_t *) zlistx_next (self->timers);
    }
    //  Tickets are sorted, so check first ticket
    s_ticket_t *ticket = (s_ticket_t *) zlistx_first (self->tickets);
    if (ticket && tickless > ticket->when)
        tickless = ticket->when;

    long timeout = (long) (tickless - zclock_mono ());
    if (timeout < 0)
        timeout = 0;
    if (self->verbose)
        zsys_debug ("zloop polling for %d msec", (int) timeout);
    
    return timeout * ZMQ_POLL_MSEC;
}


//  --------------------------------------------------------------------------
//  Constructor

zloop_t *
zloop_new (void)
{
    zloop_t
    *self;

    self = (zloop_t *) zmalloc (sizeof (zloop_t));
    if (!self)
        return NULL;

    self->readers = zlistx_new ();
    if (self->readers)
        self->pollers = zlistx_new ();
    if (self->pollers)
        self->timers = zlistx_new ();
    if (self->timers)
        self->zombies = zlistx_new ();
    if (self->zombies)
        self->tickets = zlistx_new ();
    if (self->tickets) {
        self->last_timer_id = 0;
        zlistx_set_destructor (self->readers, (czmq_destructor *) s_reader_destroy);
        zlistx_set_destructor (self->pollers, (czmq_destructor *) s_poller_destroy);
        zlistx_set_destructor (self->timers, (czmq_destructor *) s_timer_destroy);
        zlistx_set_comparator (self->timers, (czmq_comparator *) s_timer_comparator);
        zlistx_set_destructor (self->tickets, (czmq_destructor *) s_ticket_destroy);
        zlistx_set_comparator (self->tickets, (czmq_comparator *) s_ticket_comparator);
    }
    else
        zloop_destroy (&self);
    return self;
}


//  --------------------------------------------------------------------------
//  Destructor

void
zloop_destroy (zloop_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zloop_t *self = *self_p;

        //  If we never started the loop, yet manipulated timers, we'll have
        //  a zombie list
        while (zlistx_first (self->zombies)) {
            //  Get timer_id back from pointer
            int timer_id = (byte *) zlistx_detach (self->zombies, NULL) - (byte *) NULL;
            s_timer_remove (self, timer_id);
        }
        zlistx_destroy (&self->zombies);
        zlistx_destroy (&self->readers);
        zlistx_destroy (&self->pollers);
        zlistx_destroy (&self->timers);
        zlistx_destroy (&self->tickets);
        free (self->pollset);
        free (self->readact);
        free (self->pollact);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Register socket reader with the reactor. When the reader has messages,
//  the reactor will call the handler, passing the arg. Returns 0 if OK, -1
//  if there was an error. If you register the same socket more than once,
//  each instance will invoke its corresponding handler.

int
zloop_reader (zloop_t *self, zsock_t *sock, zloop_reader_fn handler, void *arg)
{
    assert (self);
    assert (sock);

    s_reader_t *reader = s_reader_new (sock, handler, arg);
    if (reader) {
        reader->list_handle = zlistx_add_end (self->readers, reader);
        if (!reader->list_handle) {
            s_reader_destroy (&reader);
            return -1;
        }
        self->need_rebuild = true;
        if (self->verbose)
            zsys_debug ("zloop: register %s reader", zsock_type_str (sock));
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Cancel a socket reader from the reactor. If multiple readers exist for
//  same socket, cancels ALL of them.

void
zloop_reader_end (zloop_t *self, zsock_t *sock)
{
    assert (self);
    assert (sock);

    s_reader_t *reader = (s_reader_t *) zlistx_first (self->readers);
    while (reader) {
        if (reader->sock == sock) {
            zlistx_delete (self->readers, reader->list_handle);
            self->need_rebuild = true;
        }
        reader = (s_reader_t *) zlistx_next (self->readers);
    }
    if (self->verbose)
        zsys_debug ("zloop: cancel %s reader", zsock_type_str (sock));
}


//  --------------------------------------------------------------------------
//  Configure a registered reader to ignore errors. If you do not set this,
//  then reader that have errors are removed from the reactor silently.

void
zloop_reader_set_tolerant (zloop_t *self, zsock_t *sock)
{
    assert (self);
    assert (sock);

    s_reader_t *reader = (s_reader_t *) zlistx_first (self->readers);
    while (reader) {
        if (reader->sock == sock)
            reader->tolerant = true;
        reader = (s_reader_t *) zlistx_next (self->readers);
    }
}


//  --------------------------------------------------------------------------
//  Register low-level libzmq pollitem with the reactor. When the pollitem
//  is ready, will call the handler, passing the arg. Returns 0 if OK, -1
//  if there was an error. If you register the pollitem more than once, each
//  instance will invoke its corresponding handler. A pollitem with
//  socket=NULL and fd=0 means 'poll on FD zero'.

int
zloop_poller (zloop_t *self, zmq_pollitem_t *item, zloop_fn handler, void *arg)
{
    assert (self);

    if (item->socket
    &&  streq (zsys_sockname (zsock_type (item->socket)), "UNKNOWN"))
        return -1;

    s_poller_t *poller = s_poller_new (item, handler, arg);
    if (poller) {
        poller->list_handle = zlistx_add_end (self->pollers, poller);
        if (!poller->list_handle) {
            s_poller_destroy (&poller);
            return -1;
        }
        self->need_rebuild = true;
        if (self->verbose)
            zsys_debug ("zloop: register %s poller (%p, %d)",
                        item->socket ? zsys_sockname (zsock_type (item->socket)) : "FD",
                        item->socket, item->fd);
        return 0;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Cancel a pollitem from the reactor, specified by socket or FD. If both
//  are specified, uses only socket. If multiple poll items exist for same
//  socket/FD, cancels ALL of them.

void
zloop_poller_end (zloop_t *self, zmq_pollitem_t *item)
{
    assert (self);

    s_poller_t *poller = (s_poller_t *) zlistx_first (self->pollers);
    while (poller) {
        bool match = false;
        if (item->socket) {
            if (item->socket == poller->item.socket)
                match = true;
        }
        else {
            if (item->fd == poller->item.fd)
                match = true;
        }
        if (match) {
            zlistx_delete (self->pollers, poller->list_handle);
            //  Force rebuild to avoid reading from freed poller
            self->need_rebuild = true;
        }
        poller = (s_poller_t *) zlistx_next (self->pollers);
    }
    if (self->verbose)
        zsys_debug ("zloop: cancel %s poller (%p, %d)",
                    item->socket ? zsys_sockname (zsock_type (item->socket)) : "FD",
                    item->socket, item->fd);
}


//  --------------------------------------------------------------------------
//  Configure a registered poller to ignore errors. If you do not set this,
//  then poller that have errors are removed from the reactor silently.

void
zloop_poller_set_tolerant (zloop_t *self, zmq_pollitem_t *item)
{
    assert (self);

    //  Find matching poller(s) and mark as tolerant
    s_poller_t *poller = (s_poller_t *) zlistx_first (self->pollers);
    while (poller) {
        bool match = false;
        if (item->socket) {
            if (item->socket == poller->item.socket)
                match = true;
        }
        else {
            if (item->fd == poller->item.fd)
                match = true;
        }
        if (match)
            poller->tolerant = true;
        
        poller = (s_poller_t *) zlistx_next (self->pollers);
    }
}


//  --------------------------------------------------------------------------
//  Register a timer that expires after some delay and repeats some number of
//  times. At each expiry, will call the handler, passing the arg. To run a
//  timer forever, use 0 times. Returns a timer_id that is used to cancel the
//  timer in the future. Returns -1 if there was an error.
//
//  TODO: if we returned a void *handle to the timer then we could work
//  directly into the list rather than scanning... how to do this without
//  breaking the API or making the code horribly complex?

int
zloop_timer (zloop_t *self, size_t delay, size_t times, zloop_timer_fn handler, void *arg)
{
    assert (self);
    //  Catch excessive use of timers
    if (self->max_timers && zlistx_size (self->timers) == self->max_timers) {
        zsys_error ("zloop: timer limit reached (max=%d)", self->max_timers);
        return -1;
    }
    int timer_id = s_next_timer_id (self);
    s_timer_t *timer = s_timer_new (timer_id, delay, times, handler, arg);
    if (timer) {
        timer->list_handle = zlistx_add_end (self->timers, timer);
        if (!timer->list_handle) {
            s_timer_destroy (&timer);
            return -1;
        }
        if (self->verbose)
            zsys_debug ("zloop: register timer id=%d delay=%d times=%d",
                        timer_id, (int) delay, (int) times);
        return timer_id;
    }
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Cancel a timer by timer id (as returned by zloop_timer()).
//  Returns 0 on success.

int
zloop_timer_end (zloop_t *self, int timer_id)
{
    assert (self);

    if (self->terminated)
        s_timer_remove (self, timer_id);
    else
    //  We cannot touch self->timers because we may be executing that
    //  from inside the poll loop. So, we hold the arg on the zombie
    //  list, and process that list when we're done executing timers.
    //  This hack lets us store an integer timer ID as a pointer
    if (zlistx_add_end (self->zombies, (byte *) NULL + timer_id) == NULL)
        return -1;

    if (self->verbose)
        zsys_debug ("zloop: cancel timer id=%d", timer_id);

    return 0;
}


//  --------------------------------------------------------------------------
//  Register a ticket timer. Ticket timers are very fast in the case where
//  you use a lot of timers (thousands), and frequently remove and add them.
//  The main use case is expiry timers for servers that handle many clients,
//  and which reset the expiry timer for each message received from a client.
//  Whereas normal timers perform poorly as the number of clients grows, the
//  cost of ticket timers is constant, no matter the number of clients. You
//  must set the ticket delay using zloop_set_ticket_delay before creating a
//  ticket. Returns a handle to the timer that you should use in
//  zloop_ticket_reset and zloop_ticket_delete.

void *
zloop_ticket (zloop_t *self, zloop_timer_fn handler, void *arg)
{
    assert (self);
    assert (self->ticket_delay > 0);
    s_ticket_t *ticket = s_ticket_new (self->ticket_delay, handler, arg);
    if (ticket) {
        ticket->list_handle = zlistx_add_end (self->tickets, ticket);
        if (!ticket->list_handle) 
            s_ticket_destroy (&ticket);
    }
    return ticket;
}


//  --------------------------------------------------------------------------
//  Reset a ticket timer, which moves it to the end of the ticket list and
//  resets its execution time. This is a very fast operation.

void
zloop_ticket_reset (zloop_t *self, void *handle)
{
    s_ticket_t *ticket = (s_ticket_t *) handle;
    assert (ticket->tag == TICKET_TAG);
    ticket->when = zclock_mono () + ticket->delay;
    zlistx_move_end (self->tickets, ticket->list_handle);
}


//  --------------------------------------------------------------------------
//  Delete a ticket timer.

void
zloop_ticket_delete (zloop_t *self, void *handle)
{
    s_ticket_t *ticket = (s_ticket_t *) handle;
    assert (ticket->tag == TICKET_TAG);
    zlistx_delete (self->tickets, ticket->list_handle);
}


//  --------------------------------------------------------------------------
//  Set the ticket delay, which applies to all tickets. If you lower the
//  delay and there are already tickets created, the results are undefined.

void
zloop_set_ticket_delay (zloop_t *self, size_t ticket_delay)
{
    assert (self);
    self->ticket_delay = ticket_delay;
}


//  --------------------------------------------------------------------------
//  Set verbose tracing of reactor on/off

void
zloop_set_verbose (zloop_t *self, bool verbose)
{
    assert (self);
    self->verbose = verbose;
}


//  --------------------------------------------------------------------------
//  Set hard limit on number of timers allowed. Setting more than a small
//  number of timers (10-100) can have a dramatic impact on the performance
//  of the reactor. For high-volume cases, use ticket timers. If the hard
//  limit is reached, the reactor stops creating new timers and logs an
//  error.

void
zloop_set_max_timers (zloop_t *self, size_t max_timers)
{
    assert (self);
    self->max_timers = max_timers;
}


//  --------------------------------------------------------------------------
//  Start the reactor. Takes control of the thread and returns when the 0MQ
//  context is terminated or the process is interrupted, or any event handler
//  returns -1. Event handlers may register new sockets and timers, and
//  cancel sockets. Returns 0 if interrupted, -1 if canceled by a handler,
//  positive on internal error

int
zloop_start (zloop_t *self)
{
    assert (self);
    int rc = 0;

    //  Main reactor loop
    while (!zsys_interrupted) {
        if (self->need_rebuild) {
            //  If s_rebuild_pollset() fails, break out of the loop and
            //  return its error
            rc = s_rebuild_pollset (self);
            if (rc)
                break;
        }
        rc = zmq_poll (self->pollset, (int) self->poll_size, s_tickless (self));
        if (rc == -1 || zsys_interrupted) {
            if (self->verbose)
                zsys_debug ("zloop: interrupted");
            rc = 0;
            break;              //  Context has been shut down
        }
        
        //  Handle any timers that have now expired
        int64_t time_now = zclock_mono ();
        s_timer_t *timer = (s_timer_t *) zlistx_first (self->timers);
        while (timer) {
            if (time_now >= timer->when) {
                if (self->verbose)
                    zsys_debug ("zloop: call timer handler id=%d", timer->timer_id);
                rc = timer->handler (self, timer->timer_id, timer->arg);
                if (rc == -1)
                    break;      //  Timer handler signaled break
                if (timer->times && --timer->times == 0)
                    zlistx_delete (self->timers, timer->list_handle);
                else
                    timer->when += timer->delay;
            }
            timer = (s_timer_t *) zlistx_next (self->timers);
        }
        
        //  Handle any tickets that have now expired
        s_ticket_t *ticket = (s_ticket_t *) zlistx_first (self->tickets);
        while (ticket && time_now >= ticket->when) {
            if (self->verbose)
                zsys_debug ("zloop: call ticket handler");
            rc = ticket->handler (self, 0, ticket->arg);
            if (rc == -1)
                break;      //  Timer handler signaled break
            zlistx_delete (self->tickets, ticket->list_handle);
            ticket = (s_ticket_t *) zlistx_next (self->tickets);
        }
        
        //  Handle any readers and pollers that are ready
        size_t item_nbr;
        for (item_nbr = 0; item_nbr < self->poll_size && rc >= 0; item_nbr++) {
            s_reader_t *reader = &self->readact [item_nbr];
            if (reader->handler) {
                if ((self->pollset [item_nbr].revents & ZMQ_POLLERR)
                && !reader->tolerant) {
                    if (self->verbose)
                        zsys_warning ("zloop: can't read %s socket: %s",
                                      zsock_type_str (reader->sock),
                                      zmq_strerror (zmq_errno ()));
                    //  Give handler one chance to handle error, then kill
                    //  reader because it'll disrupt the reactor otherwise.
                    if (reader->errors++) {
                        zloop_reader_end (self, reader->sock);
                        self->pollset [item_nbr].revents = 0;
                    }
                }
                else
                    reader->errors = 0;     //  A non-error happened

                if (self->pollset [item_nbr].revents) {
                    if (self->verbose)
                        zsys_debug ("zloop: call %s socket handler",
                                    zsock_type_str (reader->sock));
                    rc = reader->handler (self, reader->sock, reader->arg);
                    if (rc == -1 || self->need_rebuild)
                        break;
                }
            }
            else {
                s_poller_t *poller = &self->pollact [item_nbr];
                assert (self->pollset [item_nbr].socket == poller->item.socket);

                if ((self->pollset [item_nbr].revents & ZMQ_POLLERR)
                && !poller->tolerant) {
                    if (self->verbose)
                        zsys_warning ("zloop: can't poll %s socket (%p, %d): %s",
                                      poller->item.socket ?
                                      zsys_sockname (zsock_type (poller->item.socket)) : "FD",
                                      poller->item.socket, poller->item.fd,
                                      zmq_strerror (zmq_errno ()));
                    //  Give handler one chance to handle error, then kill
                    //  poller because it'll disrupt the reactor otherwise.
                    if (poller->errors++) {
                        zloop_poller_end (self, &poller->item);
                        self->pollset [item_nbr].revents = 0;
                    }
                }
                else
                    poller->errors = 0;     //  A non-error happened

                if (self->pollset [item_nbr].revents) {
                    if (self->verbose)
                        zsys_debug ("zloop: call %s socket handler (%p, %d)",
                                    poller->item.socket ?
                                    zsys_sockname (zsock_type (poller->item.socket)) : "FD",
                                    poller->item.socket, poller->item.fd);
                    rc = poller->handler (self, &self->pollset [item_nbr], poller->arg);
                    if (rc == -1 || self->need_rebuild)
                        break;
                }
            }
        }
        //  Now handle any timer zombies
        //  This is going to be slow if we have many timers; we might use
        //  a faster lookup on the timer list.
        while (zlistx_first (self->zombies)) {
            //  Get timer_id back from pointer
            int timer_id = (byte *) zlistx_detach (self->zombies, NULL) - (byte *) NULL;
            s_timer_remove (self, timer_id);
        }
        if (rc == -1)
            break;
    }
    self->terminated = true;
    return rc;
}


//  --------------------------------------------------------------------------
//  Selftest

static int
s_cancel_timer_event (zloop_t *loop, int timer_id, void *arg)
{
    //  We are handling timer 2, and will cancel timer 1
    int cancel_timer_id = *((int *) arg);
    return zloop_timer_end (loop, cancel_timer_id);
}

static int
s_timer_event (zloop_t *loop, int timer_id, void *output)
{
    zstr_send (output, "PING");
    return 0;
}

static int
s_socket_event (zloop_t *loop, zsock_t *handle, void *arg)
{
    //  Just end the reactor
    return -1;
}

void
zloop_test (bool verbose)
{
    printf (" * zloop: ");
    int rc = 0;
    //  @selftest
    //  Create two PAIR sockets and connect over inproc
    zsock_t *output = zsock_new (ZMQ_PAIR);
    assert (output);
    zsock_bind (output, "inproc://zloop.test");

    zsock_t *input = zsock_new (ZMQ_PAIR);
    assert (input);
    zsock_connect (input, "inproc://zloop.test");

    zloop_t *loop = zloop_new ();
    assert (loop);
    zloop_set_verbose (loop, verbose);

    //  Create a timer that will be cancelled
    int timer_id = zloop_timer (loop, 1000, 1, s_timer_event, NULL);
    zloop_timer (loop, 5, 1, s_cancel_timer_event, &timer_id);

    //  After 20 msecs, send a ping message to output3
    zloop_timer (loop, 20, 1, s_timer_event, output);

    //  Set up some tickets that will never expire
    zloop_set_ticket_delay (loop, 10000);
    void *ticket1 = zloop_ticket (loop, s_timer_event, NULL);
    void *ticket2 = zloop_ticket (loop, s_timer_event, NULL);
    void *ticket3 = zloop_ticket (loop, s_timer_event, NULL);

    //  When we get the ping message, end the reactor
    rc = zloop_reader (loop, input, s_socket_event, NULL);
    assert (rc == 0);
    zloop_reader_set_tolerant (loop, input);
    zloop_start (loop);

    zloop_ticket_delete (loop, ticket1);
    zloop_ticket_delete (loop, ticket2);
    zloop_ticket_delete (loop, ticket3);

    zloop_destroy (&loop);
    assert (loop == NULL);

    zsock_destroy (&input);
    zsock_destroy (&output);
    //  @end
    printf ("OK\n");
}
