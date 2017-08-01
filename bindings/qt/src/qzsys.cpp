/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/

#include "qczmq.h"

///
//  Copy-construct to return the proper wrapped c types
QZsys::QZsys (zsys_t *self, QObject *qObjParent) : QObject (qObjParent)
{
    this->self = self;
}


///
//  Initialize CZMQ zsys layer; this happens automatically when you create
//  a socket or an actor; however this call lets you force initialization
//  earlier, so e.g. logging is properly set-up before you start working.
//  Not threadsafe, so call only from main thread. Safe to call multiple
//  times. Returns global CZMQ context.
void * QZsys::init ()
{
    void * rv = zsys_init ();
    return rv;
}

///
//  Optionally shut down the CZMQ zsys layer; this normally happens automatically
//  when the process exits; however this call lets you force a shutdown
//  earlier, avoiding any potential problems with atexit() ordering, especially
//  with Windows dlls.
void QZsys::shutdown ()
{
    zsys_shutdown ();

}

///
//  Get a new ZMQ socket, automagically creating a ZMQ context if this is
//  the first time. Caller is responsible for destroying the ZMQ socket
//  before process exits, to avoid a ZMQ deadlock. Note: you should not use
//  this method in CZMQ apps, use zsock_new() instead.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
void * QZsys::socket (int type, const QString &filename, size_t lineNbr)
{
    void * rv = zsys_socket (type, filename.toUtf8().data(), lineNbr);
    return rv;
}

///
//  Destroy/close a ZMQ socket. You should call this for every socket you
//  create using zsys_socket().
//  *** This is for CZMQ internal use only and may change arbitrarily ***
int QZsys::close (void *handle, const QString &filename, size_t lineNbr)
{
    int rv = zsys_close (handle, filename.toUtf8().data(), lineNbr);
    return rv;
}

///
//  Return ZMQ socket name for socket type
//  *** This is for CZMQ internal use only and may change arbitrarily ***
const QString QZsys::sockname (int socktype)
{
    const QString rv = QString (zsys_sockname (socktype));
    return rv;
}

///
//  Create a pipe, which consists of two PAIR sockets connected over inproc.
//  The pipe is configured to use the zsys_pipehwm setting. Returns the
//  frontend socket successful, NULL if failed.
QZsock * QZsys::createPipe (QZsock *backendP)
{
    QZsock *rv = new QZsock (zsys_create_pipe (&backendP->self));
    return rv;
}

///
//  Set interrupt handler; this saves the default handlers so that a
//  zsys_handler_reset () can restore them. If you call this multiple times
//  then the last handler will take affect. If handler_fn is NULL, disables
//  default SIGINT/SIGTERM handling in CZMQ.
void QZsys::handlerSet (zsys_handler_fn *handlerFn)
{
    zsys_handler_set (handlerFn);

}

///
//  Reset interrupt handler, call this at exit if needed
void QZsys::handlerReset ()
{
    zsys_handler_reset ();

}

///
//  Set default interrupt handler, so Ctrl-C or SIGTERM will set
//  zsys_interrupted. Idempotent; safe to call multiple times.
//  Can be supressed by ZSYS_SIGHANDLER=false
//  *** This is for CZMQ internal use only and may change arbitrarily ***
void QZsys::catchInterrupts ()
{
    zsys_catch_interrupts ();

}

///
//  Return 1 if file exists, else zero
bool QZsys::fileExists (const QString &filename)
{
    bool rv = zsys_file_exists (filename.toUtf8().data());
    return rv;
}

///
//  Return file modification time. Returns 0 if the file does not exist.
time_t QZsys::fileModified (const QString &filename)
{
    time_t rv = zsys_file_modified (filename.toUtf8().data());
    return rv;
}

///
//  Return file mode; provides at least support for the POSIX S_ISREG(m)
//  and S_ISDIR(m) macros and the S_IRUSR and S_IWUSR bits, on all boxes.
//  Returns a mode_t cast to int, or -1 in case of error.
int QZsys::fileMode (const QString &filename)
{
    int rv = zsys_file_mode (filename.toUtf8().data());
    return rv;
}

///
//  Delete file. Does not complain if the file is absent
int QZsys::fileDelete (const QString &filename)
{
    int rv = zsys_file_delete (filename.toUtf8().data());
    return rv;
}

