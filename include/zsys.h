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

#ifndef __ZSYS_H_INCLUDED__
#define __ZSYS_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  @interface
#define UDP_FRAME_MAX   255         //  Max size of UDP frame

//  Callback for interrupt signal handler
typedef void (zsys_handler_fn) (int signal_value);

//  Get a new ZMQ socket, automagically creating a ZMQ context if this is
//  the first time. Caller is responsible for destroying the ZMQ socket
//  before process exits, to avoid a ZMQ deadlock. Note: you should not use
//  this method in CZMQ apps, use zsock_new() instead.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT void *
    zsys_socket (int type, const char *filename, size_t line_nbr);

//  Destroy/close a ZMQ socket. You should call this for every socket you
//  create using zsys_socket().
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT int
    zsys_close (void *handle);
    
//  Set interrupt handler (NULL means external handler)
CZMQ_EXPORT void
    zsys_handler_set (zsys_handler_fn *handler_fn);

//  Reset interrupt handler, call this at exit if needed
CZMQ_EXPORT void
    zsys_handler_reset (void);

//  Set default interrupt handler, so Ctrl-C or SIGTERM will set
//  zsys_interrupted. Idempotent; safe to call multiple times.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT void
    zsys_catch_interrupts (void);

//  Return 1 if file exists, else zero
CZMQ_EXPORT bool
    zsys_file_exists (const char *filename);

//  Return size of file, or -1 if not found
CZMQ_EXPORT ssize_t
    zsys_file_size (const char *filename);

//  Return file modification time. Returns 0 if the file does not exist.
CZMQ_EXPORT time_t
    zsys_file_modified (const char *filename);

//  Return file mode; provides at least support for the POSIX S_ISREG(m)
//  and S_ISDIR(m) macros and the S_IRUSR and S_IWUSR bits, on all boxes.
CZMQ_EXPORT mode_t
    zsys_file_mode (const char *filename);

//  Delete file. Does not complain if the file is absent
CZMQ_EXPORT int
    zsys_file_delete (const char *filename);

//  Check if file is 'stable'
CZMQ_EXPORT bool
    zsys_file_stable (const char *filename);

//  Create a file path if it doesn't exist. The file path is treated as a 
//  printf format.
CZMQ_EXPORT int
    zsys_dir_create (const char *pathname, ...);

//  Remove a file path if empty; the pathname is treated as printf format.
CZMQ_EXPORT int
    zsys_dir_delete (const char *pathname, ...);

//  Set private file creation mode; all files created from here will be
//  readable/writable by the owner only.
CZMQ_EXPORT void
    zsys_file_mode_private (void);

//  Reset default file creation mode; all files created from here will use
//  process file mode defaults.
CZMQ_EXPORT void
    zsys_file_mode_default (void);

//  Return the CZMQ version for run-time API detection
CZMQ_EXPORT void
    zsys_version (int *major, int *minor, int *patch);

//  Format a string using printf formatting, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().
CZMQ_EXPORT char *
    zsys_sprintf (const char *format, ...);

//  Format a string with a va_list argument, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().
CZMQ_EXPORT char *
    zsys_vprintf (const char *format, va_list argptr);

//  Create UDP beacon socket; if the routable option is true, uses
//  multicast (not yet implemented), else uses broadcast. This method
//  and related ones might _eventually_ be moved to a zudp class.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT SOCKET
    zsys_udp_new (bool routable);

//  Close a UDP socket
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT int
    zsys_udp_close (SOCKET handle);

//  Send zframe to UDP socket
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT void
    zsys_udp_send (SOCKET udpsock, zframe_t *frame, inaddr_t *address);

//  Receive zframe from UDP socket, and set address of peer that sent it
//  The peername must be a char [INET_ADDRSTRLEN] array.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT zframe_t *
    zsys_udp_recv (SOCKET udpsock, char *peername);

//  Handle an I/O error on some socket operation; will report and die on
//  fatal errors, and continue silently on "try again" errors.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
CZMQ_EXPORT void
    zsys_socket_error (const char *reason);

//  Return current host name, for use in public tcp:// endpoints. Caller gets
//  a freshly allocated string, should free it using zstr_free().
CZMQ_EXPORT char *
    zsys_hostname (void);

//  Move the current process into the background. The precise effect depends
//  on the operating system. On POSIX boxes, moves to a specified working
//  directory (if specified), closes all file handles, reopens stdin, stdout,
//  and stderr to the null device, and sets the process to ignore SIGHUP. On
//  Windows, does nothing. Returns 0 if OK, -1 if there was an error.
CZMQ_EXPORT int
    zsys_daemonize (const char *workdir);

