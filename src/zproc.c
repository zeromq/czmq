/*  =========================================================================
    zproc - process configuration and status

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
    zproc - process configuration and status, plus unix pipes on steroids

WARNING: zproc class have several limitations atm
 * is tested on zmq4 on Linux and OSX.
 * does not work on Windows, where you get empty stubs for most of the methods
 * does not work on libzmq3 and libzmq2. We have experienced stalls and timeouts
   when running tests against such old version

Note: zproc is not yet stable, so there are no guarantees regarding API stability.
Some methods can have weird semantics or strange API.

Class zproc run an external process and to use ZeroMQ sockets to communicate
with it. In other words standard input and outputs MAY be connected with appropriate
zeromq socket and data flow is managed by zproc itself. This makes zproc
the best in class way how to run and manage sub processes.

Data are sent and received as zframes (zframe_t), so zproc does not try to interpret
content of the messages in any way. See test example on how to use it.

 +----------------------------------------+
 |    /bin/cat cat /etc/passwd            |
 |    stdin   | stdout      |    stderr   |
 |------||--------||---------------||-----|
 |      fd1       fd2              fd3    |
 |       ^         v                v     |
 |zmq://stdin |zmq://stdout |zmq://stderr |
 |         [zproc supervisor]          |
 +----------------------------------------+
 
 ----------> zeromq magic here <-----------

 +----------------------------------------+
 |zmq://stdin |zmq://stdout |zmq://stderr |
 |                                        |
 |          consumer                      |
 |                                        |
 |                                        |
 +----------------------------------------+

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

#define ZPROC_RUNNING -42

//      #######     internal helpers for zproc      #######
//      vvvvvvv                                     vvvvvvv
typedef struct _zpair_t zpair_t;

struct _zpair_t {
    char *endpoint;
    bool write_owned;
    void *write;
    bool read_owned;
    void *read;
};

static zpair_t*
zpair_new (char* endpoint) {
    zpair_t *self = (zpair_t*) zmalloc (sizeof (zpair_t));
    self->endpoint = endpoint;
    return self;
}

static void
zpair_destroy (zpair_t **self_p) {
    assert (self_p);
    if (*self_p) {
        zpair_t *self = *self_p;
        if (self->write_owned)
            zsock_destroy ((zsock_t**)&self->write);
        if (self->read_owned)
            zsock_destroy ((zsock_t**)&self->read);
        zstr_free (&self->endpoint);
        freen (self);
        *self_p = NULL;
    }
}

static void
zpair_set_read (zpair_t *self, void *sock, bool owned) {
    self->read = sock;
    self->read_owned = owned;
}

static void
zpair_set_write (zpair_t *self, void *sock, bool owned) {
    self->write = sock;
    self->write_owned = owned;
}

static void *
zpair_read (zpair_t *self) {
    return self->read;
}

static void *
zpair_write (zpair_t *self) {
    return self->write;
}

static void
zpair_mkpair (zpair_t *self) {
    self->endpoint[0] = '>';
    self->read = zsock_new_pair (self->endpoint);
    self->read_owned = true;
    self->endpoint [0] = '@';
    self->write = zsock_new_pair (self->endpoint);
    self->write_owned = true;
    self->endpoint [0] = '#';
}

static void
zpair_print (zpair_t *self) {
    assert (self);
    zsys_debug ("pair<%p> {.read = <%p>, .write = <%p>}",
        (void*) self,
        self->read,
        self->write);
}

static char **
arr_new (size_t len) {
    // we allocate one entry more, because there must be NULL at the end!
    char **ret = (char**) zmalloc (sizeof (char*) * (len + 1));
    return ret;
}

static void
arr_free (char **self) {
    assert (self);
    char **foo = self;
    while (*self) {
        freen (*(self++));
    }
    freen (foo);
}

static void
arr_add_ref (char **self, size_t i, char *s) {
    assert (self);
    self [i] = s;
}

static void
arr_print (char**self) {
    assert (self);
    size_t i = 0;
    while (self [i]) {
        zsys_debug ("%zu:\t%s<%p>", i, self [i], (void*) *self);
        i += 1;
    }
}

//      ^^^^^^^                                     ^^^^^^^
//      #######     internal helpers for zproc      #######

struct _zproc_t {
#if ! defined (__WINDOWS__)
    //TODO: there is no windows port, so lets exclude pid from struct
    //      zproc wasn't ported there, so no reason to do so
    pid_t pid;
#endif
    int return_code;
    bool running;
    bool verbose;

    zactor_t *actor;
    zloop_t *loop_ref;
    zsock_t *pipe;          // destroy actor pipe in a case of execve fail
    int stdinpipe [2];      // stdin pipe
    int stdoutpipe [2];     // stdout pipe
    int stderrpipe [2];     // stderr pipe

    zpair_t *stdinpair;     // stdin socketpair
    zpair_t *stdoutpair;    // stdout socketpair
    zpair_t *stderrpair;    // stderr socketpair

    zlistx_t *args;         // command line arguments
    zhashx_t *env;          // environment
};

zproc_t*
zproc_new ()
{
#if defined (__WINDOWS__)
    zsys_error ("zproc_set_stdin not implemented for Windows");
    return NULL;
#elif ZMQ_VERSION_MAJOR < 4
    zsys_error ("Cannot use zproc with zmq older than 4");
    return NULL;
#endif
    int major, minor, patch;
    zsys_version (&major, &minor, &patch);
    if (major < 4) {
        zsys_error ("Cannot use zproc with zmq older than 4");
        return NULL;
    }

    zproc_t *self = (zproc_t*) zmalloc (sizeof (zproc_t));
    self->verbose = false;

    zuuid_t *uuid = zuuid_new ();
    self->stdinpair = zpair_new (
        zsys_sprintf ("#inproc://zproc-%s-stdin", zuuid_str_canonical (uuid))
    );
    self->stdoutpair = zpair_new (
        zsys_sprintf ("#inproc://zproc-%s-stdout", zuuid_str_canonical (uuid))
    );
    self->stderrpair = zpair_new (
        zsys_sprintf ("#inproc://zproc-%s-stderr", zuuid_str_canonical (uuid))
    );
    zuuid_destroy (&uuid);

    return self;
}

void
zproc_destroy (zproc_t **self_p) {
    assert (self_p);
    if (*self_p) {
        zproc_t *self = *self_p;
        zproc_wait (self, true);
        zactor_destroy (&self->actor);

        close (self->stdinpipe [0]);
        close (self->stdinpipe [1]);
        close (self->stdoutpipe [0]);
        close (self->stdoutpipe [1]);
        close (self->stderrpipe [0]);
        close (self->stderrpipe [1]);

        zpair_destroy (&self->stdinpair);
        zpair_destroy (&self->stdoutpair);
        zpair_destroy (&self->stderrpair);

        zlistx_destroy (&self->args);
        zhashx_destroy (&self->env);
        freen (self);
        *self_p = NULL;
    }
}

void
zproc_set_args (zproc_t *self, zlistx_t *args) {
    assert (self);
    zlistx_destroy (&self->args);
    self->args = args;
}

void
zproc_set_env (zproc_t *self, zhashx_t *env) {
    assert (self);
    zhashx_destroy (&self->env);
    self->env = env;
}

void
zproc_set_stdin (zproc_t *self, void* socket) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("zproc_set_stdin not implemented for Windows");
    return;
#else
    assert (self->stdinpipe [0] == 0);
    int r = pipe (self->stdinpipe);
    assert (r == 0);

    if (!socket)
        zpair_mkpair (self->stdinpair);
    else
        zpair_set_read (self->stdinpair, socket, false);
#endif
}

void
zproc_set_stdout (zproc_t *self, void* socket) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("zproc_set_stdout not implemented for Windows");
    return;
#else
    assert (self->stdoutpipe [0] == 0);
    int r = pipe (self->stdoutpipe);
    assert (r == 0);

    if (!socket)
        zpair_mkpair (self->stdoutpair);
    else
        zpair_set_write (self->stdoutpair, socket, false);
#endif
}

void
zproc_set_stderr (zproc_t *self, void* socket) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("zproc_set_stdout not implemented for Windows");
    return;
#else
    assert (self->stderrpipe [0] == 0);
    int r = pipe (self->stderrpipe);
    assert (r == 0);

    if (!socket)
        zpair_mkpair (self->stderrpair);
    else
        zpair_set_write (self->stderrpair, socket, false);
#endif
}

void*
zproc_stdin (zproc_t *self) {
    return zpair_write (self->stdinpair);
}

void*
zproc_stdout (zproc_t *self) {
    return zpair_read (self->stdoutpair);
}

void*
zproc_stderr (zproc_t *self) {
    return zpair_read (self->stderrpair);
}

int
zproc_returncode (zproc_t *self) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("zproc_returncode not implemented on Windows");
    return -1;
#else
    assert (self->pid);
    zproc_wait (self, false);
    return self->return_code;
#endif
}

int
zproc_pid (zproc_t *self) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("zproc_pid not implemented on Windows");
    return -1;
#else
    return self->pid;
#endif
}


static int
s_fd_in_handler (zloop_t *self, zmq_pollitem_t *item, void *socket)
{
#define BUF_SIZE 1024
    byte buf [BUF_SIZE];
    ssize_t r = 1;

    while (r > 0) {
        r = read (item->fd, buf, BUF_SIZE);
        if (r == -1) {
            zsys_error ("read from fd %d: %s", item->fd, strerror (errno));
            break;
        }
        else
        if (r == 0)
            break;
        zframe_t *frame = zframe_new (buf, r);
        zsock_bsend (socket, "f", frame, NULL);
        zframe_destroy (&frame);
    }
    return 0;
#undef BUF_SIZE
}

static int
s_fd_out_handler (zloop_t *self, zmq_pollitem_t *item, void *socket)
{
    ssize_t r = 1;

    while (r > 0) {

        zframe_t *frame;
        r = zsock_brecv (socket, "f", &frame);
        if (r == -1) {
            zsys_error ("read from socket <%p>: %s", socket, strerror (errno));
            break;
        }

        r = write (item->fd, zframe_data (frame), zframe_size (frame));
        zframe_destroy (&frame);

        if (r == -1) {
            zsys_error ("write to fd %d: %s", item->fd, strerror (errno));
            break;
        }
    }
    return 0;
}

static int
s_zproc_addfd (zproc_t *self, int fd, void* socket, int flags) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("s_zproc_addfd not implemented for Windows");
    return -1;
#else
    zmq_pollitem_t it = {NULL, fd, flags, 0};
    return zloop_poller (
        self->loop_ref,
        &it,
        flags == ZMQ_POLLIN ? s_fd_in_handler : s_fd_out_handler,
        socket);
#endif
}

static int
s_zproc_alive (zloop_t *loop, int timer_id, void *args)
{
    zproc_t *self = (zproc_t*) args;
    if (zsys_interrupted)
        return -1;
    if (zproc_pid (self) && zproc_running (self))
        return 0;
    return -1;
}

static int
s_zproc_execve (zproc_t *self)
{
#if defined(__WINDOWS__)
    zsys_debug ("s_zproc_execve not implemented on Windows");
    return -1;
#else
    assert (self);
    int r;

    char *filename = (char*) zlistx_first (self->args);
    self->pid = fork ();
    if (self->pid == 0) {

        if (self->stdinpipe [0] != 0) {
            int o_flags = fcntl (self->stdinpipe [0], F_GETFL);
            int n_flags = o_flags & (~O_NONBLOCK);
            fcntl (self->stdinpipe [0], F_SETFL, n_flags);
            dup2 (self->stdinpipe [0], STDIN_FILENO);
            close (self->stdinpipe [1]);
        }

        // redirect stdout if set_stdout was called
        if (self->stdoutpipe [0] != 0) {
            close (self->stdoutpipe [0]);
            dup2 (self->stdoutpipe [1], STDOUT_FILENO);
        }

        // redirect stdout if set_stderr was called
        if (self->stderrpipe [0] != 0) {
            close (self->stderrpipe [0]);
            dup2 (self->stderrpipe [1], STDERR_FILENO);
        }

        // build argv for now and use self->args
        char **argv2 = arr_new (zlistx_size (self->args) + 1);

        size_t i = 0;
        for (char *arg = (char*) zlistx_first (self->args);
                   arg != NULL;
                   arg = (char*) zlistx_next (self->args)) {
            arr_add_ref (argv2, i, arg);
            i++;
        }
        arr_add_ref (argv2, i, NULL);

        // build environ for a new process
        char **env = arr_new (zhashx_size (self->env) + 1);

        i = 0;
        for (char *arg = (char*) zhashx_first (self->env);
                   arg != NULL;
                   arg = (char*) zhashx_next (self->env)) {
            char *name = (char*) zhashx_cursor (self->env);
            arr_add_ref (env, i, zsys_sprintf ("%s=%s", name, arg));
            i++;
        }
        arr_add_ref (env, i, NULL);

        r = execve (filename, argv2, env);
        if (r == -1) {
            zsys_error ("fail to run %s: %s", filename, strerror (errno));
            zproc_destroy (&self);
            zsock_destroy (&self->pipe);
            arr_free (argv2);
            exit (errno);
        }
    }
    else
    if (self->pid == -1) {
        zsys_error ("error fork: %s", strerror (errno));
        exit (EXIT_FAILURE);
    }
    else {
        if (self->verbose)
            zsys_debug ("process %s with pid %d started", filename, self->pid);

        if (self->stdinpipe [0] != 0) {
            s_zproc_addfd (self, self->stdinpipe [1], zpair_read (self->stdinpair), ZMQ_POLLOUT);
            close (self->stdinpipe [0]);
        }

        // add a handler for read end of stdout
        if (self->stdoutpipe [1] != 0) {
            s_zproc_addfd (self, self->stdoutpipe [0], zpair_write (self->stdoutpair), ZMQ_POLLIN);
            close(self->stdoutpipe[1]);
        }
        // add a handler for read end of stderr
        if (self->stderrpipe [1] != 0) {
            s_zproc_addfd (self, self->stderrpipe [0], zpair_write (self->stderrpair), ZMQ_POLLIN);
            close(self->stderrpipe[1]);
        }
    }

    return 0;
#endif
}

static int
s_pipe_handler (zloop_t *loop, zsock_t *pipe, void *args) {
    zproc_t *self = (zproc_t*) args;

    int ret = 0;

    zmsg_t *msg = zmsg_recv (pipe);
    char *command = zmsg_popstr (msg);
    if (self->verbose)
        zsys_debug ("API command=%s", command);

    if (streq (command, "$TERM"))
        ret = -1;
    else
    if (streq (command, "RUN")) {

        if (zproc_pid (self) > 0) {
            zsys_error ("Can't run command twice!!");
            goto end;
        }

        s_zproc_execve (self);
    }

end:
    zstr_free(&command);
    zmsg_destroy (&msg);

    return ret;
}

static void
s_zproc_actor (zsock_t *pipe, void *args)
{

    zproc_t *self = (zproc_t*) args;
    zloop_t *loop = zloop_new ();
    assert (loop);
    self->loop_ref = loop;
    self->pipe = pipe;

    zloop_reader (loop, pipe, s_pipe_handler, (void*) self);
    zloop_timer (loop, 500, 0, s_zproc_alive, (void*) self);

    zsock_signal (pipe, 0);
    zloop_start (loop);
    zloop_destroy (&loop);
    zsock_signal (pipe, 0);
}

int
zproc_run (zproc_t *self)
{
#if defined (__WINDOWS__)
    zsys_error ("zproc not yet implemented for Windows");
    return -1;
#endif
    assert (self);
    assert (!self->actor);

    if (!self->args || zlistx_size (self->args) == 0) {
        zsys_error ("No arguments, nothing to run. Call zproc_set_args before");
        return -1;
    }

    self->actor = zactor_new (s_zproc_actor, (void*) self);
    self->running = true;
    self->return_code = ZPROC_RUNNING;

    zstr_send (self->actor, "RUN");
    return 0;
}

int
zproc_wait (zproc_t *self, bool wait) {
#if defined (__WINDOWS__)
    zsys_error ("zproc not yet implemented for Windows");
    return -1;
#else
    assert (self);
    if (!self->pid)
        return 0;

    if (self->verbose)
        zsys_debug ("zproc_wait [%d]: wait=%s", self->pid, wait ? "true" : "false");
    int status = -1;
    int options = !wait ? WNOHANG : 0;
    if (self->verbose)
        zsys_debug ("zproc_wait [%d]:\t!self->running=%s", self->pid, self->running ? "true" : "false");
    if (!self->running)
        return self->return_code;

    if (self->verbose)
        zsys_debug ("zproc_wait [%d]:\twaitpid", self->pid);
    int r = waitpid (self->pid, &status, options);
    if (self->verbose)
        zsys_debug ("zproc_wait [%d]:\twaitpid, r=%d", self->pid, r);
    if (!wait && r == 0)
        return self->return_code;

    if (WIFEXITED(status)) {
        if (self->verbose)
            zsys_debug ("zproc_wait [%d]:\tWIFEXITED", self->pid);
        self->running = false;
        self->return_code = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status)) {
        if (self->verbose)
            zsys_debug ("zproc_wait [%d]:\tWIFSIGNALED", self->pid);
        self->running = false;
        self->return_code = - WTERMSIG(status);

        /*
        if (WCOREDUMP(status)) {
            self->core_dumped = true;
        }
        */
    }
    if (self->verbose)
        zsys_debug ("zproc_wait [%d]: self->return_code=%d", self->pid, self->return_code);
    return ZPROC_RUNNING;