///
//  Check if file is 'stable'
bool QZsys::fileStable (const QString &filename)
{
    bool rv = zsys_file_stable (filename.toUtf8().data());
    return rv;
}

///
//  Move to a specified working directory. Returns 0 if OK, -1 if this failed.
int QZsys::dirChange (const QString &pathname)
{
    int rv = zsys_dir_change (pathname.toUtf8().data());
    return rv;
}

///
//  Set private file creation mode; all files created from here will be
//  readable/writable by the owner only.
void QZsys::fileModePrivate ()
{
    zsys_file_mode_private ();

}

///
//  Reset default file creation mode; all files created from here will use
//  process file mode defaults.
void QZsys::fileModeDefault ()
{
    zsys_file_mode_default ();

}

///
//  Return the CZMQ version for run-time API detection; returns version
//  number into provided fields, providing reference isn't null in each case.
void QZsys::version (int *major, int *minor, int *patch)
{
    zsys_version (major, minor, patch);

}

///
//  Format a string with a va_list argument, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().
const QString QZsys::vprintf (const QString &format, va_list argptr)
{
    const QString rv = QString (zsys_vprintf (format.toUtf8().data(), argptr));
    return rv;
}

///
//  Create UDP beacon socket; if the routable option is true, uses
//  multicast (not yet implemented), else uses broadcast. This method
//  and related ones might _eventually_ be moved to a zudp class.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
SOCKET QZsys::udpNew (bool routable)
{
    SOCKET rv = zsys_udp_new (routable);
    return rv;
}

///
//  Close a UDP socket
//  *** This is for CZMQ internal use only and may change arbitrarily ***
int QZsys::udpClose (SOCKET handle)
{
    int rv = zsys_udp_close (handle);
    return rv;
}

///
//  Send zframe to UDP socket, return -1 if sending failed due to
//  interface having disappeared (happens easily with WiFi)
//  *** This is for CZMQ internal use only and may change arbitrarily ***
int QZsys::udpSend (SOCKET udpsock, QZframe *frame, inaddr_t *address, int addrlen)
{
    int rv = zsys_udp_send (udpsock, frame->self, address, addrlen);
    return rv;
}

///
//  Receive zframe from UDP socket, and set address of peer that sent it
//  The peername must be a char [INET_ADDRSTRLEN] array if IPv6 is disabled or
//  NI_MAXHOST if it's enabled. Returns NULL when failing to get peer address.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
QZframe * QZsys::udpRecv (SOCKET udpsock, const QString &peername, int peerlen)
{
    QZframe *rv = new QZframe (zsys_udp_recv (udpsock, peername.toUtf8().data(), peerlen));
    return rv;
}

///
//  Handle an I/O error on some socket operation; will report and die on
//  fatal errors, and continue silently on "try again" errors.
//  *** This is for CZMQ internal use only and may change arbitrarily ***
void QZsys::socketError (const QString &reason)
{
    zsys_socket_error (reason.toUtf8().data());

}

///
//  Return current host name, for use in public tcp:// endpoints. Caller gets
//  a freshly allocated string, should free it using zstr_free(). If the host
//  name is not resolvable, returns NULL.
const QString QZsys::hostname ()
{
    const QString rv = QString (zsys_hostname ());
    return rv;
}

///
//  Move the current process into the background. The precise effect depends
//  on the operating system. On POSIX boxes, moves to a specified working
//  directory (if specified), closes all file handles, reopens stdin, stdout,
//  and stderr to the null device, and sets the process to ignore SIGHUP. On
//  Windows, does nothing. Returns 0 if OK, -1 if there was an error.
int QZsys::daemonize (const QString &workdir)
{
    int rv = zsys_daemonize (workdir.toUtf8().data());
    return rv;
}

///
//  Drop the process ID into the lockfile, with exclusive lock, and switch
//  the process to the specified group and/or user. Any of the arguments
//  may be null, indicating a no-op. Returns 0 on success, -1 on failure.
//  Note if you combine this with zsys_daemonize, run after, not before
//  that method, or the lockfile will hold the wrong process ID.
int QZsys::runAs (const QString &lockfile, const QString &group, const QString &user)
{
    int rv = zsys_run_as (lockfile.toUtf8().data(), group.toUtf8().data(), user.toUtf8().data());
    return rv;
}

