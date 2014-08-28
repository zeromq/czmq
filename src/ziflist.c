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
    information using an iterator that works like zlist. Only stores those
    interfaces with broadcast capability, and ignores the loopback interface.
@discuss
@end
*/

#include "platform.h"
#include "../include/czmq.h"

//  Structure of our class
struct _ziflist_t {
    //  We store our interfaces as separate lists for each property
    zlist_t *names;             //  Name as reported by the OS
    zlist_t *addresses;         //  IP(v4) address as a string
    zlist_t *netmasks;          //  Network mask as a string
    zlist_t *broadcasts;        //  Broadcast address as a string
};


static void
s_store (ziflist_t *self, char *name, inaddr_t address, inaddr_t netmask, inaddr_t broadcast)
{
    zlist_append (self->names, name);
    zlist_append (self->addresses, inet_ntoa (address.sin_addr));
    zlist_append (self->netmasks, inet_ntoa (netmask.sin_addr));
    zlist_append (self->broadcasts, inet_ntoa (broadcast.sin_addr));
}


//  --------------------------------------------------------------------------
//  Get a list of network interfaces currently defined on the system

ziflist_t *
ziflist_new (void)
{
    ziflist_t *self = (ziflist_t *) zmalloc (sizeof (struct _ziflist_t));
    if (self) {
        self->names = zlist_new ();
        self->addresses = zlist_new ();
        self->netmasks = zlist_new ();
        self->broadcasts = zlist_new ();
        
        if (self->names && self->addresses && self->netmasks && self->broadcasts) {
            zlist_autofree (self->names);
            zlist_autofree (self->addresses);
            zlist_autofree (self->netmasks);
            zlist_autofree (self->broadcasts);
            ziflist_reload (self);
        }
        else
            ziflist_destroy (&self);
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy a ziflist instance

void
ziflist_destroy (ziflist_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        ziflist_t *self = *self_p;
        zlist_destroy (&self->names);
        zlist_destroy (&self->addresses);
        zlist_destroy (&self->netmasks);
        zlist_destroy (&self->broadcasts);
        free (self);
        *self_p = NULL;
    }
}


#ifdef __UNIX__
//  --------------------------------------------------------------------------
//  Helper function to verify if one interface's flags are what we want.

static bool
s_valid_flags (short flags)
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


//  --------------------------------------------------------------------------
//  Reload network interfaces from system

void
ziflist_reload (ziflist_t *self)
{
#if defined (HAVE_GETIFADDRSx)
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        struct ifaddrs *interface = interfaces;
        while (interface) {
            //  On Solaris, loopback interfaces have a NULL in ifa_broadaddr
            if (interface->ifa_addr->sa_family == AF_INET
            &&  interface->ifa_broadaddr
            &&  interface->ifa_addr
            &&  s_valid_flags (interface->ifa_flags)) {
                inaddr_t address = *(inaddr_t *) interface->ifa_addr;
                inaddr_t netmask = *(inaddr_t *) interface->ifa_netmask;
                inaddr_t broadcast = *(inaddr_t *) interface->ifa_broadaddr;

                //  If the returned broadcast address is the same as source
                //  address, build the broadcast address from the source
                //  address and netmask.
                if (address.sin_addr.s_addr == broadcast.sin_addr.s_addr)
                    broadcast.sin_addr.s_addr |= ~(netmask.sin_addr.s_addr);

                s_store (self, interface->ifa_name, address, netmask, broadcast);
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

            if (is_valid)
                s_store (self, ifr->ifr_name, address, netmask, broadcast);
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
        size_t friendlyLength = 0;
        size_t asciiSize = wcstombs (0, friendlyName, 0) + 1;
        char *asciiFriendlyName = (char*) zmalloc (asciiSize);
        friendlyLength = wcstombs (asciiFriendlyName, friendlyName, asciiSize);

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
            s_store (self, asciiFriendlyName, address, netmask, broadcast);
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
    return zlist_size (self->names);
}


//  --------------------------------------------------------------------------
//  Get first network interface, return NULL if there are none

const char *
ziflist_first (ziflist_t *self)
{
    assert (self);
    zlist_first (self->addresses);
    zlist_first (self->netmasks);
    zlist_first (self->broadcasts);
    return (const char *) zlist_first (self->names);
}


//  --------------------------------------------------------------------------
//  Get next network interface, return NULL if we hit the last one

const char *
ziflist_next (ziflist_t *self)
{
    assert (self);
    zlist_next (self->addresses);
    zlist_next (self->netmasks);
    zlist_next (self->broadcasts);
    return (const char *) zlist_next (self->names);
}


//  --------------------------------------------------------------------------
//  Return the current interface IP address as a printable string

const char *
ziflist_address (ziflist_t *self)
{
    assert (self);
    return (const char *) zlist_item (self->addresses);
}


//  --------------------------------------------------------------------------
//  Return the current interface broadcast address as a printable string

const char *
ziflist_broadcast (ziflist_t *self)
{
    assert (self);
    return (const char *) zlist_item (self->broadcasts);
}


//  --------------------------------------------------------------------------
//  Return the current interface network mask as a printable string

const char *
ziflist_netmask (ziflist_t *self)
{
    assert (self);
    return (const char *) zlist_item (self->netmasks);
}


//  --------------------------------------------------------------------------
//  Selftest for this class

void
ziflist_test (bool verbose)
{
    printf (" * ziflist: ");
    if (verbose)
        printf ("\n");
    
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);

    if (verbose) {
        printf ("ziflist: interfaces=%zu\n", ziflist_size (iflist));
        const char *name = ziflist_first (iflist);
        while (name) {
            printf (" - name=%s address=%s netmask=%s broadcast=%s\n",
                name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist));
            name = ziflist_next (iflist);
        }
    }
    ziflist_destroy (&iflist);
    printf ("OK\n");
}
