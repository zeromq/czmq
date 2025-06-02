/*  =========================================================================
    ziflist - list of network interfaces available on system

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

#include "czmq_classes.h"

//  Structure of an interface
typedef struct {
    char *name;
    char *address;
    char *netmask;
    char *broadcast;
    char *mac;
    bool is_ipv6;
} interface_t;


//  --------------------------------------------------------------------------
//  interface destructor

static void
s_interface_destroy (interface_t **self_p)
{
    assert (self_p);
    interface_t *self = *self_p;
    if (self) {
        freen (self->name);
        freen (self->address);
        freen (self->netmask);
        freen (self->broadcast);
        freen (self->mac);
        freen (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  interface constructor

static interface_t *
s_interface_new (char *name, struct sockaddr *address, struct sockaddr *netmask,
        struct sockaddr *broadcast, const char *mac)
{
    char hbuf[NI_MAXHOST];
    int rc;

    interface_t *self = (interface_t *) zmalloc (sizeof (interface_t));
    assert (self);
    self->name = strdup (name);
    assert (self->name);

    rc = getnameinfo (address, address->sa_family == AF_INET ?
            sizeof (struct sockaddr_in) : sizeof (struct sockaddr_in6),
            hbuf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    assert (rc == 0);
    //  Some platform's getnameinfo, like Solaris, appear not to append the
    //  interface name when parsing a link-local IPv6 address. These addresses
    //  cannot be used without the interface, so we must append it manually.
    if (address->sa_family == AF_INET6 &&
            IN6_IS_ADDR_LINKLOCAL (&((struct sockaddr_in6 *)address)->sin6_addr) &&
            !strchr (hbuf, '%')) {
        strcat (hbuf, "%");
        strcat (hbuf, name);
    }
    self->address = strdup (hbuf);
    assert (self->address);

    rc = getnameinfo (netmask, netmask->sa_family == AF_INET ?
            sizeof (struct sockaddr_in) : sizeof (struct sockaddr_in6),
            hbuf, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
    assert (rc == 0);
    self->netmask = strdup (hbuf);
    assert (self->netmask);

    if (address->sa_family == AF_INET) {
        //  If the returned broadcast address is the same as source
        //  address, build the broadcast address from the source
        //  address and netmask.
        if (((inaddr_t *)address)->sin_addr.s_addr == ((inaddr_t *)broadcast)->sin_addr.s_addr)
            ((inaddr_t *)broadcast)->sin_addr.s_addr |= ~(((inaddr_t *)netmask)->sin_addr.s_addr);

        rc = getnameinfo (broadcast, sizeof (struct sockaddr_in), hbuf, NI_MAXHOST,
                NULL, 0, NI_NUMERICHOST);
        assert (rc == 0);
        self->broadcast = strdup (hbuf);
        assert (self->broadcast);
    } else {
        //  The default is link-local all-node multicast group fe02::1
        self->broadcast = strdup (zsys_ipv6_mcast_address ());
        assert (self->broadcast);
    }

    self->mac = strdup(mac);

    self->is_ipv6 = address->sa_family == AF_INET6 ? true : false;

    return self;
}

//  Structure of our class
struct _ziflist_t;

//  --------------------------------------------------------------------------
//  Get a list of network interfaces currently defined on the system

ziflist_t *
ziflist_new (void)
{
    ziflist_t *self = (ziflist_t *) zlistx_new ();
    assert (self);
    zlistx_set_destructor ((zlistx_t *) self, (czmq_destructor *) s_interface_destroy);
    ziflist_reload (self);
    return self;
}

//  --------------------------------------------------------------------------
//  Get a list of network interfaces currently defined on the system
//  Includes IPv6 interfaces

ziflist_t *
ziflist_new_ipv6 (void)
{
    ziflist_t *self = (ziflist_t *) zlistx_new ();
    assert (self);
    zlistx_set_destructor ((zlistx_t *) self, (czmq_destructor *) s_interface_destroy);
    ziflist_reload_ipv6 (self);
    return self;
}


//  Print properties of the ziflist object.
//  --------------------------------------------------------------------------

void
ziflist_print (ziflist_t *self)
{
    interface_t *iface;
    for (iface = (interface_t *) zlistx_first ((zlistx_t *) self);
         iface != NULL;
         iface = (interface_t *) zlistx_next ((zlistx_t *) self)) {
        zsys_info (" - interface name : %s", iface->name);
        zsys_info (" - interface address : %s", iface->address);
        zsys_info (" - interface netmask : %s", iface->netmask);
        zsys_info (" - interface broadcast : %s", iface->broadcast);
        zsys_info (" - interface mac : %s", iface->mac);
    }
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
s_valid_flags (short flags, bool ipv6)
{
    return (flags & IFF_UP)             //  Only use interfaces that are running
           && !(flags & IFF_LOOPBACK)   //  Ignore loopback interface
           && ((ipv6 || (flags & IFF_BROADCAST))  //  Only use interfaces that have BROADCAST
                   && (!ipv6 || (flags & IFF_MULTICAST))) //  or IPv6 and MULTICAST
#   if defined (IFF_SLAVE)
           && !(flags & IFF_SLAVE)      //  Ignore devices that are bonding slaves.
#   endif
           && !(flags & IFF_POINTOPOINT); //  Ignore point to point interfaces.
}
#endif


//  --------------------------------------------------------------------------
//  Helper to reload network interfaces from system

static void
s_reload (ziflist_t *self, bool ipv6)
{
    assert (self);
    zlistx_t *list = (zlistx_t *) self;
    zlistx_purge (list);

#if defined (HAVE_GETIFADDRS)
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        struct ifaddrs *interface = interfaces;
        zhash_t *mactable = zhash_new();
        zhash_autofree(mactable);
        while (interface) {
            // first try to get a mac addr and save it in a table
#if defined __UTYPE_OSX  || defined __UTYPE_IOS
            if (interface->ifa_addr != NULL && interface->ifa_flags & IFF_UP && interface->ifa_addr->sa_family == AF_LINK)
            {
                struct sockaddr_dl *sdl = (struct sockaddr_dl *) interface->ifa_addr;
                if (sdl->sdl_type == IFT_ETHER)
                {
                    const unsigned char *rawmac = (unsigned char *)sdl->sdl_data + sdl->sdl_nlen;
                    char mac[18] = "NA";
                    int i;
                    int len = 0;
                    for (i = 0; i < 6; i++)
                    {
                        len += snprintf(mac+len, 18, "%02X%s", rawmac[i], i < 5 ? ":":"");
                    }
                    zhash_insert(mactable, interface->ifa_name, (void *)mac);
                }
            }
#else
            if (interface->ifa_addr && ((interface)->ifa_addr)->sa_family == AF_PACKET)
            {
                unsigned char mac[18] = "NA";
                struct sockaddr_ll *s = (struct sockaddr_ll*)(interface->ifa_addr);
                int i;
                int len = 0;
                for (i = 0; i < 6; i++) {
                    len += snprintf(mac+len, 18, "%02X%s", s->sll_addr[i], i < 5 ? ":":"");
                }
                zhash_insert(mactable, (interface)->ifa_name, (void *)mac);
            }
#endif
            // next get layer 3 info if any

            //  On Solaris, loopback interfaces have a NULL in ifa_broadaddr
            if (interface->ifa_addr
            && (interface->ifa_broadaddr
                    || (ipv6 && (interface->ifa_addr->sa_family == AF_INET6)))
            &&(interface->ifa_addr->sa_family == AF_INET
                    || (ipv6 && (interface->ifa_addr->sa_family == AF_INET6)))
            // Seems to be needed for running VirtualBox VMs on MacOS (see #1802)
            &&(interface->ifa_netmask->sa_family == AF_INET
                    || (ipv6 && (interface->ifa_netmask->sa_family == AF_INET6)))
            &&  s_valid_flags (interface->ifa_flags,
                    ipv6 && (interface->ifa_addr->sa_family == AF_INET6)))
            {
                const char *mac = zhash_lookup(mactable, interface->ifa_name);
                if (mac) {
                    interface_t *item = s_interface_new (interface->ifa_name,
                            interface->ifa_addr, interface->ifa_netmask,
                            interface->ifa_broadaddr, mac);
                    if (item)
                        zlistx_add_end (list, item);
                }
            }
            interface = interface->ifa_next;
        }
        zhash_destroy(&mactable);
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

            //  Get interface properties
            inaddr_t address = { 0 };
            if (!ioctl (sock, SIOCGIFADDR, (caddr_t) ifr, sizeof (struct ifreq)))
                address = *((inaddr_t *) &ifr->ifr_addr);
            else
                is_valid = false;

            if (!ioctl (sock, SIOCGIFFLAGS, (caddr_t) ifr, sizeof (struct ifreq)))
                is_valid = s_valid_flags (ifr->ifr_flags,
                        ipv6 && (address.sin_family == AF_INET6));

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

            unsigned char mac[18] = "NA";
            unsigned char rawmac[6];
            if (!ioctl(sock, SIOCGIFHWADDR, (caddr_t)ifr, sizeof(struct ifreq))) {
                memcpy(rawmac, ifr->ifr_hwaddr.sa_data, 6);
                int len = 0;
                for (int i = 0; i < 6; i++) {
                    len += snprintf(mac+len, 18, "%02X%s", rawmac[i], i < 5 ? ":":"" );
                }
            }

            if (is_valid) {
                interface_t *item = s_interface_new (ifr->ifr_name,
                        (struct sockaddr *)&address, (struct sockaddr *)&netmask,
                        (struct sockaddr *)&broadcast, mac);
                if (item)
                    zlistx_add_end (list, item);
            }
        }
        freen (ifconfig.ifc_buf);
        close (sock);
    }

#   elif defined (__WINDOWS__)
    // TODO: IPv6 support
    ULONG addr_size = 0;
    DWORD rc = GetAdaptersAddresses (AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &addr_size);
    assert (rc == ERROR_BUFFER_OVERFLOW);

    static const int maxTries = 3;
    int iterations = 0;

    PIP_ADAPTER_ADDRESSES pip_addresses = NULL;

    do {
        pip_addresses = (PIP_ADAPTER_ADDRESSES) zmalloc (addr_size);
        rc = GetAdaptersAddresses (AF_INET,
                                   GAA_FLAG_INCLUDE_PREFIX, NULL, pip_addresses, &addr_size);
        
        if (rc == ERROR_BUFFER_OVERFLOW) {
            freen (pip_addresses);
            pip_addresses = NULL;
        } 
        else {
            break;
        }
        
        ++iterations;
    }
    while ((rc == ERROR_BUFFER_OVERFLOW) && (iterations < maxTries));

    assert (rc == NO_ERROR);

    PIP_ADAPTER_ADDRESSES cur_address = pip_addresses;
    while (cur_address) {
        PIP_ADAPTER_UNICAST_ADDRESS pUnicast = cur_address->FirstUnicastAddress;
        PIP_ADAPTER_PREFIX pPrefix = cur_address->FirstPrefix;

        PWCHAR friendlyName = cur_address->FriendlyName;
        if (friendlyName == NULL) {
            cur_address = cur_address->Next;
            continue;
        }
        size_t asciiSize = wcstombs (0, friendlyName, 0) + 1;
        char *asciiFriendlyName = (char *) zmalloc (asciiSize);
        wcstombs (asciiFriendlyName, friendlyName, asciiSize);

        bool valid = (cur_address->OperStatus == IfOperStatusUp)
                     && (pUnicast && pPrefix)
                     && (pUnicast->Address.lpSockaddr->sa_family == AF_INET)
                     && (pPrefix->PrefixLength <= 32);

        if (valid) {
            struct sockaddr_in address, netmask, broadcast;
            address = *(struct sockaddr_in *)pUnicast->Address.lpSockaddr;
            address.sin_family = AF_INET;
            netmask.sin_addr.s_addr = htonl ((0xffffffffU) << (32 - pPrefix->PrefixLength));
            netmask.sin_family = AF_INET;
            broadcast = address;
            broadcast.sin_addr.s_addr |= ~(netmask.sin_addr.s_addr);
            // Retrieve the MAC address from the PIP_ADAPTER_ADDRESSES structure
            unsigned char *mac_address = cur_address->PhysicalAddress;
            char mac[18] = "NA";
            if (mac_address != NULL && cur_address->PhysicalAddressLength == 6) {
                int len = 0;
                for (int i = 0; i < 6; i++) {
                    len += snprintf( mac+len, 18, "%02X%s", mac_address[i], i < 5 ? ":":"" );
                }
            }
            interface_t *item = s_interface_new (asciiFriendlyName,
                    (struct sockaddr *)&address, (struct sockaddr *)&netmask,
                    (struct sockaddr *)&broadcast, mac);

            if (item)
                zlistx_add_end (list, item);
        }
        freen (asciiFriendlyName);
        cur_address = cur_address->Next;
    }
    freen (pip_addresses);

#   else
#       error "Interface detection TBD on this operating system"
#   endif
}


//  --------------------------------------------------------------------------
//  Reload network interfaces from system

void
ziflist_reload (ziflist_t *self)
{
    s_reload (self, false);
}


//  --------------------------------------------------------------------------
//  Reload network interfaces from system, including IPv6

void
ziflist_reload_ipv6 (ziflist_t *self)
{
    s_reload (self, true);
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

const char *
ziflist_mac (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_item ((zlistx_t *) self);
    if (iface)
        return iface->mac;
    else
        return NULL;
}

//  --------------------------------------------------------------------------
//  Return true if the current interface uses IPv6

bool
ziflist_is_ipv6 (ziflist_t *self)
{
    assert (self);
    interface_t *iface = (interface_t *) zlistx_item ((zlistx_t *) self);
    return iface->is_ipv6;
}


//  --------------------------------------------------------------------------
//  Selftest for this class

void
ziflist_test (bool verbose)
{
    printf (" * ziflist: ");
    if (verbose)
        printf ("\n");

    // TODO: for any Windows dev, any alternative to this?
#if defined (__WINDOWS__)
    WORD version_requested = MAKEWORD (2, 2);
    WSADATA wsa_data;
    int rc = WSAStartup (version_requested, &wsa_data);
    assert (rc == 0);
    assert (LOBYTE (wsa_data.wVersion) == 2 &&
        HIBYTE (wsa_data.wVersion) == 2);
#endif

    //  @selftest
    ziflist_t *iflist = ziflist_new ();
    assert (iflist);

    size_t items = ziflist_size (iflist);
    if (items)
    {
        // test mac addresses
        const char *item = ziflist_first(iflist);
        while (item)
        {
            const char *mac = ziflist_mac(iflist);
            if ( strlen(mac) == 17 )
                assert( mac[2] == ':' && mac[5] == ':' && mac[8] == ':'
                        && mac[11] == ':' && mac[14] == ':' );
            else
               assert( strlen(mac) == 2 );
            item = ziflist_next (iflist);
        }
    }

    if (verbose) {
        printf ("ziflist: interfaces=%zu\n", ziflist_size (iflist));
        const char *name = ziflist_first (iflist);
        while (name) {
            printf (" - name=%s address=%s netmask=%s broadcast=%s mac=%s\n",
                    name, ziflist_address (iflist), ziflist_netmask (iflist), ziflist_broadcast (iflist), ziflist_mac(iflist));
            name = ziflist_next (iflist);
        }
    }
    ziflist_reload (iflist);
    assert (items == ziflist_size (iflist));
    ziflist_destroy (&iflist);

#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end
    
    printf ("OK\n");
}
