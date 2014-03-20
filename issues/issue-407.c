#include <czmq.h>
#include <net/if.h>

static SOCKET udpsock;             //  UDP socket for send/recv
static inaddr_t address;           //  Our own address
static inaddr_t broadcast;         //  Our broadcast address

void
s_socket_error (const char *reason)
{
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

SOCKET
z_udp_new (bool routable)
{
    //  We haven't implemented multicast yet
    assert (!routable);
    int udpsock = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpsock == INVALID_SOCKET) {
        s_socket_error ("socket");
        return INVALID_SOCKET;
    }

    //  Ask operating system for broadcast permissions on socket
    int on = 1;
    if (setsockopt (udpsock, SOL_SOCKET, SO_BROADCAST,
                   (char *) &on, sizeof (on)) == SOCKET_ERROR)
        s_socket_error ("setsockopt (SO_BROADCAST)");

    //  Allow multiple owners to bind to socket; incoming
    //  messages will replicate to each owner
    if (setsockopt (udpsock, SOL_SOCKET, SO_REUSEADDR,
                   (char *) &on, sizeof (on)) == SOCKET_ERROR)
        s_socket_error ("setsockopt (SO_REUSEADDR)");

#if defined (SO_REUSEPORT)
    //  On some platforms we have to ask to reuse the port
    if (setsockopt (udpsock, SOL_SOCKET, SO_REUSEPORT,
                   (char *) &on, sizeof (on)) == SOCKET_ERROR)
        s_socket_error ("setsockopt (SO_REUSEPORT)");
#endif
    return udpsock;
}

static void
s_get_interface (void)
{
    struct ifaddrs *interfaces;
    if (getifaddrs (&interfaces) == 0) {
        int num_interfaces = 0;
        struct ifaddrs *interface = interfaces;
        while (interface) {
            //  On Solaris, loopback interfaces have a NULL in ifa_broadaddr
            if  (interface->ifa_broadaddr
            && !(interface->ifa_flags & IFF_LOOPBACK)       //  Ignore loopback interface
            &&  (interface->ifa_flags & IFF_BROADCAST)      //  Only use interfaces that have BROADCAST
            && !(interface->ifa_flags & IFF_POINTOPOINT)    //  Ignore point to point interfaces.
            &&   interface->ifa_addr
            &&  (interface->ifa_addr->sa_family == AF_INET)) {
                address = *(inaddr_t *) interface->ifa_addr;
                broadcast = *(inaddr_t *) interface->ifa_broadaddr;
                broadcast.sin_port = htons (9999);

                //  If the returned broadcast address is the same as source
                //  address, build the broadcast address from the source
                //  address and netmask.
                if (address.sin_addr.s_addr == broadcast.sin_addr.s_addr)
                    broadcast.sin_addr.s_addr
                        |= ~(((inaddr_t *) interface->ifa_netmask)->sin_addr.s_addr);

                //  Keep track of the number of interfaces on this host
                if (address.sin_addr.s_addr != ntohl (INADDR_LOOPBACK))
                    num_interfaces++;
            }
            interface = interface->ifa_next;
        }
        //  Subnet broadcast addresses don't work on some platforms but
        //  is assumed to work if the interface is specified.
        broadcast.sin_family = AF_INET;
        broadcast.sin_addr.s_addr = INADDR_BROADCAST;
        broadcast.sin_port = htons (9999);
        if (num_interfaces > 1) {
            //  Our source address is unknown in this case so set it to
            //  INADDR_ANY so hostname isn't set to an incorrect IP.
            address = broadcast;
            address.sin_addr.s_addr = INADDR_ANY;
        }
    }
    freeifaddrs (interfaces);
}

int main (void)
{
    udpsock = z_udp_new (false);
    assert (udpsock != INVALID_SOCKET);

    //  Get the network interface
    s_get_interface ();

    //  Bind to the port on all interfaces
    inaddr_t sockaddr = broadcast;
    if (bind (udpsock, (struct sockaddr *) &sockaddr, sizeof (sockaddr)) == SOCKET_ERROR)
        s_socket_error ("bind");

    char hostname [255];
    int rc = getnameinfo ((struct sockaddr *) &address, sizeof (address),
                           hostname, 255, NULL, 0, NI_NUMERICHOST);
    printf ("OK, host=%s\n", hostname);

    closesocket (udpsock);
    return 0;
}
