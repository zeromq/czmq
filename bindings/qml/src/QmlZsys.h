/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/

#ifndef QML_ZSYS_H
#define QML_ZSYS_H

#include <QtQml>

#include <czmq.h>
#include "qml_czmq_plugin.h"


class QmlZsys : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isNULL READ isNULL)

public:
    zsys_t *self;

    QmlZsys() { self = NULL; }
    bool isNULL() { return self == NULL; }

    static QObject* qmlAttachedProperties(QObject* object); // defined in QmlZsys.cpp

public slots:};

class QmlZsysAttached : public QObject
{
    Q_OBJECT
    QObject* m_attached;

public:
    QmlZsysAttached (QObject* attached) {
        Q_UNUSED (attached);
    };

public slots:
    //  Initialize CZMQ zsys layer; this happens automatically when you create
    //  a socket or an actor; however this call lets you force initialization
    //  earlier, so e.g. logging is properly set-up before you start working.
    //  Not threadsafe, so call only from main thread. Safe to call multiple
    //  times. Returns global CZMQ context.
    void *init ();

    //  Optionally shut down the CZMQ zsys layer; this normally happens automatically
    //  when the process exits; however this call lets you force a shutdown
    //  earlier, avoiding any potential problems with atexit() ordering, especially
    //  with Windows dlls.
    void shutdown ();

    //  Get a new ZMQ socket, automagically creating a ZMQ context if this is
    //  the first time. Caller is responsible for destroying the ZMQ socket
    //  before process exits, to avoid a ZMQ deadlock. Note: you should not use
    //  this method in CZMQ apps, use zsock_new() instead.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    void *socket (int type, const QString &filename, size_t lineNbr);

    //  Destroy/close a ZMQ socket. You should call this for every socket you
    //  create using zsys_socket().
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    int close (void *handle, const QString &filename, size_t lineNbr);

    //  Return ZMQ socket name for socket type
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    const QString sockname (int socktype);

    //  Create a pipe, which consists of two PAIR sockets connected over inproc.
    //  The pipe is configured to use the zsys_pipehwm setting. Returns the
    //  frontend socket successful, NULL if failed.
    QmlZsock *createPipe (QmlZsock *backendP);

    //  Set interrupt handler; this saves the default handlers so that a
    //  zsys_handler_reset () can restore them. If you call this multiple times
    //  then the last handler will take affect. If handler_fn is NULL, disables
    //  default SIGINT/SIGTERM handling in CZMQ.
    void handlerSet (zsys_handler_fn *handlerFn);

    //  Reset interrupt handler, call this at exit if needed
    void handlerReset ();

    //  Set default interrupt handler, so Ctrl-C or SIGTERM will set
    //  zsys_interrupted. Idempotent; safe to call multiple times.
    //  Can be suppressed by ZSYS_SIGHANDLER=false
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    void catchInterrupts ();

    //  Check if default interrupt handler of Ctrl-C or SIGTERM was called.
    //  Does not work if ZSYS_SIGHANDLER is false and code does not call
    //  set interrupted on signal.
    bool isInterrupted ();

    //  Set interrupted flag. This is done by default signal handler, however
    //  this can be handy for language bindings or cases without default
    //  signal handler.
    void setInterrupted ();

    //  Return 1 if file exists, else zero
    bool fileExists (const QString &filename);

    //  Return file modification time. Returns 0 if the file does not exist.
    time_t fileModified (const QString &filename);

    //  Return file mode; provides at least support for the POSIX S_ISREG(m)
    //  and S_ISDIR(m) macros and the S_IRUSR and S_IWUSR bits, on all boxes.
    //  Returns a mode_t cast to int, or -1 in case of error.
    int fileMode (const QString &filename);

    //  Delete file. Does not complain if the file is absent
    int fileDelete (const QString &filename);

    //  Check if file is 'stable'
    bool fileStable (const QString &filename);

    //  Create a file path if it doesn't exist. The file path is treated as
    //  printf format.
    int dirCreate (const QString &pathname);

    //  Remove a file path if empty; the pathname is treated as printf format.
    int dirDelete (const QString &pathname);

    //  Move to a specified working directory. Returns 0 if OK, -1 if this failed.
    int dirChange (const QString &pathname);

    //  Set private file creation mode; all files created from here will be
    //  readable/writable by the owner only.
    void fileModePrivate ();

    //  Reset default file creation mode; all files created from here will use
    //  process file mode defaults.
    void fileModeDefault ();

    //  Return the CZMQ version for run-time API detection; returns version
    //  number into provided fields, providing reference isn't null in each case.
    void version (int *major, int *minor, int *patch);

