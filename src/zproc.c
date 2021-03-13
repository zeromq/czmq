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

#if defined (__UNIX__)
#   if defined (__UTYPE_OSX)
// issue#1836
#include <crt_externs.h>
#define environ (*_NSGetEnviron ())
#   else
extern char **environ;              // should be declared as a part of unistd.h, but fail in some targets in Travis
                                    // declare it explicitly
#   endif
#endif

// For getcwd() variants
#if (defined (WIN32))
# include <direct.h>
#else
# include <unistd.h>
#endif

#if defined (__UTYPE_OSX)
#include <crt_externs.h>
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
zpair_new (char *endpoint) {
    zpair_t *self = (zpair_t*) zmalloc (sizeof (zpair_t));
    if (self) {
        self->endpoint = endpoint;
    }
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
#if defined (__WINDOWS__)
   PROCESS_INFORMATION piProcInfo;
#else
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

    zlist_t *args;         // command line arguments
    zhash_t *env;          // environment
};

zproc_t*
zproc_new ()
{
#if ZMQ_VERSION_MAJOR < 4
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
    assert (self);
    self->verbose = false;

    self->stdinpipe [0] = -1;
    self->stdinpipe [1] = -1;
    self->stdoutpipe [0] = -1;
    self->stdoutpipe [1] = -1;
    self->stderrpipe [0] = -1;
    self->stderrpipe [1] = -1;

    zuuid_t *uuid = zuuid_new ();
    assert (uuid);
    self->stdinpair = zpair_new (
        zsys_sprintf ("#inproc://zproc-%s-stdin", zuuid_str_canonical (uuid))
    );
    assert (self->stdinpair);
    self->stdoutpair = zpair_new (
        zsys_sprintf ("#inproc://zproc-%s-stdout", zuuid_str_canonical (uuid))
    );
    assert (self->stdoutpair);
    self->stderrpair = zpair_new (
        zsys_sprintf ("#inproc://zproc-%s-stderr", zuuid_str_canonical (uuid))
    );
    assert (self->stderrpair);
    zuuid_destroy (&uuid);

    return self;
}


