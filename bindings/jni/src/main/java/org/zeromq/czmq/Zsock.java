package org.zeromq.czmq;

public class Zsock implements AutoCloseable {
    static {
        try {
            System.loadLibrary ("czmqjni");
        }
        catch (Exception e) {
            System.exit (-1);
        }
    }
    long pointer;

    native static long __init ();
    public () {
        pointer = __init ();
    }
    public Zsock (long address) {
        pointer = address;
    }

    native static Zsock __new_pub (String endpoint);
    public Zsock new_pub (String endpoint) {
        return Zsock.__new_pub (endpoint);
    }

    native static Zsock __new_sub (String endpoint, String subscribe);
    public Zsock new_sub (String endpoint, String subscribe) {
        return Zsock.__new_sub (endpoint, subscribe);
    }

    native static Zsock __new_req (String endpoint);
    public Zsock new_req (String endpoint) {
        return Zsock.__new_req (endpoint);
    }

    native static Zsock __new_rep (String endpoint);
    public Zsock new_rep (String endpoint) {
        return Zsock.__new_rep (endpoint);
    }

    native static Zsock __new_dealer (String endpoint);
    public Zsock new_dealer (String endpoint) {
        return Zsock.__new_dealer (endpoint);
    }

    native static Zsock __new_router (String endpoint);
    public Zsock new_router (String endpoint) {
        return Zsock.__new_router (endpoint);
    }

    native static Zsock __new_push (String endpoint);
    public Zsock new_push (String endpoint) {
        return Zsock.__new_push (endpoint);
    }

    native static Zsock __new_pull (String endpoint);
    public Zsock new_pull (String endpoint) {
        return Zsock.__new_pull (endpoint);
    }

    native static Zsock __new_xpub (String endpoint);
    public Zsock new_xpub (String endpoint) {
        return Zsock.__new_xpub (endpoint);
    }

    native static Zsock __new_xsub (String endpoint);
    public Zsock new_xsub (String endpoint) {
        return Zsock.__new_xsub (endpoint);
    }

    native static Zsock __new_pair (String endpoint);
    public Zsock new_pair (String endpoint) {
        return Zsock.__new_pair (endpoint);
    }

    native static Zsock __new_stream (String endpoint);
    public Zsock new_stream (String endpoint) {
        return Zsock.__new_stream (endpoint);
    }

    native static Zsock __new_server (String endpoint);
    public Zsock new_server (String endpoint) {
        return Zsock.__new_server (endpoint);
    }

    native static Zsock __new_client (String endpoint);
    public Zsock new_client (String endpoint) {
        return Zsock.__new_client (endpoint);
    }

    native static int __bind (long pointer,Object... );
    public int bind (long pointer,Object... ) {
        return Zsock.__bind (pointer,);
    }

    native static String __endpoint (long pointer);
    public String endpoint (long pointer) {
        return Zsock.__endpoint (pointer);
    }

    native static int __unbind (long pointer,Object... );
    public int unbind (long pointer,Object... ) {
        return Zsock.__unbind (pointer,);
    }

    native static int __connect (long pointer,Object... );
    public int connect (long pointer,Object... ) {
        return Zsock.__connect (pointer,);
    }

    native static int __disconnect (long pointer,Object... );
    public int disconnect (long pointer,Object... ) {
        return Zsock.__disconnect (pointer,);
    }

    native static int __attach (long pointer,String endpoints, boolean serverish);
    public int attach (long pointer,String endpoints, boolean serverish) {
        return Zsock.__attach (pointer,endpoints, serverish);
    }

    native static String __type_str (long pointer);
    public String type_str (long pointer) {
        return Zsock.__type_str (pointer);
    }

    native static int __send (long pointer,String picture, Object... );
    public int send (long pointer,String picture, Object... ) {
        return Zsock.__send (pointer,picture, );
    }