#endif
}

bool
zproc_running (zproc_t *self) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_debug ("zproc_running not implemented on Windows");
    return false;
#else
    assert (self->pid);
    return zproc_wait (self, false) == ZPROC_RUNNING;
#endif
}

void *
zproc_actor (zproc_t *self) {
    assert (self);
    return self->actor;
}

//  send a signal to the subprocess
void
zproc_kill (zproc_t *self, int signum) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_debug ("zproc_kill not implemented on Windows");
    return;
#else
    int r = kill (self->pid, signum);
    if (r != 0)
        zsys_error ("kill of pid=%d failed: %s", self->pid, strerror (errno));
    zproc_wait (self, false);
#endif
}

void
zproc_set_verbose (zproc_t *self, bool verbose) {
    assert (self);
    self->verbose = verbose;
}

//  --------------------------------------------------------------------------
//  Returns CZMQ version as a single 6-digit integer encoding the major
//  version (x 10000), the minor version (x 100) and the patch.        

int
zproc_czmq_version (void)
{
    return CZMQ_VERSION;
}


//  --------------------------------------------------------------------------
//  Returns true if the process received a SIGINT or SIGTERM signal.
//  It is good practice to use this method to exit any infinite loop
//  processing messages.                                            

bool
zproc_interrupted (void)
{
    return zsys_interrupted == 1;
}