///
//  Returns true if the underlying libzmq supports CURVE security.
//  Uses a heuristic probe according to the version of libzmq being used.
bool QZsys::hasCurve ()
{
    bool rv = zsys_has_curve ();
    return rv;
}

///
//  Configure the number of I/O threads that ZeroMQ will use. A good
//  rule of thumb is one thread per gigabit of traffic in or out. The
//  default is 1, sufficient for most applications. If the environment
//  variable ZSYS_IO_THREADS is defined, that provides the default.
//  Note that this method is valid only before any socket is created.
void QZsys::setIoThreads (size_t ioThreads)
{
    zsys_set_io_threads (ioThreads);

}

///
//  Configure the scheduling policy of the ZMQ context thread pool.
//  Not available on Windows. See the sched_setscheduler man page or sched.h
//  for more information. If the environment variable ZSYS_THREAD_SCHED_POLICY
//  is defined, that provides the default.
//  Note that this method is valid only before any socket is created.
void QZsys::setThreadSchedPolicy (int policy)
{
    zsys_set_thread_sched_policy (policy);

}

///
//  Configure the scheduling priority of the ZMQ context thread pool.
//  Not available on Windows. See the sched_setscheduler man page or sched.h
//  for more information. If the environment variable ZSYS_THREAD_PRIORITY is
//  defined, that provides the default.
//  Note that this method is valid only before any socket is created.
void QZsys::setThreadPriority (int priority)
{
    zsys_set_thread_priority (priority);

}

///
//  Configure the number of sockets that ZeroMQ will allow. The default
//  is 1024. The actual limit depends on the system, and you can query it
//  by using zsys_socket_limit (). A value of zero means "maximum".
//  Note that this method is valid only before any socket is created.
void QZsys::setMaxSockets (size_t maxSockets)
{
    zsys_set_max_sockets (maxSockets);

}

///
//  Return maximum number of ZeroMQ sockets that the system will support.
size_t QZsys::socketLimit ()
{
    size_t rv = zsys_socket_limit ();
    return rv;
}

///
//  Configure the maximum allowed size of a message sent.
//  The default is INT_MAX.
void QZsys::setMaxMsgsz (int maxMsgsz)
{
    zsys_set_max_msgsz (maxMsgsz);

}

///
//  Return maximum message size.
int QZsys::maxMsgsz ()
{
    int rv = zsys_max_msgsz ();
    return rv;
}

///
//  Configure the default linger timeout in msecs for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  linger time is zero, i.e. any pending messages will be dropped. If the
//  environment variable ZSYS_LINGER is defined, that provides the default.
//  Note that process exit will typically be delayed by the linger time.
void QZsys::setLinger (size_t linger)
{
    zsys_set_linger (linger);

}

///
//  Configure the default outgoing pipe limit (HWM) for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
//  ZSYS_SNDHWM is defined, that provides the default. Note that a value of
//  zero means no limit, i.e. infinite memory consumption.
void QZsys::setSndhwm (size_t sndhwm)
{
    zsys_set_sndhwm (sndhwm);

}

///
//  Configure the default incoming pipe limit (HWM) for new zsock instances.
//  You can also set this separately on each zsock_t instance. The default
//  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
//  ZSYS_RCVHWM is defined, that provides the default. Note that a value of
//  zero means no limit, i.e. infinite memory consumption.
void QZsys::setRcvhwm (size_t rcvhwm)
{
    zsys_set_rcvhwm (rcvhwm);

}

///
//  Configure the default HWM for zactor internal pipes; this is set on both
//  ends of the pipe, for outgoing messages only (sndhwm). The default HWM is
//  1,000, on all versions of ZeroMQ. If the environment var ZSYS_ACTORHWM is
//  defined, that provides the default. Note that a value of zero means no
//  limit, i.e. infinite memory consumption.
void QZsys::setPipehwm (size_t pipehwm)
{
    zsys_set_pipehwm (pipehwm);

}

///
//  Return the HWM for zactor internal pipes.
size_t QZsys::pipehwm ()
{
    size_t rv = zsys_pipehwm ();
    return rv;
}

///
//  Configure use of IPv6 for new zsock instances. By default sockets accept
//  and make only IPv4 connections. When you enable IPv6, sockets will accept
//  and connect to both IPv4 and IPv6 peers. You can override the setting on
//  each zsock_t instance. The default is IPv4 only (ipv6 set to 0). If the
//  environment variable ZSYS_IPV6 is defined (as 1 or 0), this provides the
//  default. Note: has no effect on ZMQ v2.
void QZsys::setIpv6 (int ipv6)
{
    zsys_set_ipv6 (ipv6);

}

