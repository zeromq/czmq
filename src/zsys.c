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

#include "../include/czmq.h"

//  We use these variables for signal handling

#if defined (__UNIX__)
static bool s_first_time = true;
static struct sigaction sigint_default;
static struct sigaction sigterm_default;

#elif defined (__WINDOWS__)
static bool s_shim_installed = false;
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
//  Set interrupt handler (NULL means external handler)
//  Idempotent; safe to call multiple times

void
zsys_handler_set (zsys_handler_fn *handler_fn)
{
#if defined (__UNIX__)
    //  Install signal handler for SIGINT and SIGTERM if not NULL
    //  and if this is the first time we've been called
    if (s_first_time) {
        s_first_time = false;
        if (handler_fn) {
            struct sigaction action;
            action.sa_handler = handler_fn;
            action.sa_flags = 0;
            sigemptyset (&action.sa_mask);
            sigaction (SIGINT, &action, &sigint_default);
            sigaction (SIGTERM, &action, &sigterm_default);
        }
        else {
            //  Save default handlers if not already done
            sigaction (SIGINT, NULL, &sigint_default);
            sigaction (SIGTERM, NULL, &sigterm_default);
        }
    }
#elif defined (__WINDOWS__)
    installed_handler_fn = handler_fn;
    if (!s_shim_installed) {
        s_shim_installed = true;
        SetConsoleCtrlHandler (s_handler_fn_shim, TRUE);
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
    if (sigint_default.sa_handler) {
        sigaction (SIGINT, &sigint_default, NULL);
        sigaction (SIGTERM, &sigterm_default, NULL);
        sigint_default.sa_handler = NULL;
        sigterm_default.sa_handler = NULL;
        s_first_time = true;
    }
#elif defined (__WINDOWS__)
    if (s_shim_installed) {
        SetConsoleCtrlHandler (s_handler_fn_shim, FALSE);
        s_shim_installed = false;
    }
    installed_handler_fn = NULL;
#endif
}


//  --------------------------------------------------------------------------
//  Set network interface name to use for broadcasts
//  Use this to force the interface for beacons
//  This is experimental; may be merged into zbeacon class.

//  NOT thread safe, not a good design; this is to test the feasibility
//  of forcing a network interface name instead of writing code to find it.
static char *
    s_interface = NULL;

void
zsys_set_interface (const char *interface_name)
{
    free (s_interface);
    s_interface = strdup (interface_name);
}


//  Return network interface name to use for broadcasts.
//  Returns "" if no interface was set.

char *
zsys_interface (void)
{
    if (s_interface)
        return s_interface;

    //  If the environment variable ZSYS_INTERFACE is set, use that as
    //  the default interface name. This lets the environment variable
    //  be configured for test environments where required. For example,
    //  on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is the
    //  default when there is no specified interface.
    char *env = getenv ("ZSYS_INTERFACE");
    if (env)
        return env;
    return "";
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
//  Return file mode

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
        mode |= S_IREAD;
    if (!(dwfa & FILE_ATTRIBUTE_READONLY))
        mode |= S_IWRITE;

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
#ifdef _MSC_VER
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
//  Selftest

static char *
s_vprintf (const char *format, ...)
{
    va_list argptr;
    va_start (argptr, format);
    char *string = zsys_vprintf (format, argptr);
    va_end (argptr);
    return (string);
}

int
zsys_test (bool verbose)
{
    printf (" * zsys: ");

    //  @selftest
    zsys_handler_reset ();
    zsys_handler_set (NULL);
    zsys_handler_set (NULL);
    zsys_handler_reset ();
    zsys_handler_reset ();

    int rc = zsys_file_delete ("nosuchfile");
    assert (rc == -1);

    bool rc_bool = zsys_file_exists ("nosuchfile");
    assert (rc_bool != true);

    rc = (int) zsys_file_size ("nosuchfile");
    assert (rc == -1);

    time_t when = zsys_file_modified (".");
    assert (when > 0);

    rc = zsys_dir_create ("%s/%s", ".", ".testsys/subdir");
    assert (rc == 0);
    when = zsys_file_modified ("./.testsys/subdir");
    assert (when > 0);
    rc = zsys_dir_delete ("%s/%s", ".", ".testsys/subdir");
    assert (rc == 0);
    rc = zsys_dir_delete ("%s/%s", ".", ".testsys");
    assert (rc == 0);

    int major, minor, patch;
    zsys_version (&major, &minor, &patch);
    assert (major == CZMQ_VERSION_MAJOR);
    assert (minor == CZMQ_VERSION_MINOR);
    assert (patch == CZMQ_VERSION_PATCH);

    char *string = s_vprintf ("%s %02x", "Hello", 16);
    assert (streq (string, "Hello 10"));
    zstr_free (&string);

    char *str64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.";
    int num10 = 1234567890;
    string = s_vprintf ("%s%s%s%s%d", str64, str64, str64, str64, num10);
    assert (strlen (string) == (4 * 64 + 10));
    zstr_free (&string);
    //  @end

    printf ("OK\n");
    return 0;
}