//  --------------------------------------------------------------------------
//  Returns true if the underlying libzmq supports CURVE security.

bool
zproc_has_curve (void)
{
	return zsys_has_curve ();
}


//  --------------------------------------------------------------------------
//  Return current host name, for use in public tcp:// endpoints.
//  If the host name is not resolvable, returns NULL. The caller is
//  responsible for destroying the return value when finished with it.

char *
zproc_hostname (void)
{
    return zsys_hostname ();
}


//  --------------------------------------------------------------------------
//  Move the current process into the background. The precise effect     
//  depends on the operating system. On POSIX boxes, moves to a specified
//  working directory (if specified), closes all file handles, reopens   
//  stdin, stdout, and stderr to the null device, and sets the process to
//  ignore SIGHUP. On Windows, does nothing. Returns 0 if OK, -1 if there
//  was an error.                                                        

void
zproc_daemonize (const char *workdir)
{
    zsys_daemonize (workdir);
}


//  --------------------------------------------------------------------------
//  Drop the process ID into the lockfile, with exclusive lock, and   
//  switch the process to the specified group and/or user. Any of the 
//  arguments may be null, indicating a no-op. Returns 0 on success,  
//  -1 on failure. Note if you combine this with zsys_daemonize, run  
//  after, not before that method, or the lockfile will hold the wrong
//  process ID.                                                       

