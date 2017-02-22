/*  =========================================================================
    zsubproc - class description

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
    zsubproc - czmq based sub process supervisor
@discuss
@end
*/

#include "czmq_classes.h"

#define ZPROC_RUNNING -42

typedef struct _zpair_t zpair_t;

struct _zpair_t {
    char *endpoint;
    bool write_owned;
    void *write;
    bool read_owned;
    void *read;
};

zpair_t*
zpair_new (char* endpoint) {
    zpair_t *self = (zpair_t*) zmalloc (sizeof (zpair_t));
    self->endpoint = endpoint;
    return self;
}

void
zpair_destroy (zpair_t **self_p) {
    assert (self_p);
    if (*self_p) {
        zpair_t *self = *self_p;
        if (self->write_owned)
            zsock_destroy ((zsock_t**)&self->write);
        if (self->read_owned)
            zsock_destroy ((zsock_t**)&self->read);
        zstr_free (&self->endpoint);
        free (self);
        *self_p = NULL;
    }
}

void
zpair_set_read (zpair_t *self, void *sock, bool owned) {
    self->read = sock;
    self->read_owned = owned;
}

void
zpair_set_write (zpair_t *self, void *sock, bool owned) {
    self->write = sock;
    self->write_owned = owned;
}

void *
zpair_read (zpair_t *self) {
    return self->read;
}

void *
zpair_write (zpair_t *self) {
    return self->write;
}

void
zpair_mkpair (zpair_t *self) {

    zsys_debug ("zpair_mkpair: endpoint=%s", self->endpoint);

    self->endpoint[0] = '>';
    self->read = zsock_new_pair (self->endpoint);
    self->read_owned = true;
    self->endpoint [0] = '@';
    self->write = zsock_new_pair (self->endpoint);
    self->write_owned = true;
    self->endpoint [0] = '#';
}

struct _zsubproc_t {
    pid_t pid;
    int return_code;
    bool running;

    zactor_t *actor;
    zloop_t *loop_ref;
    zsock_t *pipe;          // destroy actor pipe in a case of execve fail
    int stdinpipe [2];      // stdin pipe
    int stdoutpipe [2];     // stdout pipe
    int stderrpipe [2];     // stderr pipe

    zpair_t *stdinpair;     // stdin socketpair
    zpair_t *stdoutpair;    // stdout socketpair
    zpair_t *stderrpair;    // stderr socketpair
};

zsubproc_t*
zsubproc_new ()
{
    zsubproc_t *self = (zsubproc_t*) zmalloc (sizeof (zsubproc_t));
    self->pid = 0;

    zuuid_t *uuid = zuuid_new ();
    self->stdinpair = zpair_new (
        zsys_sprintf ("#inproc://zsubproc-%s-stdin", zuuid_str_canonical (uuid))
    );
    self->stdoutpair = zpair_new (
        zsys_sprintf ("#inproc://zsubproc-%s-stdout", zuuid_str_canonical (uuid))
    );
    self->stderrpair = zpair_new (
        zsys_sprintf ("#inproc://zsubproc-%s-stderr", zuuid_str_canonical (uuid))
    );
    zuuid_destroy (&uuid);

    return self;
}

void
zsubproc_destroy (zsubproc_t **self_p) {
    assert (self_p);
    if (*self_p) {
        zsubproc_t *self = *self_p;
        zsubproc_wait (self, true);
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

        free (self);
        *self_p = NULL;
    }
}

void
zsubproc_set_stdin (zsubproc_t *self, void* socket) {
    assert (self);
    assert (self->stdinpipe [0] == 0);
    int r = pipe (self->stdinpipe);
    assert (r == 0);

    if (!socket)
        zpair_mkpair (self->stdinpair);
    else
        zpair_set_read (self->stdinpair, socket, false);
}

void
zsubproc_set_stdout (zsubproc_t *self, void* socket) {
    assert (self);
    assert (self->stdoutpipe [0] == 0);
    int r = pipe (self->stdoutpipe);
    assert (r == 0);

    if (!socket)
        zpair_mkpair (self->stdoutpair);
    else
        zpair_set_write (self->stdoutpair, socket, false);
}

void
zsubproc_set_stderr (zsubproc_t *self, void* socket) {
    assert (self);
    assert (self->stderrpipe [0] == 0);
    int r = pipe (self->stderrpipe);
    assert (r == 0);

    if (!socket)
        zpair_mkpair (self->stderrpair);
    else
        zpair_set_write (self->stderrpair, socket, false);
}

void*
zsubproc_stdin (zsubproc_t *self) {
    return zpair_write (self->stdinpair);
}

