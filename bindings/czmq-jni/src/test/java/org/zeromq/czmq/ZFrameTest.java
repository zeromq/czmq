package org.zeromq.czmq;

import org.junit.Assert;
import org.junit.Test;

public class ZFrameTest {
    @Test
    public void zframeTest() {
        ZFrame frame = new ZFrame("hello".getBytes());
        byte[] data = frame.data();

        Assert.assertEquals("hello", new String(data));
    }
}