void
zproc_destroy (zproc_t **self_p) {
    assert (self_p);
    if (*self_p) {
        zproc_t *self = *self_p;
        zproc_shutdown (self, 5000);
        zactor_destroy (&self->actor);

        if (self->stdinpipe [0] != -1)  close (self->stdinpipe [0]);
        if (self->stdinpipe [1] != -1)  close (self->stdinpipe [1]);
        if (self->stdoutpipe [0] != -1) close (self->stdoutpipe [0]);
        if (self->stdoutpipe [1] != -1) close (self->stdoutpipe [1]);
        if (self->stderrpipe [0] != -1) close (self->stderrpipe [0]);
        if (self->stderrpipe [1] != -1) close (self->stderrpipe [1]);

        zpair_destroy (&self->stdinpair);
        zpair_destroy (&self->stdoutpair);
        zpair_destroy (&self->stderrpair);

        zlist_destroy (&self->args);
        zhash_destroy (&self->env);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------
//  Return command line arguments (the first item is the executable) or
//  NULL if not set.
//  Caller owns return value and must destroy it when done.

zlist_t *
zproc_args (zproc_t *self)
{
    assert (self);
    if (self->args) {
        zlist_t *args_dup = zlist_dup (self->args);
        assert (args_dup);
        return args_dup;
    }
    return NULL;
}


void
zproc_set_args (zproc_t *self, zlist_t **args_p) {
    assert (self);
    assert (*args_p);
    zlist_t *args = *args_p;
    zlist_destroy (&self->args);
    self->args = args;
    *args_p = NULL;
}

//  Setup the command line arguments, the first item must be an (absolute) filename
//  to run. Variadic function, must be NULL terminated.
void
    zproc_set_argsx (zproc_t *self, const char *args, ...)
{
    assert (self);
    va_list vargs;
    va_start (vargs, args);
    zlist_t *zargs = zlist_new ();
    zlist_autofree (zargs);
    while (args) {
        zlist_append (zargs, (void*) args);
        args = va_arg (vargs, const char *);
    }
    zproc_set_args (self, &zargs);
}

void
zproc_set_env (zproc_t *self, zhash_t **env_p) {
    assert (self);
    assert (*env_p);
    zhash_t *env = *env_p;
    zhash_destroy (&self->env);
    self->env = env;
    *env_p = NULL;
}

void
zproc_set_stdin (zproc_t *self, void* socket) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("zproc_set_stdin not implemented for Windows");
    return;
#else
    if (self->stdinpipe [0] != -1)
        return;
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
    if (self->stdoutpipe [0] != -1)
        return;
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
    if (self->stderrpipe [0] != -1)
        return;
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
    assert (zproc_pid(self));
    zproc_wait (self, 0);
    return self->return_code;
}

int
zproc_pid (zproc_t *self) {
    assert (self);
#if defined (__WINDOWS__)
    return self->piProcInfo.dwProcessId;
#else
    return self->pid;
#endif
}


static int
s_fd_in_handler (zloop_t *self, zmq_pollitem_t *item, void *socket)
{
#if defined (PIPE_BUF)
#   define BUF_SIZE PIPE_BUF
#else
#   define BUF_SIZE 65535
#endif
    byte buf [BUF_SIZE];
    ssize_t r = 1;

    memset (buf, '\0', BUF_SIZE);
    r = read (item->fd, buf, BUF_SIZE);
    if (r == -1) {
        zsys_warning ("read from fd %d: %s", item->fd, strerror (errno));
        return 0;
    }
    else
    if (r == 0)
        return 0;
    zframe_t *frame = zframe_new (buf, r);
    zsock_bsend (socket, "f", frame, NULL);
    zframe_destroy (&frame);
    return 0;
#undef BUF_SIZE
}

static int
s_fd_out_handler (zloop_t *self, zsock_t *socket, void *fd_p)
{
    assert (self);
    assert (socket);
    assert (fd_p);
    ssize_t r = 1;
    int fd = *(int*)fd_p;

    zframe_t *frame;
    r = zsock_brecv (socket, "f", &frame);
    if (r == -1) {
        zframe_destroy (&frame);
        zsys_error ("read from socket <%p>: %s", socket, strerror (errno));
        return -1;
    }

    r = write (fd, zframe_data (frame), zframe_size (frame));
    zframe_destroy (&frame);

    if (r == -1) {
        zsys_error ("write to fd %d: %s", fd, strerror (errno));
        return -1;
    }
    return 0;
}

// connect pipe (fd) with zeromq socket, so when data are signaled on `fd`, they are forwarded to `socket`
// used for readable ends of pipesm like stdout/stderr
static int
s_zproc_readfd (zproc_t *self, int fd, void* socket) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("s_zproc_readfd not implemented for Windows");
    return -1;
#else
    assert (socket);
    assert (zsock_is (socket));
    zmq_pollitem_t it = {NULL, fd, ZMQ_POLLIN, 0};
    return zloop_poller (
        self->loop_ref,
        &it,
        s_fd_in_handler,
        socket);
#endif
}

// connect zeromq `socket` with writable end of pipe. When data are signaled on `fd`, they are forwarded to `fd`
// used for writable ends of pipes like stdin
static int
s_zproc_readsocket (zproc_t *self, int* fd_p, void* socket) {
    assert (self);
#if defined (__WINDOWS__)
    zsys_error ("s_zproc_readfd not implemented for Windows");
    return -1;
#else
    assert (socket);
    assert (zsock_is (socket));
    return zloop_reader (
        self->loop_ref,
        (zsock_t*)socket,
        s_fd_out_handler,
        (void*)fd_p);
#endif
}