    native static int __vsend (long pointer,String picture);
    public int vsend (long pointer,String picture) {
        return Zsock.__vsend (pointer,picture);
    }

    native static int __recv (long pointer,String picture, Object... );
    public int recv (long pointer,String picture, Object... ) {
        return Zsock.__recv (pointer,picture, );
    }

    native static int __vrecv (long pointer,String picture);
    public int vrecv (long pointer,String picture) {
        return Zsock.__vrecv (pointer,picture);
    }

    native static int __bsend (long pointer,String picture, Object... );
    public int bsend (long pointer,String picture, Object... ) {
        return Zsock.__bsend (pointer,picture, );
    }

    native static int __brecv (long pointer,String picture, Object... );
    public int brecv (long pointer,String picture, Object... ) {
        return Zsock.__brecv (pointer,picture, );
    }

    native static void __routing_id (long pointer);
    public void routing_id (long pointer) {
        return Zsock.__routing_id (pointer);
    }

    native static void __set_routing_id (long pointer);
    public void set_routing_id (long pointer) {
        return Zsock.__set_routing_id (pointer);
    }

    native static void __set_unbounded (long pointer);
    public void set_unbounded (long pointer) {
        return Zsock.__set_unbounded (pointer);
    }

    native static int __signal (long pointer,byte status);
    public int signal (long pointer,byte status) {
        return Zsock.__signal (pointer,status);
    }

    native static int __wait (long pointer);
    public int wait (long pointer) {
        return Zsock.__wait (pointer);
    }

    native static void __flush (long pointer);
    public void flush (long pointer) {
        return Zsock.__flush (pointer);
    }

    native static boolean __is (void * self);
    public boolean is (void * self) {
        return Zsock.__is (self);
    }

    native static void * __resolve (void * self);
    public void * resolve (void * self) {
        return Zsock.__resolve (self);
    }

    native static void __test (boolean verbose);
    public void test (boolean verbose) {
        return Zsock.__test (verbose);
    }

    native static int __tos (long pointer);
    public int tos (long pointer) {
        return Zsock.__tos (pointer);
    }

    native static void __set_tos (long pointer,int tos);
    public void set_tos (long pointer,int tos) {
        return Zsock.__set_tos (pointer,tos);
    }

    native static void __set_router_handover (long pointer,int routerHandover);
    public void set_router_handover (long pointer,int routerHandover) {
        return Zsock.__set_router_handover (pointer,routerHandover);
    }

    native static void __set_router_mandatory (long pointer,int routerMandatory);
    public void set_router_mandatory (long pointer,int routerMandatory) {
        return Zsock.__set_router_mandatory (pointer,routerMandatory);
    }

    native static void __set_probe_router (long pointer,int probeRouter);
    public void set_probe_router (long pointer,int probeRouter) {
        return Zsock.__set_probe_router (pointer,probeRouter);
    }

    native static void __set_req_relaxed (long pointer,int reqRelaxed);
    public void set_req_relaxed (long pointer,int reqRelaxed) {
        return Zsock.__set_req_relaxed (pointer,reqRelaxed);
    }

    native static void __set_req_correlate (long pointer,int reqCorrelate);
    public void set_req_correlate (long pointer,int reqCorrelate) {
        return Zsock.__set_req_correlate (pointer,reqCorrelate);
    }

    native static void __set_conflate (long pointer,int conflate);
    public void set_conflate (long pointer,int conflate) {
        return Zsock.__set_conflate (pointer,conflate);
    }

    native static String __zap_domain (long pointer);
    public String zap_domain (long pointer) {
        return Zsock.__zap_domain (pointer);
    }

    native static void __set_zap_domain (long pointer,String zapDomain);
    public void set_zap_domain (long pointer,String zapDomain) {
        return Zsock.__set_zap_domain (pointer,zapDomain);
    }

    native static int __mechanism (long pointer);
    public int mechanism (long pointer) {
        return Zsock.__mechanism (pointer);
    }

