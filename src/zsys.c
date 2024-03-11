/*  =========================================================================
    zsys - system-level methods

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
    The zsys class provides a portable wrapper for system calls. We collect
    them here to reduce the number of weird #ifdefs in other classes. As far
    as possible, the bulk of CZMQ classes are fully portable.
@discuss
@end
*/

#include "czmq_classes.h"

// For getcwd() variants
#if (defined (WIN32))
# include <direct.h>
#else
# include <unistd.h>
#endif

//  --------------------------------------------------------------------------
//  Signal handling

//  These are global variables accessible to CZMQ application code
volatile int zsys_interrupted = 0;  //  Current name
volatile int zctx_interrupted = 0;  //  Deprecated name

static void s_signal_handler (int signal_value);

//  We use these variables for signal handling
static bool s_first_time = true;
static bool handle_signals = true;
#if defined (__UNIX__)
static struct sigaction sigint_default;
static struct sigaction sigterm_default;

#elif defined (__WINDOWS__)
static zsys_handler_fn *installed_handler_fn;
static BOOL WINAPI
s_handler_fn_shim (DWORD ctrltype)
{
    //  Return TRUE for events that we handle
    if (ctrltype == CTRL_C_EVENT && installed_handler_fn != NULL) {
        installed_handler_fn (ctrltype);
        return TRUE;
    }
    if (ctrltype == CTRL_CLOSE_EVENT && installed_handler_fn != NULL) {
        installed_handler_fn (ctrltype);
        return TRUE;
    }
    return FALSE;
}
#endif

//  --------------------------------------------------------------------------
//  Global context handling

//  ZeroMQ context for this process
static void *s_process_ctx = NULL;
static bool s_initialized = false;
static bool s_shutting_down = false;
#if defined (__UNIX__)
static void zsys_cleanup (void);
#endif

#ifndef S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC
// This is a private tunable that is likely to be replaced or tweaked later
// per comment block at s_zsys_file_stable() implementation, to reflect
// the best stat data granularity available on host OS *and* used by czmq.
#define S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC 5000
#endif

//  Default globals for new sockets and other joys; these can all be set
//  from the environment, or via the zsys_set_xxx API.
static size_t s_io_threads = 1;     //  ZSYS_IO_THREADS=1
static int s_thread_sched_policy = -1; //  ZSYS_THREAD_SCHED_POLICY=-1
static int s_thread_priority = -1;  //  ZSYS_THREAD_PRIORITY=-1
static int s_thread_name_prefix = -1;  //  ZSYS_THREAD_NAME_PREFIX=-1
static char s_thread_name_prefix_str[16] = "0";  //  ZSYS_THREAD_NAME_PREFIX_STR="0"
static size_t s_max_sockets = 1024; //  ZSYS_MAX_SOCKETS=1024
static int s_max_msgsz = INT_MAX;   //  ZSYS_MAX_MSGSZ=INT_MAX
static int64_t s_file_stable_age_msec = S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC;
                                    //  ZSYS_FILE_STABLE_AGE_MSEC=5000
static size_t s_linger = 0;         //  ZSYS_LINGER=0
static size_t s_sndhwm = 1000;      //  ZSYS_SNDHWM=1000
static size_t s_rcvhwm = 1000;      //  ZSYS_RCVHWM=1000
static size_t s_pipehwm = 1000;     //  ZSYS_PIPEHWM=1000
static int s_ipv6 = 0;              //  ZSYS_IPV6=0
static char *s_interface = NULL;    //  ZSYS_INTERFACE=
static char *s_ipv6_address = NULL; //  ZSYS_IPV6_ADDRESS=
static char *s_ipv6_mcast_address = NULL; //  ZSYS_IPV6_MCAST_ADDRESS=
static int s_auto_use_fd = 0;       //  ZSYS_AUTO_USE_FD=0
static char *s_logident = NULL;     //  ZSYS_LOGIDENT=
static FILE *s_logstream = NULL;    //  ZSYS_LOGSTREAM=stdout/stderr
static bool s_logsystem = false;    //  ZSYS_LOGSYSTEM=true/false
static zsock_t *s_logsender = NULL;    //  ZSYS_LOGSENDER=
static int s_zero_copy_recv = 1;    // ZSYS_ZERO_COPY_RECV=1
static char *s_ipv4_mcast_address = NULL; //  ZSYS_IPV4_MCAST_ADDRESS=
static unsigned char s_mcast_ttl = 1;     //  ZSYS_MCAST_TTL=1

//  Track number of open sockets so we can zmq_term() safely
static size_t s_open_sockets = 0;

//  We keep a list of open sockets to report leaks to developers
static zlist_t *s_sockref_list = NULL;

//  This defines a single zsock_new() caller instance
typedef struct {
    void *handle;
    int type;
    const char *filename;
    size_t line_nbr;
} s_sockref_t;

//  Mutex macros
#if defined (__UNIX__)
typedef pthread_mutex_t zsys_mutex_t;
#   define ZMUTEX_INIT(m)    pthread_mutex_init (&m, NULL);
#   define ZMUTEX_LOCK(m)    pthread_mutex_lock (&m);
#   define ZMUTEX_UNLOCK(m)  pthread_mutex_unlock (&m);
#   define ZMUTEX_DESTROY(m) pthread_mutex_destroy (&m);
#elif defined (__WINDOWS__)
typedef CRITICAL_SECTION zsys_mutex_t;
#   define ZMUTEX_INIT(m)    InitializeCriticalSection (&m);
#   define ZMUTEX_LOCK(m)    EnterCriticalSection (&m);
#   define ZMUTEX_UNLOCK(m)  LeaveCriticalSection (&m);
#   define ZMUTEX_DESTROY(m) DeleteCriticalSection (&m);
#endif

//  Mutex to guard socket counter
static zsys_mutex_t s_mutex;
#if defined (__UNIX__)
// Mutex to guard the multiple zsys_init() - to make it threadsafe
static zsys_mutex_t s_init_mutex;
#endif
//  Implementation for the zsys_vprintf() which is known from legacy
//  and poses as a stable interface now.
static inline
char *
s_zsys_vprintf_hint (int hint, const char *format, va_list argptr);

//  --------------------------------------------------------------------------
//  Initialize CZMQ zsys layer; this happens automatically when you create
//  a socket or an actor; however this call lets you force initialization
//  earlier, so e.g. logging is properly set-up before you start working.
//  Not threadsafe, so call only from main thread. Safe to call multiple
//  times. Returns global CZMQ context.

#if defined (__UNIX__)
// mutex for pthread_once to run the init function only once in a process
static pthread_once_t init_all_mutex_var = PTHREAD_ONCE_INIT;

// handler to destroy s_mutex
static void zsys_destroy_mutex() {
    ZMUTEX_DESTROY(s_mutex);
}

// handler to initialize mutexes one time in multi threaded env
static void zsys_initialize_mutex() {
    ZMUTEX_INIT (s_mutex);
    ZMUTEX_INIT (s_init_mutex);
    atexit (zsys_destroy_mutex);
}

// handler to detect fork condition and cleanup the stale context inherited from parent process
static void zsys_pthread_at_fork_handler(void) {
    // re-initialize mutexes
    ZMUTEX_INIT (s_init_mutex);
    ZMUTEX_INIT (s_mutex);
    // call cleanup
    zsys_cleanup();
}

// mutex for pthread_once to register the atexit function only once in a process
static pthread_once_t register_atexit_shutdown = PTHREAD_ONCE_INIT;
// handler to register the atexit function one time in multi threaded env
static void zsys_register_atexit_shutdown(void) {
    atexit (zsys_shutdown);
}
#endif

void *
zsys_init (void)
{
#if defined (__UNIX__)
    //To avoid two inits at same time
    pthread_once(&init_all_mutex_var, zsys_initialize_mutex);

    if (s_initialized) {
        assert (s_process_ctx);
        return s_process_ctx;
    }

    ZMUTEX_LOCK (s_init_mutex);
#endif

    // Doing this again here... to ensure that after mutex wait if the thread 2 gets execution, it will
    // will get the context right away
    if (s_initialized) {
        assert (s_process_ctx);
#if defined (__UNIX__)
        // unlock the mutex before returning the context
        ZMUTEX_UNLOCK(s_init_mutex);
#endif
        return s_process_ctx;
    }

    //  Pull process defaults from environment
    if (getenv ("ZSYS_IO_THREADS"))
        s_io_threads = atoi (getenv ("ZSYS_IO_THREADS"));

    if (getenv ("ZSYS_MAX_SOCKETS"))
        s_max_sockets = atoi (getenv ("ZSYS_MAX_SOCKETS"));

    if (getenv ("ZSYS_MAX_MSGSZ"))
        s_max_msgsz = atoi (getenv ("ZSYS_MAX_MSGSZ"));

    if (getenv ("ZSYS_ZERO_COPY_RECV"))
        s_zero_copy_recv = atoi (getenv ("ZSYS_ZERO_COPY_RECV"));

    if (getenv ("ZSYS_FILE_STABLE_AGE_MSEC"))
        s_file_stable_age_msec = atoi (getenv ("ZSYS_FILE_STABLE_AGE_MSEC"));

    if (getenv ("ZSYS_LINGER"))
        s_linger = atoi (getenv ("ZSYS_LINGER"));

    if (getenv ("ZSYS_SNDHWM"))
        s_sndhwm = atoi (getenv ("ZSYS_SNDHWM"));

    if (getenv ("ZSYS_RCVHWM"))
        s_rcvhwm = atoi (getenv ("ZSYS_RCVHWM"));

    if (getenv ("ZSYS_PIPEHWM"))
        s_pipehwm = atoi (getenv ("ZSYS_PIPEHWM"));

    if (getenv ("ZSYS_IPV6"))
        s_ipv6 = atoi (getenv ("ZSYS_IPV6"));

    if (getenv ("ZSYS_LOGSTREAM")) {
        if (streq (getenv ("ZSYS_LOGSTREAM"), "stdout"))
            s_logstream = stdout;
        else
        if (streq (getenv ("ZSYS_LOGSTREAM"), "stderr"))
            s_logstream = stderr;
    }
    else
        s_logstream = stdout;

    if (getenv ("ZSYS_LOGSYSTEM")) {
        if (streq (getenv ("ZSYS_LOGSYSTEM"), "true"))
            s_logsystem = true;
        else
        if (streq (getenv ("ZSYS_LOGSYSTEM"), "false"))
            s_logsystem = false;
    }

    if (getenv ("ZSYS_AUTO_USE_FD"))
        s_auto_use_fd = atoi (getenv ("ZSYS_AUTO_USE_FD"));

    zsys_catch_interrupts ();

#if defined (__WINDOWS__)
    ZMUTEX_INIT (s_mutex);
#endif
    s_sockref_list = zlist_new ();
    if (!s_sockref_list) {
        zsys_shutdown ();
#if defined (__UNIX__)
        ZMUTEX_UNLOCK(s_init_mutex);
#endif
        return NULL;
    }
    srandom ((unsigned) time (NULL));

    assert (!s_process_ctx);
    //  We use zmq_init/zmq_term to keep compatibility back to ZMQ v2
    s_process_ctx = zmq_init ((int) s_io_threads);
#if defined (ZMQ_MAX_SOCKETS)
    zmq_ctx_set (s_process_ctx, ZMQ_MAX_SOCKETS, (int) s_max_sockets);
#endif
    s_initialized = true;

#if defined (__UNIX__)
    pthread_once(&register_atexit_shutdown, zsys_register_atexit_shutdown);
    pthread_atfork(NULL, NULL, &zsys_pthread_at_fork_handler);
    //don't hold the lock because some of the function will call zsys_init again
    ZMUTEX_UNLOCK(s_init_mutex);
#endif

    //  The following functions call zsys_init(), so they MUST be called after
    //  s_initialized is set in order to avoid an infinite recursion
    if (getenv ("ZSYS_INTERFACE"))
        zsys_set_interface (getenv ("ZSYS_INTERFACE"));

    if (getenv ("ZSYS_IPV6_ADDRESS"))
        zsys_set_ipv6_address (getenv ("ZSYS_IPV6_ADDRESS"));

    if (getenv ("ZSYS_IPV6_MCAST_ADDRESS"))
        zsys_set_ipv6_mcast_address (getenv ("ZSYS_IPV6_MCAST_ADDRESS"));
    else
        zsys_set_ipv6_mcast_address ("ff02:0:0:0:0:0:0:1");

    if (getenv ("ZSYS_IPV4_MCAST_ADDRESS"))
        zsys_set_ipv4_mcast_address (getenv ("ZSYS_IPV4_MCAST_ADDRESS"));
    else
        zsys_set_ipv4_mcast_address (NULL);


    if (getenv ("ZSYS_LOGIDENT"))
        zsys_set_logident (getenv ("ZSYS_LOGIDENT"));

    if (getenv ("ZSYS_LOGSENDER"))
        zsys_set_logsender (getenv ("ZSYS_LOGSENDER"));

    zsys_set_max_msgsz (s_max_msgsz);

#if defined ZMQ_ZERO_COPY_RECV
    zmq_ctx_set (s_process_ctx, ZMQ_ZERO_COPY_RECV, s_zero_copy_recv);
#endif

    zsys_set_file_stable_age_msec (s_file_stable_age_msec);

    if (getenv ("ZSYS_THREAD_PRIORITY"))
        zsys_set_thread_priority (atoi (getenv ("ZSYS_THREAD_PRIORITY")));
    else
        zsys_set_thread_priority (s_thread_priority);

    if (getenv ("ZSYS_THREAD_SCHED_POLICY"))
        zsys_set_thread_sched_policy (atoi (getenv ("ZSYS_THREAD_SCHED_POLICY")));
    else
        zsys_set_thread_sched_policy (s_thread_sched_policy);

    if (getenv ("ZSYS_THREAD_NAME_PREFIX"))
        zsys_set_thread_name_prefix (atoi (getenv ("ZSYS_THREAD_NAME_PREFIX")));
    else
        zsys_set_thread_name_prefix (s_thread_name_prefix);

    if (getenv ("ZSYS_THREAD_NAME_PREFIX_STR"))
        zsys_set_thread_name_prefix_str (getenv ("ZSYS_THREAD_NAME_PREFIX_STR"));

    return s_process_ctx;
}

