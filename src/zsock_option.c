/*  =========================================================================
    zsock_option - get/set 0MQ socket options

            ****************************************************
            *   GENERATED SOURCE CODE, DO NOT EDIT!!           *
            *   TO CHANGE THIS, EDIT scripts/sockopts.gsl      *
            *   AND RUN ./generate in models/.                 *
            ****************************************************

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
    The zsock_option class provides access to the 0MQ getsockopt/setsockopt API.
    All methods in this class take a void * "polymorphic" reference, which
    can be either a zsock_t or a zactor_r reference, or a libzmq void *.
@discuss
    This class is generated, using the GSL code generator. See the sockopts
    XML file, which provides the metadata, and the sock_option.gsl template,
    which does the work.
@end
*/

#include "../include/czmq.h"

#if (ZMQ_VERSION_MAJOR == 4)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_TOS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tos (void *self, int tos)
{
    assert (self);
#   if defined (ZMQ_TOS)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TOS, &tos, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TOS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tos (void *self)
{
    assert (self);
#   if defined (ZMQ_TOS)
    int tos;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TOS, &tos, &option_len);
    return tos;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_HANDOVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_router_handover (void *self, int router_handover)
{
    assert (self);
#   if defined (ZMQ_ROUTER_HANDOVER)
    if (zsock_type (self) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_HANDOVER is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_ROUTER_HANDOVER, &router_handover, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_MANDATORY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_router_mandatory (void *self, int router_mandatory)
{
    assert (self);
#   if defined (ZMQ_ROUTER_MANDATORY)
    if (zsock_type (self) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_MANDATORY is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_ROUTER_MANDATORY, &router_mandatory, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_PROBE_ROUTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_probe_router (void *self, int probe_router)
{
    assert (self);
#   if defined (ZMQ_PROBE_ROUTER)
    if (zsock_type (self) != ZMQ_ROUTER
    &&  zsock_type (self) != ZMQ_DEALER
    &&  zsock_type (self) != ZMQ_REQ) {
        printf ("ZMQ_PROBE_ROUTER is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_PROBE_ROUTER, &probe_router, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_REQ_RELAXED value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_req_relaxed (void *self, int req_relaxed)
{
    assert (self);
#   if defined (ZMQ_REQ_RELAXED)
    if (zsock_type (self) != ZMQ_REQ) {
        printf ("ZMQ_REQ_RELAXED is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_REQ_RELAXED, &req_relaxed, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_REQ_CORRELATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_req_correlate (void *self, int req_correlate)
{
    assert (self);
#   if defined (ZMQ_REQ_CORRELATE)
    if (zsock_type (self) != ZMQ_REQ) {
        printf ("ZMQ_REQ_CORRELATE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_REQ_CORRELATE, &req_correlate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CONFLATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_conflate (void *self, int conflate)
{
    assert (self);
#   if defined (ZMQ_CONFLATE)
    if (zsock_type (self) != ZMQ_PUSH
    &&  zsock_type (self) != ZMQ_PULL
    &&  zsock_type (self) != ZMQ_PUB
    &&  zsock_type (self) != ZMQ_SUB
    &&  zsock_type (self) != ZMQ_DEALER) {
        printf ("ZMQ_CONFLATE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CONFLATE, &conflate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_ZAP_DOMAIN value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_zap_domain (void *self, const char * zap_domain)
{
    assert (self);
#   if defined (ZMQ_ZAP_DOMAIN)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_ZAP_DOMAIN, zap_domain, strlen (zap_domain));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_ZAP_DOMAIN value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_zap_domain (void *self)
{
    assert (self);
#   if defined (ZMQ_ZAP_DOMAIN)
    size_t option_len = 255;
    char *zap_domain = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_ZAP_DOMAIN, zap_domain, &option_len);
    return (char *) zap_domain;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_MECHANISM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_mechanism (void *self)
{
    assert (self);
#   if defined (ZMQ_MECHANISM)
    int mechanism;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_MECHANISM, &mechanism, &option_len);
    return mechanism;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_PLAIN_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_plain_server (void *self, int plain_server)
{
    assert (self);
#   if defined (ZMQ_PLAIN_SERVER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_PLAIN_SERVER, &plain_server, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PLAIN_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_plain_server (void *self)
{
    assert (self);
#   if defined (ZMQ_PLAIN_SERVER)
    int plain_server;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_PLAIN_SERVER, &plain_server, &option_len);
    return plain_server;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_PLAIN_USERNAME value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_plain_username (void *self, const char * plain_username)
{
    assert (self);
#   if defined (ZMQ_PLAIN_USERNAME)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_PLAIN_USERNAME, plain_username, strlen (plain_username));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PLAIN_USERNAME value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_plain_username (void *self)
{
    assert (self);
#   if defined (ZMQ_PLAIN_USERNAME)
    size_t option_len = 255;
    char *plain_username = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_PLAIN_USERNAME, plain_username, &option_len);
    return (char *) plain_username;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_PLAIN_PASSWORD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_plain_password (void *self, const char * plain_password)
{
    assert (self);
#   if defined (ZMQ_PLAIN_PASSWORD)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_PLAIN_PASSWORD, plain_password, strlen (plain_password));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_PLAIN_PASSWORD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_plain_password (void *self)
{
    assert (self);
#   if defined (ZMQ_PLAIN_PASSWORD)
    size_t option_len = 255;
    char *plain_password = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_PLAIN_PASSWORD, plain_password, &option_len);
    return (char *) plain_password;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_server (void *self, int curve_server)
{
    assert (self);
#   if defined (ZMQ_CURVE_SERVER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_SERVER, &curve_server, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_curve_server (void *self)
{
    assert (self);
#   if defined (ZMQ_CURVE_SERVER)
    int curve_server;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_CURVE_SERVER, &curve_server, &option_len);
    return curve_server;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_PUBLICKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_publickey (void *self, const char * curve_publickey)
{
    assert (self);
#   if defined (ZMQ_CURVE_PUBLICKEY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_PUBLICKEY, curve_publickey, strlen (curve_publickey));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_PUBLICKEY value from 32-octet binary
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_publickey_bin (void *self, const byte *curve_publickey)
{
#   if defined (ZMQ_CURVE_PUBLICKEY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_PUBLICKEY, curve_publickey, 32);
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_PUBLICKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_curve_publickey (void *self)
{
    assert (self);
#   if defined (ZMQ_CURVE_PUBLICKEY)
    size_t option_len = 40 + 1;     //  Z85 key + terminator
    char *curve_publickey = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_CURVE_PUBLICKEY, curve_publickey, &option_len);
    return (char *) curve_publickey;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SECRETKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_secretkey (void *self, const char * curve_secretkey)
{
    assert (self);
#   if defined (ZMQ_CURVE_SECRETKEY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_SECRETKEY, curve_secretkey, strlen (curve_secretkey));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SECRETKEY value from 32-octet binary
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_secretkey_bin (void *self, const byte *curve_secretkey)
{
#   if defined (ZMQ_CURVE_SECRETKEY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_SECRETKEY, curve_secretkey, 32);
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_SECRETKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_curve_secretkey (void *self)
{
    assert (self);
#   if defined (ZMQ_CURVE_SECRETKEY)
    size_t option_len = 40 + 1;     //  Z85 key + terminator
    char *curve_secretkey = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_CURVE_SECRETKEY, curve_secretkey, &option_len);
    return (char *) curve_secretkey;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SERVERKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_serverkey (void *self, const char * curve_serverkey)
{
    assert (self);
#   if defined (ZMQ_CURVE_SERVERKEY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_SERVERKEY, curve_serverkey, strlen (curve_serverkey));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_CURVE_SERVERKEY value from 32-octet binary
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_curve_serverkey_bin (void *self, const byte *curve_serverkey)
{
#   if defined (ZMQ_CURVE_SERVERKEY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_CURVE_SERVERKEY, curve_serverkey, 32);
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_CURVE_SERVERKEY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_curve_serverkey (void *self)
{
    assert (self);
#   if defined (ZMQ_CURVE_SERVERKEY)
    size_t option_len = 40 + 1;     //  Z85 key + terminator
    char *curve_serverkey = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_CURVE_SERVERKEY, curve_serverkey, &option_len);
    return (char *) curve_serverkey;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_GSSAPI_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_gssapi_server (void *self, int gssapi_server)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_SERVER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_GSSAPI_SERVER, &gssapi_server, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_GSSAPI_SERVER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_gssapi_server (void *self)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_SERVER)
    int gssapi_server;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_GSSAPI_SERVER, &gssapi_server, &option_len);
    return gssapi_server;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_GSSAPI_PLAINTEXT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_gssapi_plaintext (void *self, int gssapi_plaintext)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_PLAINTEXT)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_GSSAPI_PLAINTEXT, &gssapi_plaintext, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_GSSAPI_PLAINTEXT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_gssapi_plaintext (void *self)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_PLAINTEXT)
    int gssapi_plaintext;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_GSSAPI_PLAINTEXT, &gssapi_plaintext, &option_len);
    return gssapi_plaintext;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_GSSAPI_PRINCIPAL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_gssapi_principal (void *self, const char * gssapi_principal)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_PRINCIPAL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_GSSAPI_PRINCIPAL, gssapi_principal, strlen (gssapi_principal));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_GSSAPI_PRINCIPAL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_gssapi_principal (void *self)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_PRINCIPAL)
    size_t option_len = 255;
    char *gssapi_principal = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_GSSAPI_PRINCIPAL, gssapi_principal, &option_len);
    return (char *) gssapi_principal;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_GSSAPI_SERVICE_PRINCIPAL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_gssapi_service_principal (void *self, const char * gssapi_service_principal)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_SERVICE_PRINCIPAL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_GSSAPI_SERVICE_PRINCIPAL, gssapi_service_principal, strlen (gssapi_service_principal));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_GSSAPI_SERVICE_PRINCIPAL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_gssapi_service_principal (void *self)
{
    assert (self);
#   if defined (ZMQ_GSSAPI_SERVICE_PRINCIPAL)
    size_t option_len = 255;
    char *gssapi_service_principal = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_GSSAPI_SERVICE_PRINCIPAL, gssapi_service_principal, &option_len);
    return (char *) gssapi_service_principal;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV6 value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_ipv6 (void *self, int ipv6)
{
    assert (self);
#   if defined (ZMQ_IPV6)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IPV6, &ipv6, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV6 value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_ipv6 (void *self)
{
    assert (self);
#   if defined (ZMQ_IPV6)
    int ipv6;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IPV6, &ipv6, &option_len);
    return ipv6;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_IMMEDIATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_immediate (void *self, int immediate)
{
    assert (self);
#   if defined (ZMQ_IMMEDIATE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IMMEDIATE, &immediate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IMMEDIATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_immediate (void *self)
{
    assert (self);
#   if defined (ZMQ_IMMEDIATE)
    int immediate;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IMMEDIATE, &immediate, &option_len);
    return immediate;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_RAW value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_router_raw (void *self, int router_raw)
{
    assert (self);
#   if defined (ZMQ_ROUTER_RAW)
    if (zsock_type (self) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_RAW is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_ROUTER_RAW, &router_raw, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV4ONLY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_ipv4only (void *self, int ipv4only)
{
    assert (self);
#   if defined (ZMQ_IPV4ONLY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IPV4ONLY, &ipv4only, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV4ONLY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_ipv4only (void *self)
{
    assert (self);
#   if defined (ZMQ_IPV4ONLY)
    int ipv4only;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IPV4ONLY, &ipv4only, &option_len);
    return ipv4only;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_DELAY_ATTACH_ON_CONNECT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect)
{
    assert (self);
#   if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_DELAY_ATTACH_ON_CONNECT, &delay_attach_on_connect, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_type (void *self)
{
    assert (self);
#   if defined (ZMQ_TYPE)
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TYPE, &type, &option_len);
    return type;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndhwm (void *self, int sndhwm)
{
    assert (self);
#   if defined (ZMQ_SNDHWM)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDHWM, &sndhwm, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndhwm (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDHWM)
    int sndhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDHWM, &sndhwm, &option_len);
    return sndhwm;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvhwm (void *self, int rcvhwm)
{
    assert (self);
#   if defined (ZMQ_RCVHWM)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVHWM, &rcvhwm, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvhwm (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVHWM)
    int rcvhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVHWM, &rcvhwm, &option_len);
    return rcvhwm;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_affinity (void *self, int affinity)
{
    assert (self);
#   if defined (ZMQ_AFFINITY)
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_affinity (void *self)
{
    assert (self);
#   if defined (ZMQ_AFFINITY)
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_subscribe (void *self, const char * subscribe)
{
    assert (self);
#   if defined (ZMQ_SUBSCRIBE)
    if (zsock_type (self) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_unsubscribe (void *self, const char * unsubscribe)
{
    assert (self);
#   if defined (ZMQ_UNSUBSCRIBE)
    if (zsock_type (self) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_identity (void *self, const char * identity)
{
    assert (self);
#   if defined (ZMQ_IDENTITY)
    if (zsock_type (self) != ZMQ_REQ
    &&  zsock_type (self) != ZMQ_REP
    &&  zsock_type (self) != ZMQ_DEALER
    &&  zsock_type (self) != ZMQ_ROUTER) {
        printf ("ZMQ_IDENTITY is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_identity (void *self)
{
    assert (self);
#   if defined (ZMQ_IDENTITY)
    size_t option_len = 255;
    char *identity = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rate (void *self, int rate)
{
    assert (self);
#   if defined (ZMQ_RATE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RATE, &rate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rate (void *self)
{
    assert (self);
#   if defined (ZMQ_RATE)
    int rate;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RATE, &rate, &option_len);
    return rate;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_recovery_ivl (void *self, int recovery_ivl)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL, &recovery_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_recovery_ivl (void *self)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL)
    int recovery_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return recovery_ivl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndbuf (void *self, int sndbuf)
{
    assert (self);
#   if defined (ZMQ_SNDBUF)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDBUF, &sndbuf, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndbuf (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDBUF)
    int sndbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDBUF, &sndbuf, &option_len);
    return sndbuf;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvbuf (void *self, int rcvbuf)
{
    assert (self);
#   if defined (ZMQ_RCVBUF)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVBUF, &rcvbuf, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvbuf (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVBUF)
    int rcvbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVBUF, &rcvbuf, &option_len);
    return rcvbuf;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_linger (void *self, int linger)
{
    assert (self);
#   if defined (ZMQ_LINGER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_linger (void *self)
{
    assert (self);
#   if defined (ZMQ_LINGER)
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_LINGER, &linger, &option_len);
    return linger;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_reconnect_ivl (void *self, int reconnect_ivl)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_reconnect_ivl (void *self)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL)
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_reconnect_ivl_max (void *self)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_backlog (void *self, int backlog)
{
    assert (self);
#   if defined (ZMQ_BACKLOG)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_backlog (void *self)
{
    assert (self);
#   if defined (ZMQ_BACKLOG)
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_MAXMSGSIZE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_maxmsgsize (void *self, int maxmsgsize)
{
    assert (self);
#   if defined (ZMQ_MAXMSGSIZE)
    int64_t value = maxmsgsize;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_MAXMSGSIZE, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MAXMSGSIZE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_maxmsgsize (void *self)
{
    assert (self);
#   if defined (ZMQ_MAXMSGSIZE)
    int64_t maxmsgsize;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_MAXMSGSIZE, &maxmsgsize, &option_len);
    return (int) maxmsgsize;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_MULTICAST_HOPS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_multicast_hops (void *self, int multicast_hops)
{
    assert (self);
#   if defined (ZMQ_MULTICAST_HOPS)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_MULTICAST_HOPS, &multicast_hops, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MULTICAST_HOPS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_multicast_hops (void *self)
{
    assert (self);
#   if defined (ZMQ_MULTICAST_HOPS)
    int multicast_hops;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_MULTICAST_HOPS, &multicast_hops, &option_len);
    return multicast_hops;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvtimeo (void *self, int rcvtimeo)
{
    assert (self);
#   if defined (ZMQ_RCVTIMEO)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvtimeo (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVTIMEO)
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndtimeo (void *self, int sndtimeo)
{
    assert (self);
#   if defined (ZMQ_SNDTIMEO)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndtimeo (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDTIMEO)
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_XPUB_VERBOSE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_xpub_verbose (void *self, int xpub_verbose)
{
    assert (self);
#   if defined (ZMQ_XPUB_VERBOSE)
    if (zsock_type (self) != ZMQ_XPUB) {
        printf ("ZMQ_XPUB_VERBOSE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_XPUB_VERBOSE, &xpub_verbose, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive (void *self, int tcp_keepalive)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE, &tcp_keepalive, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE)
    int tcp_keepalive;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE, &tcp_keepalive, &option_len);
    return tcp_keepalive;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_IDLE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_IDLE, &tcp_keepalive_idle, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_IDLE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive_idle (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    int tcp_keepalive_idle;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_IDLE, &tcp_keepalive_idle, &option_len);
    return tcp_keepalive_idle;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_CNT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_CNT)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_CNT, &tcp_keepalive_cnt, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_CNT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive_cnt (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_CNT)
    int tcp_keepalive_cnt;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_CNT, &tcp_keepalive_cnt, &option_len);
    return tcp_keepalive_cnt;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_INTVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_INTVL, &tcp_keepalive_intvl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_INTVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive_intvl (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    int tcp_keepalive_intvl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_INTVL, &tcp_keepalive_intvl, &option_len);
    return tcp_keepalive_intvl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_ACCEPT_FILTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_accept_filter (void *self, const char * tcp_accept_filter)
{
    assert (self);
#   if defined (ZMQ_TCP_ACCEPT_FILTER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_ACCEPT_FILTER, tcp_accept_filter, strlen (tcp_accept_filter));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_ACCEPT_FILTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_tcp_accept_filter (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_ACCEPT_FILTER)
    size_t option_len = 255;
    char *tcp_accept_filter = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_ACCEPT_FILTER, tcp_accept_filter, &option_len);
    return (char *) tcp_accept_filter;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvmore (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVMORE)
    int rcvmore;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVMORE, &rcvmore, &option_len);
    return rcvmore;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

SOCKET
zsock_fd (void *self)
{
    assert (self);
#   if defined (ZMQ_FD)
    SOCKET fd;
    size_t option_len = sizeof (SOCKET);
    zmq_getsockopt (zsock_resolve (self), ZMQ_FD, &fd, &option_len);
    return fd;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_events (void *self)
{
    assert (self);
#   if defined (ZMQ_EVENTS)
    int events;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_EVENTS, &events, &option_len);
    return events;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_LAST_ENDPOINT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_last_endpoint (void *self)
{
    assert (self);
#   if defined (ZMQ_LAST_ENDPOINT)
    size_t option_len = 255;
    char *last_endpoint = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_LAST_ENDPOINT, last_endpoint, &option_len);
    return (char *) last_endpoint;
#   else
    return NULL;
#   endif
}

#endif

#if (ZMQ_VERSION_MAJOR == 3)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_ROUTER_RAW value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_router_raw (void *self, int router_raw)
{
    assert (self);
#   if defined (ZMQ_ROUTER_RAW)
    if (zsock_type (self) != ZMQ_ROUTER) {
        printf ("ZMQ_ROUTER_RAW is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_ROUTER_RAW, &router_raw, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IPV4ONLY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_ipv4only (void *self, int ipv4only)
{
    assert (self);
#   if defined (ZMQ_IPV4ONLY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IPV4ONLY, &ipv4only, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IPV4ONLY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_ipv4only (void *self)
{
    assert (self);
#   if defined (ZMQ_IPV4ONLY)
    int ipv4only;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IPV4ONLY, &ipv4only, &option_len);
    return ipv4only;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_DELAY_ATTACH_ON_CONNECT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_delay_attach_on_connect (void *self, int delay_attach_on_connect)
{
    assert (self);
#   if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_DELAY_ATTACH_ON_CONNECT, &delay_attach_on_connect, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_type (void *self)
{
    assert (self);
#   if defined (ZMQ_TYPE)
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TYPE, &type, &option_len);
    return type;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndhwm (void *self, int sndhwm)
{
    assert (self);
#   if defined (ZMQ_SNDHWM)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDHWM, &sndhwm, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndhwm (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDHWM)
    int sndhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDHWM, &sndhwm, &option_len);
    return sndhwm;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvhwm (void *self, int rcvhwm)
{
    assert (self);
#   if defined (ZMQ_RCVHWM)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVHWM, &rcvhwm, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVHWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvhwm (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVHWM)
    int rcvhwm;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVHWM, &rcvhwm, &option_len);
    return rcvhwm;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_affinity (void *self, int affinity)
{
    assert (self);
#   if defined (ZMQ_AFFINITY)
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_affinity (void *self)
{
    assert (self);
#   if defined (ZMQ_AFFINITY)
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_subscribe (void *self, const char * subscribe)
{
    assert (self);
#   if defined (ZMQ_SUBSCRIBE)
    if (zsock_type (self) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_unsubscribe (void *self, const char * unsubscribe)
{
    assert (self);
#   if defined (ZMQ_UNSUBSCRIBE)
    if (zsock_type (self) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_identity (void *self, const char * identity)
{
    assert (self);
#   if defined (ZMQ_IDENTITY)
    if (zsock_type (self) != ZMQ_REQ
    &&  zsock_type (self) != ZMQ_REP
    &&  zsock_type (self) != ZMQ_DEALER
    &&  zsock_type (self) != ZMQ_ROUTER) {
        printf ("ZMQ_IDENTITY is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_identity (void *self)
{
    assert (self);
#   if defined (ZMQ_IDENTITY)
    size_t option_len = 255;
    char *identity = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rate (void *self, int rate)
{
    assert (self);
#   if defined (ZMQ_RATE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RATE, &rate, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rate (void *self)
{
    assert (self);
#   if defined (ZMQ_RATE)
    int rate;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RATE, &rate, &option_len);
    return rate;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_recovery_ivl (void *self, int recovery_ivl)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL, &recovery_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_recovery_ivl (void *self)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL)
    int recovery_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return recovery_ivl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndbuf (void *self, int sndbuf)
{
    assert (self);
#   if defined (ZMQ_SNDBUF)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDBUF, &sndbuf, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndbuf (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDBUF)
    int sndbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDBUF, &sndbuf, &option_len);
    return sndbuf;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvbuf (void *self, int rcvbuf)
{
    assert (self);
#   if defined (ZMQ_RCVBUF)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVBUF, &rcvbuf, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvbuf (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVBUF)
    int rcvbuf;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVBUF, &rcvbuf, &option_len);
    return rcvbuf;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_linger (void *self, int linger)
{
    assert (self);
#   if defined (ZMQ_LINGER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_linger (void *self)
{
    assert (self);
#   if defined (ZMQ_LINGER)
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_LINGER, &linger, &option_len);
    return linger;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_reconnect_ivl (void *self, int reconnect_ivl)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_reconnect_ivl (void *self)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL)
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_reconnect_ivl_max (void *self)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_backlog (void *self, int backlog)
{
    assert (self);
#   if defined (ZMQ_BACKLOG)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_backlog (void *self)
{
    assert (self);
#   if defined (ZMQ_BACKLOG)
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_MAXMSGSIZE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_maxmsgsize (void *self, int maxmsgsize)
{
    assert (self);
#   if defined (ZMQ_MAXMSGSIZE)
    int64_t value = maxmsgsize;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_MAXMSGSIZE, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MAXMSGSIZE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_maxmsgsize (void *self)
{
    assert (self);
#   if defined (ZMQ_MAXMSGSIZE)
    int64_t maxmsgsize;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_MAXMSGSIZE, &maxmsgsize, &option_len);
    return (int) maxmsgsize;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_MULTICAST_HOPS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_multicast_hops (void *self, int multicast_hops)
{
    assert (self);
#   if defined (ZMQ_MULTICAST_HOPS)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_MULTICAST_HOPS, &multicast_hops, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MULTICAST_HOPS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_multicast_hops (void *self)
{
    assert (self);
#   if defined (ZMQ_MULTICAST_HOPS)
    int multicast_hops;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_MULTICAST_HOPS, &multicast_hops, &option_len);
    return multicast_hops;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvtimeo (void *self, int rcvtimeo)
{
    assert (self);
#   if defined (ZMQ_RCVTIMEO)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvtimeo (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVTIMEO)
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndtimeo (void *self, int sndtimeo)
{
    assert (self);
#   if defined (ZMQ_SNDTIMEO)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndtimeo (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDTIMEO)
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_XPUB_VERBOSE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_xpub_verbose (void *self, int xpub_verbose)
{
    assert (self);
#   if defined (ZMQ_XPUB_VERBOSE)
    if (zsock_type (self) != ZMQ_XPUB) {
        printf ("ZMQ_XPUB_VERBOSE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_XPUB_VERBOSE, &xpub_verbose, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive (void *self, int tcp_keepalive)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE, &tcp_keepalive, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE)
    int tcp_keepalive;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE, &tcp_keepalive, &option_len);
    return tcp_keepalive;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_IDLE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive_idle (void *self, int tcp_keepalive_idle)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_IDLE, &tcp_keepalive_idle, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_IDLE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive_idle (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    int tcp_keepalive_idle;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_IDLE, &tcp_keepalive_idle, &option_len);
    return tcp_keepalive_idle;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_CNT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive_cnt (void *self, int tcp_keepalive_cnt)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_CNT)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_CNT, &tcp_keepalive_cnt, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_CNT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive_cnt (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_CNT)
    int tcp_keepalive_cnt;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_CNT, &tcp_keepalive_cnt, &option_len);
    return tcp_keepalive_cnt;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_KEEPALIVE_INTVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_keepalive_intvl (void *self, int tcp_keepalive_intvl)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_INTVL, &tcp_keepalive_intvl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_KEEPALIVE_INTVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_tcp_keepalive_intvl (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    int tcp_keepalive_intvl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_KEEPALIVE_INTVL, &tcp_keepalive_intvl, &option_len);
    return tcp_keepalive_intvl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_TCP_ACCEPT_FILTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_tcp_accept_filter (void *self, const char * tcp_accept_filter)
{
    assert (self);
#   if defined (ZMQ_TCP_ACCEPT_FILTER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_TCP_ACCEPT_FILTER, tcp_accept_filter, strlen (tcp_accept_filter));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TCP_ACCEPT_FILTER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_tcp_accept_filter (void *self)
{
    assert (self);
#   if defined (ZMQ_TCP_ACCEPT_FILTER)
    size_t option_len = 255;
    char *tcp_accept_filter = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TCP_ACCEPT_FILTER, tcp_accept_filter, &option_len);
    return (char *) tcp_accept_filter;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvmore (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVMORE)
    int rcvmore;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVMORE, &rcvmore, &option_len);
    return rcvmore;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

SOCKET
zsock_fd (void *self)
{
    assert (self);
#   if defined (ZMQ_FD)
    SOCKET fd;
    size_t option_len = sizeof (SOCKET);
    zmq_getsockopt (zsock_resolve (self), ZMQ_FD, &fd, &option_len);
    return fd;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_events (void *self)
{
    assert (self);
#   if defined (ZMQ_EVENTS)
    int events;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_EVENTS, &events, &option_len);
    return events;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_LAST_ENDPOINT value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_last_endpoint (void *self)
{
    assert (self);
#   if defined (ZMQ_LAST_ENDPOINT)
    size_t option_len = 255;
    char *last_endpoint = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_LAST_ENDPOINT, last_endpoint, &option_len);
    return (char *) last_endpoint;
#   else
    return NULL;
#   endif
}

#endif

#if (ZMQ_VERSION_MAJOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_HWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_hwm (void *self, int hwm)
{
    assert (self);
#   if defined (ZMQ_HWM)
    uint64_t value = hwm;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_HWM, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_HWM value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_hwm (void *self)
{
    assert (self);
#   if defined (ZMQ_HWM)
    uint64_t hwm;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_HWM, &hwm, &option_len);
    return (int) hwm;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SWAP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_swap (void *self, int swap)
{
    assert (self);
#   if defined (ZMQ_SWAP)
    int64_t value = swap;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SWAP, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SWAP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_swap (void *self)
{
    assert (self);
#   if defined (ZMQ_SWAP)
    int64_t swap;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SWAP, &swap, &option_len);
    return (int) swap;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_affinity (void *self, int affinity)
{
    assert (self);
#   if defined (ZMQ_AFFINITY)
    uint64_t value = affinity;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_AFFINITY, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_AFFINITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_affinity (void *self)
{
    assert (self);
#   if defined (ZMQ_AFFINITY)
    uint64_t affinity;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_AFFINITY, &affinity, &option_len);
    return (int) affinity;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_identity (void *self, const char * identity)
{
    assert (self);
#   if defined (ZMQ_IDENTITY)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_IDENTITY, identity, strlen (identity));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_IDENTITY value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

char *
zsock_identity (void *self)
{
    assert (self);
#   if defined (ZMQ_IDENTITY)
    size_t option_len = 255;
    char *identity = (char *) zmalloc (option_len);
    zmq_getsockopt (zsock_resolve (self), ZMQ_IDENTITY, identity, &option_len);
    return (char *) identity;
#   else
    return NULL;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rate (void *self, int rate)
{
    assert (self);
#   if defined (ZMQ_RATE)
    int64_t value = rate;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RATE, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RATE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rate (void *self)
{
    assert (self);
#   if defined (ZMQ_RATE)
    int64_t rate;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RATE, &rate, &option_len);
    return (int) rate;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_recovery_ivl (void *self, int recovery_ivl)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL)
    int64_t value = recovery_ivl;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_recovery_ivl (void *self)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL)
    int64_t recovery_ivl;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL, &recovery_ivl, &option_len);
    return (int) recovery_ivl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECOVERY_IVL_MSEC value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_recovery_ivl_msec (void *self, int recovery_ivl_msec)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL_MSEC)
    int64_t value = recovery_ivl_msec;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL_MSEC, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECOVERY_IVL_MSEC value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_recovery_ivl_msec (void *self)
{
    assert (self);
#   if defined (ZMQ_RECOVERY_IVL_MSEC)
    int64_t recovery_ivl_msec;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECOVERY_IVL_MSEC, &recovery_ivl_msec, &option_len);
    return (int) recovery_ivl_msec;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_MCAST_LOOP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_mcast_loop (void *self, int mcast_loop)
{
    assert (self);
#   if defined (ZMQ_MCAST_LOOP)
    int64_t value = mcast_loop;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_MCAST_LOOP, &value, sizeof (int64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_MCAST_LOOP value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_mcast_loop (void *self)
{
    assert (self);
#   if defined (ZMQ_MCAST_LOOP)
    int64_t mcast_loop;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_MCAST_LOOP, &mcast_loop, &option_len);
    return (int) mcast_loop;
#   else
    return 0;
#   endif
}

#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvtimeo (void *self, int rcvtimeo)
{
    assert (self);
#   if defined (ZMQ_RCVTIMEO)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVTIMEO, &rcvtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvtimeo (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVTIMEO)
    int rcvtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVTIMEO, &rcvtimeo, &option_len);
    return rcvtimeo;
#   else
    return 0;
#   endif
}

#   endif
#   if (ZMQ_VERSION_MINOR == 2)
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndtimeo (void *self, int sndtimeo)
{
    assert (self);
#   if defined (ZMQ_SNDTIMEO)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDTIMEO, &sndtimeo, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDTIMEO value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndtimeo (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDTIMEO)
    int sndtimeo;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDTIMEO, &sndtimeo, &option_len);
    return sndtimeo;
#   else
    return 0;
#   endif
}

#   endif
//  --------------------------------------------------------------------------
//  Set socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_sndbuf (void *self, int sndbuf)
{
    assert (self);
#   if defined (ZMQ_SNDBUF)
    uint64_t value = sndbuf;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SNDBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_SNDBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_sndbuf (void *self)
{
    assert (self);
#   if defined (ZMQ_SNDBUF)
    uint64_t sndbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_SNDBUF, &sndbuf, &option_len);
    return (int) sndbuf;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_rcvbuf (void *self, int rcvbuf)
{
    assert (self);
#   if defined (ZMQ_RCVBUF)
    uint64_t value = rcvbuf;
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RCVBUF, &value, sizeof (uint64_t));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVBUF value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvbuf (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVBUF)
    uint64_t rcvbuf;
    size_t option_len = sizeof (uint64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVBUF, &rcvbuf, &option_len);
    return (int) rcvbuf;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_linger (void *self, int linger)
{
    assert (self);
#   if defined (ZMQ_LINGER)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_LINGER, &linger, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_LINGER value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_linger (void *self)
{
    assert (self);
#   if defined (ZMQ_LINGER)
    int linger;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_LINGER, &linger, &option_len);
    return linger;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_reconnect_ivl (void *self, int reconnect_ivl)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL, &reconnect_ivl, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_reconnect_ivl (void *self)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL)
    int reconnect_ivl;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL, &reconnect_ivl, &option_len);
    return reconnect_ivl;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_reconnect_ivl_max (void *self, int reconnect_ivl_max)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_RECONNECT_IVL_MAX value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_reconnect_ivl_max (void *self)
{
    assert (self);
#   if defined (ZMQ_RECONNECT_IVL_MAX)
    int reconnect_ivl_max;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RECONNECT_IVL_MAX, &reconnect_ivl_max, &option_len);
    return reconnect_ivl_max;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_backlog (void *self, int backlog)
{
    assert (self);
#   if defined (ZMQ_BACKLOG)
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_BACKLOG, &backlog, sizeof (int));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_BACKLOG value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_backlog (void *self)
{
    assert (self);
#   if defined (ZMQ_BACKLOG)
    int backlog;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_BACKLOG, &backlog, &option_len);
    return backlog;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Set socket ZMQ_SUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_subscribe (void *self, const char * subscribe)
{
    assert (self);
#   if defined (ZMQ_SUBSCRIBE)
    if (zsock_type (self) != ZMQ_SUB) {
        printf ("ZMQ_SUBSCRIBE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_SUBSCRIBE, subscribe, strlen (subscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Set socket ZMQ_UNSUBSCRIBE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_set_unsubscribe (void *self, const char * unsubscribe)
{
    assert (self);
#   if defined (ZMQ_UNSUBSCRIBE)
    if (zsock_type (self) != ZMQ_SUB) {
        printf ("ZMQ_UNSUBSCRIBE is not valid on %s sockets\n", zsys_sockname (zsock_type (self)));
        assert (false);
    }
    int rc = zmq_setsockopt (zsock_resolve (self), ZMQ_UNSUBSCRIBE, unsubscribe, strlen (unsubscribe));
    assert (rc == 0 || zmq_errno () == ETERM);
#   endif
}


//  --------------------------------------------------------------------------
//  Return socket ZMQ_TYPE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_type (void *self)
{
    assert (self);
#   if defined (ZMQ_TYPE)
    int type;
    size_t option_len = sizeof (int);
    zmq_getsockopt (zsock_resolve (self), ZMQ_TYPE, &type, &option_len);
    return type;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_RCVMORE value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_rcvmore (void *self)
{
    assert (self);
#   if defined (ZMQ_RCVMORE)
    int64_t rcvmore;
    size_t option_len = sizeof (int64_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_RCVMORE, &rcvmore, &option_len);
    return (int) rcvmore;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_FD value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

SOCKET
zsock_fd (void *self)
{
    assert (self);
#   if defined (ZMQ_FD)
    SOCKET fd;
    size_t option_len = sizeof (SOCKET);
    zmq_getsockopt (zsock_resolve (self), ZMQ_FD, &fd, &option_len);
    return fd;
#   else
    return 0;
#   endif
}

//  --------------------------------------------------------------------------
//  Return socket ZMQ_EVENTS value
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

int
zsock_events (void *self)
{
    assert (self);
#   if defined (ZMQ_EVENTS)
    uint32_t events;
    size_t option_len = sizeof (uint32_t);
    zmq_getsockopt (zsock_resolve (self), ZMQ_EVENTS, &events, &option_len);
    return (int) events;
#   else
    return 0;
#   endif
}

#endif

//  --------------------------------------------------------------------------
//  Selftest
//  *** GENERATED SOURCE CODE, DO NOT EDIT, SEE INSTRUCTIONS AT START ***

void
zsock_option_test (bool verbose)
{
    printf (" * zsock_option: ");

    //  @selftest
    zsock_t *self;
#if (ZMQ_VERSION_MAJOR == 4)
#     if defined (ZMQ_TOS)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_tos (self, 1);
    assert (zsock_tos (self) == 1);
    zsock_tos (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_ROUTER_HANDOVER)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_handover (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_ROUTER_MANDATORY)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_mandatory (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_PROBE_ROUTER)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_probe_router (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_REQ_RELAXED)
    self = zsock_new (ZMQ_REQ);
    assert (self);
    zsock_set_req_relaxed (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_REQ_CORRELATE)
    self = zsock_new (ZMQ_REQ);
    assert (self);
    zsock_set_req_correlate (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_CONFLATE)
    self = zsock_new (ZMQ_PUSH);
    assert (self);
    zsock_set_conflate (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_ZAP_DOMAIN)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_zap_domain (self, "test");
    char *zap_domain = zsock_zap_domain (self);
    assert (zap_domain);
    free (zap_domain);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_MECHANISM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_mechanism (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_PLAIN_SERVER)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_plain_server (self, 1);
    assert (zsock_plain_server (self) == 1);
    zsock_plain_server (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_PLAIN_USERNAME)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_plain_username (self, "test");
    char *plain_username = zsock_plain_username (self);
    assert (plain_username);
    free (plain_username);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_PLAIN_PASSWORD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_plain_password (self, "test");
    char *plain_password = zsock_plain_password (self);
    assert (plain_password);
    free (plain_password);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IPV6)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_ipv6 (self, 1);
    assert (zsock_ipv6 (self) == 1);
    zsock_ipv6 (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IMMEDIATE)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_immediate (self, 1);
    assert (zsock_immediate (self) == 1);
    zsock_immediate (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_ROUTER_RAW)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_raw (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IPV4ONLY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_ipv4only (self, 1);
    assert (zsock_ipv4only (self) == 1);
    zsock_ipv4only (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_delay_attach_on_connect (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TYPE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_type (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SNDHWM)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndhwm (self, 1);
    assert (zsock_sndhwm (self) == 1);
    zsock_sndhwm (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVHWM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvhwm (self, 1);
    assert (zsock_rcvhwm (self) == 1);
    zsock_rcvhwm (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_AFFINITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_affinity (self, 1);
    assert (zsock_affinity (self) == 1);
    zsock_affinity (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_subscribe (self, "test");
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_UNSUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_unsubscribe (self, "test");
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IDENTITY)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_identity (self, "test");
    char *identity = zsock_identity (self);
    assert (identity);
    free (identity);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RATE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rate (self, 1);
    assert (zsock_rate (self) == 1);
    zsock_rate (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECOVERY_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl (self, 1);
    assert (zsock_recovery_ivl (self) == 1);
    zsock_recovery_ivl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SNDBUF)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndbuf (self, 1);
    assert (zsock_sndbuf (self) == 1);
    zsock_sndbuf (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvbuf (self, 1);
    assert (zsock_rcvbuf (self) == 1);
    zsock_rcvbuf (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_LINGER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_linger (self, 1);
    assert (zsock_linger (self) == 1);
    zsock_linger (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECONNECT_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl (self, 1);
    assert (zsock_reconnect_ivl (self) == 1);
    zsock_reconnect_ivl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECONNECT_IVL_MAX)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl_max (self, 1);
    assert (zsock_reconnect_ivl_max (self) == 1);
    zsock_reconnect_ivl_max (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_BACKLOG)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_backlog (self, 1);
    assert (zsock_backlog (self) == 1);
    zsock_backlog (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_MAXMSGSIZE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_maxmsgsize (self, 1);
    assert (zsock_maxmsgsize (self) == 1);
    zsock_maxmsgsize (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_MULTICAST_HOPS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_multicast_hops (self, 1);
    assert (zsock_multicast_hops (self) == 1);
    zsock_multicast_hops (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvtimeo (self, 1);
    assert (zsock_rcvtimeo (self) == 1);
    zsock_rcvtimeo (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SNDTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndtimeo (self, 1);
    assert (zsock_sndtimeo (self) == 1);
    zsock_sndtimeo (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_XPUB_VERBOSE)
    self = zsock_new (ZMQ_XPUB);
    assert (self);
    zsock_set_xpub_verbose (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive (self, 1);
    assert (zsock_tcp_keepalive (self) == 1);
    zsock_tcp_keepalive (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_idle (self, 1);
    assert (zsock_tcp_keepalive_idle (self) == 1);
    zsock_tcp_keepalive_idle (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE_CNT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_cnt (self, 1);
    assert (zsock_tcp_keepalive_cnt (self) == 1);
    zsock_tcp_keepalive_cnt (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_intvl (self, 1);
    assert (zsock_tcp_keepalive_intvl (self) == 1);
    zsock_tcp_keepalive_intvl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_ACCEPT_FILTER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_accept_filter (self, "127.0.0.1");
    char *tcp_accept_filter = zsock_tcp_accept_filter (self);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVMORE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_rcvmore (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_FD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_fd (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_EVENTS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_events (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_LAST_ENDPOINT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    char *last_endpoint = zsock_last_endpoint (self);
    assert (last_endpoint);
    free (last_endpoint);
    zsock_destroy (&self);
#     endif
#endif

#if (ZMQ_VERSION_MAJOR == 3)
#     if defined (ZMQ_ROUTER_RAW)
    self = zsock_new (ZMQ_ROUTER);
    assert (self);
    zsock_set_router_raw (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IPV4ONLY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_ipv4only (self, 1);
    assert (zsock_ipv4only (self) == 1);
    zsock_ipv4only (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_DELAY_ATTACH_ON_CONNECT)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_delay_attach_on_connect (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TYPE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_type (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SNDHWM)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndhwm (self, 1);
    assert (zsock_sndhwm (self) == 1);
    zsock_sndhwm (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVHWM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvhwm (self, 1);
    assert (zsock_rcvhwm (self) == 1);
    zsock_rcvhwm (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_AFFINITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_affinity (self, 1);
    assert (zsock_affinity (self) == 1);
    zsock_affinity (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_subscribe (self, "test");
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_UNSUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_unsubscribe (self, "test");
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IDENTITY)
    self = zsock_new (ZMQ_DEALER);
    assert (self);
    zsock_set_identity (self, "test");
    char *identity = zsock_identity (self);
    assert (identity);
    free (identity);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RATE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rate (self, 1);
    assert (zsock_rate (self) == 1);
    zsock_rate (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECOVERY_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl (self, 1);
    assert (zsock_recovery_ivl (self) == 1);
    zsock_recovery_ivl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SNDBUF)
    self = zsock_new (ZMQ_PUB);
    assert (self);
    zsock_set_sndbuf (self, 1);
    assert (zsock_sndbuf (self) == 1);
    zsock_sndbuf (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvbuf (self, 1);
    assert (zsock_rcvbuf (self) == 1);
    zsock_rcvbuf (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_LINGER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_linger (self, 1);
    assert (zsock_linger (self) == 1);
    zsock_linger (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECONNECT_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl (self, 1);
    assert (zsock_reconnect_ivl (self) == 1);
    zsock_reconnect_ivl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECONNECT_IVL_MAX)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl_max (self, 1);
    assert (zsock_reconnect_ivl_max (self) == 1);
    zsock_reconnect_ivl_max (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_BACKLOG)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_backlog (self, 1);
    assert (zsock_backlog (self) == 1);
    zsock_backlog (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_MAXMSGSIZE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_maxmsgsize (self, 1);
    assert (zsock_maxmsgsize (self) == 1);
    zsock_maxmsgsize (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_MULTICAST_HOPS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_multicast_hops (self, 1);
    assert (zsock_multicast_hops (self) == 1);
    zsock_multicast_hops (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvtimeo (self, 1);
    assert (zsock_rcvtimeo (self) == 1);
    zsock_rcvtimeo (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SNDTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndtimeo (self, 1);
    assert (zsock_sndtimeo (self) == 1);
    zsock_sndtimeo (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_XPUB_VERBOSE)
    self = zsock_new (ZMQ_XPUB);
    assert (self);
    zsock_set_xpub_verbose (self, 1);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive (self, 1);
    assert (zsock_tcp_keepalive (self) == 1);
    zsock_tcp_keepalive (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE_IDLE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_idle (self, 1);
    assert (zsock_tcp_keepalive_idle (self) == 1);
    zsock_tcp_keepalive_idle (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE_CNT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_cnt (self, 1);
    assert (zsock_tcp_keepalive_cnt (self) == 1);
    zsock_tcp_keepalive_cnt (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_KEEPALIVE_INTVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_keepalive_intvl (self, 1);
    assert (zsock_tcp_keepalive_intvl (self) == 1);
    zsock_tcp_keepalive_intvl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TCP_ACCEPT_FILTER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_tcp_accept_filter (self, "127.0.0.1");
    char *tcp_accept_filter = zsock_tcp_accept_filter (self);
    assert (tcp_accept_filter);
    free (tcp_accept_filter);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVMORE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_rcvmore (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_FD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_fd (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_EVENTS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_events (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_LAST_ENDPOINT)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    char *last_endpoint = zsock_last_endpoint (self);
    assert (last_endpoint);
    free (last_endpoint);
    zsock_destroy (&self);
#     endif
#endif

#if (ZMQ_VERSION_MAJOR == 2)
#     if defined (ZMQ_HWM)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_hwm (self, 1);
    assert (zsock_hwm (self) == 1);
    zsock_hwm (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SWAP)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_swap (self, 1);
    assert (zsock_swap (self) == 1);
    zsock_swap (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_AFFINITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_affinity (self, 1);
    assert (zsock_affinity (self) == 1);
    zsock_affinity (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_IDENTITY)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_identity (self, "test");
    char *identity = zsock_identity (self);
    assert (identity);
    free (identity);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RATE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rate (self, 1);
    assert (zsock_rate (self) == 1);
    zsock_rate (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECOVERY_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl (self, 1);
    assert (zsock_recovery_ivl (self) == 1);
    zsock_recovery_ivl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECOVERY_IVL_MSEC)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_recovery_ivl_msec (self, 1);
    assert (zsock_recovery_ivl_msec (self) == 1);
    zsock_recovery_ivl_msec (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_MCAST_LOOP)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_mcast_loop (self, 1);
    assert (zsock_mcast_loop (self) == 1);
    zsock_mcast_loop (self);
    zsock_destroy (&self);
#     endif
#   if (ZMQ_VERSION_MINOR == 2)
#     if defined (ZMQ_RCVTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvtimeo (self, 1);
    assert (zsock_rcvtimeo (self) == 1);
    zsock_rcvtimeo (self);
    zsock_destroy (&self);
#     endif
#   endif
#   if (ZMQ_VERSION_MINOR == 2)
#     if defined (ZMQ_SNDTIMEO)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndtimeo (self, 1);
    assert (zsock_sndtimeo (self) == 1);
    zsock_sndtimeo (self);
    zsock_destroy (&self);
#     endif
#   endif
#     if defined (ZMQ_SNDBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_sndbuf (self, 1);
    assert (zsock_sndbuf (self) == 1);
    zsock_sndbuf (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVBUF)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_rcvbuf (self, 1);
    assert (zsock_rcvbuf (self) == 1);
    zsock_rcvbuf (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_LINGER)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_linger (self, 1);
    assert (zsock_linger (self) == 1);
    zsock_linger (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECONNECT_IVL)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl (self, 1);
    assert (zsock_reconnect_ivl (self) == 1);
    zsock_reconnect_ivl (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RECONNECT_IVL_MAX)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_reconnect_ivl_max (self, 1);
    assert (zsock_reconnect_ivl_max (self) == 1);
    zsock_reconnect_ivl_max (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_BACKLOG)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_backlog (self, 1);
    assert (zsock_backlog (self) == 1);
    zsock_backlog (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_SUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_subscribe (self, "test");
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_UNSUBSCRIBE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_set_unsubscribe (self, "test");
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_TYPE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_type (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_RCVMORE)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_rcvmore (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_FD)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_fd (self);
    zsock_destroy (&self);
#     endif
#     if defined (ZMQ_EVENTS)
    self = zsock_new (ZMQ_SUB);
    assert (self);
    zsock_events (self);
    zsock_destroy (&self);
#     endif
#endif

    //  @end

    printf ("OK\n");
}
