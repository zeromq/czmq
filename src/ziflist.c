/*  =========================================================================
    ziflist - List of network interfaces available on system

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
    The ziflist class takes a snapshot of the network interfaces that the
    system currently supports (this can change arbitrarily, especially on
    mobile devices). The caller can then access the network interface
    information using an iterator that works like zlistx. Only stores those
    interfaces with broadcast capability, and ignores the loopback interface.
@discuss
@end
*/

#include "platform.h"
#include "../include/czmq.h"

//  Structure of an interface
typedef struct {
    char *name;
    char *address;
    char *netmask;
    char *broadcast;
} interface_t;


//  --------------------------------------------------------------------------
//  interface destructor

static void
s_interface_destroy (interface_t **self_p)
{
    assert (self_p);
    interface_t *self = *self_p;
    if (self) {
        free (self->name);
        free (self->address);
        free (self->netmask);
        free (self->broadcast);
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  interface constructor

static interface_t *
s_interface_new (char *name, inaddr_t address, inaddr_t netmask, inaddr_t broadcast)
{
    interface_t *self = (interface_t *) zmalloc (sizeof (interface_t));
    if (!self)
        return NULL;
    self->name = strdup (name);
    if (self->name)
        self->address = strdup (inet_ntoa (address.sin_addr));
    if (self->address)
        self->netmask = strdup (inet_ntoa (netmask.sin_addr));
    if (self->netmask)
        self->broadcast = strdup (inet_ntoa (broadcast.sin_addr));
    if (!self->broadcast)
        s_interface_destroy (&self);
    return self;
}

//  Structure of our class
struct _ziflist_t;

//  --------------------------------------------------------------------------
//  Get a list of network interfaces currently defined on the system

ziflist_t *
ziflist_new (void)
{
    zlistx_t *list = zlistx_new ();
    ziflist_t *self = (ziflist_t *) list;
    if (self) {
        zlistx_set_destructor (list, (czmq_destructor *) s_interface_destroy);
        ziflist_reload (self);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a ziflist instance

void
ziflist_destroy (ziflist_t **self_p)
{
    zlistx_destroy ((zlistx_t **) self_p);
}


#ifdef __UNIX__
//  --------------------------------------------------------------------------
//  Helper function to verify if one interface's flags are what we want.

static bool
s_valid_flags (short flags)
{
    return (flags & IFF_UP)             //  Only use interfaces that are running
           && !(flags & IFF_LOOPBACK)   //  Ignore loopback interface
           && (flags & IFF_BROADCAST)   //  Only use interfaces that have BROADCAST
#   if defined (IFF_SLAVE)
           && !(flags & IFF_SLAVE)      //  Ignore devices that are bonding slaves.
#   endif
           && !(flags & IFF_POINTOPOINT); //  Ignore point to point interfaces.
}
#endif


//  --------------------------------------------------------------------------
//  Reload network interfaces from system

void
ziflist_reload (ziflist_t *self)
{
    assert (self);
    zlistx_t *list = (zlistx_t *) self;
    zlistx_purge (list);

#if defined (HAVE_GETIFADDRS)
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        struct ifaddrs *interface = interfaces;
        while (interface) {
            //  On Solaris, loopback interfaces have a NULL in ifa_broadaddr
            if (  interface->ifa_broadaddr
               && interface->ifa_addr
               && interface->ifa_addr->sa_family == AF_INET
               && s_valid_flags (interface->ifa_flags)) {
                inaddr_t address = *(inaddr_t *) interface->ifa_addr;
                inaddr_t netmask = *(inaddr_t *) interface->ifa_netmask;
                inaddr_t broadcast = *(inaddr_t *) interface->ifa_broadaddr;

                //  If the returned broadcast address is the same as source
                //  address, build the broadcast address from the source
                //  address and netmask.
                if (address.sin_addr.s_addr == broadcast.sin_addr.s_addr)
                    broadcast.sin_addr.s_addr |= ~(netmask.sin_addr.s_addr);

                interface_t *item =
                    s_interface_new (interface->ifa_name, address, netmask,
                                     broadcast);
                if (item)
                    zlistx_add_end (list, item);
            }
            interface = interface->ifa_next;
        }
    }
    freeifaddrs (interfaces);

#   elif defined (__UNIX__)
    int sock = socket (AF_INET, SOCK_DGRAM, 0);
    if (sock != -1) {
        int num_interfaces = 0;
        struct ifconf ifconfig = { 0 };
        //  First ioctl call gets us length of buffer; second call gets us contents
        if (!ioctl (sock, SIOCGIFCONF, (caddr_t) &ifconfig, sizeof (struct ifconf))) {
            ifconfig.ifc_buf = (char *) zmalloc (ifconfig.ifc_len);
            if (!ioctl (sock, SIOCGIFCONF, (caddr_t) &ifconfig, sizeof (struct ifconf)))
                num_interfaces = ifconfig.ifc_len / sizeof (struct ifreq);
        }
        int index;
        for (index = 0; index < num_interfaces; index++) {
            struct ifreq *ifr = &ifconfig.ifc_req [index];
            //  Check interface flags
            bool is_valid = false;
            if (!ioctl (sock, SIOCGIFFLAGS, (caddr_t) ifr, sizeof (struct ifreq)))
                is_valid = s_valid_flags (ifr->ifr_flags);

            //  Get interface properties
            inaddr_t address = { 0 };
            if (!ioctl (sock, SIOCGIFADDR, (caddr_t) ifr, sizeof (struct ifreq)))
                address = *((inaddr_t *) &ifr->ifr_addr);
            else
                is_valid = false;

            inaddr_t broadcast = { 0 };
            if (!ioctl (sock, SIOCGIFBRDADDR, (caddr_t) ifr, sizeof (struct ifreq)))
                broadcast = *((inaddr_t *) &ifr->ifr_addr);
            else
                is_valid = false;

            inaddr_t netmask = { 0 };
            if (!ioctl (sock, SIOCGIFNETMASK, (caddr_t) ifr, sizeof (struct ifreq)))
                netmask = *((inaddr_t *) &ifr->ifr_addr);
            else
                is_valid = false;

            if (is_valid) {
                interface_t *item = s_interface_new (ifr->ifr_name, address,
                                                     netmask, broadcast);
                if (item)
                    zlistx_add_end (list, item);
            }
        }
        free (ifconfig.ifc_buf);
        close (sock);
    }

#   elif defined (__WINDOWS__)
    ULONG addr_size = 0;
    DWORD rc = GetAdaptersAddresses (AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &addr_size);
    assert (rc == ERROR_BUFFER_OVERFLOW);

    PIP_ADAPTER_ADDRESSES pip_addresses = (PIP_ADAPTER_ADDRESSES) zmalloc (addr_size);
    rc = GetAdaptersAddresses (AF_INET,
                               GAA_FLAG_INCLUDE_PREFIX, NULL, pip_addresses, &addr_size);
    assert (rc == NO_ERROR);

    PIP_ADAPTER_ADDRESSES cur_address = pip_addresses;
    while (cur_address) {
        PIP_ADAPTER_UNICAST_ADDRESS pUnicast = cur_address->FirstUnicastAddress;
        PIP_ADAPTER_PREFIX pPrefix = cur_address->FirstPrefix;

        PWCHAR friendlyName = cur_address->FriendlyName;
        size_t asciiSize = wcstombs (0, friendlyName, 0) + 1;
        char *asciiFriendlyName = (char *) zmalloc (asciiSize);
        wcstombs (asciiFriendlyName, friendlyName, asciiSize);

        bool valid = (cur_address->OperStatus == IfOperStatusUp)
                     && (pUnicast && pPrefix)
                     && (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
                     && (pPrefix->PrefixLength <= 32);

        if (valid) {
            inaddr_t address = *(inaddr_t *) pUnicast->Address.lpSockaddr;
            inaddr_t netmask;
            netmask.sin_addr.s_addr = htonl ((0xffffffffU) << (32 - pPrefix->PrefixLength));
            inaddr_t broadcast = address;
            broadcast.sin_addr.s_addr |= ~(netmask.sin_addr.s_addr);
            interface_t *item = s_interface_new (asciiFriendlyName, address,
                                                 netmask, broadcast);
            if (item)
                zlistx_add_end (list, item);
        }
        free (asciiFriendlyName);
        cur_address = cur_address->Next;
    }
    free (pip_addresses);

#   else
#       error "Interface detection TBD on this operating system"
#   endif
}


//  --------------------------------------------------------------------------
//  Return the number of network interfaces on system

size_t
ziflist_size (ziflist_t *self)
{
    assert (self);
    return zlistx_size ((zlistx_t *) self);
}


//  --------------------------------------------------------------------------
//  Get first network interface, return NULL if there are none

const char *
ziflist_first (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_first ((zlistx_t *) self);
    if (iface)
        return iface->name;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Get next network interface, return NULL if we hit the last one

const char *
ziflist_next (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_next ((zlistx_t *) self);
    if (iface)
        return iface->name;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the current interface IP address as a printable string

const char *
ziflist_address (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_item ((zlistx_t *) self);
    if (iface)
        return iface->address;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the current interface broadcast address as a printable string

const char *
ziflist_broadcast (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_item ((zlistx_t *) self);
    if (iface)
        return iface->broadcast;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Return the current interface network mask as a printable string

const char *
ziflist_netmask (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_item ((zlistx_t *) self);
    if (iface)
        return iface->netmask;
    else
        return NULL;
}


//  --------------------------------------------------------------------------
//  Selftest for this class

void
ziflist_test (bool verbose)
{
    printf (" * ziflist: ");
    if (verbose)
        printf ("\n");

    //  @selftest
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);
    size_t items = ziflist_size (iflist);

    if (verbose) {
//        printf ("ziflist: interfaces=%zu\n", ziflist_size (iflist));
        const char *name = ziflist_first (iflist);
        while (name) {
            printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
                    name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
            name = ziflist_next (iflist);
        }
    }
    ziflist_reload (iflist);
    assert (items == ziflist_size (iflist));
    ziflist_destroy (&iflist);
    //  @end
    
    printf ("OK\n");
}
