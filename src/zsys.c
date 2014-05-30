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

#include "platform.h"
#include "../include/czmq.h"

//  --------------------------------------------------------------------------
//  Signal handling

//  These are global variables accessible to CZMQ application code
volatile int zsys_interrupted = 0;  //  Current name
volatile int zctx_interrupted = 0;  //  Deprecated name

static void s_initialize_process (void);
static void s_terminate_process (void);
static void s_signal_handler (int signal_value);

//  We use these variables for signal handling
static bool s_first_time = true;
#if defined (__UNIX__)
static struct sigaction sigint_default;
static struct sigaction sigterm_default;

#elif defined (__WINDOWS__)
static zsys_handler_fn *installed_handler_fn;
static BOOL WINAPI s_handler_fn_shim (DWORD ctrltype)
{
   //  Return TRUE for events that we handle
   if (ctrltype == CTRL_C_EVENT && installed_handler_fn != NULL) {
       installed_handler_fn (ctrltype);
       return TRUE;
   }
   else
       return FALSE;
}
#endif

//  --------------------------------------------------------------------------
//  Global context handling

//  Global ZeroMQ context for this process
void *process_ctx = NULL;

//  Default globals for new sockets and other joys; these can all be set
//  from the environment, or via the zsys_set_xxx API.
static size_t s_iothreads = 1;      //  ZSYS_IOTHREADS=1
static size_t s_linger = 0;         //  ZSYS_LINGER=0
static size_t s_sndhwm = 1000;      //  ZSYS_SNDHWM=1000
static size_t s_rcvhwm = 1000;      //  ZSYS_RCVHWM=1000
static int s_ipv6 = 0;              //  ZSYS_IPV6=0
static char *s_interface = NULL;    //  ZSYS_INTERFACE=

//  Track number of open sockets so we can zmq_ctx_term() safely
static size_t s_open_sockets = 0;

//  We keep a list of open sockets to report leaks to developers
static zlist_t *s_sockref_list = NULL;

//  This defines a single zsocket_new() caller instance
typedef struct {
    void *handle;
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
typedef CRITICAL_SECTION  zsys_mutex_t;
#   define ZMUTEX_INIT(m)    InitializeCriticalSection (&m);
#   define ZMUTEX_LOCK(m)    EnterCriticalSection (&m);
#   define ZMUTEX_UNLOCK(m)  LeaveCriticalSection (&m);
#   define ZMUTEX_DESTROY(m) DeleteCriticalSection (&m);
#endif

//  Mutex to guard socket counter
static zsys_mutex_t s_mutex;


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
    if (!process_ctx)
        s_initialize_process ();

    ZMUTEX_LOCK (s_mutex);
    void *handle = zmq_socket (process_ctx, type);

    //  Configure socket with process defaults
    zsock_set_linger (handle, s_linger);
#if (ZMQ_VERSION_MAJOR == 2)
    //  For ZeroMQ/2.x we use sndhwm for both send and receive
    zsock_set_hwm (handle, s_sndhwm);
#else
    //  For later versions we use separate SNDHWM and RCVHWM
    zsock_set_sndhwm (handle, s_sndhwm);
    zsock_set_rcvhwm (handle, s_rcvhwm);
#   if defined (ZMQ_IPV6)
    zsock_set_ipv6 (handle, s_ipv6);
#   else
    zsock_set_ipv4only (handle, s_ipv6? 0: 1);
#   endif
#endif
    //  Add socket to reference tracker so we can report leaks
    //  TODO: let user disable this via environment variable; by default
    //  it should be enabled to force correct destruction of sockets.
    if (filename) {
        s_sockref_t *sockref = (s_sockref_t *) malloc (sizeof (s_sockref_t));
        sockref->handle = handle;
        sockref->filename = filename;
        sockref->line_nbr = line_nbr;
        zlist_append (s_sockref_list, sockref);
    }
    s_open_sockets++;
    ZMUTEX_UNLOCK (s_mutex);
    return handle;
}