static int
s_zproc_alive (zloop_t *loop, int timer_id, void *args)
{
    zproc_t *self = (zproc_t*) args;
    if (! zproc_running (self))
        return 0;
#if defined (__WINDOWS__)
    if (zproc_running (self))
        return 0;
#else
    int status;
    int r = waitpid (self->pid, &status, WNOHANG);
    if (r > 0) {
        if (WIFEXITED(status)) {
            self->return_code = WEXITSTATUS(status);
            if (self->verbose)
                zsys_debug ("zproc_wait [%d]:\tWIFEXITED, self->return_code=%d", self->pid, self->return_code);
            self->running = false;
        }
        else if (WIFSIGNALED(status)) {
            self->return_code = - WTERMSIG(status);
            if (self->verbose)
                zsys_debug ("zproc_wait [%d]:\tWIFSIGNALED, self->return_code=%d", self->pid, self->return_code);
            self->running = false;
        }
        return -1;
    }
    return 0;
#endif
}

static int
s_zproc_execve (zproc_t *self)
{
    assert (self);
#if defined(__WINDOWS__)
    STARTUPINFOA siStartInfo;
    ZeroMemory (&siStartInfo, sizeof (siStartInfo));

    char *commandline = strdup ((char *)zlist_first (self->args));
    char *arg = (char *) zlist_next (self->args);
    while (arg) {
        char *tmp = zsys_sprintf ("%s %s", commandline, arg);
        zstr_free (&commandline);
        commandline = tmp;
        arg = (char *) zlist_next (self->args);
    }
    if (self->verbose)
        zsys_debug ("zproc: command to start: %s", commandline);

    siStartInfo.cb = sizeof (siStartInfo);
    self->running = CreateProcessA(
        NULL,          // app name
        commandline,   // command line
        NULL,          // process security attributes
        NULL,          // primary thread security attributes
        TRUE,         // handles are not inherited
        0,             // creation flags
        NULL,          // use parent's environment
        NULL ,         // use parent's current directory
        &siStartInfo,  // STARTUPINFO pointer
        &self->piProcInfo);  // receives PROCESS_INFORMATION
    WaitForInputIdle (self->piProcInfo.hProcess, 1000);
    if (self->verbose)
        zsys_debug ("zproc: %s", self->running ? "started" : "failed to start");
    zstr_free (&commandline);
    return 0;
#else
    int r;

    char *filename = (char*) zlist_first (self->args);
    self->pid = fork ();
    if (self->pid == 0) {
        // Child
        if (self->stdinpipe [0] != -1) {
            int o_flags = fcntl (self->stdinpipe [0], F_GETFL);
            int n_flags = o_flags & (~O_NONBLOCK);
            fcntl (self->stdinpipe [0], F_SETFL, n_flags);
            dup2 (self->stdinpipe [0], STDIN_FILENO);
            close (self->stdinpipe [1]);
            self->stdinpipe[1] = -1;
        }

        // redirect stdout if set_stdout was called
        if (self->stdoutpipe [0] != -1) {
            close (self->stdoutpipe [0]);
            self->stdoutpipe [0] = -1;
            dup2 (self->stdoutpipe [1], STDOUT_FILENO);
        }

        // redirect stderr if set_stderr was called
        if (self->stderrpipe [0] != -1) {
            close (self->stderrpipe [0]);
            self->stderrpipe [0] = -1;
            dup2 (self->stderrpipe [1], STDERR_FILENO);
        }

        // build argv for now and use self->args
        char **argv2 = arr_new (zlist_size (self->args) + 1);

        size_t i = 0;
        for (char *arg = (char*) zlist_first (self->args);
                   arg != NULL;
                   arg = (char*) zlist_next (self->args)) {
            arr_add_ref (argv2, i, arg);
            i++;
        }
        arr_add_ref (argv2, i, NULL);

        // build environ for a new process
        char **env = NULL;

        if (self->env) {
            env = arr_new (zhash_size (self->env) + 1);

            i = 0;
            for (char *arg = (char*) zhash_first (self->env);
                       arg != NULL;
                       arg = (char*) zhash_next (self->env)) {
                char *name = (char*) zhash_cursor (self->env);
                arr_add_ref (env, i, zsys_sprintf ("%s=%s", name, arg));
                i++;
            }
            arr_add_ref (env, i, NULL);
        }
        else
            env = environ;

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
        // Error (still in parent)
        zsys_error ("error fork: %s", strerror (errno));
        exit (EXIT_FAILURE);
    }
    else {
        // Parent
        if (self->verbose)
            zsys_debug ("process %s with pid %d started", filename, self->pid);

        if (self->stdinpipe [0] != -1) {
            s_zproc_readsocket (self, self->stdinpipe+1, zpair_read (self->stdinpair));
            close (self->stdinpipe [0]);
            self->stdinpipe [0] = -1;
        }

        // add a handler for read end of stdout
        if (self->stdoutpipe [1] != -1) {
            s_zproc_readfd (self, self->stdoutpipe [0], zpair_write (self->stdoutpair));
            close (self->stdoutpipe[1]);
            self->stdoutpipe [1] = -1;
        }
        // add a handler for read end of stderr
        if (self->stderrpipe [1] != -1) {
            s_zproc_readfd (self, self->stderrpipe [0], zpair_write (self->stderrpair));
            close (self->stderrpipe[1]);
            self->stderrpipe [1] = -1;
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
        zclock_sleep (10); // magic sleep, give execve a bit of time
        zsock_signal (pipe, 0);
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
    // zloop_set_verbose (loop, self->verbose);
    self->loop_ref = loop;
    self->pipe = pipe;

    zloop_reader (loop, pipe, s_pipe_handler, (void*) self);
    zloop_timer (loop, 500, 0, s_zproc_alive, (void*) self);

    zsock_signal (pipe, 0);
    zloop_start (loop);
    zloop_destroy (&loop);
    while (zproc_running (self)) {
        zclock_sleep (500);
        s_zproc_alive (NULL, -1, self);
    }
    zsock_signal (pipe, 0);
}

int
zproc_run (zproc_t *self)
{
    assert (self);
    assert (!self->actor);

    if (!self->args || zlist_size (self->args) == 0) {
        if (self->verbose)
            zsys_error ("zproc: No arguments, nothing to run. Call zproc_set_args before");
        return -1;
    }
    const char *filename = (const char*) zlist_first (self->args);
    if (!zfile_exists (filename)) {
        if (self->verbose)
            zsys_error ("zproc: '%s' does not exists", filename);
        return -1;
    }

    self->actor = zactor_new (s_zproc_actor, (void*) self);
    self->running = true;
    self->return_code = ZPROC_RUNNING;

    zstr_send (self->actor, "RUN");
    zsock_wait (self->actor);
    return 0;
}

int
zproc_wait (zproc_t *self, int timeout) {
#if defined (__WINDOWS__)
    if (!self->running) {
        if (self->verbose)
            zsys_debug ("zproc_wait: not running");
        return self->return_code;
    }

    uint32_t r = WaitForSingleObject (self->piProcInfo.hProcess, timeout == -1 ? INFINITE : timeout);
    if (self->verbose)
        zsys_debug ("zproc_wait [%d]:\twaitforsingleobject, r=%d", zproc_pid (self), r);
    if (timeout >= 0 && r == 0x00000102) {
        // still running
        return self->return_code;
    }
    if (r == 0) {
        // finished
        DWORD exitcode = 0;
        if (!GetExitCodeProcess(self->piProcInfo.hProcess, &exitcode))
            zsys_error ("zproc_wait: [%d]\tfailed to get exit code", zproc_pid (self));
        self->return_code = exitcode;
        CloseHandle (self->piProcInfo.hProcess);
        CloseHandle (self->piProcInfo.hThread);
        return self->return_code;
    }
    return -1;
#else
    assert (self);

    if (!self->pid)
        return 0;

    if (self->verbose)
        zsys_debug ("zproc_wait [%d]: timeout=%d", self->pid, timeout);
    if (self->verbose)
        zsys_debug ("zproc_wait [%d]:\t!self->running=%s", self->pid, self->running ? "true" : "false");
    if (!self->running)
        return self->return_code;

    if (self->verbose)
        zsys_debug ("zproc_wait [%d]:\twaitpid", self->pid);

    if (timeout < 0) {
        // infinite wait
        while (zproc_running (self))
            zclock_sleep (200);
        return self->return_code;
    }
    else if (timeout == 0) {
        // just check and continue
        return self->return_code;
    } else {
        // wait up to timeout
        int64_t quit = zclock_mono () + timeout;
        while (true) {
            if (! zproc_running (self))
                break;
            if (zclock_mono () >= quit)
                break;
            zclock_sleep (200);
        }
        return self->return_code;
    }
#endif
}

bool
zproc_running (zproc_t *self) {
    assert (self);
    if (! zproc_pid (self)) return false;
    return zproc_wait (self, 0) == ZPROC_RUNNING;
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
    if (zproc_pid (self)) {
        if (signum == SIGTERM) {
            if (! TerminateProcess (self->piProcInfo.hProcess, 255))
                zsys_error ("zproc_kill [%d]:\tTerminateProcess failed", zproc_pid (self));
            zproc_wait (self, 0);
        } else {
            zsys_error ("zproc_kill: [%d]:\tOnly SIGTERM is implemented on windows", zproc_pid (self));
        }
    }
#else
    if (zproc_running (self)) {
        int r = kill (self->pid, signum);
        if (r != 0)
            zsys_error ("kill of pid=%d failed: %s", self->pid, strerror (errno));
        zproc_wait (self, 0);
    }
#endif
}

//  send SIGTERM signal to the subprocess, wait for grace period and KILL
void
zproc_shutdown (zproc_t *self, int timeout)
{
    assert (self);
    if (timeout < 0) timeout = 0;

    zproc_kill (self, SIGTERM);
    zproc_wait (self, timeout);
#if ! defined (__WINDOWS__)
    if (zproc_running (self)) {
        zproc_kill (self, SIGKILL);
        zproc_wait (self, timeout);
    }
#endif
}

void
zproc_set_verbose (zproc_t *self, bool verbose) {
    assert (self);
    self->verbose = verbose;
}

//  --------------------------------------------------------------------------
//  Self test of this class.

void
zproc_test (bool verbose)
{
    printf (" * zproc: ");

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

    //  0. initialization

    if (verbose) {
        printf("\n");
    }

#ifdef __WINDOWS__
#  define ZSPBIN "zsp.exe"
#else
#  define ZSPBIN "zsp"
#endif
    //  find the right binary for current build (in-tree, distcheck, etc.)
    char *file = NULL;
    if (zsys_file_exists ("src/" ZSPBIN) || zsys_file_exists ("./src/" ZSPBIN))
        file = "./src/" ZSPBIN;
    else
    if (zsys_file_exists ("../" ZSPBIN))
    //  WHOA: zproc: zproc_test() : current working directory is
    //      /home/travis/build/username/czmq/czmq-4.0.3/_build/src/selftest-rw
        file = "../zsp";
    else
    if (zsys_file_exists ("_build/../src/" ZSPBIN))
        file = "_build/../src/" ZSPBIN;
    else
    if (zsys_file_exists ("_build/src/" ZSPBIN))
        file = "_build/src/" ZSPBIN;
    else
    if (zsys_file_exists ("../_build/src/" ZSPBIN))
        file = "../_build/src/" ZSPBIN;
    else
    if (zsys_file_exists ("../../_build/src/" ZSPBIN))
        file = "../../_build/src/" ZSPBIN;
    else
    if (zsys_file_exists ("_build/sub/src/" ZSPBIN))
        file = "_build/sub/src/" ZSPBIN;
    else
    if (zsys_file_exists ("../_build/sub/src/" ZSPBIN))
        file = "../_build/sub/src/" ZSPBIN;
    else
    if (zsys_file_exists ("../../_build/sub/src/" ZSPBIN))
        file = "../../_build/sub/src/" ZSPBIN;
    else
    if (zsys_file_exists ("" ZSPBIN) || zsys_file_exists ("./" ZSPBIN))
        file = "./" ZSPBIN;
    else
    if (zsys_file_exists ("../src/" ZSPBIN))
        file = "../src/" ZSPBIN;

    if (file == NULL || !zsys_file_exists (file)) {
        zsys_warning ("cannot detect zsp binary, %s does not exist", file ? file : "<null>");

        printf ("SKIPPED (zsp helper not found)\n");
        char cwd [PATH_MAX];
        memset (cwd, 0, sizeof (cwd));
        if (getcwd(cwd, sizeof(cwd)) != NULL)
            printf ("zproc_test() : current working directory is %s\n", cwd);

        return;
    }
    if (verbose) {
        zsys_info ("zproc_test() : detected a zsp binary at %s\n", file);
    }

    //  @selftest

    //  variable file contains path to zsp executable:
    //  char *file = "path/to/zsp";

#if defined (__WINDOWS__)
    printf ("Very limited (on Windows) ");
    {
        zsys_init ();
        zproc_t *self = zproc_new ();
        assert (self);

        zproc_set_verbose (self, verbose);
        zproc_set_argsx (self, file, "-v", NULL);
        zproc_run (self);
        zclock_sleep (100); // to let actor start the process
        assert (zproc_pid (self));

        zproc_kill (self, SIGTERM);
        assert (zproc_returncode (self) == 255);
        zproc_destroy (&self);
    }
    printf ("OK\n");
    return;
#endif
    {
    // Test case #1: run command, wait until it ends and get the (stdandard) output
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);

    //  join stdout of the process to zeromq socket
    //  all data will be readable from zproc_stdout socket
    assert (!zproc_stdout (self));
    zproc_set_stdout (self, NULL);
    assert (zproc_stdout (self));

    zproc_set_argsx (self, file, "--help", NULL);

    if (verbose)
        zsys_debug("zproc_test() : launching helper '%s' --help", file );

    int r = zproc_run (self);
    assert (r == 0);
    zframe_t *frame;
    zsock_brecv (zproc_stdout (self), "f", &frame);
    assert (frame);
    assert (zframe_data (frame));
    // TODO: real test
    if (verbose)
        zframe_print (frame, "1:");
    zframe_destroy (&frame);
    r = zproc_wait (self, -1);
    assert (r == 0);
    zproc_destroy (&self);
    }

    {
    // Test case#2: run zsp helper with a content written on stdin, check if it was passed to stdout
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);
    //  forward input from stdin to stderr
    zproc_set_argsx (self, file, "--stdin", "--stderr", NULL);
    // FIXME: there is a BUG in zproc somewhere, you can't gen an output from both stdout/stderr
    //zproc_set_argsx (self, file, "--stdin", "--stdout", "--stderr", NULL);
    zproc_set_stdin (self, NULL);
    // FIXME: the bug
    //zproc_set_stdout (self, NULL);
    zproc_set_stderr (self, NULL);

    // send data to stdin
    int r = zproc_run (self);
    assert (r == 0);
    zframe_t *frame = zframe_new ("Lorem ipsum\0\0", strlen ("Lorem ipsum")+2);
    assert (frame);
    zsock_bsend (zproc_stdin (self), "f", frame);
    zframe_destroy (&frame);

    // FIXME: the bug
    //zproc_set_stdout (self, NULL);
    // read data from stdout
    /*
    zsys_debug ("BAF1");
    zsock_brecv (zproc_stdout (self), "f", &frame);
    zsys_debug ("BAF2");
    assert (frame);
    assert (zframe_data (frame));
    if (verbose)
        zframe_print (frame, "2.stdout:");
    assert (!strncmp ((char*) zframe_data (frame), "Lorem ipsum", 11));
    */

    // read data from stderr
    zsock_brecv (zproc_stderr (self), "f", &frame);
    assert (frame);
    assert (zframe_data (frame));
    if (verbose)
        zframe_print (frame, "2.stderr:");
    assert (!strncmp ((char*) zframe_data (frame), "Lorem ipsum", 11));
    zproc_kill (self, SIGTERM);
    zproc_wait (self, -1);
    zframe_destroy (&frame);
    zproc_destroy (&self);
    }

    {
    // Test case#3: run non existing binary
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);
    //  forward input from stdin to stderr
    zproc_set_argsx (self, "/not/existing/file", NULL);

    int r = zproc_run (self);
    assert (r == -1);
    zproc_destroy (&self);
    }

    {
    // Test case #4: child abort itself
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);
    zproc_set_argsx (self, file, "--verbose", "--abrt", NULL);
    zproc_set_stdout (self, NULL);
    zproc_set_stderr (self, NULL);
    zproc_set_stdin (self, NULL);

    int r = zproc_run (self);
    zclock_sleep (100); // to let actor start the process
    assert (r != -1);
    zclock_sleep (100);
    zframe_t *frame;
    zsock_brecv (zproc_stdout (self), "f", &frame);
    assert (zframe_is (frame));
    assert (zframe_size (frame) > 0);
    zframe_destroy (&frame);
    zproc_wait (self, -1);
    assert (zproc_returncode (self) == -SIGABRT);
    zproc_destroy (&self);
    }

    {
    // Test case #5: use never ending subprocess and poller to read data from it
    //  Create new zproc instance
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);
    //  join stdout of the process to zeromq socket
    //  all data will be readable from zproc_stdout socket
    zproc_set_stdout (self, NULL);

    zlist_t *args = zlist_new ();
    zlist_autofree (args);
    zlist_append (args, file);
    zlist_append (args, "--stdout");
    zproc_set_args (self, &args);

    zhash_t *env = zhash_new ();
    zhash_autofree (env);
    zhash_insert (env, "ZSP_MESSAGE", "czmq is great\n");
    zproc_set_env (self, &env);

    // execute the binary. It runs in own actor, which monitor the process and
    // pass data across pipes and zeromq sockets
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
    zproc_wait (self, -1);

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
    }
    {
    // testcase #6 wait for process that hangs, kill it
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);

    zproc_set_argsx (self, file, NULL);

    if (verbose)
        zsys_debug("zproc_test() : launching helper '%s'", file);

    int r = zproc_run (self);
    assert (r == 0);
    r = zproc_wait (self, 1000);
    assert (r == ZPROC_RUNNING);
    assert (zproc_running (self));
    zproc_shutdown (self, 1000);
    assert (!zproc_running (self));
    zproc_destroy (&self);
    }
    {
    // testcase #7 wait for process that exits earlier
    zproc_t *self = zproc_new ();
    assert (self);
    zproc_set_verbose (self, verbose);

    zproc_set_argsx (self, file, "--quit", "1", NULL);

    if (verbose)
        zsys_debug("zproc_test() : launching helper '%s' --quit 1", file);

    int r = zproc_run (self);
    assert (r == 0);
    int t = zclock_mono ();
    r = zproc_wait (self, 8000);
    assert (r == 0);
    t = zclock_mono () - t;
    assert (t < 2000);
    zproc_destroy (&self);
    }
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
