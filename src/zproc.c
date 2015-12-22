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
    zproc - process configuration and status
@discuss
@end
*/

#include "czmq_classes.h"

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
    //  @end

    printf ("OK\n");
}