    native static int __plain_server (long pointer);
    public int plain_server (long pointer) {
        return Zsock.__plain_server (pointer);
    }

    native static void __set_plain_server (long pointer,int plainServer);
    public void set_plain_server (long pointer,int plainServer) {
        return Zsock.__set_plain_server (pointer,plainServer);
    }

    native static String __plain_username (long pointer);
    public String plain_username (long pointer) {
        return Zsock.__plain_username (pointer);
    }

    native static void __set_plain_username (long pointer,String plainUsername);
    public void set_plain_username (long pointer,String plainUsername) {
        return Zsock.__set_plain_username (pointer,plainUsername);
    }

    native static String __plain_password (long pointer);
    public String plain_password (long pointer) {
        return Zsock.__plain_password (pointer);
    }

    native static void __set_plain_password (long pointer,String plainPassword);
    public void set_plain_password (long pointer,String plainPassword) {
        return Zsock.__set_plain_password (pointer,plainPassword);
    }

    native static int __curve_server (long pointer);
    public int curve_server (long pointer) {
        return Zsock.__curve_server (pointer);
    }

    native static void __set_curve_server (long pointer,int curveServer);
    public void set_curve_server (long pointer,int curveServer) {
        return Zsock.__set_curve_server (pointer,curveServer);
    }

    native static String __curve_publickey (long pointer);
    public String curve_publickey (long pointer) {
        return Zsock.__curve_publickey (pointer);
    }

    native static void __set_curve_publickey (long pointer,String curvePublickey);
    public void set_curve_publickey (long pointer,String curvePublickey) {
        return Zsock.__set_curve_publickey (pointer,curvePublickey);
    }

    native static void __set_curve_publickey_bin (long pointer,byte [] curvePublickey);
    public void set_curve_publickey_bin (long pointer,byte [] curvePublickey) {
        return Zsock.__set_curve_publickey_bin (pointer,curvePublickey);
    }

    native static String __curve_secretkey (long pointer);
    public String curve_secretkey (long pointer) {
        return Zsock.__curve_secretkey (pointer);
    }

    native static void __set_curve_secretkey (long pointer,String curveSecretkey);
    public void set_curve_secretkey (long pointer,String curveSecretkey) {
        return Zsock.__set_curve_secretkey (pointer,curveSecretkey);
    }

    native static void __set_curve_secretkey_bin (long pointer,byte [] curveSecretkey);
    public void set_curve_secretkey_bin (long pointer,byte [] curveSecretkey) {
        return Zsock.__set_curve_secretkey_bin (pointer,curveSecretkey);
    }

    native static String __curve_serverkey (long pointer);
    public String curve_serverkey (long pointer) {
        return Zsock.__curve_serverkey (pointer);
    }

    native static void __set_curve_serverkey (long pointer,String curveServerkey);
    public void set_curve_serverkey (long pointer,String curveServerkey) {
        return Zsock.__set_curve_serverkey (pointer,curveServerkey);
    }

    native static void __set_curve_serverkey_bin (long pointer,byte [] curveServerkey);
    public void set_curve_serverkey_bin (long pointer,byte [] curveServerkey) {
        return Zsock.__set_curve_serverkey_bin (pointer,curveServerkey);
    }

    native static int __gssapi_server (long pointer);
    public int gssapi_server (long pointer) {
        return Zsock.__gssapi_server (pointer);
    }

    native static void __set_gssapi_server (long pointer,int gssapiServer);
    public void set_gssapi_server (long pointer,int gssapiServer) {
        return Zsock.__set_gssapi_server (pointer,gssapiServer);
    }

    native static int __gssapi_plaintext (long pointer);
    public int gssapi_plaintext (long pointer) {
        return Zsock.__gssapi_plaintext (pointer);
    }

    native static void __set_gssapi_plaintext (long pointer,int gssapiPlaintext);
    public void set_gssapi_plaintext (long pointer,int gssapiPlaintext) {
        return Zsock.__set_gssapi_plaintext (pointer,gssapiPlaintext);
    }

