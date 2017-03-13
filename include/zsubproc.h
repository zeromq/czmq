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

#ifndef ZSUBPROC_H_INCLUDED
#define ZSUBPROC_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
//  Create a new zsubproc
CZMQ_EXPORT zsubproc_t *
    zsubproc_new (void);

//  Destroy the zsubproc, wait until process ends
CZMQ_EXPORT void
    zsubproc_destroy (zsubproc_t **self_p);

//  Set the output socket for subprocess stdin, if NULL
//  create internal EXCLPAIR socket. It must be possible
//  to recv messages from the socket. External sockets
//  are not destroy'ed by zsubproc instance.
CZMQ_EXPORT void
    zsubproc_set_stdin (zsubproc_t *self, void* socket);

//  Set the output socket for subprocess stdin, if NULL
//  create internal EXCLPAIR socket. It must be possible
//  to send messages to the socket. External sockets
//  are not destroy'ed by zsubproc instance.
CZMQ_EXPORT void
    zsubproc_set_stdout (zsubproc_t *self, void* socket);

//  Set the output socket for subprocess stderr, if NULL
//  create internal EXCLPAIR socket. It must be possible
//  to send messages to the socket. External sockets
//  are not destroy'ed by zsubproc instance.
CZMQ_EXPORT void
    zsubproc_set_stderr (zsubproc_t *self, void* socket);

//  Return subprocess stdin writable socket. NULL for
//  not initialized or external sockets.
CZMQ_EXPORT void*
    zsubproc_stdin (zsubproc_t *self);

//  Return subprocess stdout readable socket. NULL for
//  not initialized or external sockets.
CZMQ_EXPORT void*
    zsubproc_stdout (zsubproc_t *self);

//  Return subprocess stderr readable socket. NULL for
//  not initialized or external sockets.
CZMQ_EXPORT void*
    zsubproc_stderr (zsubproc_t *self);

//  subprocess exit code
CZMQ_EXPORT int
    zsubproc_returncode (zsubproc_t *self);

//  pid of a subprocess, 0 if not yet started
CZMQ_EXPORT int
    zsubproc_pid (zsubproc_t *self);

//  return true if process is running, false if not yet started or finished
CZMQ_EXPORT bool
    zsubproc_running (zsubproc_t *self);

//  run the subprocess
CZMQ_EXPORT int
    zsubproc_run (zsubproc_t *self, const char *filename, char *const argv[], char *const envp[]);

//  wait or poll process status
CZMQ_EXPORT int
    zsubproc_wait (zsubproc_t *self, bool wait);

//  return internal actor
CZMQ_EXPORT void *
    zsubproc_actor (zsubproc_t *self);

//  send a signal to the subprocess
CZMQ_EXPORT void
    zsubproc_kill (zsubproc_t *self, int signum);

//  set verbose or quiet mode
CZMQ_EXPORT void
    zsubproc_set_verbose (zsubproc_t *self, bool verbose);

//  Self test of this class
CZMQ_EXPORT void
    zsubproc_test (bool verbose);

//  @end

#ifdef __cplusplus
}
#endif

#endif