void*
zsubproc_stdout (zsubproc_t *self) {
    return zpair_read (self->stdoutpair);
}

void*
zsubproc_stderr (zsubproc_t *self) {
    return zpair_read (self->stderrpair);
}

int
zsubproc_returncode (zsubproc_t *self) {
    assert (self);
    assert (self->pid);

    return self->return_code;
}

int
zsubproc_pid (zsubproc_t *self) {
    assert (self);

    return self->pid;
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
        free (*(self++));
    }
    free (foo);
}

static void
arr_add_ref (char **self, size_t i, char *s) {
    assert (self);
    self [i] = s;
}

/*
static void
arr_print (char**self) {
    assert (self);
    size_t i = 0;
    while (self [i]) {
        zsys_debug ("%zu:\t%s<%p>", i, self [i], (void*) *self);
        i += 1;
    }
}
*/

static size_t
arr_size (char **self) {
    if (!self)
        return 0;
    char **foo = self;
    size_t size = 0;
    while (*foo) {
        size ++;
        foo ++;
    }
    return size;
}

static int
s_fd_in_handler (zloop_t *self, zmq_pollitem_t *item, void *socket)
{
    static size_t BUF_SIZE = 1024;
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
s_zsubproc_addfd (zsubproc_t *self, int fd, void* socket, int flags) {
    assert (self);
    zmq_pollitem_t it = {NULL, fd, flags, 0};
    return zloop_poller (
        self->loop_ref,
        &it,
        flags == ZMQ_POLLIN ? s_fd_in_handler : s_fd_out_handler,
        socket);
}



static int
s_zsubproc_alive (zloop_t *loop, int timer_id, void *args)
{
    zsubproc_t *self = (zsubproc_t*) args;
    if (zsys_interrupted)
        return -1;
    if (zsubproc_pid (self) && zsubproc_running (self))
        return 0;
    return -1;
}

static int
s_zsubproc_execve (
    zsubproc_t *self,
    char *filename,
    char **argv,
    char **env)
{
    assert (self);
    int r;

    self->pid = fork ();
    if (self->pid == 0) {

        if (self->stdinpipe [0] != 0) {
            int o_flags = fcntl (self->stdinpipe[0], F_GETFL);
            int n_flags = o_flags & (~O_NONBLOCK);
            fcntl (self->stdinpipe[0], F_SETFL, n_flags);
            dup2 (self->stdinpipe[0], STDIN_FILENO);
            close (self->stdinpipe[1]);
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

        r = execve (filename, argv, env);
        if (r == -1) {
            zsys_error ("fail to run %s: %s", filename, strerror (errno));
            zsubproc_destroy (&self);
            zsock_destroy (&self->pipe);
            exit (errno);
        }
    }
    else
    if (self->pid == -1) {
        zsys_error ("error fork: %s", strerror (errno));
        exit (EXIT_FAILURE);
    }
    else {
        zsys_info ("process %s with pid %d started", filename, self->pid);

        if (self->stdinpipe [0] != 0) {
            s_zsubproc_addfd (self, self->stdinpipe [1], zpair_read (self->stdinpair), ZMQ_POLLOUT);
            close (self->stdinpipe [0]);
        }

        // add a handler for read end of stdout
        if (self->stdoutpipe [1] != 0) {
            s_zsubproc_addfd (self, self->stdoutpipe [0], zpair_write (self->stdoutpair), ZMQ_POLLIN);
            close(self->stdoutpipe[1]);
        }
        // add a handler for read end of stderr
        if (self->stderrpipe [1] != 0) {
            s_zsubproc_addfd (self, self->stderrpipe [0], zpair_write (self->stderrpair), ZMQ_POLLIN);
            close(self->stderrpipe[1]);
        }
    }

    return 0;
}

static int
s_pipe_handler (zloop_t *loop, zsock_t *pipe, void *args) {
    zsubproc_t *self = (zsubproc_t*) args;

    int ret = 0;

    zmsg_t *msg = zmsg_recv (pipe);
    char *command = zmsg_popstr (msg);
    zsys_debug ("API command=%s", command);

    if (streq (command, "$TERM"))
        ret = -1;
    else
    if (streq (command, "RUN")) {

        if (self->pid != 0) {
            zsys_error ("Can't run command twice!!");
            goto end;
        }

        char *filename = zmsg_popstr (msg);
        char *argca = zmsg_popstr (msg);
        int argc = atoi (argca);

        char **argv = arr_new (argc);

        for (int i = 0; i != argc; i++)
            arr_add_ref (argv, i, zmsg_popstr (msg));

        char *envca = zmsg_popstr (msg);
        char **env = NULL;
        if (envca) {
            int envc = atoi (envca);
            env = arr_new (envc);

            for (int i = 0; i != envc; i++)
                arr_add_ref (env, i, zmsg_popstr (msg));
        }

        s_zsubproc_execve (self, filename, argv, env);

        arr_free (env);
        arr_free (argv);
        zstr_free (&envca);
        zstr_free (&argca);
        zstr_free (&filename);
    }

end:
    zstr_free (&command);
    zmsg_destroy (&msg);

    return ret;
}

static void
s_zsubproc_actor (zsock_t *pipe, void *args)
{

    zsubproc_t *self = (zsubproc_t*) args;
    zloop_t *loop = zloop_new ();
    assert (loop);
    self->loop_ref = loop;
    self->pipe = pipe;

    zloop_reader (loop, pipe, s_pipe_handler, (void*) self);
    zloop_timer (loop, 500, 0, s_zsubproc_alive, (void*) self);

    zsock_signal (pipe, 0);
    zloop_start (loop);
    zloop_destroy (&loop);
    zsock_signal (pipe, 0);
}

int
zsubproc_run (zsubproc_t *self, const char *filename, char *const argv[], char *const envp[])
{
#if defined (__WINDOWS__)
    zsys_error ("zsubproc not yet implemented for Windows");
    assert (false);
#endif
    assert (self);
    assert (!self->actor);

    self->actor = zactor_new (s_zsubproc_actor, (void*) self);
    self->running = true;
    zmsg_t *msg = zmsg_new ();
    zmsg_addstr (msg, "RUN");
    zmsg_addstr (msg, filename);

    char *asize = zsys_sprintf ("%zu", arr_size ((char**)argv));
    zmsg_addstr (msg, asize);
    if (asize [0] != '0') {
        while (*argv) {
            zmsg_addstr (msg, *(argv++));
        }
    }

    char *esize = zsys_sprintf ("%zu", arr_size ((char**)envp));
    zmsg_addstr (msg, esize);
    if (esize [0] != '0') {
        while (*envp) {
            zmsg_addstr (msg, *(envp++));
        }
    }

    zmsg_print (msg);
    zmsg_send (&msg, self->actor);
    zstr_free (&asize);
    zstr_free (&esize);
    self->return_code = ZPROC_RUNNING;
    return 0;
}

int
zsubproc_wait (zsubproc_t *self, bool wait) {
#if defined (__WINDOWS__)
    zsys_error ("zsubproc not yet implemented for Windows");
    assert (false);
#endif
    assert (self);
    if (!self->pid)
        return 0;

    int status = -1;
    int options = !wait ? WNOHANG : 0;
    if (!self->running)
        return self->return_code;

    int r = waitpid (self->pid, &status, options);
    if (!wait && r == 0)
        return self->return_code;

    if (WIFEXITED(status)) {
        self->running = false;
        self->return_code = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status)) {
        self->running = false;
        self->return_code = - WTERMSIG(status);

        /*
        if (WCOREDUMP(status)) {
            self->core_dumped = true;
        }
        */
    }
    return ZPROC_RUNNING;
}

bool
zsubproc_running (zsubproc_t *self) {
    assert (self);
    assert (self->pid);

    return zsubproc_wait (self, false) == ZPROC_RUNNING;
}

void *
zsubproc_actor (zsubproc_t *self) {
    assert (self);
    return self->actor;
}

//  --------------------------------------------------------------------------
//  Self test of this class

void
zsubproc_test (bool verbose)
{
    printf (" * zsubproc: ");

    //  @selftest
    //  Simple create/destroy test
    zsubproc_t *self = zsubproc_new ();
    assert (self);
    zsubproc_set_stdout (self, NULL);

    char *const xargv[] = {"cat", "/etc/passwd", NULL};
    char *const xenvp[] = {"PATH=/bin/:/sbin/:/usr/bin/:/usr/sbin", NULL};

    zsubproc_run (self, "cat", xargv, xenvp);

    zpoller_t *poller = zpoller_new (zsubproc_actor (self), zsubproc_stdout (self), NULL);

    while (!zsys_interrupted) {
        void *which = zpoller_wait (poller, -1);

        if (which == zsubproc_actor (self)) {
            break;
        }

        if (which == zsubproc_stdout (self)) {
            zframe_t *frame;
            zsock_brecv (zsubproc_stdout (self), "f", &frame);
            write (STDERR_FILENO, zframe_data (frame), zframe_size (frame));
            zframe_destroy (&frame);
        }
    }

    zsys_info ("Process %d exited with %d", zsubproc_pid (self), zsubproc_returncode (self));

    zsubproc_destroy (&self);
    //  @end
    printf ("OK\n");
}