//  First-time initializations for the process
static void
s_initialize_process (void)
{
    //  Pull process defaults from environment
    if (getenv ("ZSYS_IOTHREADS"))
        s_iothreads = atoi (getenv ("ZSYS_IOTHREADS"));
    if (getenv ("ZSYS_LINGER"))
        s_linger = atoi (getenv ("ZSYS_LINGER"));
    if (getenv ("ZSYS_SNDHWM"))
        s_sndhwm = atoi (getenv ("ZSYS_SNDHWM"));
    if (getenv ("ZSYS_RCVHWM"))
        s_rcvhwm = atoi (getenv ("ZSYS_RCVHWM"));
    if (getenv ("ZSYS_IPV6"))
        s_ipv6 = atoi (getenv ("ZSYS_IPV6"));
    if (getenv ("ZSYS_INTERFACE"))
        s_interface = strdup (getenv ("ZSYS_INTERFACE"));

    //  This call keeps compatibility back to ZMQ v2
    process_ctx = zmq_init (s_iothreads);
    ZMUTEX_INIT (s_mutex);
    s_sockref_list = zlist_new ();
    zsys_catch_interrupts ();
    srandom ((unsigned) time (NULL));
    atexit (s_terminate_process);
}

//  atexit termination for the process
static void
s_terminate_process (void)
{
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
        zclock_sleep (100);

    //  No matter, we are now going to shut down
    //  Print the source reference for any sockets the app did not
    //  destroy properly.
    ZMUTEX_LOCK (s_mutex);
    s_sockref_t *sockref = (s_sockref_t *) zlist_pop (s_sockref_list);
    while (sockref) {
#ifdef __WINDOWS__
		printf ("E: dangling socket created at %s:%u\n",
                sockref->filename, sockref->line_nbr);
#else
        printf ("E: dangling socket created at %s:%zd\n",
                sockref->filename, sockref->line_nbr);
#endif
        zmq_close (sockref->handle);
        free (sockref);
        sockref = (s_sockref_t *) zlist_pop (s_sockref_list);
    }
    zlist_destroy (&s_sockref_list);
    ZMUTEX_UNLOCK (s_mutex);

    zmq_ctx_term (process_ctx);
    ZMUTEX_DESTROY (s_mutex);

    //  Free dynamically allocated properties
    free (s_interface);
}


//  --------------------------------------------------------------------------
//  Destroy/close a ZMQ socket. You should call this for every socket you 
//  create using zsys_socket().

int
zsys_close (void *handle)
{
    ZMUTEX_LOCK (s_mutex);
    //  It's possible atexit() has already happened if we're running under
    //  a debugger that redirects the main thread exit.
    if (s_sockref_list) {
        s_sockref_t *sockref = (s_sockref_t *) zlist_first (s_sockref_list);
        while (sockref) {
            if (sockref->handle == handle) {
                zlist_remove (s_sockref_list, sockref);
                free (sockref);
                s_open_sockets--;
                zmq_close (handle);
                break;
            }
            sockref = (s_sockref_t *) zlist_next (s_sockref_list);
        }
    }
    ZMUTEX_UNLOCK (s_mutex);
    return 0;
}


//  --------------------------------------------------------------------------
//  Set interrupt handler; this saves the default handlers so that a
//  zsys_handler_reset () can restore them. If you call this multiple times
//  then the last handler will take affect. Note handler_fn cannot be null.