//  atexit or manual termination for the process
void
zsys_shutdown (void)
{
    if (!s_initialized || s_shutting_down)
        return;

    s_shutting_down = true;

    //  The atexit handler is called when the main function exits;
    //  however we may have zactor threads shutting down and still
    //  trying to close their sockets. So if we suspect there are
    //  actors busy (s_open_sockets > 0), then we sleep for a few
    //  hundred milliseconds to allow the actors, if any, to get in
    //  and close their sockets.
    ZMUTEX_LOCK (s_mutex);
    size_t busy = s_open_sockets;
    ZMUTEX_UNLOCK (s_mutex);
    if (busy)
        zclock_sleep (200);

    //  No matter, we are now going to shut down
    //  Print the source reference for any sockets the app did not
    //  destroy properly.
    ZMUTEX_LOCK (s_mutex);
    s_sockref_t *sockref = (s_sockref_t *) zlist_pop (s_sockref_list);
    while (sockref) {
        assert (sockref->filename);
        zsys_error ("[%d]dangling '%s' socket created at %s:%d",
                    getpid (),
                    zsys_sockname (sockref->type),
                    sockref->filename, (int) sockref->line_nbr);
        zmq_close (sockref->handle);
        freen (sockref);
        sockref = (s_sockref_t *) zlist_pop (s_sockref_list);
        --s_open_sockets;
    }
    zlist_destroy (&s_sockref_list);
    ZMUTEX_UNLOCK (s_mutex);

    //  Close logsender socket if opened (don't do this in critical section)
    if (s_logsender)
        zsock_destroy (&s_logsender);

    if (s_open_sockets == 0)
    {
      zmq_term(s_process_ctx);
      s_process_ctx = NULL;
      s_io_threads = 1;
      s_thread_sched_policy = -1;
      s_thread_priority = -1;
      s_thread_name_prefix = -1;
      strcpy (s_thread_name_prefix_str, "0");
      s_max_sockets = 1024;
      s_max_msgsz = INT_MAX;
      s_file_stable_age_msec = S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC;
      s_linger = 0;
      s_sndhwm = 1000;
      s_rcvhwm = 1000;
      s_pipehwm = 1000;
      s_ipv6 = 0;
      s_auto_use_fd = 0;
      s_logstream = NULL;
      s_logsystem = false;
    }
    else
        zsys_error ("dangling sockets: cannot terminate ZMQ safely");

#if !defined (__UNIX__)
    ZMUTEX_DESTROY (s_mutex);
#endif

    //  Free dynamically allocated properties
    freen (s_interface);
    freen (s_ipv6_address);
    freen (s_ipv6_mcast_address);
    freen (s_logident);

    zsys_interrupted = 0;
    zctx_interrupted = 0;

    zsys_handler_reset ();

#if defined (__UNIX__)
    closelog ();                //  Just to be pedantic
#endif

    s_initialized = false;
    s_shutting_down = false;
}

#if defined (__UNIX__)
//  Restores all CZMQ global state to initial values
static void
zsys_cleanup (void)
{
    ZMUTEX_LOCK (s_init_mutex);
    s_process_ctx = NULL;
    zsys_interrupted = 0;
    zctx_interrupted = 0;

    s_first_time = true;
    handle_signals = true;

    s_initialized = false;

    s_io_threads = 1;
    s_max_sockets = 1024;
    s_max_msgsz = INT_MAX;
    s_linger = 0;
    s_sndhwm = 1000;
    s_rcvhwm = 1000;
    s_pipehwm = 1000;
    s_ipv6 = 0;
    s_interface = NULL;
    s_ipv6_address = NULL;
    s_ipv6_mcast_address = NULL;
    s_auto_use_fd = 0;
    s_logident = NULL;
    s_logstream = NULL;
    s_logsystem = false;
    s_logsender = NULL;

    s_open_sockets = 0;
    ZMUTEX_UNLOCK (s_init_mutex);
}
#endif

//  --------------------------------------------------------------------------
//  Get a new ZMQ socket, automagically creating a ZMQ context if this is
//  the first time. Caller is responsible for destroying the ZMQ socket
//  before process exits, to avoid a ZMQ deadlock. Note: you should not use
//  this method in CZMQ apps, use zsock_new() instead. This is for system
//  use only, really.

void *
zsys_socket (int type, const char *filename, size_t line_nbr)
{
    //  First time initialization; if the application is mixing
    //  its own threading calls with zsock, this may fail if two
    //  threads try to create sockets at the same time. In such
    //  apps, they MUST create a socket in the main program before
    //  starting any threads. If the app uses zactor for its threads
    //  then we can guarantee this to always be safe.
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    void *handle = zmq_socket (s_process_ctx, type);
    if (handle) {
        //  Configure socket with process defaults
        zsock_set_linger (handle, (int) s_linger);
#if (ZMQ_VERSION_MAJOR == 2)
        // TODO: v2/v3 socket api in zsock_option.inc are not public (not
        // added to include/zsock.h) so we have to use zmq_setsockopt directly
        // This should be fixed and zsock_set_hwm should be used instead
#       if defined (ZMQ_HWM)
        uint64_t value = s_sndhwm;
        int rc = zmq_setsockopt (handle, ZMQ_HWM, &value, sizeof (uint64_t));
        assert (rc == 0 || zmq_errno () == ETERM);
#       endif
#else
        //  For later versions we use separate SNDHWM and RCVHWM
        zsock_set_sndhwm (handle, (int) s_sndhwm);
        zsock_set_rcvhwm (handle, (int) s_rcvhwm);
#   if defined (ZMQ_IPV6)
        zsock_set_ipv6 (handle, s_ipv6);
#   else
        zsock_set_ipv4only (handle, s_ipv6? 0: 1);
#   endif
#endif
        //  Add socket to reference tracker so we can report leaks; this is
        //  done only when the caller passes a filename/line_nbr
        if (filename) {
            s_sockref_t *sockref = (s_sockref_t *) zmalloc (sizeof (s_sockref_t));
            if (sockref) {
                sockref->handle = handle;
                sockref->type = type;
                sockref->filename = filename;
                sockref->line_nbr = line_nbr;
                zlist_append (s_sockref_list, sockref);
            }
            else {
                zmq_close (handle);
                ZMUTEX_UNLOCK (s_mutex);
                return NULL;
            }
        }
        s_open_sockets++;
    }
    ZMUTEX_UNLOCK (s_mutex);
    return handle;
}

//  --------------------------------------------------------------------------
//  Destroy/close a ZMQ socket. You should call this for every socket you
//  create using zsys_socket().

int
zsys_close (void *handle, const char *filename, size_t line_nbr)
{
    ZMUTEX_LOCK (s_mutex);
    //  It's possible atexit() has already happened if we're running under
    //  a debugger that redirects the main thread exit.
    if (s_sockref_list) {
        s_sockref_t *sockref = (s_sockref_t *) zlist_first (s_sockref_list);
        while (sockref) {
            if (sockref->handle == handle) {
                zlist_remove (s_sockref_list, sockref);
                freen (sockref);
                break;
            }
            sockref = (s_sockref_t *) zlist_next (s_sockref_list);
        }
    }
    s_open_sockets--;
    zmq_close (handle);
    ZMUTEX_UNLOCK (s_mutex);
    return 0;
}


//  --------------------------------------------------------------------------
//  Return ZMQ socket name for socket type

char *
zsys_sockname (int socktype)
{
    char *type_names [] = {
        "PAIR", "PUB", "SUB", "REQ", "REP",
        "DEALER", "ROUTER", "PULL", "PUSH",
        "XPUB", "XSUB", "STREAM",
        "SERVER", "CLIENT",
        "RADIO", "DISH",
        "SCATTER", "GATHER", "DGRAM"
    };
    //  This array matches ZMQ_XXX type definitions
    assert (ZMQ_PAIR == 0);
#if defined (ZMQ_DGRAM)
    assert (socktype >= 0 && socktype <= ZMQ_DGRAM);
#elif defined (ZMQ_SCATTER)
    assert (socktype >= 0 && socktype <= ZMQ_SCATTER);
#elif defined (ZMQ_DISH)
    assert (socktype >= 0 && socktype <= ZMQ_DISH);
#elif defined (ZMQ_CLIENT)
    assert (socktype >= 0 && socktype <= ZMQ_CLIENT);
#elif defined (ZMQ_STREAM)
    assert (socktype >= 0 && socktype <= ZMQ_STREAM);
#else
    assert (socktype >= 0 && socktype <= ZMQ_XSUB);
#endif
    return type_names [socktype];
}


//  --------------------------------------------------------------------------
//  Create a pipe, which consists of two PAIR sockets connected over inproc.
//  The pipe is configured to use the zsys_pipehwm setting. Returns the
//  frontend socket successful, NULL if failed.

zsock_t *
zsys_create_pipe (zsock_t **backend_p)
{
    zsock_t *frontend = zsock_new (ZMQ_PAIR);
    zsock_t *backend = zsock_new (ZMQ_PAIR);
    assert (frontend);
    assert (backend);

#if (ZMQ_VERSION_MAJOR == 2)
    // TODO: v2/v3 socket api in zsock_option.inc are not public (not
    // added to include/zsock.h) so we have to use zmq_setsockopt directly
    // This should be fixed and zsock_set_hwm should be used instead
#   if defined (ZMQ_HWM)
    uint64_t value = zsys_pipehwm ();
    int ret = zmq_setsockopt (zsock_resolve (frontend), ZMQ_HWM, &value,
        sizeof (uint64_t));
    assert (ret == 0 || zmq_errno () == ETERM);
    value = zsys_pipehwm ();
    ret = zmq_setsockopt (zsock_resolve (backend), ZMQ_HWM, &value,
        sizeof (uint64_t));
    assert (ret == 0 || zmq_errno () == ETERM);
#   endif
#else
    zsock_set_sndhwm (frontend, (int) zsys_pipehwm ());
    zsock_set_sndhwm (backend, (int) zsys_pipehwm ());
#endif
    //  Now bind and connect pipe ends
    char endpoint [32];
    while (true) {
        sprintf (endpoint, "inproc://pipe-%04x-%04x",
                 randof (0x10000), randof (0x10000));
        if (zsock_bind (frontend, "%s", endpoint) == 0)
            break;
    }
    int rc = zsock_connect (backend, "%s", endpoint);
    assert (rc != -1);          //  Connect cannot fail

    //  Return frontend and backend sockets
    *backend_p = backend;
    return frontend;
}


