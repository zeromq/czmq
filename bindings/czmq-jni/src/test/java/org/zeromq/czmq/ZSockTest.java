package org.zeromq.czmq;

import org.junit.Test;

public class ZSockTest {
    @Test
    public void testZSocketCreation() {
        try (final ZSock push = new ZSock(ZMQ.ZMQ_PUSH)) {
            System.out.println(push.pointer);
        }
    }
}