void
zproc_run_as (const char *lockfile, const char *group, const char *user)
{
    zsys_run_as (lockfile, group, user);
}


//  --------------------------------------------------------------------------
//  Configure the number of I/O threads that ZeroMQ will use. A good  
//  rule of thumb is one thread per gigabit of traffic in or out. The 
//  default is 1, sufficient for most applications. If the environment
//  variable ZSYS_IO_THREADS is defined, that provides the default.   
//  Note that this method is valid only before any socket is created. 

void
zproc_set_io_threads (size_t io_threads)
{
    zsys_set_io_threads (io_threads);
}


//  --------------------------------------------------------------------------
//  Configure the number of sockets that ZeroMQ will allow. The default  
//  is 1024. The actual limit depends on the system, and you can query it
//  by using zsys_socket_limit (). A value of zero means "maximum".      
//  Note that this method is valid only before any socket is created.    

void
zproc_set_max_sockets (size_t max_sockets)
{
    zsys_set_max_sockets (max_sockets);
}


//  --------------------------------------------------------------------------
//  Set network interface name to use for broadcasts, particularly zbeacon.    
//  This lets the interface be configured for test environments where required.
//  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is  
//  the default when there is no specified interface. If the environment       
//  variable ZSYS_INTERFACE is set, use that as the default interface name.    
//  Setting the interface to "*" means "use all available interfaces".         

