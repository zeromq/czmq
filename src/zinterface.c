/*  =========================================================================
    zinterface - Information about network interfaces

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
    The zinterface class contains information about a network interface, 
    namely the IP address, network mask and broadcast address. A list of
    all interfaces present in the system is obtained by the zinterface_list
    function. This list "owns" all the interfaces contained in it, so it is
    only necessary to destroy the list, not every listed interface.
@discuss
@end
*/

#include "platform.h"
#include "czmq.h"

#ifdef __UNIX__
//  --------------------------------------------------------------------------
//  Helper function to verify if one interface's flags are what we want.
static bool s_check_flags(short flags)
{
    return (flags & IFF_UP)             //  Only use interfaces that are running
       && !(flags & IFF_LOOPBACK)       //  Ignore loopback interface
       &&  (flags & IFF_BROADCAST)      //  Only use interfaces that have BROADCAST
    #if defined(IFF_SLAVE)
       && !(flags & IFF_SLAVE)          //  Ignore devices that are bonding slaves.
    #endif
       && !(flags & IFF_POINTOPOINT);   //  Ignore point to point interfaces.
}
#endif

struct _zinterface_t {
    char *name;
    char *address;
    char *netmask;
    char *broadcast;
};

static zinterface_t *
    s_zinterface_new(char *name, inaddr_t address, inaddr_t netmask, inaddr_t broadcast);
static void s_freefn(void *ptr);

//  --------------------------------------------------------------------------
//  Builds a list of available interfaces.
//  Currently implemented for POSIX and Windows

zlist_t *
zinterface_list ()
{
    zlist_t *ifaces = zlist_new();
    assert(ifaces);
#if defined (HAVE_GETIFADDRS)
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        struct ifaddrs *interface = interfaces;
        while (interface) {
            //  On Solaris, loopback interfaces have a NULL in ifa_broadaddr
            if  (interface->ifa_broadaddr
            &&   s_check_flags(interface->ifa_flags)
            &&   interface->ifa_addr
            &&  (interface->ifa_addr->sa_family == AF_INET)) {
                inaddr_t address   = *(inaddr_t*) interface->ifa_addr;
                inaddr_t netmask   = *(inaddr_t*) interface->ifa_netmask;
                inaddr_t broadcast = *(inaddr_t*) interface->ifa_broadaddr;

                //  If the returned broadcast address is the same as source
                //  address, build the broadcast address from the source
                //  address and netmask.
                if (address.sin_addr.s_addr == broadcast.sin_addr.s_addr)
                    broadcast.sin_addr.s_addr |= ~(netmask.sin_addr.s_addr);

                zinterface_t* ziface = s_zinterface_new(interface->ifa_name,
                    address, netmask, broadcast);
                zlist_append(ifaces, ziface);
                zlist_freefn(ifaces, ziface, s_freefn, true);
            }
            interface = interface->ifa_next;
        }
    }
    freeifaddrs (interfaces);

#   elif defined (__UNIX__)
    int sock = socket (AF_INET, SOCK_DGRAM, 0);
    if (sock != -1) {
        int rc;
        int num_interfaces = 0;
        struct ifconf ifconfig;
        ifconfig.ifc_len = 0;
        ifconfig.ifc_req = NULL;

        rc = ioctl (sock, SIOCGIFCONF, (caddr_t) &ifconfig, sizeof (struct ifconf));
        if (rc != -1) {
            ifconfig.ifc_buf = calloc(1, ifconfig.ifc_len);
            rc = ioctl (sock, SIOCGIFCONF, (caddr_t) &ifconfig, sizeof (struct ifconf));
            if (rc != -1) {
                num_interfaces = ifconfig.ifc_len / sizeof (struct ifreq);
            }
        }

        for (int i=0; i<num_interfaces; i++) {
            struct ifreq* ifr = &ifconfig.ifc_req[i];
            rc = ioctl (sock, SIOCGIFFLAGS, (caddr_t) ifr, sizeof (struct ifreq));
            if  (rc == -1) continue;
            if  (!s_check_flags(ifr->ifr_flags)) continue;

            ifr->ifr_addr.sa_family = AF_INET;

            //  Get interface address
            rc = ioctl (sock, SIOCGIFADDR, (caddr_t) ifr, sizeof (struct ifreq));
            if (rc == -1) continue;
            inaddr_t address   = *((inaddr_t*) &ifr->ifr_addr);

            //  Get interface netmask
            rc = ioctl (sock, SIOCGIFBRDADDR, (caddr_t) ifr, sizeof (struct ifreq));
            if (rc == -1) continue;
            inaddr_t broadcast = *((inaddr_t*) &ifr->ifr_addr);

            //  Get interface broadcast address
            rc = ioctl (sock, SIOCGIFNETMASK, (caddr_t) ifr, sizeof (struct ifreq));
            if (rc == -1) continue;
            inaddr_t netmask = *((inaddr_t*) &ifr->ifr_addr);

            zinterface_t* ziface = s_zinterface_new(ifr->ifr_name,
                address, netmask, broadcast);
            zlist_append(ifaces, ziface);
            zlist_freefn(ifaces, ziface, s_freefn, true);
        }

        close(sock);
    }