//  Drop the process ID into the lockfile, with exclusive lock, and switch
//  the process to the specified group and/or user. Any of the arguments
//  may be null, indicating a no-op. Returns 0 on success, -1 on failure.
//  Note if you combine this with zsys_daemonize, run after, not before
//  that method, or the lockfile will hold the wrong process ID.
CZMQ_EXPORT int
    zsys_run_as (const char *lockfile, const char *group, const char *user);

//  Configure the number of I/O threads that ZeroMQ will use. A good
//  rule of thumb is one thread per gigabit of traffic in or out. The
//  default is 1, sufficient for most applications. If the environment
//  variable ZSYS_IO_THREADS is defined, that provides the default.
//  Note that this method is valid only before any socket is created.
CZMQ_EXPORT void
    zsys_set_io_threads (size_t io_threads);

//  Configure the number of sockets that ZeroMQ will allow. The default
//  is 1024. The actual limit depends on the system, and you can query it
//  by using zsys_socket_limit (). A value of zero means "maximum".
//  Note that this method is valid only before any socket is created.
CZMQ_EXPORT void
    zsys_set_max_sockets (size_t max_sockets);

//  Return maximum number of ZeroMQ sockets that the system will support.
CZMQ_EXPORT size_t
    zsys_socket_limit (void);

//  Configure the default linger timeout in msecs for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  linger time is zero, i.e. any pending messages will be dropped. If the
//  environment variable ZSYS_LINGER is defined, that provides the default.
//  Note that process exit will typically be delayed by the linger time.
CZMQ_EXPORT void
    zsys_set_linger (size_t linger);

//  Configure the default outgoing pipe limit (HWM) for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
//  ZSYS_SNDHWM is defined, that provides the default. Note that a value of
//  zero means no limit, i.e. infinite memory consumption.
CZMQ_EXPORT void
    zsys_set_sndhwm (size_t sndhwm);

//  Configure the default incoming pipe limit (HWM) for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
//  ZSYS_RCVHWM is defined, that provides the default. Note that a value of
//  zero means no limit, i.e. infinite memory consumption.
CZMQ_EXPORT void
    zsys_set_rcvhwm (size_t rcvhwm);

//  Configure use of IPv6 for new zsock instances. By default sockets accept
//  and make only IPv4 connections. When you enable IPv6, sockets will accept
//  and connect to both IPv4 and IPv6 peers. You can override the setting on
//  each zsock_t instance. The default is IPv4 only (ipv6 set to 0). If the
//  environment variable ZSYS_IPV6 is defined (as 1 or 0), this provides the
//  default. Note: has no effect on ZMQ v2.
CZMQ_EXPORT void
    zsys_set_ipv6 (int ipv6);

//  Set network interface name to use for broadcasts, particularly zbeacon.
//  This lets the interface be configured for test environments where required.
//  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
//  the default when there is no specified interface. If the environment
//  variable ZSYS_INTERFACE is set, use that as the default interface name.
CZMQ_EXPORT void
    zsys_set_interface (const char *value);

//  Return network interface to use for broadcasts, or "" if none was set.
CZMQ_EXPORT const char *
    zsys_interface (void);

//  Set log identity, which is a string that prefixes all log messages sent
//  by this process. The log identity defaults to the environment variable
//  ZSYS_LOGIDENT, if that is set.
CZMQ_EXPORT void
    zsys_set_logident (const char *value);

//  Set stream to receive log traffic. By default log traffic is sent to the
//  system logging facility (syslog on POSIX, event log on Windows). When you
//  set the logstream to an open file stream (typically stdout or stderr),
//  log traffic goes here instead. If stream is NULL, traffic is sent to the
//  system logging facility (as by default).
CZMQ_EXPORT void
    zsys_set_logstream (FILE *stream);
    
//  Log error condition - highest priority
CZMQ_EXPORT void
    zsys_error (const char *format, ...);

//  Log warning condition - high priority
CZMQ_EXPORT void
    zsys_warning (const char *format, ...);
    
//  Log normal, but significant, condition - normal priority
CZMQ_EXPORT void
    zsys_notice (const char *format, ...);
    
//  Log informational message - low priority
CZMQ_EXPORT void
    zsys_info (const char *format, ...);
    
//  Log debug-level message - lowest priority
CZMQ_EXPORT void
    zsys_debug (const char *format, ...);

//  Self test of this class
CZMQ_EXPORT void
    zsys_test (bool verbose);
    
//  Global signal indicator, TRUE when user presses Ctrl-C or the process
//  gets a SIGTERM signal.
CZMQ_EXPORT extern volatile int zsys_interrupted;
//  Deprecated name for this variable
CZMQ_EXPORT extern volatile int zctx_interrupted;
//  @end

#ifdef __cplusplus
}
#endif

#endif