void
zproc_set_biface (const char *value)
{
    zsys_set_interface (value);
}


//  --------------------------------------------------------------------------
//  Return network interface to use for broadcasts, or "" if none was set.

const char *
zproc_biface (void)
{
    return zsys_interface ();
}


//  --------------------------------------------------------------------------
//  Set log identity, which is a string that prefixes all log messages sent
//  by this process. The log identity defaults to the environment variable 
//  ZSYS_LOGIDENT, if that is set.                                         

void
zproc_set_log_ident (const char *value)
{
    zsys_set_logident (value);
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
zproc_set_log_sender (const char *endpoint)
{
    zsys_set_logsender (endpoint);
}


//  --------------------------------------------------------------------------
//  Enable or disable logging to the system facility (syslog on POSIX boxes,
//  event log on Windows). By default this is disabled.                     

void
zproc_set_log_system (bool logsystem)
{
    zsys_set_logsystem (logsystem);
}


//  --------------------------------------------------------------------------
//  Log error condition - highest priority

void
zproc_log_error (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    zsys_error (string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log warning condition - high priority

void
zproc_log_warning (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    zsys_warning (string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log normal, but significant, condition - normal priority

void
zproc_log_notice (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    zsys_notice (string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log informational message - low priority

void
zproc_log_info (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    zsys_info (string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Log debug-level message - lowest priority

void
zproc_log_debug (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    zsys_debug (string);
    zstr_free (&string);
}


//  --------------------------------------------------------------------------
//  Self test of this class.

void
zproc_test (bool verbose)
{
    printf (" * zproc: ");

    //  @selftest
#if defined (__WINDOWS__)
    printf ("SKIPPED (on Windows)\n");
    return;
#endif
#if ZMQ_VERSION_MAJOR < 4
    printf ("SKIPPED (on zmq pre-4)\n");
    return;
#endif
    int major, minor, patch;
    zsys_version (&major, &minor, &patch);
    if (major < 4) {
        printf ("SKIPPED (on zmq pre-4)\n");
        return;
    }

    //  @selftest
    //  0. initialization

    if (verbose) {
        printf("\n");
    }

    //  find the right binary for current build (in-tree, distcheck, etc.)
    char *file = NULL;
    if (zsys_file_exists ("src/zsp") || zsys_file_exists ("./src/zsp"))
        file = "./src/zsp";
    else
    if (zsys_file_exists ("../zsp"))
    //  WHOA: zproc: zproc_test() : current working directory is
    //      /home/travis/build/username/czmq/czmq-4.0.3/_build/src/selftest-rw
        file = "../zsp";
    else
    if (zsys_file_exists ("_build/../src/zsp"))
        file = "_build/../src/zsp";
    else
    if (zsys_file_exists ("_build/src/zsp"))
        file = "_build/src/zsp";
    else
    if (zsys_file_exists ("../_build/src/zsp"))
        file = "../_build/src/zsp";
    else
    if (zsys_file_exists ("../../_build/src/zsp"))
        file = "../../_build/src/zsp";
    else
    if (zsys_file_exists ("_build/sub/src/zsp"))
        file = "_build/sub/src/zsp";
    else
    if (zsys_file_exists ("../_build/sub/src/zsp"))
        file = "../_build/sub/src/zsp";
    else
    if (zsys_file_exists ("../../_build/sub/src/zsp"))
        file = "../../_build/sub/src/zsp";
    else
    if (zsys_file_exists ("zsp") || zsys_file_exists ("./zsp"))
        file = "./zsp";
    else
    if (zsys_file_exists ("../src/zsp"))
        file = "../src/zsp";

    if (file == NULL || !zsys_file_exists (file)) {
        zsys_warning ("cannot detect zsp binary, %s does not exist", file ? file : "<null>");

        printf ("SKIPPED (zsp helper not found)\n");

#if (defined (PATH_MAX))
        char cwd[PATH_MAX];
#else
# if (defined (_MAX_PATH))
        char cwd[_MAX_PATH];
# else
        char cwd[1024];
# endif
#endif
        memset (cwd, 0, sizeof (cwd));
#if (defined (WIN32))
        if (_getcwd(cwd, sizeof(cwd)) != NULL)
#else
        if (getcwd(cwd, sizeof(cwd)) != NULL)
#endif
            printf ("zproc_test() : current working directory is %s\n", cwd);

        return;
    }
    if (verbose) {
        zsys_info ("zproc_test() : detected a zsp binary at %s\n", file);
    }

    //  Create new subproc instance
    zproc_t *self = zproc_new ();
    zproc_set_verbose (self, verbose);
    assert (self);
    //  join stdout of the process to zeromq socket
    //  all data will be readable from zproc_stdout socket
    zproc_set_stdout (self, NULL);

    zlistx_t *args = zlistx_new ();
    zlistx_add_end (args, file);
    zlistx_add_end (args, "--stdout");
    zproc_set_args (self, args);

    zhashx_t *env = zhashx_new ();
    zhashx_insert (env, "ZSP_MESSAGE", "czmq is great\n");
    zproc_set_env (self, env);

    // execute the binary. It runs in own actor, which monitor the process and
    // pass data accross pipes and zeromq sockets
    if (verbose)
        zsys_debug("zproc_test() : launching helper '%s'", file );
    zproc_run (self);
    zpoller_t *poller = zpoller_new (zproc_stdout (self), NULL);

    // kill the binary, it never ends, but the test must:
    // termination also flushes the output streams so we can
    // read them entirely; note that other process runs in
    // parallel to this thread
    if (verbose)
        zsys_debug("zproc_test() : sleeping 4000 msec to gather some output from helper");
    zclock_sleep (4000);
    zproc_kill (self, SIGTERM);
    zproc_wait (self, true);

    // read the content from zproc_stdout - use zpoller and a loop
    bool stdout_read = false;
    int64_t zproc_timeout_msec = 10000;
    int64_t zproc_test_start_msec = zclock_mono();
    int64_t zproc_test_elapsed_msec = 0;

    while (!zsys_interrupted) {
        void *which = zpoller_wait (poller, 800);
        zproc_test_elapsed_msec = zclock_mono() - zproc_test_start_msec;

        if (!which) {
            if (stdout_read) {
                if (verbose)
                    zsys_debug("zproc_test() : did not get stdout from helper, but we already have some (%" PRIi64 " msec remaining to retry)", (zproc_timeout_msec - zproc_test_elapsed_msec) );
                break;
            }
            if (zproc_timeout_msec > zproc_test_elapsed_msec) {
                if (verbose)
                    zsys_debug("zproc_test() : did not get stdout from helper, %" PRIi64 " msec remaining to retry", (zproc_timeout_msec - zproc_test_elapsed_msec) );
                continue;
            }
            // ...else : we've slept a lot and got no response; kill the helper
            if (verbose)
                zsys_debug("zproc_test() : did not get stdout from helper, patience expired (%" PRIi64 " msec remaining to retry)", (zproc_timeout_msec - zproc_test_elapsed_msec) );
            break;
        }

        if (which == zproc_stdout (self)) {
            // it suffices for us to have read something
            // we only check the first frame, others may start with the
            // expected key string broken mid-way due to alignment etc.,
            // but we drain the whole incoming queue of stdout frames.
            zframe_t *frame;
            zsock_brecv (zproc_stdout (self), "f", &frame);
            assert (frame);
            assert (zframe_data (frame));
            if (!stdout_read) {
                if (verbose)
                    zsys_debug("zproc_test() : got stdout from helper, %" PRIi64 " msec was remaining to retry", (zproc_timeout_msec - zproc_test_elapsed_msec));
                assert (!strncmp(
                    "czmq is great\n",
                    (char*) zframe_data (frame),
                    14));
                stdout_read = true;
            }

            if (verbose)
                zframe_print (frame, "zproc_test");

            zframe_destroy (&frame);
            continue;
        }

        // should not get there
        if (verbose)
            zsys_debug("zproc_test() : reached the unreachable point (unexpected zpoller result), %" PRIi64 " msec was remaining to retry", (zproc_timeout_msec - zproc_test_elapsed_msec) );
        assert (false);
    }

    assert (stdout_read);
    zpoller_destroy (&poller);
    zproc_destroy (&self);
    //  @end

    // to have zpair print and arr print methods
    zpair_t *pair = zpair_new (strdup ("e"));
    assert (pair);
    if (verbose)
        zpair_print (pair);
    zpair_destroy (&pair);

    char ** a = arr_new (1);
    if (verbose)
        arr_print (a);
    arr_free (a);

    printf ("OK\n");
}