    native static String __gssapi_principal (long pointer);
    public String gssapi_principal (long pointer) {
        return Zsock.__gssapi_principal (pointer);
    }

    native static void __set_gssapi_principal (long pointer,String gssapiPrincipal);
    public void set_gssapi_principal (long pointer,String gssapiPrincipal) {
        return Zsock.__set_gssapi_principal (pointer,gssapiPrincipal);
    }

    native static String __gssapi_service_principal (long pointer);
    public String gssapi_service_principal (long pointer) {
        return Zsock.__gssapi_service_principal (pointer);
    }

    native static void __set_gssapi_service_principal (long pointer,String gssapiServicePrincipal);
    public void set_gssapi_service_principal (long pointer,String gssapiServicePrincipal) {
        return Zsock.__set_gssapi_service_principal (pointer,gssapiServicePrincipal);
    }

    native static int __ipv6 (long pointer);
    public int ipv6 (long pointer) {
        return Zsock.__ipv6 (pointer);
    }

    native static void __set_ipv6 (long pointer,int ipv6);
    public void set_ipv6 (long pointer,int ipv6) {
        return Zsock.__set_ipv6 (pointer,ipv6);
    }

    native static int __immediate (long pointer);
    public int immediate (long pointer) {
        return Zsock.__immediate (pointer);
    }

    native static void __set_immediate (long pointer,int immediate);
    public void set_immediate (long pointer,int immediate) {
        return Zsock.__set_immediate (pointer,immediate);
    }

    native static void __set_router_raw (long pointer,int routerRaw);
    public void set_router_raw (long pointer,int routerRaw) {
        return Zsock.__set_router_raw (pointer,routerRaw);
    }

    native static int __ipv4only (long pointer);
    public int ipv4only (long pointer) {
        return Zsock.__ipv4only (pointer);
    }

    native static void __set_ipv4only (long pointer,int ipv4only);
    public void set_ipv4only (long pointer,int ipv4only) {
        return Zsock.__set_ipv4only (pointer,ipv4only);
    }

    native static void __set_delay_attach_on_connect (long pointer,int delayAttachOnConnect);
    public void set_delay_attach_on_connect (long pointer,int delayAttachOnConnect) {
        return Zsock.__set_delay_attach_on_connect (pointer,delayAttachOnConnect);
    }

    native static int __type (long pointer);
    public int type (long pointer) {
        return Zsock.__type (pointer);
    }

    native static int __sndhwm (long pointer);
    public int sndhwm (long pointer) {
        return Zsock.__sndhwm (pointer);
    }

    native static void __set_sndhwm (long pointer,int sndhwm);
    public void set_sndhwm (long pointer,int sndhwm) {
        return Zsock.__set_sndhwm (pointer,sndhwm);
    }

    native static int __rcvhwm (long pointer);
    public int rcvhwm (long pointer) {
        return Zsock.__rcvhwm (pointer);
    }

    native static void __set_rcvhwm (long pointer,int rcvhwm);
    public void set_rcvhwm (long pointer,int rcvhwm) {
        return Zsock.__set_rcvhwm (pointer,rcvhwm);
    }

    native static int __affinity (long pointer);
    public int affinity (long pointer) {
        return Zsock.__affinity (pointer);
    }

    native static void __set_affinity (long pointer,int affinity);
    public void set_affinity (long pointer,int affinity) {
        return Zsock.__set_affinity (pointer,affinity);
    }

    native static void __set_subscribe (long pointer,String subscribe);
    public void set_subscribe (long pointer,String subscribe) {
        return Zsock.__set_subscribe (pointer,subscribe);
    }

    native static void __set_unsubscribe (long pointer,String unsubscribe);
    public void set_unsubscribe (long pointer,String unsubscribe) {
        return Zsock.__set_unsubscribe (pointer,unsubscribe);
    }