//  --------------------------------------------------------------------------
//  Set interrupt handler; this saves the default handlers so that a
//  zsys_handler_reset () can restore them. If you call this multiple times
//  then the last handler will take affect. If handler_fn is NULL, disables
//  default SIGINT/SIGTERM handling in CZMQ.

void
zsys_handler_set (zsys_handler_fn *handler_fn)
{
    if (!handler_fn) {
        //  Disable existing or future signal handling
        zsys_handler_reset ();
        handle_signals = false;
    }
    else {
        handle_signals = true;
#if defined (__UNIX__)
        if (s_first_time) {
            //  If first time, save default handlers
            sigaction (SIGINT, NULL, &sigint_default);
            sigaction (SIGTERM, NULL, &sigterm_default);
            s_first_time = false;
        }
        //  Install signal handler for SIGINT and SIGTERM
        struct sigaction action;
        action.sa_handler = handler_fn;
        action.sa_flags = 0;
        sigemptyset (&action.sa_mask);
        sigaction (SIGINT, &action, NULL);
        sigaction (SIGTERM, &action, NULL);
#elif defined (__WINDOWS__)
        installed_handler_fn = handler_fn;
        if (s_first_time) {
            SetConsoleCtrlHandler (s_handler_fn_shim, TRUE);
            s_first_time = false;
        }
#else
#   error "No signal handling defined for this platform"
#endif
    }
}


//  --------------------------------------------------------------------------
//  Reset interrupt handler, call this at exit if needed
//  Idempotent; safe to call multiple times

void
zsys_handler_reset (void)
{
#if defined (__UNIX__)
    //  Restore default handlers if not already done
    if (handle_signals && !s_first_time) {
        sigaction (SIGINT, &sigint_default, NULL);
        sigaction (SIGTERM, &sigterm_default, NULL);
        sigint_default.sa_handler = NULL;
        sigterm_default.sa_handler = NULL;
        s_first_time = true;
    }
#elif defined (__WINDOWS__)
    if (handle_signals && !s_first_time) {
        SetConsoleCtrlHandler (s_handler_fn_shim, FALSE);
        installed_handler_fn = NULL;
        s_first_time = true;
    }
#endif
}


//  --------------------------------------------------------------------------
//  Set default interrupt handler, so Ctrl-C or SIGTERM will set
//  zsys_interrupted. Idempotent; safe to call multiple times.

void
zsys_catch_interrupts (void)
{
    //  Catch SIGINT and SIGTERM unless ZSYS_SIGHANDLER=false
    if ((getenv ("ZSYS_SIGHANDLER") == NULL
    ||   strneq (getenv ("ZSYS_SIGHANDLER"), "false"))
    &&   handle_signals)
        zsys_handler_set (s_signal_handler);
}

//  Default internal signal handler
static void
s_signal_handler (int signal_value)
{
    zctx_interrupted = 1;
    zsys_interrupted = 1;
}


//  --------------------------------------------------------------------------
//  Return true if file exists, else zero

bool
zsys_file_exists (const char *filename)
{
    assert (filename);
    return zsys_file_mode (filename) != -1;
}


//  --------------------------------------------------------------------------
//  Return size of file, or -1 if not found

ssize_t
zsys_file_size (const char *filename)
{
    struct stat
        stat_buf;

    assert (filename);
    if (stat ((char *) filename, &stat_buf) == 0)
        return stat_buf.st_size;
    else
        return -1;
}


//  --------------------------------------------------------------------------
//  Return file modification time (accounted in seconds usually since
//  UNIX Epoch, with granularity dependent on underlying filesystem,
//  and starting point dependent on host OS and maybe its bitness).
//  Per https://msdn.microsoft.com/en-us/library/w4ddyt9h(vs.71).aspx :
//      Note   In all versions of Microsoft C/C++ except Microsoft C/C++
//      version 7.0, and in all versions of Microsoft Visual C++, the time
//      function returns the current time as the number of seconds elapsed
//      since midnight on January 1, 1970. In Microsoft C/C++ version 7.0,
//      time() returned the current time as the number of seconds elapsed
//      since midnight on December 31, 1899.
//  This value is "arithmetic" with no big guarantees in the standards, and
//  normally it should be manipulated with host's datetime suite of routines,
//  including difftime(), or converted to "struct tm" for any predictable use.
//  Returns 0 if the file does not exist.