void
zsys_handler_set (zsys_handler_fn *handler_fn)
{
    //  After 2014-04-20 this code does not accept NULL handlers, which
    //  was complexity for no known benefit.
    assert (handler_fn);

#if defined (__UNIX__)
    //  If first time, save default handlers
    if (s_first_time) {
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
#endif
}


//  --------------------------------------------------------------------------
//  Reset interrupt handler, call this at exit if needed
//  Idempotent; safe to call multiple times

void
zsys_handler_reset (void)
{
#if defined (__UNIX__)
    //  Restore default handlers if not already done
    if (!s_first_time) {
        sigaction (SIGINT, &sigint_default, NULL);
        sigaction (SIGTERM, &sigterm_default, NULL);
        sigint_default.sa_handler = NULL;
        sigterm_default.sa_handler = NULL;
        s_first_time = true;
    }
#elif defined (__WINDOWS__)
    if (!s_first_time) {
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
    zsys_handler_set (s_signal_handler);
}

//  Default internal signal handler
static void
s_signal_handler (int signal_value)
{
    puts ("INTERRUPTED");
    zctx_interrupted = 1;
    zsys_interrupted = 1;
}


//  --------------------------------------------------------------------------
//  Return true if file exists, else zero

bool
zsys_file_exists (const char *filename)
{
    assert (filename);
    return zsys_file_mode (filename) != (mode_t) -1;
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
//  Return file modification time. Returns 0 if the file does not exist.

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

mode_t
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
    return DeleteFileA (filename) ? 0: -1;
#else
    return unlink (filename);
#endif
}


//  --------------------------------------------------------------------------
//  Check if file is 'stable'

bool
zsys_file_stable (const char *filename)
{
    struct stat stat_buf;
    if (stat (filename, &stat_buf) == 0) {
        //  File is 'stable' if more than 1 second old
#if (defined (WIN32))
#   define EPOCH_DIFFERENCE 11644473600LL
        long age = (long) (zclock_time () - EPOCH_DIFFERENCE * 1000 - (stat_buf.st_mtime * 1000));
#else
        long age = (long) (zclock_time () - (stat_buf.st_mtime * 1000));
#endif
        return (age > 1000);
    }
    else
        return false;           //  File doesn't exist, so not stable
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

    //  Create parent directory levels if needed
    char *slash = strchr (formatted + 1, '/');
    while (true) {
        if (slash)
            *slash = 0;         //  Cut at slash
        mode_t mode = zsys_file_mode (formatted);
        if (mode == (mode_t)-1) {
            //  Does not exist, try to create it
#if (defined (__WINDOWS__))
            if (!CreateDirectoryA (formatted, NULL))
#else
            if (mkdir (formatted, 0775))
#endif
                return -1;      //  Failed
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
    free (formatted);
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

#if (defined (__WINDOWS__))
    int rc = RemoveDirectoryA (formatted)? 0: -1;
#else
    int rc = rmdir (formatted);
#endif
    free (formatted);
    return rc;
}


//  --------------------------------------------------------------------------
//  Set private file creation mode; all files created from here will be
//  readable/writable by the owner only.

#if !defined(__WINDOWS__)
static mode_t s_old_mask = 0;
#endif

void
zsys_file_mode_private (void)
{
#   if !defined(__WINDOWS__)
    s_old_mask = umask (S_IWGRP | S_IWOTH | S_IRGRP | S_IROTH);
#   endif
}


//  --------------------------------------------------------------------------
//  Reset default file creation mode; all files created from here will use
//  process file mode defaults.

void
zsys_file_mode_default (void)
{
    //  Reset process file create mask
#   if !defined(__WINDOWS__)
    if (s_old_mask)
        umask (s_old_mask);
#   endif
}


//  --------------------------------------------------------------------------
//  Return the czmq version for run-time API detection

void zsys_version (int *major, int *minor, int *patch)
{
    *major = CZMQ_VERSION_MAJOR;
    *minor = CZMQ_VERSION_MINOR;
    *patch = CZMQ_VERSION_PATCH;
}


//  --------------------------------------------------------------------------
//  Format a string using printf formatting, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().

char *
zsys_sprintf (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    return (string);
}


//  --------------------------------------------------------------------------
//  Format a string with variable arguments, returning a freshly allocated
//  buffer. If there was insufficient memory, returns NULL. Free the returned
//  string using zstr_free().

char *
zsys_vprintf (const char *format, va_list argptr)
{
    int size = 256;
    char *string = (char *) malloc (size);
    //  Using argptr is destructive, so we take a copy each time we need it
    //  We define va_copy for Windows in czmq_prelude.h
    va_list my_argptr;
    va_copy (my_argptr, argptr);
    int required = vsnprintf (string, size, format, my_argptr);
    va_end (my_argptr);
#if (defined (_MSC_VER) || defined (__MINGW32__))
    if (required < 0 || required >= size) {
        va_copy (my_argptr, argptr);
        required = _vscprintf (format, argptr);
        va_end (my_argptr);
    }
#endif
    //  If formatted string cannot fit into small string, reallocate a
    //  larger buffer for it.
    if (required >= size) {
        size = required + 1;
        string = (char *) realloc (string, size);
        if (string) {
            va_copy (my_argptr, argptr);
            vsnprintf (string, size, format, my_argptr);
            va_end (my_argptr);
        }
    }
    return string;
}


//  --------------------------------------------------------------------------
//  Create a UDP beacon socket; if the routable option is true, uses
//  multicast (not yet implemented), else uses broadcast. This method
//  and related ones might _eventually_ be moved to a zudp class.

SOCKET
zsys_udp_new (bool routable)
{
    //  We haven't implemented multicast yet
    assert (!routable);
    int udpsock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpsock == INVALID_SOCKET) {
        zsys_socket_error ("socket");
        return INVALID_SOCKET;
    }
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
    return udpsock;
}


//  --------------------------------------------------------------------------
//  Send zframe to UDP socket

void
zsys_udp_send (SOCKET udpsock, zframe_t *frame, inaddr_t *address)
{
    assert (frame);
    assert (address);

    //  Sending can fail if the OS is blocking multicast. In such cases we
    //  don't try to report the error. We might log this or send to an error
    //  console at some point.
    sendto (udpsock,
            (char *) zframe_data (frame), zframe_size (frame),
            0,      //  Flags
            (struct sockaddr *) address, sizeof (inaddr_t));
}


//  --------------------------------------------------------------------------
//  Receive zframe from UDP socket, and set address of peer that sent it
//  The peername must be a char [INET_ADDRSTRLEN] array.

zframe_t *
zsys_udp_recv (SOCKET udpsock, char *peername)
{
    char buffer [UDP_FRAME_MAX];
    inaddr_t address;
    socklen_t address_len = sizeof (inaddr_t);
    ssize_t size = recvfrom (
        udpsock,
        buffer, UDP_FRAME_MAX,
        0,      //  Flags
        (struct sockaddr *) &address, &address_len);
    if (size == SOCKET_ERROR)
        zsys_socket_error ("recvfrom");

    //  Get sender address as printable string
#if (defined (__WINDOWS__))
    getnameinfo ((struct sockaddr *) &address, address_len,
                peername, INET_ADDRSTRLEN, NULL, 0, NI_NUMERICHOST);
#else
    inet_ntop (AF_INET, &address.sin_addr, peername, address_len);
#endif
    return zframe_new (buffer, size);
}


//  --------------------------------------------------------------------------
//  Handle an I/O error on some socket operation; will report and die on
//  fatal errors, and continue silently on "try again" errors.

void
zsys_socket_error (const char *reason)
{
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
        default:              errno = GetLastError ();
    }
#endif
    if (errno == EAGAIN
    ||  errno == ENETDOWN
    ||  errno == EHOSTUNREACH
    ||  errno == ENETUNREACH
    ||  errno == EINTR
    ||  errno == EPIPE
    ||  errno == ECONNRESET
#if defined (ENOPROTOOPT)
    ||  errno == ENOPROTOOPT
#endif
#if defined (EHOSTDOWN)
    ||  errno == EHOSTDOWN
#endif
#if defined (EOPNOTSUPP)
    ||  errno == EOPNOTSUPP
#endif
#if defined (EWOULDBLOCK)
    ||  errno == EWOULDBLOCK
#endif
#if defined (EPROTO)
    ||  errno == EPROTO
#endif
#if defined (ENONET)
    ||  errno == ENONET
#endif
    )
        return;             //  Ignore error and try again
    else {
        zclock_log ("E: (UDP) error '%s' on %s", strerror (errno), reason);
        assert (false);
    }
}


//  --------------------------------------------------------------------------
//  Return current host name, for use in public tcp:// endpoints. Caller gets
//  a freshly allocated string, should free it using zstr_free().

char *
zsys_hostname (void)
{
    char hostname [NI_MAXHOST];
    gethostname (hostname, NI_MAXHOST);
    hostname [NI_MAXHOST - 1] = 0;
    struct hostent *host = gethostbyname (hostname);
    return strdup (host->h_name);
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
    if (workdir && chdir (workdir)) {
        fprintf (stderr, "E: cannot chdir to '%s'\n", workdir);
        return -1;
    }
    //  Close all open file descriptors inherited from the parent
    //  process, to reduce the resources we use
    int file_handle = sysconf (_SC_OPEN_MAX);
    while (file_handle)
    close (file_handle--);      //  Ignore any errors

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
        zclock_log ("E: cannot set effective user id: %s\n",
                    strerror (errno));
        return -1;
    }
    if (lockfile) {
        //  We enforce a lock on the lockfile, if specified, so that
        //  only one copy of the process can run at once.
        int handle = open (lockfile, O_RDWR | O_CREAT, 0640);
        if (handle < 0) {
            zclock_log ("E: cannot open lockfile '%s': %s\n",
                        lockfile, strerror (errno));
            return -1;
        }
        else {
            struct flock filelock;
            filelock.l_type   = F_WRLCK;    //  F_RDLCK, F_WRLCK, F_UNLCK
            filelock.l_whence = SEEK_SET;   //  SEEK_SET, SEEK_CUR, SEEK_END
            filelock.l_start  = 0;          //  Offset from l_whence
            filelock.l_len    = 0;          //  length, 0 = to EOF
            filelock.l_pid    = getpid ();
            if (fcntl (handle, F_SETLK, &filelock)) {
                zclock_log ("E: cannot get lock: %s\n", strerror (errno));
                return -1;
            }
        }
        //   We record the broker's process id in the lock file
        char pid_buffer [10];
        snprintf (pid_buffer, sizeof (pid_buffer), "%6d\n", getpid ());
        if (write (handle, pid_buffer, strlen (pid_buffer)) != strlen (pid_buffer)) {
            zclock_log ("E: cannot write to lockfile: %s\n",
                        strerror (errno));
            return -1;
        }
        close (handle);
    }
    if (group) {
        zclock_log ("I: broker running under group '%s'", group);
        struct group *grpbuf = NULL;
        grpbuf = getgrnam (group);
        if (grpbuf == NULL || setgid (grpbuf->gr_gid)) {
            zclock_log ("E: could not switch group: %s", strerror (errno));
            return -1;
        }
    }
    if (user) {
        zclock_log ("I: broker running under user '%s'", user);
        struct passwd *pwdbuf = NULL;
        pwdbuf = getpwnam (user);
        if (pwdbuf == NULL || setuid (pwdbuf->pw_uid)) {
            zclock_log ("E: could not switch user: %s", strerror (errno));
            return -1;
        }
    }
    else {
        //  Switch back to real user ID (who started process)
        if (setuid (getuid ())) {
            zclock_log ("E: cannot set real user id: %s\n",
                        strerror (errno));
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
//  Configure the number of I/O threads that ZeroMQ will use. A good
//  rule of thumb is one thread per gigabit of traffic in or out. The
//  default is 1, sufficient for most applications. If the environment
//  variable ZSYS_IOTHREADS is defined, that provides the default.
//  Note that this method is valid only before any socket is created.

void
zsys_set_iothreads (size_t iothreads)
{
    ZMUTEX_LOCK (s_mutex);
    //  If the app is misusing this method, burn it with fire
    if (process_ctx)
        zclock_log ("E: zsys_iothreads() is not valid after creating sockets");
    assert (process_ctx == NULL);
    s_iothreads = iothreads;
    ZMUTEX_UNLOCK (s_mutex);
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
    ZMUTEX_LOCK (s_mutex);
    s_rcvhwm = rcvhwm;
    ZMUTEX_UNLOCK (s_mutex);
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
    ZMUTEX_LOCK (s_mutex);
    s_ipv6 = ipv6;
    ZMUTEX_UNLOCK (s_mutex);
}


//  --------------------------------------------------------------------------
//  Set network interface name to use for broadcasts, particularly zbeacon.
//  This lets the interface be configured for test environments where required.
//  For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
//  the default when there is no specified interface. If the environment
//  variable ZSYS_INTERFACE is set, use that as the default interface name.

void
zsys_set_interface (const char *value)
{
    free (s_interface);
    s_interface = strdup (value);
}


//  --------------------------------------------------------------------------
//  Return network interface to use for broadcasts, or "" if none was set.

const char *
zsys_interface (void)
{
    return s_interface? s_interface: "";
}


//  --------------------------------------------------------------------------
//  Selftest

void
zsys_test (bool verbose)
{
    printf (" * zsys: ");

    //  @selftest
    zsys_catch_interrupts ();
    int rc;

    zsys_set_iothreads (1);
    zsys_set_linger (0);
    zsys_set_sndhwm (1000);
    zsys_set_rcvhwm (1000);
    zsys_set_ipv6 (0);

    void *handle = zsys_socket (ZMQ_ROUTER, __FILE__, __LINE__);
    //  Sanity check on libzmq/CZMQ build consistency
#if defined (ZMQ_CURVE_SERVER) && defined (HAVE_LIBSODIUM)
    int as_server = 1;
    rc = zmq_setsockopt (handle, ZMQ_CURVE_SERVER, &as_server, sizeof (int));
    if (rc == -1) {
        zclock_log ("E: libzmq was built without libsodium. Please rebuild libzmq and CZMQ.");
        zsys_close (handle);
        exit (1);
    }
#endif
    if (verbose) {
        char *hostname = zsys_hostname ();
        printf ("I: host name is %s\n", hostname);
        zstr_free (&hostname);
    }
    rc = zsys_close (handle);
    assert (rc == 0);

    rc = zsys_file_delete ("nosuchfile");
    assert (rc == -1);

    bool rc_bool = zsys_file_exists ("nosuchfile");
    assert (rc_bool != true);

    rc = (int) zsys_file_size ("nosuchfile");
    assert (rc == -1);

    time_t when = zsys_file_modified (".");
    assert (when > 0);

    mode_t mode = zsys_file_mode (".");
    assert (S_ISDIR (mode));
    assert (mode & S_IRUSR);
    assert (mode & S_IWUSR);

    zsys_file_mode_private ();
    rc = zsys_dir_create ("%s/%s", ".", ".testsys/subdir");
    assert (rc == 0);
    when = zsys_file_modified ("./.testsys/subdir");
    assert (when > 0);
    assert (!zsys_file_stable ("./.testsys/subdir"));
    rc = zsys_dir_delete ("%s/%s", ".", ".testsys/subdir");
    assert (rc == 0);
    rc = zsys_dir_delete ("%s/%s", ".", ".testsys");
    assert (rc == 0);
    zsys_file_mode_default ();

    int major, minor, patch;
    zsys_version (&major, &minor, &patch);
    assert (major == CZMQ_VERSION_MAJOR);
    assert (minor == CZMQ_VERSION_MINOR);
    assert (patch == CZMQ_VERSION_PATCH);

    char *string = zsys_sprintf ("%s %02x", "Hello", 16);
    assert (streq (string, "Hello 10"));
    zstr_free (&string);

    char *str64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.";
    int num10 = 1234567890;
    string = zsys_sprintf ("%s%s%s%s%d", str64, str64, str64, str64, num10);
    assert (strlen (string) == (4 * 64 + 10));
    zstr_free (&string);

    //  @end

    printf ("OK\n");
}