#   elif defined (__WINDOWS__)
    ULONG addr_size = 0;
    DWORD rc = GetAdaptersAddresses (AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &addr_size);
    assert (rc == ERROR_BUFFER_OVERFLOW);

    PIP_ADAPTER_ADDRESSES pip_addresses = (PIP_ADAPTER_ADDRESSES) malloc (addr_size);
    rc = GetAdaptersAddresses (AF_INET,
        GAA_FLAG_INCLUDE_PREFIX, NULL, pip_addresses, &addr_size);
    assert (rc == NO_ERROR);

    PIP_ADAPTER_ADDRESSES cur_address = pip_addresses;
    while (cur_address) {
        PIP_ADAPTER_UNICAST_ADDRESS pUnicast = cur_address->FirstUnicastAddress;
        PIP_ADAPTER_PREFIX pPrefix = cur_address->FirstPrefix;

        PWCHAR friendlyName = cur_address->FriendlyName;
        size_t friendlyLength = 0;
        size_t asciiSize = wcstombs(0, friendlyName, 0) + 1;
        char *asciiFriendlyName = (char*) zmalloc(asciiSize);
        friendlyLength = wcstombs(asciiFriendlyName, friendlyName, asciiSize);

        int valid = cur_address->OperStatus == IfOperStatusUp;
        
        if (valid && pUnicast && pPrefix) {
            if (pUnicast->Address.lpSockaddr->sa_family != AF_INET) continue;
        
            inaddr_t address   = *(inaddr_t*) pUnicast->Address.lpSockaddr;
            inaddr_t netmask;
            
            if (pPrefix->PrefixLength > 32) continue;
            netmask.sin_addr.s_addr = htonl((0xffffffffU) << (32 - pPrefix->PrefixLength));
            
            inaddr_t broadcast = address;
            broadcast.sin_addr.s_addr |= ~(netmask.sin_addr.s_addr);
                        
            zinterface_t* ziface = s_zinterface_new(asciiFriendlyName,
                address, netmask, broadcast);
            zlist_append(ifaces, ziface);
            zlist_freefn(ifaces, ziface, s_freefn, true);
        }
        free(asciiFriendlyName);
        cur_address = cur_address->Next;
    }
    free (pip_addresses);

#   else
#       error "Interface detection TBD on this operating system"
#   endif

    return ifaces;
}

//  --------------------------------------------------------------------------
//  Private constructor

zinterface_t *
s_zinterface_new(char *name, inaddr_t address, inaddr_t netmask, inaddr_t broadcast)
{
    assert(name);
    zinterface_t *self = malloc(sizeof(struct _zinterface_t));
    assert(self);
    self->name      = strdup(name);
    self->address   = strdup(inet_ntoa(address.sin_addr));
    self->netmask   = strdup(inet_ntoa(netmask.sin_addr));
    self->broadcast = strdup(inet_ntoa(broadcast.sin_addr));

    assert(self->name);
    assert(self->address);
    assert(self->netmask);
    assert(self->broadcast);
    return self;
}

//  --------------------------------------------------------------------------
//  Copy constructor

zinterface_t *
zinterface_dup (zinterface_t *self)
{
    assert(self);

    zinterface_t *copy = malloc(sizeof(struct _zinterface_t));
    assert(copy);
    copy->name      = strdup(self->name);
    copy->address   = strdup(self->address);
    copy->netmask   = strdup(self->netmask);
    copy->broadcast = strdup(self->broadcast);

    assert(copy->name);
    assert(copy->address);
    assert(copy->netmask);
    assert(copy->broadcast);

    return copy;
}

//  --------------------------------------------------------------------------
//  Destructor

void
s_freefn(void *ptr)
{
    zinterface_t *self = ptr;
    zinterface_destroy(&self);
}

//  --------------------------------------------------------------------------
//  Destructor

void
zinterface_destroy (zinterface_t **self_p)
{
    assert(self_p);
    if (*self_p) {
        zinterface_t *self = *self_p;
        free (self->address);
        free (self->netmask);
        free (self->broadcast);
        free (self);
        *self_p = NULL;
    }
}

//  --------------------------------------------------------------------------

char *
zinterface_name (zinterface_t *self)
{
    assert(self);
    return self->name;
}

//  --------------------------------------------------------------------------

char *
zinterface_address (zinterface_t *self)
{
    assert(self);
    return self->address;
}

//  --------------------------------------------------------------------------

char *
zinterface_broadcast (zinterface_t *self)
{
    assert(self);
    return self->broadcast;
}

//  --------------------------------------------------------------------------

char *
zinterface_netmask (zinterface_t *self)
{
    assert(self);
    return self->netmask;
}

//  --------------------------------------------------------------------------

void
zinterface_test(bool verbose)
{
    printf(" * zinterface: ");
    zlist_t *interfaces = zinterface_list();
    assert(interfaces);

    if (verbose) {
        printf("Len: %zu\n", zlist_size(interfaces));
        zinterface_t *iface = zlist_first(interfaces);
        while (iface) {
            printf ("%s\t%s\t%s\t%s\n", zinterface_name(iface), zinterface_address(iface),
                zinterface_netmask(iface), zinterface_broadcast(iface));
            iface = zlist_next(interfaces);
        }
    }

    zlist_destroy(&interfaces);
    printf("OK\n");
}