///
//  Return use of IPv6 for zsock instances.
int QZsys::ipv6 ()
{
    int rv = zsys_ipv6 ();
    return rv;
}

///
//  Set network interface name to use for broadcasts, particularly zbeacon.
//  This lets the interface be configured for test environments where required.
//  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
//  the default when there is no specified interface. If the environment
//  variable ZSYS_INTERFACE is set, use that as the default interface name.
//  Setting the interface to "*" means "use all available interfaces".
void QZsys::setInterface (const QString &value)
{
    zsys_set_interface (value.toUtf8().data());

}

///
//  Return network interface to use for broadcasts, or "" if none was set.
const QString QZsys::interface ()
{
    const QString rv = QString (zsys_interface ());
    return rv;
}

///
//  Set IPv6 address to use zbeacon socket, particularly for receiving zbeacon.
//  This needs to be set IPv6 is enabled as IPv6 can have multiple addresses
//  on a given interface. If the environment variable ZSYS_IPV6_ADDRESS is set,
//  use that as the default IPv6 address.
void QZsys::setIpv6Address (const QString &value)
{
    zsys_set_ipv6_address (value.toUtf8().data());

}

///
//  Return IPv6 address to use for zbeacon reception, or "" if none was set.
const QString QZsys::ipv6Address ()
{
    const QString rv = QString (zsys_ipv6_address ());
    return rv;
}

///
//  Set IPv6 milticast address to use for sending zbeacon messages. This needs
//  to be set if IPv6 is enabled. If the environment variable
//  ZSYS_IPV6_MCAST_ADDRESS is set, use that as the default IPv6 multicast
//  address.
void QZsys::setIpv6McastAddress (const QString &value)
{
    zsys_set_ipv6_mcast_address (value.toUtf8().data());

}

///
//  Return IPv6 multicast address to use for sending zbeacon, or "" if none was
//  set.
const QString QZsys::ipv6McastAddress ()
{
    const QString rv = QString (zsys_ipv6_mcast_address ());
    return rv;
}

///
//  Configure the automatic use of pre-allocated FDs when creating new sockets.
//  If 0 (default), nothing will happen. Else, when a new socket is bound, the
//  system API will be used to check if an existing pre-allocated FD with a
//  matching port (if TCP) or path (if IPC) exists, and if it does it will be
//  set via the ZMQ_USE_FD socket option so that the library will use it
//  instead of creating a new socket.
void QZsys::setAutoUseFd (int autoUseFd)
{
    zsys_set_auto_use_fd (autoUseFd);

}

///
//  Return use of automatic pre-allocated FDs for zsock instances.
int QZsys::autoUseFd ()
{
    int rv = zsys_auto_use_fd ();
    return rv;
}

///
//  Set log identity, which is a string that prefixes all log messages sent
//  by this process. The log identity defaults to the environment variable
//  ZSYS_LOGIDENT, if that is set.
void QZsys::setLogident (const QString &value)
{
    zsys_set_logident (value.toUtf8().data());

}

///
//  Set stream to receive log traffic. By default, log traffic is sent to
//  stdout. If you set the stream to NULL, no stream will receive the log
//  traffic (it may still be sent to the system facility).
void QZsys::setLogstream (FILE *stream)
{
    zsys_set_logstream (stream);

}

///
//  Sends log output to a PUB socket bound to the specified endpoint. To
//  collect such log output, create a SUB socket, subscribe to the traffic
//  you care about, and connect to the endpoint. Log traffic is sent as a
//  single string frame, in the same format as when sent to stdout. The
//  log system supports a single sender; multiple calls to this method will
//  bind the same sender to multiple endpoints. To disable the sender, call
//  this method with a null argument.
void QZsys::setLogsender (const QString &endpoint)
{
    zsys_set_logsender (endpoint.toUtf8().data());

}

///
//  Enable or disable logging to the system facility (syslog on POSIX boxes,
//  event log on Windows). By default this is disabled.
void QZsys::setLogsystem (bool logsystem)
{
    zsys_set_logsystem (logsystem);

}

///
//  Self test of this class.
void QZsys::test (bool verbose)
{
    zsys_test (verbose);

}
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
