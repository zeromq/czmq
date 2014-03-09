/*  =========================================================================
    zauth - authentication for ZeroMQ security mechanisms

    -------------------------------------------------------------------------
    Copyright (c) 1991-2014 iMatix Corporation <www.imatix.com>
    Copyright other contributors as noted in the AUTHORS file.

    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This is free software; you can redistribute it and/or modify it under
    the terms of the GNU Lesser General Public License as published by the
    Free Software Foundation; either version 3 of the License, or (at your
    option) any later version.

    This software is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABIL-
    ITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General
    Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
    =========================================================================
*/

#ifndef __ZAUTH_H_INCLUDED__
#define __ZAUTH_H_INCLUDED__

#ifdef __cplusplus
extern "C" {
#endif

//  Opaque class structure
typedef struct _zauth_t zauth_t;

//  @interface
#define CURVE_ALLOW_ANY "*"

//  Constructor
//  Install authentication for the specified context. Returns a new zauth
//  object that you can use to configure authentication. Note that until you
//  add policies, all incoming NULL connections are allowed (classic ZeroMQ
//  behaviour), and all PLAIN and CURVE connections are denied. If there was
//  an error during initialization, returns NULL.
CZMQ_EXPORT zauth_t *
    zauth_new (zctx_t *ctx);
    
//  Allow (whitelist) a single IP address. For NULL, all clients from this
//  address will be accepted. For PLAIN and CURVE, they will be allowed to
//  continue with authentication. You can call this method multiple times 
//  to whitelist multiple IP addresses. If you whitelist a single address,
//  any non-whitelisted addresses are treated as blacklisted.
CZMQ_EXPORT void
    zauth_allow (zauth_t *self, const char *address);

//  Deny (blacklist) a single IP address. For all security mechanisms, this
//  rejects the connection without any further authentication. Use either a
//  whitelist, or a blacklist, not not both. If you define both a whitelist 
//  and a blacklist, only the whitelist takes effect.
CZMQ_EXPORT void
    zauth_deny (zauth_t *self, const char *address);

//  Configure PLAIN authentication for a given domain. PLAIN authentication
//  uses a plain-text password file. To cover all domains, use "*". You can
//  modify the password file at any time; it is reloaded automatically.
CZMQ_EXPORT void
    zauth_configure_plain (zauth_t *self, const char *domain, const char *filename);
    
//  Configure CURVE authentication for a given domain. CURVE authentication
//  uses a directory that holds all public client certificates, i.e. their
//  public keys. The certificates must be in zcert_save () format. To cover
//  all domains, use "*". You can add and remove certificates in that
//  directory at any time. To allow all client keys without checking, specify
//  CURVE_ALLOW_ANY for the location.
CZMQ_EXPORT void
    zauth_configure_curve (zauth_t *self, const char *domain, const char *location);
    
//  Enable verbose tracing of commands and activity
CZMQ_EXPORT void
    zauth_set_verbose (zauth_t *self, bool verbose);
    
//  Destructor
CZMQ_EXPORT void
    zauth_destroy (zauth_t **self_p);

//  Selftest
CZMQ_EXPORT int
    zauth_test (bool verbose);
//  @end

#ifdef __cplusplus
}
#endif

#endif
