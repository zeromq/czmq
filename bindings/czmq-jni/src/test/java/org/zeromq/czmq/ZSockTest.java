package org.zeromq.czmq;

import org.junit.Assert;
import org.junit.Test;

public class ZSockTest {
    @Test
    public void testZSockSendRecv() {
        try (final ZSock push = new ZSock(ZMQ.ZMQ_PUSH);
             final ZSock pull = new ZSock(ZMQ.ZMQ_PULL)) {
            ZMsg msg = new ZMsg();
            pull.bind("tcp://*:1234");
            push.connect("tcp://localhost:1234");
            String expected = "hello";
            msg.pushstr(expected);
            ZMsg.send(msg, push);
            ZMsg resp = ZMsg.recv(pull);
            String actual = resp.popstr();
            Assert.assertEquals(expected, actual);
            resp.close();
        }
    }
}