    //  Format a string using printf formatting, returning a freshly allocated
    //  buffer. If there was insufficient memory, returns NULL. Free the returned
    //  string using zstr_free(). The hinted version allows to optimize by using
    //  a larger starting buffer size (known to/assumed by the developer) and so
    //  avoid reallocations.
    const QString sprintfHint (int hint, const QString &format);

    //  Format a string using printf formatting, returning a freshly allocated
    //  buffer. If there was insufficient memory, returns NULL. Free the returned
    //  string using zstr_free().
    const QString sprintf (const QString &format);

    //  Format a string with a va_list argument, returning a freshly allocated
    //  buffer. If there was insufficient memory, returns NULL. Free the returned
    //  string using zstr_free().
    const QString vprintf (const QString &format, va_list argptr);

    //  Create UDP beacon socket; if the routable option is true, uses
    //  multicast (not yet implemented), else uses broadcast. This method
    //  and related ones might _eventually_ be moved to a zudp class.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    SOCKET udpNew (bool routable);

    //  Close a UDP socket
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    int udpClose (SOCKET handle);

    //  Send zframe to UDP socket, return -1 if sending failed due to
    //  interface having disappeared (happens easily with WiFi)
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    int udpSend (SOCKET udpsock, QmlZframe *frame, inaddr_t *address, int addrlen);

    //  Receive zframe from UDP socket, and set address of peer that sent it
    //  The peername must be a char [INET_ADDRSTRLEN] array if IPv6 is disabled or
    //  NI_MAXHOST if it's enabled. Returns NULL when failing to get peer address.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    QmlZframe *udpRecv (SOCKET udpsock, const QString &peername, int peerlen);

    //  Handle an I/O error on some socket operation; will report and die on
    //  fatal errors, and continue silently on "try again" errors.
    //  *** This is for CZMQ internal use only and may change arbitrarily ***
    void socketError (const QString &reason);

    //  Return current host name, for use in public tcp:// endpoints. Caller gets
    //  a freshly allocated string, should free it using zstr_free(). If the host
    //  name is not resolvable, returns NULL.
    const QString hostname ();

    //  Move the current process into the background. The precise effect depends
    //  on the operating system. On POSIX boxes, moves to a specified working
    //  directory (if specified), closes all file handles, reopens stdin, stdout,
    //  and stderr to the null device, and sets the process to ignore SIGHUP. On
    //  Windows, does nothing. Returns 0 if OK, -1 if there was an error.
    int daemonize (const QString &workdir);

    //  Drop the process ID into the lockfile, with exclusive lock, and switch
    //  the process to the specified group and/or user. Any of the arguments
    //  may be null, indicating a no-op. Returns 0 on success, -1 on failure.
    //  Note if you combine this with zsys_daemonize, run after, not before
    //  that method, or the lockfile will hold the wrong process ID.
    int runAs (const QString &lockfile, const QString &group, const QString &user);

    //  Returns true if the underlying libzmq supports CURVE security.
    //  Uses a heuristic probe according to the version of libzmq being used.
    bool hasCurve ();

    //  Configure the number of I/O threads that ZeroMQ will use. A good
    //  rule of thumb is one thread per gigabit of traffic in or out. The
    //  default is 1, sufficient for most applications. If the environment
    //  variable ZSYS_IO_THREADS is defined, that provides the default.
    //  Note that this method is valid only before any socket is created.
    void setIoThreads (size_t ioThreads);

    //  Configure the scheduling policy of the ZMQ context thread pool.
    //  Not available on Windows. See the sched_setscheduler man page or sched.h
    //  for more information. If the environment variable ZSYS_THREAD_SCHED_POLICY
    //  is defined, that provides the default.
    //  Note that this method is valid only before any socket is created.
    void setThreadSchedPolicy (int policy);

    //  Configure the scheduling priority of the ZMQ context thread pool.
    //  Not available on Windows. See the sched_setscheduler man page or sched.h
    //  for more information. If the environment variable ZSYS_THREAD_PRIORITY is
    //  defined, that provides the default.
    //  Note that this method is valid only before any socket is created.
    void setThreadPriority (int priority);

    //  Configure the numeric prefix to each thread created for the internal
    //  context's thread pool. This option is only supported on Linux.
    //  If the environment variable ZSYS_THREAD_NAME_PREFIX is defined, that
    //  provides the default.
    //  Note that this method is valid only before any socket is created.
    void setThreadNamePrefix (int prefix);

    //  Return thread name prefix.
    int threadNamePrefix ();

    //  Adds a specific CPU to the affinity list of the ZMQ context thread pool.
    //  This option is only supported on Linux.
    //  Note that this method is valid only before any socket is created.
    void threadAffinityCpuAdd (int cpu);