time_t
zsys_file_modified (const char *filename)
{
    struct stat stat_buf;
    if (stat (filename, &stat_buf) == 0)
        return stat_buf.st_mtime;
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Return file mode; provides at least support for the POSIX S_ISREG(m)
//  and S_ISDIR(m) macros and the S_IRUSR and S_IWUSR bits, on all boxes.
//  Returns a mode_t cast to int, or -1 in case of error.

int
zsys_file_mode (const char *filename)
{
#if (defined (__WINDOWS__))
    DWORD dwfa = GetFileAttributesA (filename);
    if (dwfa == 0xffffffff)
        return -1;

    dbyte mode = 0;
    if (dwfa & FILE_ATTRIBUTE_DIRECTORY)
        mode |= S_IFDIR;
    else
        mode |= S_IFREG;

    if (!(dwfa & FILE_ATTRIBUTE_HIDDEN))
        mode |= S_IRUSR;
    if (!(dwfa & FILE_ATTRIBUTE_READONLY))
        mode |= S_IWUSR;

    return mode;
#else
    struct stat stat_buf;
    if (stat ((char *) filename, &stat_buf) == 0)
        return stat_buf.st_mode;
    else
        return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Delete file, return 0 if OK, -1 if not possible.

int
zsys_file_delete (const char *filename)
{
    assert (filename);
#if (defined (__WINDOWS__))
    return DeleteFileA (filename)? 0: -1;
#else
    return unlink (filename);
#endif
}


//  --------------------------------------------------------------------------
//  Check if file is 'stable'

// Internal implementation rigged with debugs and called from selftest
static bool
s_zsys_file_stable (const char *filename, bool verbose)
{
    struct stat stat_buf;
    if (stat (filename, &stat_buf) == 0) {
        //  File is 'stable' if older (per filesystem stats) than a threshold.
        //  This used to mean more than 1 second old, counted in microseconds
        //  after inflating the st_mtime data - but this way of calculation
        //  has a caveat: if we created the file at Nsec.999msec, or rather
        //  the FS metadata was updated at that point, the st_mtime will be
        //  (after inflation) N.000. So a few milliseconds later, at (N+1)sec,
        //  we find the age difference seems over 1000 so the file is 1 sec
        //  old - even though it has barely been created. Compounding the
        //  issue, some filesystems have worse timestamp precision - e.g. the
        //  FAT filesystem variants are widespread (per SD standards) on
        //  removable media, and only account even seconds in stat data.
        //  Solutions are two-fold: when using stat fields that are precise
        //  to a second (or inpredictably two), we should actually check for
        //  (age > 3000+) in rounded-microsecond accounting. Also, for some
        //  systems we can have `configure`-time checks on presence of more
        //  precise (and less standardized) stat timestamp fields, where we
        //  can presumably avoid rounding to thousands and use (age > 2000).
        //  It might also help to define a zsys_file_modified_msec() whose
        //  actual granularity will be OS-dependent (rounded to 1000 or not).
        //  These are TODO ideas for subsequent work.

#if (defined (WIN32))
#   define EPOCH_DIFFERENCE 11644473600LL
        long age = (long) (zclock_time () - EPOCH_DIFFERENCE * 1000 - (stat_buf.st_mtime * 1000));
        if (verbose)
            zsys_debug ("zsys_file_stable@WIN32: file '%s' age is %ld msec "
                "at timestamp %" PRIi64 " where st_mtime was %jd adjusted by %jd",
                filename, age, zclock_time (), (intmax_t)(stat_buf.st_mtime * 1000),
                (intmax_t)(EPOCH_DIFFERENCE * 1000) );
#else
        long age = (long) (zclock_time () - (stat_buf.st_mtime * 1000));
        if (verbose)
            zsys_debug ("zsys_file_stable@non-WIN32: file '%s' age is %ld msec "
                "at timestamp %" PRIi64 " where st_mtime was %jd",
                filename, age, zclock_time (), (intmax_t)(stat_buf.st_mtime * 1000) );
#endif
        return (age > s_file_stable_age_msec);
    }
    else {
        if (verbose)
            zsys_debug ("zsys_file_stable: could not stat file '%s'", filename);
        return false;           //  File doesn't exist, so not stable
    }
}

// Public implementation does not do debugs
bool
zsys_file_stable (const char *filename)
{
    return s_zsys_file_stable(filename, false);
}

//  --------------------------------------------------------------------------
//  Create a file path if it doesn't exist. The file path is treated as a
//  printf format.

int
zsys_dir_create (const char *pathname, ...)
{
    va_list argptr;
    va_start (argptr, pathname);
    char *formatted = zsys_vprintf (pathname, argptr);
    va_end (argptr);
    if (!formatted)
        return -1;

    //  Create parent directory levels if needed
    char *slash = strchr (formatted + 1, '/');
    while (true) {
        if (slash)
            *slash = 0;         //  Cut at slash
        int mode = zsys_file_mode (formatted);
        if (mode == -1) {
            //  Does not exist, try to create it
#if (defined (__WINDOWS__))
            if (!CreateDirectoryA (formatted, NULL)) {
#else
            if (mkdir (formatted, 0775)) {
#endif
                freen (formatted);
                return -1;      //  Failed
            }
        }
        else
        if ((mode & S_IFDIR) == 0) {
            //  Not a directory, abort
        }
        if (!slash)             //  End if last segment
            break;
        *slash = '/';
        slash = strchr (slash + 1, '/');
    }
    zstr_free (&formatted);
    return 0;
}


//  --------------------------------------------------------------------------
//  Remove a file path if empty; the pathname is treated as printf format.

int
zsys_dir_delete (const char *pathname, ...)
{
    va_list argptr;
    va_start (argptr, pathname);
    char *formatted = zsys_vprintf (pathname, argptr);
    va_end (argptr);
    if (!formatted)
        return -1;

#if (defined (__WINDOWS__))
    int rc = RemoveDirectoryA (formatted)? 0: -1;
#else
    int rc = rmdir (formatted);
#endif
    zstr_free (&formatted);
    return rc;
}


//  --------------------------------------------------------------------------
//  Move to a specified working directory. Returns 0 if OK, -1 if this failed.

int
zsys_dir_change (const char *pathname)
{
    assert (pathname);
#if (defined (__UNIX__))
    return chdir (pathname);
#elif (defined (__WINDOWS__))
    return !SetCurrentDirectoryA (pathname);
#endif
    return -1;              //  Not implemented
}


//  --------------------------------------------------------------------------
//  Set private file creation mode; all files created from here will be
//  readable/writable by the owner only.

#if !defined (__WINDOWS__)
static mode_t s_old_mask = 0;
#endif

void
zsys_file_mode_private (void)
{
#if !defined (__WINDOWS__)
    s_old_mask = umask (S_IWGRP | S_IWOTH | S_IRGRP | S_IROTH);
#endif
}


//  --------------------------------------------------------------------------
//  Reset default file creation mode; all files created from here will use
//  process file mode defaults.

void
zsys_file_mode_default (void)
{
    //  Reset process file create mask
#if !defined (__WINDOWS__)
    if (s_old_mask)
        umask (s_old_mask);
#endif
}


//  --------------------------------------------------------------------------
//  Return the CZMQ version for run-time API detection; returns version
//  number into provided fields, providing reference isn't null in each case.

void
zsys_version (int *major, int *minor, int *patch)
{
    if (major)
        *major = CZMQ_VERSION_MAJOR;
    if (minor)
        *minor = CZMQ_VERSION_MINOR;
    if (patch)
        *patch = CZMQ_VERSION_PATCH;
}


//  --------------------------------------------------------------------------
//  Format a string using printf formatting, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free(). The hinted version allows to optimize by using
//  a larger starting buffer size (known to/assumed by the developer) and so
//  avoid reallocations.

char *
zsys_sprintf_hint (int hint, const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = s_zsys_vprintf_hint (hint, format, argptr);
    va_end (argptr);
    return (string);
}

//  --------------------------------------------------------------------------
//  Format a string using printf formatting, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().

char *
zsys_sprintf (const char *format, ...)
{
    // Effectively this is a copy of the small zsys_sprintf_hint with
    // hardcoded hint value; this is is cheaper than parsing va_list
    // several times to just call the other implementation cleanly.
    va_list argptr;
    va_start (argptr, format);
    char *string = s_zsys_vprintf_hint (256, format, argptr);
    va_end (argptr);
    return (string);
}

//  --------------------------------------------------------------------------
//  Format a string with variable arguments, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().

static inline
char *
s_zsys_vprintf_hint (int hint, const char *format, va_list argptr)
{
    if (hint <= 0) {
        //  The hint is not a hard requirement so no error here.
        //  Just fall back to legacy default.
        hint = 256;
    }
    // Must use int "size" to compare to "required" (from vsnprintf)
    int size = hint;
    char *string = (char *) malloc (size);
    if (!string)
        return NULL;

    //  Using argptr is destructive, so we take a copy each time we need it
    //  We define va_copy for Windows in czmq_prelude.h
    va_list my_argptr;
    va_copy (my_argptr, argptr);
    int required = vsnprintf (string, size, format, my_argptr);
    va_end (my_argptr);
#ifdef __WINDOWS__
    if (required < 0 || required >= size) {
        va_copy (my_argptr, argptr);
#ifdef _MSC_VER
        required = _vscprintf (format, argptr);
#else
        required = vsnprintf (NULL, 0, format, argptr);
#endif
        va_end (my_argptr);
    }
#endif
    if (required < 0) {
        // vsnprintf failed at unknown point; at least prohibit
        // accesses by string consumers into random memory
        string [hint - 1] = '\0';
        return string;
    }
    //  If formatted string cannot fit into small string, reallocate a
    //  larger buffer for it. If it did fit, we only called vsnprintf()
    //  once and already have the good result, so pre-counting with a
    //  NULL string would not be beneficial for shorter texts (always
    //  calling vsnprintf() twice then).
    if (required >= hint) {
        size = required + 1;
        freen (string);
        string = (char *) malloc (size);
        if (string) {
            va_copy (my_argptr, argptr);
            vsnprintf (string, size, format, my_argptr);
            va_end (my_argptr);
        }
    }
    return string;
}

char *
zsys_vprintf (const char *format, va_list argptr)
{
    return (s_zsys_vprintf_hint(256, format, argptr));
}

//  --------------------------------------------------------------------------
//  Create a UDP beacon socket; if the routable option is true, uses
//  multicast (not yet implemented), else uses broadcast. This method
//  and related ones might _eventually_ be moved to a zudp class.

SOCKET
zsys_udp_new (bool routable)
{
    //IPV6 Multicast not implemented yet so only allow routable if IPv4
    assert ((routable && !zsys_ipv6 ()) || !routable);
    SOCKET udpsock;
    int type = SOCK_DGRAM;
#ifdef CZMQ_HAVE_SOCK_CLOEXEC
    //  Ensure socket is closed by exec() functions.
    type |= SOCK_CLOEXEC;
#endif

    if (zsys_ipv6 ())
        udpsock = socket (AF_INET6, type, IPPROTO_UDP);
    else
        udpsock = socket (AF_INET, type, IPPROTO_UDP);
    if (udpsock == INVALID_SOCKET) {
        zsys_socket_error ("socket");
        return INVALID_SOCKET;
    }

    //  On Windows, preventing sockets to be inherited by child processes.
#if defined (__WINDOWS__) && defined (HANDLE_FLAG_INHERIT)
    if (!SetHandleInformation ((HANDLE) udpsock, HANDLE_FLAG_INHERIT, 0))
        zsys_socket_error ("SetHandleInformation (HANDLE_FLAG_INHERIT)");
#endif

    //  Ask operating system for broadcast permissions on socket
    int on = 1;
    if (setsockopt (udpsock, SOL_SOCKET, SO_BROADCAST,
                    (char *) &on, sizeof (on)) == SOCKET_ERROR)
        zsys_socket_error ("setsockopt (SO_BROADCAST)");

    //  Allow multiple owners to bind to socket; incoming
    //  messages will replicate to each owner
    if (setsockopt (udpsock, SOL_SOCKET, SO_REUSEADDR,
                    (char *) &on, sizeof (on)) == SOCKET_ERROR)
        zsys_socket_error ("setsockopt (SO_REUSEADDR)");

#if defined (SO_REUSEPORT)
    //  On some platforms we have to ask to reuse the port
    if (setsockopt (udpsock, SOL_SOCKET, SO_REUSEPORT,
                    (char *) &on, sizeof (on)) == SOCKET_ERROR)
        zsys_socket_error ("setsockopt (SO_REUSEPORT)");
#endif

    //  Only set TLL for IPv4
    if (routable && zsys_mcast_ttl () > 1) {
        int ttl = zsys_mcast_ttl ();
        if (setsockopt (udpsock, IPPROTO_IP, IP_MULTICAST_TTL, (const char *) &ttl,
                        sizeof (ttl))
            == SOCKET_ERROR)
            zsys_socket_error ("setsockopt (IP_MULTICAST_TTL)");
    }

    //  TODO
    //  Set TLL for IPv6
    /*if (routable && zsys_ipv6 ()) {
       int ttl = zsys_mcast_ttl ();
        if (setsockopt (udpsock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
                (char *) &ttl, sizeof (ttl))
            == SOCKET_ERROR)
            zsys_socket_error ("setsockopt (IP_MULTICAST_TTL)");
    }*/
    return udpsock;
}


//  --------------------------------------------------------------------------
//  Close a UDP socket

int
zsys_udp_close (SOCKET handle)
{
#if (defined (__WINDOWS__))
    return closesocket (handle);
#else
    return close (handle);
#endif
}


//  --------------------------------------------------------------------------
//  Send zframe to UDP socket, return -1 if sending failed due to
//  interface having disappeared (happens easily with WiFi)

int
zsys_udp_send (SOCKET udpsock, zframe_t *frame, inaddr_t *address, int addrlen)
{
    assert (frame);
    assert (address);

    if (sendto (udpsock,
        (char *) zframe_data (frame), (int) zframe_size (frame),
        0, //  Flags
        (struct sockaddr *) address, addrlen) == -1) {
        zsys_debug ("zsys_udp_send: failed, reason=%s", strerror (errno));
        return -1;              //  UDP broadcast not possible
    }
    else
        return 0;
}


//  --------------------------------------------------------------------------
//  Receive zframe from UDP socket, and set address of peer that sent it
//  The peername must be a char [INET_ADDRSTRLEN] array if IPv6 is disabled or
//  NI_MAXHOST if it's enabled. Returns NULL when failing to get peer address.

zframe_t *
zsys_udp_recv (SOCKET udpsock, char *peername, int peerlen)
{
    char buffer [UDP_FRAME_MAX];
    in6addr_t address6;
    socklen_t address_len = sizeof (in6addr_t);
    ssize_t size = recvfrom (
        udpsock,
        buffer, UDP_FRAME_MAX,
        0,      //  Flags
        (struct sockaddr *) &address6, &address_len);

    if (size == SOCKET_ERROR)
        zsys_socket_error ("recvfrom");

    //  Get sender address as printable string
    int rc = getnameinfo ((struct sockaddr *) &address6, address_len,
                 peername, peerlen, NULL, 0, NI_NUMERICHOST);

    if (rc) {
        zsys_warning ("zsys_udp_recv: getnameinfo failed, reason=%s",
                gai_strerror (rc));
        return NULL;
    }

    //  Some platform's getnameinfo, like Solaris, appear not to append the
    //  interface name when parsing a link-local IPv6 address. These addresses
    //  cannot be used without the interface, so we must append it manually.
    //  On Windows, if_indextoname is only available from Vista.
#if !defined (__WINDOWS__) || (_WIN32_WINNT >= 0x0600)
    if (address6.sin6_family == AF_INET6 &&
            IN6_IS_ADDR_LINKLOCAL (&address6.sin6_addr) &&
            !strchr (peername, '%')) {
        char ifname [IF_NAMESIZE] = {0};
        if_indextoname (address6.sin6_scope_id, ifname);
        strcat (peername, "%");
        strcat (peername, ifname);
    }
#endif

    return zframe_new (buffer, size);
}


//  --------------------------------------------------------------------------
//  Handle an I/O error on some socket operation; will report and die on
//  fatal errors, and continue silently on "try again" errors.

void
zsys_socket_error (const char *reason)
{
    bool check_errno;

#if defined (__WINDOWS__)
    switch (WSAGetLastError ()) {
        case WSAEINTR:        errno = EINTR;      break;
        case WSAEBADF:        errno = EBADF;      break;
        case WSAEWOULDBLOCK:  errno = EAGAIN;     break;
        case WSAEINPROGRESS:  errno = EAGAIN;     break;
        case WSAENETDOWN:     errno = ENETDOWN;   break;
        case WSAECONNRESET:   errno = ECONNRESET; break;
        case WSAECONNABORTED: errno = EPIPE;      break;
        case WSAESHUTDOWN:    errno = ECONNRESET; break;
        case WSAEINVAL:       errno = EPIPE;      break;
        case WSAEADDRNOTAVAIL: errno = EADDRNOTAVAIL; break;
        case WSAEADDRINUSE:   errno = EADDRINUSE; break;
        default:              errno = GetLastError ();
    }
#endif

    check_errno = (errno == EAGAIN
                  ||  errno == ENETDOWN
                  ||  errno == EHOSTUNREACH
                  ||  errno == ENETUNREACH
                  ||  errno == EINTR
                  ||  errno == EPIPE
                  ||  errno == ECONNRESET);
#if defined (ENOPROTOOPT)
    check_errno = (check_errno ||  errno == ENOPROTOOPT);
#endif
#if defined (EHOSTDOWN)
    check_errno = (check_errno ||  errno == EHOSTDOWN);
#endif
#if defined (EOPNOTSUPP)
    check_errno = (check_errno ||  errno == EOPNOTSUPP);
#endif
#if defined (EWOULDBLOCK)
    check_errno = (check_errno ||  errno == EWOULDBLOCK);
#endif
#if defined (EPROTO)
    check_errno = (check_errno ||  errno == EPROTO);
#endif
#if defined (ENONET)
    check_errno = (check_errno ||  errno == ENONET);
#endif

    if (check_errno)
        return;             //  Ignore error and try again
    else {
        zsys_error ("(UDP) error '%s' on %s", strerror (errno), reason);
        assert (false);
    }
}


//  --------------------------------------------------------------------------
//  Return current host name, for use in public tcp:// endpoints. Caller gets
//  a freshly allocated string, should free it using zstr_free(). If the host
//  name is not resolvable, returns NULL.

char *
zsys_hostname (void)
{
    char hostname [NI_MAXHOST];
    gethostname (hostname, NI_MAXHOST);
    hostname [NI_MAXHOST - 1] = 0;
    struct hostent *host = gethostbyname (hostname);

    if (host && host->h_name)
        return strdup (host->h_name);
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Move the current process into the background. The precise effect depends
//  on the operating system. On POSIX boxes, moves to a specified working
//  directory (if specified), closes all file handles, reopens stdin, stdout,
//  and stderr to the null device, and sets the process to ignore SIGHUP. On
//  Windows, does nothing. Returns 0 if OK, -1 if there was an error.

int
zsys_daemonize (const char *workdir)
{
#if (defined (__UNIX__))
    //  Defines umask for new files this process will create
    mode_t file_mask = 027;     //  Complement of 0750

    //  Recreate our process as a child of init
    int fork_result = fork ();
    if (fork_result < 0)        //  < 0 is an error
        return -1;              //  Could not fork
    else
    if (fork_result > 0)        //  > 0 is the parent process
        exit (0);               //  End parent process

    //  Move to a safe and known directory, which is supplied as an
    //  argument to this function (or not, if workdir is NULL or empty).
    if (workdir && zsys_dir_change (workdir)) {
        zsys_error ("cannot chdir to '%s'", workdir);
        return -1;
    }
    //  Close all open file descriptors inherited from the parent
    //  process, to reduce the resources we use
    int file_handle = sysconf (_SC_OPEN_MAX);
    while (file_handle)
        close (file_handle--);  //  Ignore any errors

    //  Set the umask for new files we might create
    umask (file_mask);

    //  Set standard input and output to the null device so that any
    //  code that assumes that these files are open will work
    file_handle = open ("/dev/null", O_RDWR);
    int fh_stdout = dup (file_handle);
    int fh_stderr = dup (file_handle);
    assert (fh_stdout);
    assert (fh_stderr);

    //  Ignore any hangup signal from the controlling console
    signal (SIGHUP, SIG_IGN);
#endif
    return 0;
}


//  --------------------------------------------------------------------------
//  Drop the process ID into the lockfile, with exclusive lock, and switch
//  the process to the specified group and/or user. Any of the arguments
//  may be null, indicating a no-op. Returns 0 on success, -1 on failure.
//  Note if you combine this with zsys_daemonize, run after, not before
//  that method, or the lockfile will hold the wrong process ID.

int
zsys_run_as (const char *lockfile, const char *group, const char *user)
{
#if (defined (__UNIX__))
    //  Switch to effective user ID (who owns executable); for
    //  system services this should be root, so that we can write
    //  the PID file into e.g. /var/run/
    if (seteuid (geteuid ())) {
        zsys_error ("cannot set effective user id: %s", strerror (errno));
        return -1;
    }
    if (lockfile) {
        //  We enforce a lock on the lockfile, if specified, so that
        //  only one copy of the process can run at once.
        int handle = open (lockfile, O_RDWR | O_CREAT, 0640);
        if (handle < 0) {
            zsys_error ("cannot open lockfile '%s': %s", lockfile, strerror (errno));
            return -1;
        }
        else {
            struct flock filelock;
            filelock.l_type = F_WRLCK;      //  F_RDLCK, F_WRLCK, F_UNLCK
            filelock.l_whence = SEEK_SET;   //  SEEK_SET, SEEK_CUR, SEEK_END
            filelock.l_start = 0;           //  Offset from l_whence
            filelock.l_len = 0;             //  length, 0 = to EOF
            filelock.l_pid = getpid ();
            if (fcntl (handle, F_SETLK, &filelock)) {
                zsys_error ("cannot get lock: %s", strerror (errno));
                return -1;
            }
        }
        //   We record the current process id in the lock file
        char pid_buffer [32];
        snprintf (pid_buffer, sizeof (pid_buffer), "%6" PRIi64 "\n", (int64_t)getpid ());
        if ((size_t) write (handle, pid_buffer, strlen (pid_buffer)) != strlen (pid_buffer)) {
            zsys_error ("cannot write to lockfile: %s", strerror (errno));
            close (handle);
            return -1;
        }
    }
    if (group) {
        zsys_info ("running under group '%s'", group);
        struct group *grpbuf = NULL;
        grpbuf = getgrnam (group);
        if (grpbuf == NULL || setgid (grpbuf->gr_gid)) {
            zsys_error ("could not switch group: %s", strerror (errno));
            return -1;
        }
    }
    if (user) {
        zsys_info ("running under user '%s'", user);
        struct passwd *pwdbuf = NULL;
        pwdbuf = getpwnam (user);
        if (pwdbuf == NULL || setuid (pwdbuf->pw_uid)) {
            zsys_error ("could not switch user: %s", strerror (errno));
            return -1;
        }
    }
    else {
        //  Switch back to real user ID (who started process)
        if (setuid (getuid ())) {
            zsys_error ("cannot set real user id: %s", strerror (errno));
            return -1;
        }
    }
    return 0;
#else
    // This is not yet ported to Windows and should not succeed there.
    return -1;
#endif
}


//  --------------------------------------------------------------------------
//  Returns true if the underlying libzmq supports CURVE security.
//  Uses a heuristic probe according to the version of libzmq being used.

bool
zsys_has_curve (void)
{
#if defined (ZMQ_CURVE_SERVER)
#   if defined (ZMQ_HAS_CAPABILITIES)
    //  This is the most modern way of probing libzmq capabilities
    return zmq_has ("curve") != 0;
#   else
    //  However trying the zmq_setsockopt will also work
    int rc = -1; // assume we fail
    void *ctx = zmq_ctx_new ();
    if (ctx) {
        void *pub = zmq_socket (ctx, ZMQ_PUB);
        if (pub) {
            int as_server = 1;
            rc = zmq_setsockopt (pub, ZMQ_CURVE_SERVER, &as_server, sizeof (int));
            zmq_close (pub);
        }
        zmq_term (ctx);
    }
    return rc != -1;
#   endif
#else
    return false;
#endif
}


//  --------------------------------------------------------------------------
//  Configure the number of I/O threads that ZeroMQ will use. A good
//  rule of thumb is one thread per gigabit of traffic in or out. The
//  default is 1, sufficient for most applications. If the environment
//  variable ZSYS_IO_THREADS is defined, that provides the default.
//  Note that this method is valid only before any socket is created.

void
zsys_set_io_threads (size_t io_threads)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    if (s_open_sockets)
        zsys_error ("zsys_io_threads() is not valid after creating sockets");
    assert (s_open_sockets == 0);

    s_io_threads = io_threads;
#if ZMQ_VERSION < ZMQ_MAKE_VERSION(3, 2, 0)
    zmq_term (s_process_ctx);
    s_process_ctx = zmq_init ((int) s_io_threads);
#else
#  if defined (ZMQ_IO_THREADS)
    zmq_ctx_set (s_process_ctx, ZMQ_IO_THREADS, s_io_threads);
#  endif
#endif
    ZMUTEX_UNLOCK (s_mutex);

#if ZMQ_VERSION < ZMQ_MAKE_VERSION(3, 2, 0)
    //  Reinitialised outside of the lock to avoid recursive lock
    zsys_set_max_msgsz (s_max_msgsz);
    zsys_set_max_sockets (s_max_sockets);
#endif
}


//  --------------------------------------------------------------------------
//  Configure the scheduling policy of the ZMQ context thread pool.
//  Not available on Windows. See the sched_setscheduler man page or sched.h
//  for more information. If the environment variable ZSYS_THREAD_SCHED_POLICY
//  is defined, that provides the default.
//  Note that this method is valid only before any socket is created.

void
zsys_set_thread_sched_policy (int policy)
{
    if (policy < 0)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_set_thread_sched_policy() is not valid after"
                " creating sockets");
    assert (s_open_sockets == 0);
    s_thread_sched_policy = policy;
#if defined (ZMQ_THREAD_SCHED_POLICY)
    zmq_ctx_set (s_process_ctx, ZMQ_THREAD_SCHED_POLICY, s_thread_sched_policy);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Configure the numeric prefix to each thread created for the internal
//  context's thread pool. This option is only supported on Linux.
//  If the environment variable ZSYS_THREAD_NAME_PREFIX is defined, that
//  provides the default.
//  Note that this method is valid only before any socket is created.

void
zsys_set_thread_name_prefix (int prefix)
{
    if (prefix < 0)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_set_thread_name_prefix() is not valid after"
                " creating sockets");
    assert (s_open_sockets == 0);
    s_thread_name_prefix = prefix;
#if defined (ZMQ_THREAD_NAME_PREFIX)
    zmq_ctx_set (s_process_ctx, ZMQ_THREAD_NAME_PREFIX, s_thread_name_prefix);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}

//  --------------------------------------------------------------------------
//  Return ZMQ_THREAD_NAME_PREFIX option.
int
zsys_thread_name_prefix ()
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
#if defined (ZMQ_THREAD_NAME_PREFIX)
    s_thread_name_prefix = zmq_ctx_get (s_process_ctx, ZMQ_THREAD_NAME_PREFIX);
#endif
    ZMUTEX_UNLOCK (s_mutex);
    return s_thread_name_prefix;
}


//  --------------------------------------------------------------------------
//  Configure the string prefix to each thread created for the internal
//  context's thread pool. This option is only supported on Linux.
//  If the environment variable ZSYS_THREAD_NAME_PREFIX_STR is defined, that
//  provides the default.
//  Note that this method is valid only before any socket is created.

void
zsys_set_thread_name_prefix_str (const char *prefix)
{
    size_t prefix_len = 0;

    if (!prefix)
        return;
    prefix_len = strlen (prefix);
    if (prefix_len == 0 || prefix_len > sizeof (s_thread_name_prefix_str) - 1)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_set_thread_name_prefix() is not valid after"
                " creating sockets");
    assert (s_open_sockets == 0);
    strcpy(s_thread_name_prefix_str, prefix);
#if defined (ZMQ_THREAD_NAME_PREFIX) && defined (ZMQ_BUILD_DRAFT_API) && \
    ((ZMQ_VERSION_MAJOR > 4) || \
        ((ZMQ_VERSION_MAJOR >= 4) && ((ZMQ_VERSION_MINOR > 3) || \
            ((ZMQ_VERSION_MINOR >= 3) && (ZMQ_VERSION_PATCH >= 3)))))
    zmq_ctx_set_ext (s_process_ctx, ZMQ_THREAD_NAME_PREFIX, s_thread_name_prefix_str, sizeof (s_thread_name_prefix_str));
#endif
    ZMUTEX_UNLOCK (s_mutex);
}

