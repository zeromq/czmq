import unittest
from ctypes import *
from czmq import Zmsg, Zhash, zhash_foreach_fn

class TestZyre(unittest.TestCase):
    def test_zhash(self):
        h = Zhash()

        self.assertEquals(h.insert('test', 'abc'), 0)
        self.assertEquals(string_at(h.lookup('test')), 'abc')

        h.update('test', 'xyz')
        self.assertEquals(string_at(h.lookup('test')), 'xyz')

        self.assertEquals(h.size(), 1)

        foreach_aggregator = []
        def aggregate(key, item, argument):
            foreach_aggregator.append((key, string_at(item)))
            return 0
        self.assertEquals(h.foreach(zhash_foreach_fn(aggregate), None), 0)
        self.assertEquals(len(foreach_aggregator), 1)
        self.assertEquals(foreach_aggregator[0], ('test', 'xyz'))

        del h

    def test_zmsg(self):
        m = Zmsg()

        self.assertEquals(m.pushstr('testing 123'), 0)
        self.assertEquals(m.popstr(), 'testing 123')

        del m

if __name__ == '__main__':
    Zhash.test(0)
    Zmsg.test(0)
    unittest.main()

