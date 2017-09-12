################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
from .utils import *
from . import native
from . import destructors
libczmq = native.lib
libczmq_destructors = destructors.lib
ffi = native.ffi

class Zproc(object):
    """
    process configuration and status
    """

    def __init__(self):
        """
        Create a new zproc.
        NOTE: On Windows and with libzmq3 and libzmq2 this function
        returns NULL. Code needs to be ported there.
        """
        p = libczmq.zproc_new(self._p)
        if p == ffi.NULL:
            raise MemoryError("Could not allocate person")

        # ffi.gc returns a copy of the cdata object which will have the
        # destructor called when the Python object is GC'd:
        # https://cffi.readthedocs.org/en/latest/using.html#ffi-interface
        self._p = ffi.gc(p, libczmq_destructors.zproc_destroy_py)

    def set_args(self, args):
        """
        Setup the command line arguments, the first item must be an (absolute) filename
        to run.
        """
        return libczmq.zproc_set_args(self._p, args._p)

    def set_env(self, args):
        """
        Setup the environment variables for the process.
        """
        return libczmq.zproc_set_env(self._p, args._p)

    def set_stdin(self, socket):
        """
        Connects process stdin with a readable ('>', connect) zeromq socket. If
        socket argument is NULL, zproc creates own managed pair of inproc
        sockets.  The writable one is then accessbile via zproc_stdin method.
        """
        return libczmq.zproc_set_stdin(self._p, socket._p)

    def set_stdout(self, socket):
        """
        Connects process stdout with a writable ('@', bind) zeromq socket. If
        socket argument is NULL, zproc creates own managed pair of inproc
        sockets.  The readable one is then accessbile via zproc_stdout method.
        """
        return libczmq.zproc_set_stdout(self._p, socket._p)

    def set_stderr(self, socket):
        """
        Connects process stderr with a writable ('@', bind) zeromq socket. If
        socket argument is NULL, zproc creates own managed pair of inproc
        sockets.  The readable one is then accessbile via zproc_stderr method.
        """
        return libczmq.zproc_set_stderr(self._p, socket._p)

    def stdin(self):
        """
        Return subprocess stdin writable socket. NULL for
        not initialized or external sockets.
        """
        return libczmq.zproc_stdin(self._p)

    def stdout(self):
        """
        Return subprocess stdout readable socket. NULL for
        not initialized or external sockets.
        """
        return libczmq.zproc_stdout(self._p)

    def stderr(self):
        """
        Return subprocess stderr readable socket. NULL for
        not initialized or external sockets.
        """
        return libczmq.zproc_stderr(self._p)

    def run(self):
        """
        Starts the process.
        """
        return libczmq.zproc_run(self._p)

    def returncode(self):
        """
        process exit code
        """
        return libczmq.zproc_returncode(self._p)

    def pid(self):
        """
        PID of the process
        """
        return libczmq.zproc_pid(self._p)

    def running(self):
        """
        return true if process is running, false if not yet started or finished
        """
        return libczmq.zproc_running(self._p)

    def wait(self, hang):
        """
        wait or poll process status, return return code
        """
        return libczmq.zproc_wait(self._p, hang)

    def actor(self):
        """
        return internal actor, usefull for the polling if process died
        """
        return libczmq.zproc_actor(self._p)

    def kill(self, signal):
        """
        send a signal to the subprocess
        """
        return libczmq.zproc_kill(self._p, signal)

    def set_verbose(self, verbose):
        """
        set verbose mode
        """
        return libczmq.zproc_set_verbose(self._p, verbose)

    def czmq_version():
        """
        Returns CZMQ version as a single 6-digit integer encoding the major
        version (x 10000), the minor version (x 100) and the patch.
        """
        return libczmq.zproc_czmq_version()

    def interrupted():
        """
        Returns true if the process received a SIGINT or SIGTERM signal.
        It is good practice to use this method to exit any infinite loop
        processing messages.
        """
        return libczmq.zproc_interrupted()

    def has_curve():
        """
        Returns true if the underlying libzmq supports CURVE security.
        """
        return libczmq.zproc_has_curve()

    def hostname():
        """
        Return current host name, for use in public tcp:// endpoints.
        If the host name is not resolvable, returns NULL.
        """
        return libczmq.zproc_hostname()

    def daemonize(workdir):
        """
        Move the current process into the background. The precise effect
        depends on the operating system. On POSIX boxes, moves to a specified
        working directory (if specified), closes all file handles, reopens
        stdin, stdout, and stderr to the null device, and sets the process to
        ignore SIGHUP. On Windows, does nothing. Returns 0 if OK, -1 if there
        was an error.
        """
        return libczmq.zproc_daemonize(to_bytes(workdir))

    def run_as(lockfile, group, user):
        """
        Drop the process ID into the lockfile, with exclusive lock, and
        switch the process to the specified group and/or user. Any of the
        arguments may be null, indicating a no-op. Returns 0 on success,
        -1 on failure. Note if you combine this with zsys_daemonize, run
        after, not before that method, or the lockfile will hold the wrong
        process ID.
        """
        return libczmq.zproc_run_as(to_bytes(lockfile), to_bytes(group), to_bytes(user))

    def set_io_threads(io_threads):
        """
        Configure the number of I/O threads that ZeroMQ will use. A good
        rule of thumb is one thread per gigabit of traffic in or out. The
        default is 1, sufficient for most applications. If the environment
        variable ZSYS_IO_THREADS is defined, that provides the default.
        Note that this method is valid only before any socket is created.
        """
        return libczmq.zproc_set_io_threads(io_threads)

    def set_max_sockets(max_sockets):
        """
        Configure the number of sockets that ZeroMQ will allow. The default
        is 1024. The actual limit depends on the system, and you can query it
        by using zsys_socket_limit (). A value of zero means "maximum".
        Note that this method is valid only before any socket is created.
        """
        return libczmq.zproc_set_max_sockets(max_sockets)

    def set_biface(value):
        """
        Set network interface name to use for broadcasts, particularly zbeacon.
        This lets the interface be configured for test environments where required.
        For example, on Mac OS X, zbeacon cannot bind to 255.255.255.255 which is
        the default when there is no specified interface. If the environment
        variable ZSYS_INTERFACE is set, use that as the default interface name.
        Setting the interface to "*" means "use all available interfaces".
        """
        return libczmq.zproc_set_biface(to_bytes(value))

    def biface():
        """
        Return network interface to use for broadcasts, or "" if none was set.
        """
        return libczmq.zproc_biface()

    def set_log_ident(value):
        """
        Set log identity, which is a string that prefixes all log messages sent
        by this process. The log identity defaults to the environment variable
        ZSYS_LOGIDENT, if that is set.
        """
        return libczmq.zproc_set_log_ident(to_bytes(value))

    def set_log_sender(endpoint):
        """
        Sends log output to a PUB socket bound to the specified endpoint. To
        collect such log output, create a SUB socket, subscribe to the traffic
        you care about, and connect to the endpoint. Log traffic is sent as a
        single string frame, in the same format as when sent to stdout. The
        log system supports a single sender; multiple calls to this method will
        bind the same sender to multiple endpoints. To disable the sender, call
        this method with a null argument.
        """
        return libczmq.zproc_set_log_sender(to_bytes(endpoint))

    def set_log_system(logsystem):
        """
        Enable or disable logging to the system facility (syslog on POSIX boxes,
        event log on Windows). By default this is disabled.
        """
        return libczmq.zproc_set_log_system(logsystem)

    def log_error(format, ):
        """
        Log error condition - highest priority
        """
        return libczmq.zproc_log_error(format, )

    def log_warning(format, ):
        """
        Log warning condition - high priority
        """
        return libczmq.zproc_log_warning(format, )

    def log_notice(format, ):
        """
        Log normal, but significant, condition - normal priority
        """
        return libczmq.zproc_log_notice(format, )

    def log_info(format, ):
        """
        Log informational message - low priority
        """
        return libczmq.zproc_log_info(format, )

    def log_debug(format, ):
        """
        Log debug-level message - lowest priority
        """
        return libczmq.zproc_log_debug(format, )

    def test(verbose):
        """
        Self test of this class.
        """
        return libczmq.zproc_test(verbose)

################################################################################
#  THIS FILE IS 100% GENERATED BY ZPROJECT; DO NOT EDIT EXCEPT EXPERIMENTALLY  #
#  Read the zproject/README.md for information about making permanent changes. #
################################################################################