    //  Removes a specific CPU to the affinity list of the ZMQ context thread pool.
    //  This option is only supported on Linux.
    //  Note that this method is valid only before any socket is created.
    void threadAffinityCpuRemove (int cpu);

    //  Configure the number of sockets that ZeroMQ will allow. The default
    //  is 1024. The actual limit depends on the system, and you can query it
    //  by using zsys_socket_limit (). A value of zero means "maximum".
    //  Note that this method is valid only before any socket is created.
    void setMaxSockets (size_t maxSockets);

    //  Return maximum number of ZeroMQ sockets that the system will support.
    size_t socketLimit ();

    //  Configure the maximum allowed size of a message sent.
    //  The default is INT_MAX.
    void setMaxMsgsz (int maxMsgsz);

    //  Return maximum message size.
    int maxMsgsz ();

    //  Configure whether to use zero copy strategy in libzmq. If the environment
    //  variable ZSYS_ZERO_COPY_RECV is defined, that provides the default.
    //  Otherwise the default is 1.
    void setZeroCopyRecv (int zeroCopy);

    //  Return ZMQ_ZERO_COPY_RECV option.
    int zeroCopyRecv ();

    //  Configure the threshold value of filesystem object age per st_mtime
    //  that should elapse until we consider that object "stable" at the
    //  current zclock_time() moment.
    //  The default is S_DEFAULT_ZSYS_FILE_STABLE_AGE_MSEC defined in zsys.c
    //  which generally depends on host OS, with fallback value of 5000.
    void setFileStableAgeMsec (int64_t fileStableAgeMsec);

    //  Return current threshold value of file stable age in msec.
    //  This can be used in code that chooses to wait for this timeout
    //  before testing if a filesystem object is "stable" or not.
    int64_t fileStableAgeMsec ();

    //  Configure the default linger timeout in msecs for new zsock instances.
    //  You can also set this separately on each zsock_t instance. The default
    //  linger time is zero, i.e. any pending messages will be dropped. If the
    //  environment variable ZSYS_LINGER is defined, that provides the default.
    //  Note that process exit will typically be delayed by the linger time.
    void setLinger (size_t linger);

    //  Configure the default outgoing pipe limit (HWM) for new zsock instances.
    //  You can also set this separately on each zsock_t instance. The default
    //  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
    //  ZSYS_SNDHWM is defined, that provides the default. Note that a value of
    //  zero means no limit, i.e. infinite memory consumption.
    void setSndhwm (size_t sndhwm);

    //  Configure the default incoming pipe limit (HWM) for new zsock instances.
    //  You can also set this separately on each zsock_t instance. The default
    //  HWM is 1,000, on all versions of ZeroMQ. If the environment variable
    //  ZSYS_RCVHWM is defined, that provides the default. Note that a value of
    //  zero means no limit, i.e. infinite memory consumption.
    void setRcvhwm (size_t rcvhwm);

    //  Configure the default HWM for zactor internal pipes; this is set on both
    //  ends of the pipe, for outgoing messages only (sndhwm). The default HWM is
    //  1,000, on all versions of ZeroMQ. If the environment var ZSYS_ACTORHWM is
    //  defined, that provides the default. Note that a value of zero means no
    //  limit, i.e. infinite memory consumption.
    void setPipehwm (size_t pipehwm);

    //  Return the HWM for zactor internal pipes.
    size_t pipehwm ();

    //  Configure use of IPv6 for new zsock instances. By default sockets accept
    //  and make only IPv4 connections. When you enable IPv6, sockets will accept
    //  and connect to both IPv4 and IPv6 peers. You can override the setting on
    //  each zsock_t instance. The default is IPv4 only (ipv6 set to 0). If the
    //  environment variable ZSYS_IPV6 is defined (as 1 or 0), this provides the
    //  default. Note: has no effect on ZMQ v2.
    void setIpv6 (int ipv6);

    //  Return use of IPv6 for zsock instances.
    int ipv6 ();

    //  Test if ipv6 is available on the system. Return true if available.
    //  The only way to reliably check is to actually open a socket and
    //  try to bind it. (ported from libzmq)
    bool ipv6Available ();

    //  Set network interface name to use for broadcasts, particularly zbeacon.
    //  This lets the interface be configured for test environments where required.
    //  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
    //  the default when there is no specified interface. If the environment
    //  variable ZSYS_INTERFACE is set, use that as the default interface name.
    //  Setting the interface to "*" means "use all available interfaces".
    void setInterface (const QString &value);

    //  Return network interface to use for broadcasts, or "" if none was set.
    const QString interface ();