//  --------------------------------------------------------------------------
//  Return ZMQ_THREAD_NAME_PREFIX_STR option.
const char *
zsys_thread_name_prefix_str ()
{
    size_t prefix_len = sizeof (s_thread_name_prefix_str);

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
#if defined (ZMQ_THREAD_NAME_PREFIX) && defined (ZMQ_BUILD_DRAFT_API) && \
    ((ZMQ_VERSION_MAJOR > 4) || \
        ((ZMQ_VERSION_MAJOR >= 4) && ((ZMQ_VERSION_MINOR > 3) || \
            ((ZMQ_VERSION_MINOR >= 3) && (ZMQ_VERSION_PATCH >= 3)))))
    zmq_ctx_get_ext (s_process_ctx, ZMQ_THREAD_NAME_PREFIX, s_thread_name_prefix_str, &prefix_len);
#else
    (void) prefix_len;
#endif
    ZMUTEX_UNLOCK (s_mutex);
    return s_thread_name_prefix_str;
}


//  --------------------------------------------------------------------------
//  Adds a specific CPU to the affinity list of the ZMQ context thread pool.
//  This option is only supported on Linux.
//  Note that this method is valid only before any socket is created.

void
zsys_thread_affinity_cpu_add (int cpu)
{
    if (cpu < 0)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_set_thread_sched_policy() is not valid after"
                " creating sockets");
    assert (s_open_sockets == 0);
#if defined (ZMQ_THREAD_AFFINITY_CPU_ADD)
    zmq_ctx_set (s_process_ctx, ZMQ_THREAD_AFFINITY_CPU_ADD, cpu);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Removes a specific CPU to the affinity list of the ZMQ context thread pool.
//  This option is only supported on Linux.
//  Note that this method is valid only before any socket is created.