    native static String __identity (long pointer);
    public String identity (long pointer) {
        return Zsock.__identity (pointer);
    }

    native static void __set_identity (long pointer,String identity);
    public void set_identity (long pointer,String identity) {
        return Zsock.__set_identity (pointer,identity);
    }

    native static int __rate (long pointer);
    public int rate (long pointer) {
        return Zsock.__rate (pointer);
    }

    native static void __set_rate (long pointer,int rate);
    public void set_rate (long pointer,int rate) {
        return Zsock.__set_rate (pointer,rate);
    }

    native static int __recovery_ivl (long pointer);
    public int recovery_ivl (long pointer) {
        return Zsock.__recovery_ivl (pointer);
    }

    native static void __set_recovery_ivl (long pointer,int recoveryIvl);
    public void set_recovery_ivl (long pointer,int recoveryIvl) {
        return Zsock.__set_recovery_ivl (pointer,recoveryIvl);
    }

    native static int __sndbuf (long pointer);
    public int sndbuf (long pointer) {
        return Zsock.__sndbuf (pointer);
    }

    native static void __set_sndbuf (long pointer,int sndbuf);
    public void set_sndbuf (long pointer,int sndbuf) {
        return Zsock.__set_sndbuf (pointer,sndbuf);
    }

    native static int __rcvbuf (long pointer);
    public int rcvbuf (long pointer) {
        return Zsock.__rcvbuf (pointer);
    }

    native static void __set_rcvbuf (long pointer,int rcvbuf);
    public void set_rcvbuf (long pointer,int rcvbuf) {
        return Zsock.__set_rcvbuf (pointer,rcvbuf);
    }

    native static int __linger (long pointer);
    public int linger (long pointer) {
        return Zsock.__linger (pointer);
    }

    native static void __set_linger (long pointer,int linger);
    public void set_linger (long pointer,int linger) {
        return Zsock.__set_linger (pointer,linger);
    }

    native static int __reconnect_ivl (long pointer);
    public int reconnect_ivl (long pointer) {
        return Zsock.__reconnect_ivl (pointer);
    }

    native static void __set_reconnect_ivl (long pointer,int reconnectIvl);
    public void set_reconnect_ivl (long pointer,int reconnectIvl) {
        return Zsock.__set_reconnect_ivl (pointer,reconnectIvl);
    }

    native static int __reconnect_ivl_max (long pointer);
    public int reconnect_ivl_max (long pointer) {
        return Zsock.__reconnect_ivl_max (pointer);
    }

    native static void __set_reconnect_ivl_max (long pointer,int reconnectIvlMax);
    public void set_reconnect_ivl_max (long pointer,int reconnectIvlMax) {
        return Zsock.__set_reconnect_ivl_max (pointer,reconnectIvlMax);
    }

    native static int __backlog (long pointer);
    public int backlog (long pointer) {
        return Zsock.__backlog (pointer);
    }

    native static void __set_backlog (long pointer,int backlog);
    public void set_backlog (long pointer,int backlog) {
        return Zsock.__set_backlog (pointer,backlog);
    }

    native static int __maxmsgsize (long pointer);
    public int maxmsgsize (long pointer) {
        return Zsock.__maxmsgsize (pointer);
    }

    native static void __set_maxmsgsize (long pointer,int maxmsgsize);
    public void set_maxmsgsize (long pointer,int maxmsgsize) {
        return Zsock.__set_maxmsgsize (pointer,maxmsgsize);
    }

    native static int __multicast_hops (long pointer);
    public int multicast_hops (long pointer) {
        return Zsock.__multicast_hops (pointer);
    }

    native static void __set_multicast_hops (long pointer,int multicastHops);
    public void set_multicast_hops (long pointer,int multicastHops) {
        return Zsock.__set_multicast_hops (pointer,multicastHops);
    }