    //  Set IPv6 address to use zbeacon socket, particularly for receiving zbeacon.
    //  This needs to be set IPv6 is enabled as IPv6 can have multiple addresses
    //  on a given interface. If the environment variable ZSYS_IPV6_ADDRESS is set,
    //  use that as the default IPv6 address.
    void setIpv6Address (const QString &value);

    //  Return IPv6 address to use for zbeacon reception, or "" if none was set.
    const QString ipv6Address ();

    //  Set IPv6 milticast address to use for sending zbeacon messages. This needs
    //  to be set if IPv6 is enabled. If the environment variable
    //  ZSYS_IPV6_MCAST_ADDRESS is set, use that as the default IPv6 multicast
    //  address.
    void setIpv6McastAddress (const QString &value);

    //  Return IPv6 multicast address to use for sending zbeacon, or "" if none was
    //  set.
    const QString ipv6McastAddress ();

    //  Set IPv4 multicast address to use for sending zbeacon messages. By default
    //  IPv4 multicast is NOT used. If the environment variable
    //  ZSYS_IPV4_MCAST_ADDRESS is set, use that as the default IPv4 multicast
    //  address. Calling this function or setting ZSYS_IPV4_MCAST_ADDRESS
    //  will enable IPv4 zbeacon messages.
    void setIpv4McastAddress (const QString &value);

    //  Return IPv4 multicast address to use for sending zbeacon, or NULL if none was
    //  set.
    const QString ipv4McastAddress ();

    //  Set multicast TTL default is 1
    void setMcastTtl (byte value);

    //  Get multicast TTL
    byte mcastTtl ();

    //  Configure the automatic use of pre-allocated FDs when creating new sockets.
    //  If 0 (default), nothing will happen. Else, when a new socket is bound, the
    //  system API will be used to check if an existing pre-allocated FD with a
    //  matching port (if TCP) or path (if IPC) exists, and if it does it will be
    //  set via the ZMQ_USE_FD socket option so that the library will use it
    //  instead of creating a new socket.
    void setAutoUseFd (int autoUseFd);

    //  Return use of automatic pre-allocated FDs for zsock instances.
    int autoUseFd ();

    //  Print formatted string. Format is specified by variable names
    //  in Python-like format style
    //
    //  "%(KEY)s=%(VALUE)s", KEY=key, VALUE=value
    //  become
    //  "key=value"
    //
    //  Returns freshly allocated string or NULL in a case of error.
    //  Not enough memory, invalid format specifier, name not in args
    QString zprintf (const QString &format, QmlZhash *args);

    //  Return error string for given format/args combination.
    QString zprintfError (const QString &format, QmlZhash *args);

    //  Print formatted string. Format is specified by variable names
    //  in Python-like format style
    //
    //  "%(KEY)s=%(VALUE)s", KEY=key, VALUE=value
    //  become
    //  "key=value"
    //
    //  Returns freshly allocated string or NULL in a case of error.
    //  Not enough memory, invalid format specifier, name not in args
    QString zplprintf (const QString &format, QmlZconfig *args);

    //  Return error string for given format/args combination.
    QString zplprintfError (const QString &format, QmlZconfig *args);

    //  Set log identity, which is a string that prefixes all log messages sent
    //  by this process. The log identity defaults to the environment variable
    //  ZSYS_LOGIDENT, if that is set.
    void setLogident (const QString &value);

    //  Set stream to receive log traffic. By default, log traffic is sent to
    //  stdout. If you set the stream to NULL, no stream will receive the log
    //  traffic (it may still be sent to the system facility).
    void setLogstream (FILE *stream);

    //  Sends log output to a PUB socket bound to the specified endpoint. To
    //  collect such log output, create a SUB socket, subscribe to the traffic
    //  you care about, and connect to the endpoint. Log traffic is sent as a
    //  single string frame, in the same format as when sent to stdout. The
    //  log system supports a single sender; multiple calls to this method will
    //  bind the same sender to multiple endpoints. To disable the sender, call
    //  this method with a null argument.
    void setLogsender (const QString &endpoint);

    //  Enable or disable logging to the system facility (syslog on POSIX boxes,
    //  event log on Windows). By default this is disabled.
    void setLogsystem (bool logsystem);

    //  Log error condition - highest priority
    void error (const QString &format);

    //  Log warning condition - high priority
    void warning (const QString &format);

    //  Log normal, but significant, condition - normal priority
    void notice (const QString &format);

    //  Log informational message - low priority
    void info (const QString &format);

    //  Log debug-level message - lowest priority
    void debug (const QString &format);

    //  Self test of this class.
    void test (bool verbose);
};


QML_DECLARE_TYPEINFO(QmlZsys, QML_HAS_ATTACHED_PROPERTIES)

#endif
/*
################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
*/