void
zsys_thread_affinity_cpu_remove (int cpu)
{
    if (cpu < 0)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_set_thread_sched_policy() is not valid after"
                " creating sockets");
    assert (s_open_sockets == 0);
#if defined (ZMQ_THREAD_AFFINITY_CPU_REMOVE)
    zmq_ctx_set (s_process_ctx, ZMQ_THREAD_AFFINITY_CPU_REMOVE, cpu);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Configure the scheduling priority of the ZMQ context thread pool.
//  Not available on Windows. See the sched_setscheduler man page or sched.h
//  for more information. If the environment variable ZSYS_THREAD_PRIORITY is
//  defined, that provides the default.
//  Note that this method is valid only before any socket is created.

void
zsys_set_thread_priority (int priority)
{
    if (priority < 0)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_set_thread_priority() is not valid after"
                " creating sockets");
    assert (s_open_sockets == 0);
    s_thread_priority = priority;
#if defined (ZMQ_THREAD_PRIORITY)
    zmq_ctx_set (s_process_ctx, ZMQ_THREAD_PRIORITY, s_thread_priority);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Configure the number of sockets that ZeroMQ will allow. The default
//  is 1024. The actual limit depends on the system, and you can query it
//  by using zsys_socket_limit (). A value of zero means "maximum".
//  Note that this method is valid only before any socket is created.

void
zsys_set_max_sockets (size_t max_sockets)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (s_open_sockets)
        zsys_error ("zsys_max_sockets() is not valid after creating sockets");
    assert (s_open_sockets == 0);
    s_max_sockets = max_sockets? max_sockets: zsys_socket_limit ();
#if defined (ZMQ_MAX_SOCKETS)
    zmq_ctx_set (s_process_ctx, ZMQ_MAX_SOCKETS, (int) s_max_sockets);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Return maximum number of ZeroMQ sockets that the system will support.

size_t
zsys_socket_limit (void)
{
    size_t socket_limit;
#if defined (ZMQ_SOCKET_LIMIT)
    if (s_process_ctx)
        socket_limit = (size_t) zmq_ctx_get (s_process_ctx, ZMQ_SOCKET_LIMIT);
    else {
        void *ctx = zmq_init (1);
        socket_limit = (size_t) zmq_ctx_get (ctx, ZMQ_SOCKET_LIMIT);
        zmq_term (ctx);
    }
    //  ZeroMQ used to report a nonsense value (2^31) which if used would
    //  on zmq_ctx_set (ZMQ_MAX_SOCKETS) cause an out-of-memory error. So
    //  if we're running on an older library, enforce a sane limit.
    if (socket_limit > 65535)
        socket_limit = 65535;
#else
    socket_limit = 1024;
#endif
    return socket_limit;
}


//  --------------------------------------------------------------------------
//  Configure the maximum allowed size of a message sent.
//  The default is INT_MAX.

void
zsys_set_max_msgsz (int max_msgsz)
{
    if (max_msgsz < 0)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_max_msgsz = max_msgsz;
#if defined (ZMQ_MAX_MSGSZ)
    zmq_ctx_set (s_process_ctx, ZMQ_MAX_MSGSZ, (int) s_max_msgsz);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}

//  --------------------------------------------------------------------------
//  Configure whether to use zero copy strategy in libzmq. If the environment
//  variable ZSYS_ZERO_COPY_RECV is defined, that provides the default.
//  Otherwise the default is 1.

void
zsys_set_zero_copy_recv(int zero_copy)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_zero_copy_recv = zero_copy;
#if defined (ZMQ_ZERO_COPY_RECV)
    zmq_ctx_set (s_process_ctx, ZMQ_ZERO_COPY_RECV, s_zero_copy_recv);
#endif
    ZMUTEX_UNLOCK (s_mutex);
}

//  --------------------------------------------------------------------------
//  Return ZMQ_ZERO_COPY_RECV option.
int
zsys_zero_copy_recv()
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
#if defined (ZMQ_ZERO_COPY_RECV)
    s_zero_copy_recv = zmq_ctx_get (s_process_ctx, ZMQ_ZERO_COPY_RECV);
#endif
    ZMUTEX_UNLOCK (s_mutex);
    return s_zero_copy_recv;
}


//  --------------------------------------------------------------------------
//  Return maximum message size.

int
zsys_max_msgsz (void)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
#if defined (ZMQ_MAX_MSGSZ)
    s_max_msgsz = zmq_ctx_get (s_process_ctx, ZMQ_MAX_MSGSZ);
#endif
    ZMUTEX_UNLOCK (s_mutex);
    return s_max_msgsz;
}


//  --------------------------------------------------------------------------
//  *** Draft method, for development use, may change without warning ***
//  Check if default interrupt handler of Ctrl-C or SIGTERM was called.
//  Does not work if ZSYS_SIGHANDLER is false and code does not call
//  set interrupted on signal.
bool
    zsys_is_interrupted (void)
{
    return zsys_interrupted != 0;
}

//  --------------------------------------------------------------------------
//  *** Draft method, for development use, may change without warning ***
//  Set interrupted flag. This is done by default signal handler, however
//  this can be handy for language bindings or cases without default
//  signal handler.
void
    zsys_set_interrupted (void)
{
    zctx_interrupted = 1;
    zsys_interrupted = 1;
}

//  --------------------------------------------------------------------------
//  Configure the threshold value of filesystem object age per st_mtime
//  that should elapse until we consider that object "stable" at the
//  current zclock_time() moment.
//  The default is S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC defined in zsys.c
//  which generally depends on host OS, with fallback value of 5000.

void
    zsys_set_file_stable_age_msec (int64_t file_stable_age_msec)
{
    if (file_stable_age_msec < 1)
        return;

    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_file_stable_age_msec = file_stable_age_msec;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Return current threshold value of file stable age in msec.
//  This can be used in code that chooses to wait for this timeout
//  before testing if a filesystem object is "stable" or not.

//  Note that the OS timer quantization can bite you, so it may be
//  reasonably safe to sleep/wait/poll for a larger timeout before
//  assuming a fault, e.g. the default timer resolution on Windows
//  is 15.6 ms (per timer interrupt 64 times a second), graphed here:
//    https://msdn.microsoft.com/en-us/library/windows/desktop/dn553408(v=vs.85).aspx
//  and Unix/Linux OSes also have different-resolution timers.

int64_t
    zsys_file_stable_age_msec (void)
{
    zsys_init ();
    return s_file_stable_age_msec;
}


//  --------------------------------------------------------------------------
//  Configure the default linger timeout in msecs for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  linger time is zero, i.e. any pending messages will be dropped. If the
//  environment variable ZSYS_LINGER is defined, that provides the default.
//  Note that process exit will typically be delayed by the linger time.

void
zsys_set_linger (size_t linger)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_linger = linger;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Configure the default outgoing pipe limit (HWM) for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
//  ZSYS_SNDHWM is defined, that provides the default. Note that a value of
//  zero means no limit, i.e. infinite memory consumption.

void
zsys_set_sndhwm (size_t sndhwm)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_sndhwm = sndhwm;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Configure the default incoming pipe limit (HWM) for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
//  ZSYS_RCVHWM is defined, that provides the default. Note that a value of
//  zero means no limit, i.e. infinite memory consumption.

void
zsys_set_rcvhwm (size_t rcvhwm)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_rcvhwm = rcvhwm;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Configure the default HWM for zactor internal pipes; this is set on both
//  ends of the pipe, for outgoing messages only (sndhwm). The default HWM is
//  1,000, on all versions of ZeroMQ. If the environment var ZSYS_ACTORHWM is
//  defined, that provides the default. Note that a value of zero means no
//  limit, i.e. infinite memory consumption.

void
zsys_set_pipehwm (size_t pipehwm)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_pipehwm = pipehwm;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Return the HWM for zactor internal pipes.

size_t
zsys_pipehwm (void)
{
    return s_pipehwm;
}


//  --------------------------------------------------------------------------
//  Configure use of IPv6 for new zsock instances. By default sockets accept
//  and make only IPv4 connections. When you enable IPv6, sockets will accept
//  and connect to both IPv4 and IPv6 peers. You can override the setting on
//  each zsock_t instance. The default is IPv4 only (ipv6 set to 0). If the
//  environment variable ZSYS_IPV6 is defined (as 1 or 0), this provides the
//  default. Note: has no effect on ZMQ v2.

void
zsys_set_ipv6 (int ipv6)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_ipv6 = ipv6;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Return use of IPv6 for zsock instances.

int
zsys_ipv6 (void)
{
    return s_ipv6;
}

//  Test if ipv6 is available on the system. The only way to reliably
//  check is to actually open a socket and try to bind it. (ported from
//  libzmq)

bool
zsys_ipv6_available (void)
{
#if defined(__WINDOWS__) && (_WIN32_WINNT < 0x0600)
    return 0;
#else
    int rc, ipv6 = 1;
    struct sockaddr_in6 test_addr;

    memset (&test_addr, 0, sizeof (test_addr));
    test_addr.sin6_family = AF_INET6;
    inet_pton (AF_INET6, "::1", &(test_addr.sin6_addr));

    SOCKET fd = socket (AF_INET6, SOCK_STREAM, IPPROTO_IP);
    if (fd == INVALID_SOCKET)
        ipv6 = 0;
    else {
#if defined(__WINDOWS__)
        setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &ipv6,
                    sizeof (int));
        rc = setsockopt (fd, IPPROTO_IPV6, IPV6_V6ONLY, (const char *) &ipv6,
                         sizeof (int));
        if (rc == SOCKET_ERROR)
            ipv6 = 0;
        else {
            rc = bind (fd, (struct sockaddr *) &test_addr, sizeof (test_addr));
            if (rc == SOCKET_ERROR)
                ipv6 = 0;
        }
        closesocket (fd);
#else
        setsockopt (fd, SOL_SOCKET, SO_REUSEADDR, &ipv6, sizeof (int));
        rc = setsockopt (fd, IPPROTO_IPV6, IPV6_V6ONLY, &ipv6, sizeof (int));
        if (rc != 0)
            ipv6 = 0;
        else {
            rc = bind (fd, (struct sockaddr *) (&test_addr),
                       sizeof (test_addr));
            if (rc != 0)
                ipv6 = 0;
        }
        close (fd);
#endif
    }

    return ipv6;
#endif // _WIN32_WINNT < 0x0600
}

//  --------------------------------------------------------------------------
//  Set network interface name to use for broadcasts, particularly zbeacon.
//  This lets the interface be configured for test environments where required.
//  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
//  the default when there is no specified interface. If the environment
//  variable ZSYS_INTERFACE is set, use that as the default interface name.
//  Setting the interface to "*" means "use all interfaces".

void
zsys_set_interface (const char *value)
{
    zsys_init ();
    freen (s_interface);
    s_interface = strdup (value);
    assert (s_interface);
}


//  --------------------------------------------------------------------------
//  Return network interface to use for broadcasts, or "" if none was set.

const char *
zsys_interface (void)
{
    return s_interface? s_interface: "";
}


//  --------------------------------------------------------------------------
//  Set IPv6 address to use zbeacon socket, particularly for receiving zbeacon.
//  This needs to be set IPv6 is enabled as IPv6 can have multiple addresses
//  on a given interface. If the environment variable ZSYS_IPV6_ADDRESS is set,
//  use that as the default IPv6 address.

void
zsys_set_ipv6_address (const char *value)
{
    zsys_init ();
    freen (s_ipv6_address);
    s_ipv6_address = strdup (value);
    assert (s_ipv6_address);
}


//  --------------------------------------------------------------------------
//  Return IPv6 address to use for zbeacon reception, or "" if none was set.

const char *
zsys_ipv6_address (void)
{
    return s_ipv6_address? s_ipv6_address: "";
}


//  --------------------------------------------------------------------------
//  Set IPv6 multicast address to use for sending zbeacon messages. The default
//  is fe02::1 (link-local all-node). If the environment variable
//  ZSYS_IPV6_MCAST_ADDRESS is set, use that as the default IPv6 multicast
//  address.