    native static int __rcvtimeo (long pointer);
    public int rcvtimeo (long pointer) {
        return Zsock.__rcvtimeo (pointer);
    }

    native static void __set_rcvtimeo (long pointer,int rcvtimeo);
    public void set_rcvtimeo (long pointer,int rcvtimeo) {
        return Zsock.__set_rcvtimeo (pointer,rcvtimeo);
    }

    native static int __sndtimeo (long pointer);
    public int sndtimeo (long pointer) {
        return Zsock.__sndtimeo (pointer);
    }

    native static void __set_sndtimeo (long pointer,int sndtimeo);
    public void set_sndtimeo (long pointer,int sndtimeo) {
        return Zsock.__set_sndtimeo (pointer,sndtimeo);
    }

    native static void __set_xpub_verbose (long pointer,int xpubVerbose);
    public void set_xpub_verbose (long pointer,int xpubVerbose) {
        return Zsock.__set_xpub_verbose (pointer,xpubVerbose);
    }

    native static int __tcp_keepalive (long pointer);
    public int tcp_keepalive (long pointer) {
        return Zsock.__tcp_keepalive (pointer);
    }

    native static void __set_tcp_keepalive (long pointer,int tcpKeepalive);
    public void set_tcp_keepalive (long pointer,int tcpKeepalive) {
        return Zsock.__set_tcp_keepalive (pointer,tcpKeepalive);
    }

    native static int __tcp_keepalive_idle (long pointer);
    public int tcp_keepalive_idle (long pointer) {
        return Zsock.__tcp_keepalive_idle (pointer);
    }

    native static void __set_tcp_keepalive_idle (long pointer,int tcpKeepaliveIdle);
    public void set_tcp_keepalive_idle (long pointer,int tcpKeepaliveIdle) {
        return Zsock.__set_tcp_keepalive_idle (pointer,tcpKeepaliveIdle);
    }

    native static int __tcp_keepalive_cnt (long pointer);
    public int tcp_keepalive_cnt (long pointer) {
        return Zsock.__tcp_keepalive_cnt (pointer);
    }

    native static void __set_tcp_keepalive_cnt (long pointer,int tcpKeepaliveCnt);
    public void set_tcp_keepalive_cnt (long pointer,int tcpKeepaliveCnt) {
        return Zsock.__set_tcp_keepalive_cnt (pointer,tcpKeepaliveCnt);
    }

    native static int __tcp_keepalive_intvl (long pointer);
    public int tcp_keepalive_intvl (long pointer) {
        return Zsock.__tcp_keepalive_intvl (pointer);
    }

    native static void __set_tcp_keepalive_intvl (long pointer,int tcpKeepaliveIntvl);
    public void set_tcp_keepalive_intvl (long pointer,int tcpKeepaliveIntvl) {
        return Zsock.__set_tcp_keepalive_intvl (pointer,tcpKeepaliveIntvl);
    }

    native static String __tcp_accept_filter (long pointer);
    public String tcp_accept_filter (long pointer) {
        return Zsock.__tcp_accept_filter (pointer);
    }

    native static void __set_tcp_accept_filter (long pointer,String tcpAcceptFilter);
    public void set_tcp_accept_filter (long pointer,String tcpAcceptFilter) {
        return Zsock.__set_tcp_accept_filter (pointer,tcpAcceptFilter);
    }

    native static int __rcvmore (long pointer);
    public int rcvmore (long pointer) {
        return Zsock.__rcvmore (pointer);
    }

    native static void __fd (long pointer);
    public void fd (long pointer) {
        return Zsock.__fd (pointer);
    }

    native static int __events (long pointer);
    public int events (long pointer) {
        return Zsock.__events (pointer);
    }

    native static String __last_endpoint (long pointer);
    public String last_endpoint (long pointer) {
        return Zsock.__last_endpoint (pointer);
    }

    native static void __destroy (long pointer);
    @Override
    public void close() {
        __destroy (pointer);
        pointer = 0;
    }
}