void
zsys_set_ipv6_mcast_address (const char *value)
{
    zsys_init ();
    freen (s_ipv6_mcast_address);
    s_ipv6_mcast_address = strdup (value);
    assert (s_ipv6_mcast_address);
}

//  --------------------------------------------------------------------------
//  Set IPv4 multicast address to use for sending zbeacon messages. By default
//  IPv4 multicast is NOT used. If the environment variable
//  ZSYS_IPV4_MCAST_ADDRESS is set, use that as the default IPv4 multicast
//  address. Calling this function or setting ZSYS_IPV4_MCAST_ADDRESS
//  will enable IPv4 zbeacon messages.

void zsys_set_ipv4_mcast_address (const char *value)
{
    zsys_init ();
    freen (s_ipv4_mcast_address);
    s_ipv4_mcast_address = value ? strdup (value) : NULL;
    assert (!value || s_ipv4_mcast_address);
}

//  --------------------------------------------------------------------------
//  Set multicast TTL

void zsys_set_mcast_ttl (const unsigned char value)
{
    zsys_init ();
    s_mcast_ttl = value;
    assert (s_mcast_ttl);
}


//  --------------------------------------------------------------------------
//  Return IPv6 multicast address to use for sending zbeacon, or
//  "ff02:0:0:0:0:0:0:1" if none was set.

const char *
zsys_ipv6_mcast_address (void)
{
    return s_ipv6_mcast_address ? s_ipv6_mcast_address : "ff02:0:0:0:0:0:0:1";
}

//  --------------------------------------------------------------------------
//  Return IPv4 multicast address to use for sending zbeacon, or
//  NULL if none was set.

const char *zsys_ipv4_mcast_address (void)
{
    return s_ipv4_mcast_address;
}

//  --------------------------------------------------------------------------
//  Get multicast TTL

unsigned char zsys_mcast_ttl ()
{
    return s_mcast_ttl;
}


//  --------------------------------------------------------------------------
//  Configure the automatic use of pre-allocated FDs when creating new sockets.
//  If 0 (default), nothing will happen. Else, when a new socket is bound, the
//  system API will be used to check if an existing pre-allocated FD with a
//  matching port (if TCP) or path (if IPC) exists, and if it does it will be
//  set via the ZMQ_USE_FD socket option so that the library will use it
//  instead of creating a new socket.


void
zsys_set_auto_use_fd (int auto_use_fd)
{
    zsys_init ();
    ZMUTEX_LOCK (s_mutex);
    s_auto_use_fd = auto_use_fd;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Return use of automatic pre-allocated FDs for zsock instances.

int
zsys_auto_use_fd (void)
{
    return s_auto_use_fd;
}

typedef enum _zsprintf_s {
    FIND_PERCENT,
    FIND_KEY,
    FIND_FORMAT,
    END
} zsprintf_s;

typedef void* (*zsys_lookup_fn)(void *, const char*);

static char *
s_zsys_zprintf (const char *format, void *args, zsys_lookup_fn lookup_fn, bool validate)
{
    assert (format);
    assert (args);
    zchunk_t *chunk = zchunk_new (NULL, strlen (format) * 1.5);
    assert (chunk);
    char *ret = NULL;

    zsprintf_s state = FIND_PERCENT;
    size_t pos = 0;
    char *key = NULL;

    while (state != END)
    {

        if (pos >= strlen (format))
            break;

        switch (state) {
            case FIND_PERCENT:
                {

                    //zsys_debug ("\tstate=FIND_PERCENT, format+%zu=%s", pos, format+pos);
                    char *percent = strchr ((char*)(format) + pos, '%');

                    if (!percent) {
                        //zsys_debug ("!percent");
                        zchunk_extend (chunk, (format+pos), strlen (format) - pos);
                        state = END;
                    }
                    else
                    if (*(percent+1) == '%') {
                        size_t idx = percent - format;
                        //zsys_debug ("*(percent+1)=='%%':\tidx=%zu, format+%zu=%s", idx, pos, format+pos);
                        if (idx - pos > 0) {
                            //zsys_debug ("*(percent+1)=='%%':\t#2pos=%zu, idx-pos=%zu", pos, idx-pos);
                            zchunk_extend (chunk, format+pos, idx - pos);
                            pos += (idx-pos);
                            //zsys_debug ("*(percent+1)=='%%':\t#2pos=%zu, idx-pos=%zu", pos, idx-pos);
                        }
                        zchunk_extend (chunk, "%", 1);
                        pos += 2;
                    }
                    else
                    if (*(percent+1) == '(') {
                        size_t idx = percent - format;
                        //zsys_debug ("*(percent+1) == '(': idx=%zu, pos=%zu", idx, pos);
                        if (idx - pos > 0) {
                            zchunk_extend (chunk, (format+pos), idx - pos);
                            pos += (idx-pos);
                        }
                        //zsys_debug ("*(percent+1) == '(': idx=%zu, pos=%zu", idx, pos);
                        state = FIND_KEY;
                    }
                    else {
                        //zsys_debug ("else");
                        size_t idx = percent - format;
                        zchunk_extend (chunk, (format+pos), idx - pos);
                        pos += (idx-pos);
                    }
                }
                break;
            case FIND_KEY:
                {
                    //zsys_debug ("\tstate=FIND_KEY, format+%zu=%s", pos, format+pos);
                    char *key_end = strchr ((char*)(format)+pos, ')');
                    if (!key_end) {
                        zchunk_extend (chunk, (format+pos), strlen (format) - pos);
                        state = END;
                    }
                    pos += 2;
                    size_t idx = key_end - format;
                    size_t key_len = idx - pos;
                    if (key_len == 0) {
                        zchunk_extend (chunk, "()", 2);
                        pos += 2;
                        state = FIND_PERCENT;
                    }
                    zstr_free (&key);
                    key = (char*) zmalloc (key_len + 1);
                    memcpy ((void*) key, format+pos, key_len);

                    if (! lookup_fn (args, key)) {
                        char *ret = NULL;
                        if (validate)
                            ret = zsys_sprintf ("Key '%s' not found in hash", key);
                        zstr_free (&key);
                        zchunk_destroy (&chunk);
                        return ret;
                    }
                    pos += key_len + 1;
                    state = FIND_FORMAT;
                }
                break;
            case FIND_FORMAT:
                {
                    //zsys_debug ("\tstate=FIND_FORMAT, format+%zu=%s", pos, format+pos);
                    if (*(format+pos) != 's') {
                        char *ret = NULL;
                        if (validate)
                            ret = zsys_sprintf ("%s: arguments other than 's' are not implemented", key);
                        zstr_free (&key);
                        zchunk_destroy (&chunk);
                        return ret;
                    }
                    pos += 1;
                    char *v = (char *) lookup_fn (args, key);
                    zchunk_extend (chunk, v, strlen (v));
                    state = FIND_PERCENT;
                }
                break;
            case END:
                break;
        }
    }
    zstr_free (&key);

    //FIXME: is it needed?
    zchunk_extend (chunk, "\0", 1);

    if (!validate) {
        ret = strdup ((char*) zchunk_data (chunk));
        zchunk_destroy (&chunk);
        return ret;
    }

    zchunk_destroy (&chunk);
    return NULL;
}


// printf based on zhash_t
char *
zsys_zprintf (const char *format, zhash_t *args)
{
    return s_zsys_zprintf (format, (void*) args, (zsys_lookup_fn) zhash_lookup, false);
}

// return missing key or other format errors as new allocated string or NULL
char *
zsys_zprintf_error (const char *format, zhash_t *args)
{
    return s_zsys_zprintf (format, (void*) args, (zsys_lookup_fn) zhash_lookup, true);
}

static void *
s_zconfig_lookup (void *container, const char *key)
{
    zconfig_t *root = (zconfig_t*) container;
    zconfig_t *child = zconfig_locate (root, key);
    if (child)
        return zconfig_value (child);
    return NULL;
}

// printf based on zconfig
char *
zsys_zplprintf (const char *format, zconfig_t *args)
{
    return s_zsys_zprintf (format, (void*) args, (zsys_lookup_fn) s_zconfig_lookup, false);
}

// return missing key or other format errors as new allocated string or NULL
char *
zsys_zplprintf_error (const char *format, zconfig_t *args)
{
    return s_zsys_zprintf (format, (void*) args, (zsys_lookup_fn) s_zconfig_lookup, true);
}


//  --------------------------------------------------------------------------
//  Set log identity, which is a string that prefixes all log messages sent
//  by this process. The log identity defaults to the environment variable
//  ZSYS_LOGIDENT, if that is set.

void
zsys_set_logident (const char *value)
{
    zsys_init ();
    freen (s_logident);
    s_logident = strdup (value);
#if defined (__UNIX__)
    if (s_logsystem)
        openlog (s_logident, LOG_PID, LOG_USER);
#elif defined (__WINDOWS__)
    //  TODO: hook in Windows event log for Windows
#endif
    assert (s_logident);
}


//  --------------------------------------------------------------------------
//  Set stream to receive log traffic. By default, log traffic is sent to
//  stdout. If you set the stream to NULL, no stream will receive the log
//  traffic (it may still be sent to the system facility).

void
zsys_set_logstream (FILE *stream)
{
    zsys_init ();
    s_logstream = stream;
}


//  --------------------------------------------------------------------------
//  Sends log output to a PUB socket bound to the specified endpoint. To
//  collect such log output, create a SUB socket, subscribe to the traffic
//  you care about, and connect to the endpoint. Log traffic is sent as a
//  single string frame, in the same format as when sent to stdout. The
//  log system supports a single sender; multiple calls to this method will
//  bind the same sender to multiple endpoints. To disable the sender, call
//  this method with a null argument.

void
zsys_set_logsender (const char *endpoint)
{
    zsys_init ();
    if (endpoint) {
        //  Create log sender if needed
        if (!s_logsender) {
            s_logsender = zsock_new_pub(NULL);
            assert (s_logsender);
        }
        //  Bind/connect to specified endpoint(s) using zsock_attach() syntax
        int rc = zsock_attach (s_logsender, endpoint, true);
        assert (rc == 0);
    }
    else
    if (s_logsender) {
        zsock_destroy (&s_logsender);
    }
}


//  --------------------------------------------------------------------------
//  Enable or disable logging to the system facility (syslog on POSIX boxes,
//  event log on Windows). By default this is disabled.

void
zsys_set_logsystem (bool logsystem)
{
    zsys_init ();
    s_logsystem = logsystem;
#if defined (__UNIX__)
    if (s_logsystem)
        openlog (s_logident, LOG_PID, LOG_USER);
#elif defined (__WINDOWS__)
    //  TODO: hook into Windows event log
#endif
}


static void
s_log (char loglevel, char *string)
{
    if (!s_initialized)
        zsys_init ();

#if defined (__UNIX__)
#   if defined (__UTYPE_ANDROID)
    int priority = ANDROID_LOG_INFO;
    if (loglevel == 'E')
        priority = ANDROID_LOG_ERROR;
    else
    if (loglevel == 'W')
        priority = ANDROID_LOG_WARN;
    else
    if (loglevel == 'N')
        priority = ANDROID_LOG_INFO;
    else
    if (loglevel == 'I')
        priority = ANDROID_LOG_INFO;
    else
    if (loglevel == 'D')
        priority = ANDROID_LOG_DEBUG;

    __android_log_print(priority, "zsys", "%s", string);
#   else
    if (s_logsystem) {
        int priority = LOG_INFO;
        if (loglevel == 'E')
            priority = LOG_ERR;
        else
        if (loglevel == 'W')
            priority = LOG_WARNING;
        else
        if (loglevel == 'N')
            priority = LOG_NOTICE;
        else
        if (loglevel == 'I')
            priority = LOG_INFO;
        else
        if (loglevel == 'D')
            priority = LOG_DEBUG;

        syslog (priority, "%s", string);
    }
    else
#   endif
#endif
    if (s_logstream || s_logsender) {
        time_t curtime = time (NULL);
        struct tm *loctime = localtime (&curtime);
        char date [20];
        strftime (date, 20, "%y-%m-%d %H:%M:%S", loctime);
        char log_text [1024];
        if (s_logident)
            snprintf (log_text, 1024, "%c: (%s) %s %s", loglevel, s_logident, date, string);
        else
            snprintf (log_text, 1024, "%c: %s %s", loglevel, date, string);

        if (s_logstream) {
            fprintf (s_logstream, "%s\n", log_text);
            fflush (s_logstream);
        }
        if (s_logsender)
            zstr_send (s_logsender, log_text);
    }
}


//  --------------------------------------------------------------------------
//  Log error condition - highest priority

void
zsys_error (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    s_log ('E', string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log warning condition - high priority

void
zsys_warning (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    s_log ('W', string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log normal, but significant, condition - normal priority

void
zsys_notice (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    s_log ('N', string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log informational message - low priority

void
zsys_info (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    s_log ('I', string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log debug-level message - lowest priority

void
zsys_debug (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    s_log ('D', string);
    zstr_free (&string);
}

//  --------------------------------------------------------------------------
//  Selftest

void
zsys_test (bool verbose)
{
    printf (" * zsys: ");
    if (verbose)
        printf ("\n");

    // check that we can stop/restart the environnemnt
    zsys_shutdown();
    zsys_init();
    zsys_shutdown();
    zsys_init();
#ifdef CZMQ_BUILD_DRAFT_API
    // just check if we can check for ipv6
    zsys_ipv6_available();
#endif

    //  @selftest
    zsys_catch_interrupts ();

    //  Check capabilities without using the return value
    int rc = zsys_has_curve ();

    const char *SELFTEST_DIR_RW = "src/selftest-rw";

    if (verbose) {
        char *hostname = zsys_hostname ();
        zsys_info ("host name is %s", hostname);
        freen (hostname);
        zsys_info ("system limit is %zu ZeroMQ sockets", zsys_socket_limit ());
    }
#ifdef CZMQ_BUILD_DRAFT_API
    zsys_set_file_stable_age_msec (5123);
    assert (zsys_file_stable_age_msec() == 5123);
    zsys_set_file_stable_age_msec (-1);
    assert (zsys_file_stable_age_msec() == 5123);
#endif // CZMQ_BUILD_DRAFT_API
    zsys_set_linger (0);
    zsys_set_sndhwm (1000);
    zsys_set_rcvhwm (1000);
    zsys_set_pipehwm (2500);
    assert (zsys_pipehwm () == 2500);
    zsys_set_ipv6 (0);
    zsys_set_thread_priority (-1);
    zsys_set_thread_sched_policy (-1);
    zsys_set_thread_name_prefix (0);
    assert (0 == zsys_thread_name_prefix());
    assert (streq ("0", zsys_thread_name_prefix_str()));
    zsys_thread_affinity_cpu_add (0);
    zsys_thread_affinity_cpu_remove (0);
    zsys_set_zero_copy_recv(0);
    assert (0 == zsys_zero_copy_recv());
    zsys_set_zero_copy_recv(1);
    assert (1 == zsys_zero_copy_recv());

    //  Test pipe creation
    zsock_t *pipe_back;
    zsock_t *pipe_front = zsys_create_pipe (&pipe_back);
    zstr_send (pipe_front, "Hello");
    char *string = zstr_recv (pipe_back);
    assert (streq (string, "Hello"));
    freen (string);
    zsock_destroy (&pipe_back);
    zsock_destroy (&pipe_front);

    //  Test file manipulation

    // Don't let anyone fool our workspace
    if (zsys_file_exists ("nosuchfile")) {
        zsys_warning ("zsys_test() had to remove 'nosuchfile' which was not expected here at all");
        zsys_file_delete ("nosuchfile");
    }

    rc = zsys_file_delete ("nosuchfile");
    assert (rc == -1);

    bool rc_bool = zsys_file_exists ("nosuchfile");
    assert (rc_bool != true);

    rc = (int) zsys_file_size ("nosuchfile");
    assert (rc == -1);

    time_t when = zsys_file_modified (".");
    assert (when > 0);

    int mode = zsys_file_mode (".");
    assert (S_ISDIR (mode));
    assert (mode & S_IRUSR);
    assert (mode & S_IWUSR);

    const char *testbasedir  = ".testsys";
    const char *testsubdir  = "subdir";
    char *basedirpath = NULL;   // subdir in a test, under SELFTEST_DIR_RW
    char *dirpath = NULL;       // subdir in a test, under basedirpath
    char *relsubdir = NULL;     // relative short "path" of subdir under testbasedir

    basedirpath = zsys_sprintf ("%s/%s", SELFTEST_DIR_RW, testbasedir);
    assert (basedirpath);
    dirpath = zsys_sprintf ("%s/%s", basedirpath, testsubdir);
    assert (dirpath);
    relsubdir = zsys_sprintf ("%s/%s", testbasedir, testsubdir);
    assert (relsubdir);

    // Normally tests clean up in the end, but if a selftest run dies
    // e.g. on assert(), workspace remains dirty. Better clean it up.
    // We do not really care about results here - we clean up a possible
    // dirty exit of an older build. If there are permission errors etc.
    // the actual tests below would explode.
    if (zsys_file_exists(dirpath)) {
        if (verbose)
            zsys_debug ("zsys_test() has to remove ./%s that should not have been here", dirpath);
        zsys_dir_delete (dirpath);
    }
    if (zsys_file_exists (basedirpath)) {
        if (verbose)
            zsys_debug ("zsys_test() has to remove ./%s that should not have been here", basedirpath);
        zsys_dir_delete (basedirpath);
    }

    // Added tracing because this file-age check fails on some systems
    // presumably due to congestion in a mass-build and valgrind on top
    zsys_file_mode_private ();
    if (verbose)
        printf ("zsys_test() at timestamp %" PRIi64 ": "
            "Creating %s\n",
            zclock_time(), relsubdir );
    rc = zsys_dir_create ("%s/%s", SELFTEST_DIR_RW, relsubdir);
    if (verbose)
        printf ("zsys_test() at timestamp %" PRIi64 ": "
            "Finished creating %s with return-code %d\n",
            zclock_time(), relsubdir, rc );
    assert (rc == 0);
    when = zsys_file_modified (dirpath);
    if (verbose)
        printf ("zsys_test() at timestamp %" PRIi64 ": "
            "Finished calling zsys_file_modified(), got age %jd\n",
            zclock_time(), (intmax_t)when );
    assert (when > 0);
    if (verbose)
        printf ("zsys_test() at timestamp %" PRIi64 ": "
            "Checking if file is NOT stable (is younger than 1 sec)\n",
            zclock_time() );
    assert (!s_zsys_file_stable (dirpath, verbose));
    if (verbose)
        printf ("zsys_test() at timestamp %" PRIi64 ": "
            "Passed the test, file is not stable - as expected\n",
            zclock_time() );
    rc = zsys_dir_delete ("%s/%s", SELFTEST_DIR_RW, relsubdir);
    assert (rc == 0);
    rc = zsys_dir_delete ("%s/%s", SELFTEST_DIR_RW, testbasedir);
    assert (rc == 0);
    zsys_file_mode_default ();

#if (defined (PATH_MAX))
    char cwd[PATH_MAX];
#else
# if (defined (_MAX_PATH))
    char cwd[_MAX_PATH];
# else
    char cwd[1024];
# endif
#endif
    memset (cwd, 0, sizeof(cwd));
#if (defined (WIN32))
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
#else
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
#endif
        if (verbose)
            printf ("zsys_test() at timestamp %" PRIi64 ": "
                "current working directory is %s\n",
                zclock_time(), cwd);
        assert (zsys_dir_change (SELFTEST_DIR_RW) == 0);
        assert (zsys_dir_change (cwd) == 0);
    }
    else {
        zsys_warning ("zsys_test() : got getcwd() error... "
            "testing zsys_dir_change() anyway, but it can confuse "
            "subsequent tests in this process");
        assert (zsys_dir_change (SELFTEST_DIR_RW) == 0);
    }

    zstr_free (&basedirpath);
    zstr_free (&dirpath);
    zstr_free (&relsubdir);

    // Other subtests
    int major, minor, patch;
    zsys_version (&major, &minor, &patch);
    assert (major == CZMQ_VERSION_MAJOR);
    assert (minor == CZMQ_VERSION_MINOR);
    assert (patch == CZMQ_VERSION_PATCH);

    string = zsys_sprintf ("%s %02x", "Hello", 16);
    assert (streq (string, "Hello 10"));
    freen (string);

    char *str64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.";
    int num10 = 1234567890;
    string = zsys_sprintf ("%s%s%s%s%d", str64, str64, str64, str64, num10);
    assert (strlen (string) == (4 * 64 + 10));
    freen (string);

    //  Test logging system
    zsys_set_logident ("czmq_selftest");
    zsys_set_logsender ("inproc://logging");
    void *logger = zsys_socket (ZMQ_SUB, NULL, 0);
    assert (logger);
    rc = zmq_connect (logger, "inproc://logging");
    assert (rc == 0);
    rc = zmq_setsockopt (logger, ZMQ_SUBSCRIBE, "", 0);
    assert (rc == 0);

    if (verbose) {
        zsys_error ("This is an %s message", "error");
        zsys_warning ("This is a %s message", "warning");
        zsys_notice ("This is a %s message", "notice");
        zsys_info ("This is a %s message", "info");
        zsys_debug ("This is a %s message", "debug");
        zsys_set_logident ("hello, world");
        zsys_info ("This is a %s message", "info");
        zsys_debug ("This is a %s message", "debug");

        //  Check that logsender functionality is working
        char *received = zstr_recv (logger);
        assert (received);
        zstr_free (&received);
    }
    zsys_close (logger, NULL, 0);

    {
        // zhash based printf
        zhash_t *args = zhash_new ();
        zhash_insert (args, "key", "value");
        zhash_insert (args, "ham", "spam");

        char *str = zsys_zprintf ("plain string", args);
        assert (streq (str, "plain string"));
        zstr_free (&str);

        str = zsys_zprintf ("%%a%%", args);
        assert (streq (str, "%a%"));
        zstr_free (&str);

        str = zsys_zprintf ("VALUE=%(key)s123", args);
        assert (streq (str, "VALUE=value123"));
        zstr_free (&str);

        str = zsys_zprintf ("VALUE=%(key)s123, %(ham)s, %(ham)s, %%(nospam)s!!!", args);
        assert (streq (str, "VALUE=value123, spam, spam, %(nospam)s!!!"));
        zstr_free (&str);

        str = zsys_zprintf ("VALUE=%(nokey)s123, %(ham)s, %(ham)s, %%(nospam)s!!!", args);
        assert (!str);

        str = zsys_zprintf_error ("VALUE=%(nokey)s123, %(ham)s, %(ham)s, %%(nospam)s!!!", args);
        assert (streq (str, "Key 'nokey' not found in hash"));
        zstr_free (&str);

        str = zsys_zprintf ("VALUE=%(key)s/%%S", args);
        assert (streq (str, "VALUE=value/%S"));
        zstr_free (&str);

        zhash_destroy (&args);

        //ZPL based printf
        zconfig_t *root = zconfig_new ("root", NULL);
        zconfig_put (root, "zsp", "");
        zconfig_put (root, "zsp/return_code", "0");

        str = zsys_zplprintf ("return_code=%(zsp/return_code)s", root);
        assert (streq (str, "return_code=0"));
        zstr_free (&str);

        zconfig_destroy (&root);
    }

    //  @end

    zsys_set_auto_use_fd (1);
    assert (zsys_auto_use_fd () == 1);

    assert (zsys_max_msgsz () == INT_MAX);
    zsys_set_max_msgsz (2000);
    assert (zsys_max_msgsz () == 2000);
    zsys_set_max_msgsz (-1);
    assert (zsys_max_msgsz () == 2000);

    // cleanup log_sender
    zsys_set_logsender(NULL);

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif

    printf ("OK\n");
}
