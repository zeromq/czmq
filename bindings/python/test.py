from __future__ import print_function
import unittest
import sys, os, time
from ctypes import *
from czmq import *

# Part test, part example of how to use python bindings.
# Note the del statements are just to explicitly test functionality, it is generally not required.

class TestCZMQ(unittest.TestCase):
    verbose = int(os.environ.get('VERBOSE', '0'))

    def test_zdir(self):
        older = Zdir('.', None)
        self.assertTrue(older)
        if self.verbose:
            older.print(0)

        newer = Zdir('..', None)
        self.assertTrue(newer)

        patches = Zdir.diff(older, newer, '/')
        self.assertTrue(patches)

        while patches.size():
            patch = ZdirPatch(patches.pop(), True)

        nosuch = Zdir("does-not-exist", None)
        self.assertFalse(nosuch)

    def test_zdir_patch(self):
        file = Zfile(".", "bilbo")
        self.assertTrue(file)
        patch = ZdirPatch(".", file, 'create', "/")
        self.assertTrue(patch)
        del file

        file = patch.file()
        self.assertTrue(file)
        self.assertEquals(file.filename("."), "bilbo")
        self.assertEquals(patch.vpath(), "/bilbo")
        del patch

    def test_zfile(self):
        file = Zfile(None, "bilbo")
        self.assertTrue(file)
        self.assertEquals(file.filename("."), "bilbo")
        self.assertFalse(file.is_readable())
        del file

        # Create a test file in some random subdirectory
        file = Zfile("./this/is/a/test", "bilbo")
        self.assertTrue(file)
        self.assertEquals(file.output(), 0)
        #chunk = Zchunk.new(None, 100)
        #self.assertTrue(chunk)
        #chunk.fill(0, 100)

        # Write 100 bytes at position 1,000,000 in the file
        #self.assertEquals(file.write(chunk, 1000000), 0)
        #del chunk
        file.close()
        self.assertTrue(file.is_readable())
        #self.assertEquals(file.cursize(), 1000100)
        #self.assertFalse(file.is_stable())
        self.assertTrue(file.digest())

        # Now truncate file from outside
        f = open('./this/is/a/test/bilbo', 'w')
        f.truncate()
        f.write("Hello, World\n")
        f.close()
        del f

        self.assertTrue(file.has_changed())
        self.assertFalse(file.is_stable())
        time.sleep(1.001) # just over a second passes...
        self.assertTrue(file.has_changed())
        self.assertFalse(file.is_stable())

        file.restat() # restat.
        self.assertTrue(file.is_stable())
        self.assertFalse(file.has_changed())

        self.assertEquals(file.digest(), "4AB299C8AD6ED14F31923DD94F8B5F5CB89DFB54")

        # Check we can read from file
        self.assertEquals(file.input(), 0)
        #chunk = file.read(1000100, 0)
        #self.assertTrue(chunk)
        #self.assertEquals(chunk.size(), 13)
        #del chunk
        file.close()

        # Check we can read lines from file
        self.assertEquals(file.input(), 0)
        line = file.readln()
        self.assertEquals(line, "Hello, World")
        line = file.readln()
        self.assertIsNone(line)
        file.close()

        # Try some fun with symbolic links
        link = Zfile("./this/is/a/test", "bilbo.ln")
        self.assertTrue(link)
        self.assertEquals(link.output(), 0)
        link.handle().write("./this/is/a/test/bilbo\n")
        del link

        link = Zfile("./this/is/a/test", "bilbo.ln")
        self.assertTrue(link)
        self.assertEqual(link.input(), 0)
        #chunk = link.read(1000100, 0)
        #self.assertTrue(chunk)
        #self.assertEquals(chunk.size(), 13)
        #del chunk
        del link

        # Remove file and directory
        dir = Zdir("./this", None)
        self.assertTrue(dir)
        self.assertEquals(dir.cursize(), 26)
        dir.remove(True)
        self.assertEquals(dir.cursize(), 0)
        del dir

        # Check we can no longer read from file
        self.assertTrue(file.is_readable())
        file.restat()
        self.assertFalse(file.is_readable())
        self.assertEquals(file.input(), -1)
        del file

    def test_zframe(self):
        # Create two PAIR sockets and connect over inproc
        output = Zsock.new_pair ("@inproc://zframe.test")
        self.assertTrue(output)
        input = Zsock.new_pair (">inproc://zframe.test")
        self.assertTrue(input)

        # Send five different frames, test Zframe.MORE
        for frame_nbr in xrange(5):
            frame = Zframe("Hello", 5)
            self.assertTrue(frame)
            self.assertEquals(Zframe.send(frame, output, Zframe.MORE), 0)

        # Send same frame five times, test ZFRAME_REUSE
        frame = Zframe("Hello", 5)
        self.assertTrue(frame)
        for frame_nbr in xrange(5):
            self.assertEquals(Zframe.send(frame, output, Zframe.MORE + Zframe.REUSE), 0)
        self.assertTrue(frame)
        copy = frame.dup()
        self.assertTrue(frame.eq(copy))
        self.assertEquals(copy.size(), 5)
        del copy
        self.assertFalse(frame.eq(None))

        # Test zframe_new_empty
        frame = Zframe.new_empty()
        self.assertTrue(frame)
        self.assertEquals(frame.size(), 0)
        del frame

        # Send END frame
        frame = Zframe("NOT", 3)
        self.assertTrue(frame)
        frame.reset("END", 3)
        string = frame.strhex()
        self.assertEquals(string, "454E44")
        string = frame.strdup()
        self.assertEquals(string, "END")
        self.assertEquals(Zframe.send(frame, output, 0), 0)

        # Read and count until we receive END
        frame_nbr = 0
        while True:
            frame = Zframe.recv(input)
            if frame.streq("END"):
                break

            self.assertTrue(frame.more())
            frame.set_more(0)
            self.assertEquals(frame.more(), 0)
            frame_nbr += 1
        self.assertEquals(frame_nbr, 10)

    def test_zhash(self):
        hash = Zhash()
        self.assertTrue(hash)
        self.assertEquals(hash.size(), 0)
        self.assertFalse(hash.first())
        self.assertFalse(hash.cursor())

        # Insert some items
        # Note the bindings for zhash are pretty pointless for actually storing
        # things - we have to keep our ctypes buffer objects around to keep the
        # memory alive
        buffer1 = create_string_buffer("dead beef") 
        self.assertEquals(hash.insert("DEADBEEF", buffer1), 0)
        self.assertEquals(string_at(hash.first()), "dead beef")
        self.assertEquals(hash.cursor(), "DEADBEEF")

        buffer2 = create_string_buffer("a bad cafe")
        self.assertEquals(hash.insert("ABADCAFE", buffer2), 0)
        buffer3 = create_string_buffer("coded bad")
        self.assertEquals(hash.insert("C0DEDBAD", buffer3), 0)
        buffer4 = create_string_buffer("dead food")
        self.assertEquals(hash.insert("DEADF00D", "dead food"), 0)
        self.assertEquals(hash.size(), 4)

        # Look for existing items
        self.assertEquals(string_at(hash.lookup("DEADBEEF")), "dead beef")
        self.assertEquals(string_at(hash.lookup("ABADCAFE")), "a bad cafe")
        self.assertEquals(string_at(hash.lookup("DEADF00D")), "dead food")

        # Look for non-existent items
        self.assertFalse(hash.lookup("foo"))

        # Try to insert duplicate items
        self.assertEquals(hash.insert("DEADBEEF", "foo"), -1)
        self.assertEquals(string_at(hash.lookup("DEADBEEF")), "dead beef")

        # Some rename tests

        # Valid rename, key is now LIVEBEEF
        self.assertEquals(hash.rename("DEADBEEF", "LIVEBEEF"), 0)
        self.assertEquals(string_at(hash.lookup("LIVEBEEF")), "dead beef")

        # Trying to rename an unknown item to a non-existent key
        self.assertEquals(hash.rename("WHATBEEF", "NONESUCH"), -1)

        # Trying to rename an unknown item to an existing key
        self.assertEquals(hash.rename("WHATBEEF", "LIVEBEEF"), -1)
        self.assertEquals(string_at(hash.lookup("LIVEBEEF")), "dead beef")

        # Trying to rename an existing item to another existing item
        self.assertEquals(hash.rename("LIVEBEEF", "ABADCAFE"), -1)
        self.assertEquals(string_at(hash.lookup("LIVEBEEF")), "dead beef")
        self.assertEquals(string_at(hash.lookup("ABADCAFE")), "a bad cafe")

        # Test keys method
        keys = hash.keys()
        self.assertEquals(keys.size(), 4)
        del keys

        # Test dup method
        copy = hash.dup()
        self.assertEquals(copy.size(), 4)
        self.assertEquals(string_at(copy.lookup("LIVEBEEF")), "dead beef")
        del copy

        # Test pack/unpack methods
        frame = hash.pack()
        copy = Zhash.unpack(frame)
        del frame
        self.assertEquals(copy.size(), 4)
        self.assertEquals(string_at(copy.lookup("LIVEBEEF")), "dead beef")
        del copy

        # Test save and load
        hash.comment("This is a test file")
        hash.comment("Created by %s", "czmq_selftest")
        hash.save(".cache")
        copy = Zhash()
        self.assertTrue(copy)
        copy.load(".cache")
        self.assertEquals(string_at(copy.lookup("LIVEBEEF")), "dead beef")
        del copy
        os.remove(".cache")

        # Delete a item
        hash.delete("LIVEBEEF")
        self.assertFalse(hash.lookup("LIVEBEEF"))
        self.assertEquals(hash.size(), 3)

        # Test autofree automatically copies and frees string values
        # This means the python string doesn't need to persist, like above
        hash = Zhash()
        self.assertTrue(hash)
        hash.autofree()
        self.assertEquals(hash.insert("key1", "This is a string"), 0)
        self.assertEquals(hash.insert("key2", "Ring a ding ding"), 0)
        self.assertEquals(string_at(hash.lookup("key1")), "This is a string")
        self.assertEquals(string_at(hash.lookup("key2")), "Ring a ding ding")
        del hash

    def test_zlist(self):
        def _zlist_free(data):
            x = Zlist(c_void_p(data), True)
            del x
        zlist_free = zlist_free_fn(_zlist_free)

        def _compare(item1, item2):
            return cmp(string_at(item1), string_at(item2)) > 0
        zlist_compare = zlist_compare_fn(_compare)

        list = Zlist()
        self.assertTrue(list)
        self.assertEquals(list.size(), 0)

        # Three items we'll use as test data
        # List items are void *, not particularly strings
        cheese = create_string_buffer("boursin")
        bread = create_string_buffer("baguette")
        wine = create_string_buffer("bordeaux")

        list.append(cheese)
        self.assertEquals(list.size(), 1)
        list.append(bread)
        self.assertEquals(list.size(), 2)
        list.append(wine)
        self.assertEquals(list.size(), 3)

        self.assertEquals(list.head().value, addressof(cheese))
        self.assertEquals(list.next().value, addressof(cheese))

        self.assertEquals(list.first().value, addressof(cheese))
        self.assertEquals(list.tail().value, addressof(wine))
        self.assertEquals(list.next().value, addressof(bread))

        self.assertEquals(list.first().value, addressof(cheese))
        self.assertEquals(list.next().value, addressof(bread))
        self.assertEquals(list.next().value, addressof(wine))
        self.assertFalse(list.next())
        # After we reach end of list, next wraps around
        self.assertEquals(list.next().value, addressof(cheese))
        self.assertEquals(list.size(), 3)

        list.remove(wine)
        self.assertEquals(list.size(), 2)

        self.assertEquals(list.first().value, addressof(cheese))
        list.remove(cheese)
        self.assertEquals(list.size(), 1)
        self.assertEquals(list.first().value, addressof(bread))

        list.remove(bread)
        self.assertEquals(list.size(), 0)

        list.append(cheese)
        list.append(bread)
        self.assertEquals(list.last().value, addressof(bread))
        list.remove(bread)
        self.assertEquals(list.last().value, addressof(cheese))
        list.remove(cheese)
        self.assertFalse(list.last())

        list.push(cheese)
        self.assertEquals(list.size(), 1)
        self.assertEquals(list.first().value, addressof(cheese))

        list.push(bread)
        self.assertEquals(list.size(), 2)
        self.assertEquals(list.first().value, addressof(bread))
        self.assertEquals(list.item().value, addressof(bread))

        list.append(wine)
        self.assertEquals(list.size(), 3)
        self.assertEquals(list.first().value, addressof(bread))

        sub_list = list.dup()
        self.assertTrue(sub_list)
        self.assertEquals(sub_list.size(), 3)

        list.sort(zlist_compare)
        self.assertEquals(list.pop().value, addressof(bread))
        self.assertEquals(list.pop().value, addressof(cheese))
        self.assertEquals(list.pop().value, addressof(wine))
        self.assertEquals(list.size(), 0)

        self.assertEquals(sub_list.size(), 3)
        list.push(sub_list)
        sub_list_2 = sub_list.dup()
        list.append(sub_list_2)

        ret = list.freefn(sub_list, zlist_free, False)
        sub_list.allow_destruct = False # Can't double-free
        self.assertEquals(ret.value, cast(sub_list._as_parameter_, c_void_p).value)

        ret = list.freefn(sub_list_2, zlist_free, True)
        sub_list_2.allow_destruct = False # Can't double-free
        self.assertEquals(ret.value, cast(sub_list_2._as_parameter_, c_void_p).value)
        del list

        # Test autofree functionality
        list = Zlist()
        self.assertTrue(list)
        list.autofree()
        list.push("bread")
        list.append("cheese")
        self.assertEquals(list.size(), 2)
        self.assertEquals(string_at(list.first()), "bread")
        item = list.pop()
        self.assertEquals(string_at(item), "bread")
        libc.free(item)
        item = list.pop()
        self.assertEquals(string_at(item), "cheese")
        libc.free(item)

    def test_zloop(self):
        def _cancel_timer_event(loop, timer_id, arg):
            # We are handling timer 2, and will cancel timer 1
            cancel_timer_id = POINTER(c_int)(c_int(arg)).contents
            return Zloop(loop, False).timer_end(cancel_timer_id)
        cancel_timer_event = zloop_timer_fn(_cancel_timer_event)

        def _timer_event(loop, timer_id, arg):
            output.send('s', "PING")
            return 0
        timer_event = zloop_timer_fn(_timer_event)

        def _socket_event(loop, handle, arg):
            # Just end the reactor
            return -1
        socket_event = zloop_reader_fn(_socket_event)

        def _timer_event3(loop, timer_id, called):
            c_bool_p(called).contents = True
            # end the reactor
            return -1
        timer_event3 = zloop_timer_fn(_timer_event3)

        # Create two PAIR sockets and connect over inproc
        output = Zsock.new_pair("@inproc://zloop.test")
        self.assertTrue(output)
        input = Zsock.new_pair(">inproc://zloop.test")
        self.assertTrue(input)

        loop = Zloop()
        self.assertTrue(loop)
        loop.set_verbose(self.verbose)

        # Create a timer that will be cancelled
        timer_id = c_int(loop.timer(1000, 1, timer_event, None))
        loop.timer(5, 1, cancel_timer_event, byref(timer_id))

        # After 20 msecs, send a ping message to output3
        loop.timer(20, 1, timer_event, output)

        # Set up some tickets that will never expire
        loop.set_ticket_delay(10000)
        ticket1 = loop.ticket(timer_event, None)
        ticket2 = loop.ticket(timer_event, None)
        ticket3 = loop.ticket(timer_event, None)

        # When we get the ping message, end the reactor
        self.assertEquals(loop.reader(input, socket_event, None), 0)
        loop.reader_set_tolerant(input)
        loop.start()

        loop.ticket_delete(ticket1)
        loop.ticket_delete(ticket2)
        loop.ticket_delete(ticket3)
        del loop

        # Check whether loop properly ignores zsys_interrupted flag
        # when asked to
        #loop = Zloop()

        #timer_event_called = c_bool(False)
        #loop.timer(1, 1, timer_event3, byref(timer_event_called))

        #zsys_interrupted = 1
        #zloop_start (loop)
        ## zloop returns immediately without giving any handler a chance to run
        #self.assertFalse(timer_event_called)

        #zloop_ignore_interrupts (loop)
        #zloop_start (loop)
        # zloop runs the handler which will terminate the loop
        #self.assertTrue(timer_event_called)
        #zsys_interrupted = 0

        # cleanup
        #del loop
        #self.assertFalse(loop)

    def test_zmsg(self):
        # Create two PAIR sockets and connect over inproc
        output = Zsock.new_pair("@inproc://zmsg.test")
        self.assertTrue(output)
        input = Zsock.new_pair(">inproc://zmsg.test")
        self.assertTrue(input)

        # Test send and receive of single-frame message
        msg = Zmsg()
        self.assertTrue(msg)
        frame = Zframe("Hello", 5)
        self.assertTrue(frame)
        msg.prepend(frame)
        self.assertEquals(msg.size(), 1)
        self.assertEquals(msg.content_size(), 5)
        self.assertEquals(Zmsg.send(msg, output), 0)
        self.assertFalse(msg)

        msg = Zmsg.recv(input)
        self.assertTrue(msg)
        self.assertEquals(msg.size(), 1)
        self.assertEquals(msg.content_size(), 5)
        del msg

        # Test send and receive of multi-frame message
        msg = Zmsg()
        self.assertTrue(msg)
        self.assertEquals(msg.addmem("Frame0", 6), 0)
        self.assertEquals(msg.addmem("Frame1", 6), 0)
        self.assertEquals(msg.addmem("Frame2", 6), 0)
        self.assertEquals(msg.addmem("Frame3", 6), 0)
        self.assertEquals(msg.addmem("Frame4", 6), 0)
        self.assertEquals(msg.addmem("Frame5", 6), 0)
        self.assertEquals(msg.addmem("Frame6", 6), 0)
        self.assertEquals(msg.addmem("Frame7", 6), 0)
        self.assertEquals(msg.addmem("Frame8", 6), 0)
        self.assertEquals(msg.addmem("Frame9", 6), 0)
        copy = msg.dup()
        self.assertTrue(copy)
        self.assertEquals(Zmsg.send(copy, output), 0)
        self.assertEquals(Zmsg.send(msg, output), 0)

        copy = Zmsg.recv(input)
        self.assertTrue(copy)
        self.assertEquals(copy.size(), 10)
        self.assertEquals(copy.content_size(), 60)
        del copy

        msg = Zmsg.recv(input)
        self.assertTrue(msg)
        self.assertEquals(msg.size(), 10)
        self.assertEquals(msg.content_size(), 60)

        # create empty file for null test
        f = open('zmsg.test', 'w')
        f.close()

        f = open("zmsg.test", "r")
        null_msg = Zmsg.load(None, f)
        self.assertFalse(null_msg)
        f.close()
        os.remove("zmsg.test")

        # Save to a file, read back
        f = open("zmsg.test", "w")
        self.assertEquals(msg.save(f), 0)
        f.close()

        f = open ("zmsg.test", "r")
        self.assertEquals(msg.save(f), -1)
        f.close()
        del msg

        f = open ("zmsg.test", "r")
        msg = Zmsg.load(None, f)
        self.assertTrue(msg)
        f.close()
        os.remove ("zmsg.test")
        self.assertEquals(msg.size(), 10)
        self.assertEquals(msg.content_size(), 60)

        # Remove all frames except first and last
        for frame_nbr in xrange(8):
            msg.first()
            frame = msg.next()
            msg.remove(frame)

        # Test message frame manipulation
        self.assertEquals(msg.size(), 2)
        frame = msg.last()
        self.assertTrue(frame.streq("Frame9"))
        self.assertEquals(msg.content_size(), 12)
        frame = Zframe("Address", 7)
        self.assertTrue(frame)
        msg.prepend(frame)
        self.assertEquals(msg.size(), 3)
        self.assertEquals(msg.addstr("Body"), 0)
        self.assertEquals(msg.size(), 4)
        frame = msg.pop()
        del frame
        self.assertEquals(msg.size(), 3)
        body = msg.popstr()
        self.assertEquals(body, "Frame0")
        del msg

        # Test encoding/decoding
        msg = Zmsg()
        self.assertTrue(msg)
        blank = create_string_buffer(100000)
        self.assertEquals(msg.addmem(blank, 0), 0)
        self.assertEquals(msg.addmem(blank, 1), 0)
        self.assertEquals(msg.addmem(blank, 253), 0)
        self.assertEquals(msg.addmem(blank, 254), 0)
        self.assertEquals(msg.addmem(blank, 255), 0)
        self.assertEquals(msg.addmem(blank, 256), 0)
        self.assertEquals(msg.addmem(blank, 65535), 0)
        self.assertEquals(msg.addmem(blank, 65536), 0)
        self.assertEquals(msg.addmem(blank, 65537), 0)
        del blank
        self.assertEquals(msg.size(), 9)
        buffer = POINTER(c_byte)()
        buffer_size = msg.encode(buffer)
        del msg
        msg = Zmsg.decode(buffer, buffer_size)
        self.assertTrue(msg)
        libc.free (buffer)
        del msg

        # Test submessages
        msg = Zmsg()
        self.assertTrue(msg)
        submsg = Zmsg()
        msg.pushstr("matr")
        submsg.pushstr("joska")
        self.assertEquals(msg.addmsg(submsg), 0)
        self.assertFalse(submsg)
        submsg = msg.popmsg()
        self.assertFalse(submsg)   # string "matr" is not encoded zmsg_t, so was discarded
        submsg = msg.popmsg()
        self.assertTrue(submsg)
        body = submsg.popstr()
        self.assertEquals(body, "joska")
        del submsg
        frame = msg.pop()
        self.assertFalse(frame)
        del msg

        # Test comparison of two messages
        msg = Zmsg()
        msg.addstr("One")
        msg.addstr("Two")
        msg.addstr("Three")
        msg_other = Zmsg()
        msg_other.addstr("One")
        msg_other.addstr("Two")
        msg_other.addstr("One-Hundred")
        msg_dup = msg.dup()
        empty_msg = Zmsg()
        empty_msg_2 = Zmsg()
        self.assertTrue(msg.eq(msg_dup))
        self.assertFalse(msg.eq(msg_other))
        self.assertTrue(empty_msg.eq(empty_msg_2))
        self.assertFalse(msg.eq(None))
        del msg
        del msg_other
        del msg_dup
        del empty_msg

        # Test signal messages
        msg = Zmsg.new_signal(0)
        self.assertEquals(msg.signal(), 0)
        del msg
        msg = Zmsg.new_signal(-1)
        self.assertEquals(msg.signal(), 255)
        del msg

        # Now try methods on an empty message
        msg = Zmsg()
        self.assertTrue(msg)
        self.assertEquals(msg.size(), 0)
        self.assertFalse(msg.first())
        self.assertFalse(msg.last())
        self.assertFalse(msg.next())
        self.assertFalse(msg.pop())
        # Sending an empty message is valid and destroys the message
        self.assertEquals(Zmsg.send(msg, output), 0)
        self.assertFalse(msg)

    def test_zsock(self):
        writer = Zsock.new_push ("@tcp://127.0.0.1:5560")
        self.assertTrue(writer)
        self.assertTrue(Zsock.resolve(writer) != writer)
        self.assertEquals(writer.type_str(), "PUSH")

        # Check unbind
        self.assertEquals(writer.unbind("tcp://127.0.0.1:%d", 5560), 0)

        # In some cases and especially when running under Valgrind, doing
        # a bind immediately after an unbind causes an EADDRINUSE error.
        # Even a short sleep allows the OS to release the port for reuse.
        time.sleep(0.1)

        # Bind again
        self.assertEquals(writer.bind("tcp://127.0.0.1:%d", 5560), 5560)
        self.assertEquals(writer.endpoint(), "tcp://127.0.0.1:5560")
#endif

        reader = Zsock.new_pull (">tcp://127.0.0.1:5560")
        self.assertTrue(reader)
        self.assertTrue(Zsock.resolve(reader) != reader)
        self.assertEquals(reader.type_str(), "PULL")

        # Basic Hello, World
        writer.send('s', "Hello, World")
        msg = Zmsg.recv(reader)
        self.assertTrue(msg)
        string = msg.popstr()
        self.assertEquals(string, "Hello, World")
        del msg

        # Test resolve FD
        #fd = Zsock.fd(reader)
        #self.assertEquals(zsock_resolve (fd), None)

        # Test binding to ephemeral ports, sequential and random
        DYNAMIC_FIRST = 0xc000
        DYNAMIC_LAST = 0xffff
        port = writer.bind("tcp://127.0.0.1:*")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= DYNAMIC_LAST)
        port = writer.bind("tcp://127.0.0.1:*[50000-]")
        self.assertTrue(port >= 50000 and port <= DYNAMIC_LAST)
        port = writer.bind("tcp://127.0.0.1:*[-50001]")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= 50001)
        port = writer.bind("tcp://127.0.0.1:*[60000-60050]")
        self.assertTrue(port >= 60000 and port <= 60050)

        port = writer.bind("tcp://127.0.0.1:!")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= DYNAMIC_LAST)
        port = writer.bind("tcp://127.0.0.1:![50000-]")
        self.assertTrue(port >= 50000 and port <= DYNAMIC_LAST)
        port = writer.bind("tcp://127.0.0.1:![-50001]")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= 50001)
        port = writer.bind("tcp://127.0.0.1:![60000-60050]")
        self.assertTrue(port >= 60000 and port <= 60050)

        # Test zsock_attach method
        server = Zsock.new_dealer("")
        self.assertTrue(server)
        self.assertEquals(server.attach("@inproc://myendpoint,tcp://127.0.0.1:5556,inproc://others", True), 0)
        self.assertEquals(server.attach("", False), 0)
        self.assertEquals(server.attach(None, True), 0)
        self.assertEquals(server.attach(">a,@b, c,, ", False), -1)
        del server

        # Test zsock_endpoint method
        self.assertEquals(writer.bind("inproc://test.%s", "writer"), 0)
        self.assertEquals(writer.endpoint(), "inproc://test.writer")

        # Test error state when connecting to an invalid socket type
        # ('txp://' instead of 'tcp://', typo intentional)
        self.assertEquals(reader.connect("txp://127.0.0.1:5560"), -1)

        # Test signal/wait methods
        self.assertEquals(writer.signal(123), 0)
        self.assertEquals(reader.wait(), 123)

        # Test zsock_send/recv pictures
        number1 = 123
        number2 = 123 * 123
        number4 = 123 * 123 * 123
        number8 = 123 * 123 * 123 * 123

        #chunk = zchunk_new ("HELLO", 5)
        #self.assertTrue(chunk)
        frame = Zframe("WORLD", 5)
        self.assertTrue(frame)
        #hash = zhashx_new ()
        #self.assertTrue(hash)
        #hash.autofree()
        #hash.insert("1", "value A")
        #hash.insert("2", "value B")
        original = create_string_buffer("pointer")

        # Test zsock_recv into each supported type
        #writer.send("i1248zsbcfhp",
        #            -12345, number1, number2, number4, number8,
        #            "This is a string", "ABCDE", 5, chunk, frame, hash, original)
        writer.send("i1248zsbfp",
                    -12345, number1, number2, number4, c_longlong(number8),
                    "This is a string", "ABCDE", 5, frame, original)
        del frame
        #del chunk
        #del hash
        
        integer = c_int()
        number1 = c_byte()
        number2 = c_short()
        number4 = c_int()
        number8 = c_longlong()
        string = c_char_p()
        data = POINTER(c_ubyte)()
        size = c_size_t()
        #chunk = zchunk_p()
        frame = Zframe(c_void_p(), True)
        #hash = POINTER(zhashx_p)
        pointer = c_void_p()
        #rc = reader.recv("i1248zsbcfhp",
        #                 byref(integer), byref(number1), byref(number2), byref(number4), byref(number8),
        #                 byref(string), byref(data), byref(size), byref(chunk), byref(frame), byref(hash), byref(pointer))
        rc = reader.recv("i1248zsbfp",
                         byref(integer), byref(number1), byref(number2), byref(number4), byref(number8),
                         byref(string), byref(data), byref(size), byref(frame._as_parameter_), byref(pointer))
        self.assertEquals(rc, 0)
        self.assertEquals(integer.value, -12345)
        self.assertEquals(number1.value, 123)
        self.assertEquals(number2.value, 123 * 123)
        self.assertEquals(number4.value, 123 * 123 * 123)
        self.assertEquals(number8.value, 123 * 123 * 123 * 123)
        self.assertEquals(string_at(string), "This is a string")
        self.assertEquals(string_at(data), "ABCDE")
        self.assertEquals(size.value, 5)
        #self.assertEquals(memcmp (chunk.data(), "HELLO", 5), 0)
        #self.assertEquals(chunk.size(), 5)
        self.assertTrue(frame.streq("WORLD"))
        self.assertEquals(frame.size(), 5)
        #value = string_at(hash.lookup("1"))
        #self.assertEquals(value, "value A")
        #value = string_at(hash.lookup("2"))
        #self.assertEquals(value, "value B")
        self.assertEquals(addressof(original), pointer.value)
        libc.free (string)
        libc.free (data)
        del frame
        #del chunk
        #del hash

        # Test zsock_recv of short message this lets us return a failure
        # with a status code and then nothing else the receiver will get
        # the status code and None/zero for all other values
        writer.send("i", -1)
        #reader.recv("izsbcfp",
        #    byref(integer), byref(string), byref(data), byref(size), byref(chunk), byref(frame), byref(pointer))
        frame = Zframe(c_void_p(), True)
        reader.recv("izsbfp",
            byref(integer), byref(string), byref(data), byref(size), byref(frame._as_parameter_), byref(pointer))
        self.assertEquals(integer.value, -1)
        self.assertEquals(string.value, None)
        self.assertFalse(data)
        self.assertEquals(size.value, 0)
        #self.assertEquals(chunk, None)
        self.assertFalse(frame)
        self.assertEquals(pointer.value, None)

        msg = Zmsg()
        msg.addstr("frame 1")
        msg.addstr("frame 2")
        writer.send("szm", "header", msg)
        del msg

        msg = Zmsg(c_void_p(), True)
        reader.recv("szm", byref(string), byref(msg._as_parameter_))

        self.assertEquals(string_at(string), "header")
        self.assertEquals(msg.size(), 2)
        self.assertEquals(msg.popstr(), "frame 1")
        self.assertEquals(msg.popstr(), "frame 2")
        #zstr_free (byref(string))
        libc.free(string)
        del msg

        # Test zsock_recv with null arguments
        #chunk = zchunk_new ("HELLO", 5)
        #self.assertTrue(chunk)
        frame = Zframe("WORLD", 5)
        self.assertTrue(frame)
        #writer.send("izsbcfp",
        #            -12345, "This is a string", "ABCDE", 5, chunk, frame, original)
        writer.send("izsbfp",
                    -12345, "This is a string", "ABCDE", 5, frame, original)
        del frame
        #del chunk
        #reader.recv("izsbcfp", byref(integer), None, None, None, byref(chunk), None, None)
        reader.recv("izsbcfp", byref(integer), None, None, None, None, None, None)
        self.assertEquals(integer.value, -12345)
        #self.assertEquals(chunk.data(), "HELLO", 5), 0)
        #self.assertEquals(chunk.size(), 5)
        #del chunk

        # Test zsock_bsend/brecv pictures with binary encoding
        frame = Zframe("Hello", 5)
        #chunk = Zchunk("World", 5)

        msg = Zmsg()
        msg.addstr("Hello")
        msg.addstr("World")

        #writer.bsend("1248sSpcfm",
        #             number1, number2, number4, number8,
        #             "Hello, World",
        #             "Goodbye cruel World!",
        #             original,
        #             chunk, frame, msg)
        writer.bsend("1248sSpfm",
                     number1, number2, number4, number8,
                     "Hello, World",
                     "Goodbye cruel World!",
                     original,
                     frame, msg)
        #del chunk
        del frame
        del msg

        frame = Zframe(c_void_p(), True)
        msg = Zmsg(c_void_p(), True)
        longstr = c_char_p()
        #reader.brecv("1248sSpcfm",
        #             byref(number1), byref(number2), byref(number4), byref(number8),
        #             byref(string), byref(longstr),
        #             byref(pointer),
        #             byref(chunk), byref(frame._as_parameter_), byref(msg._as_parameter_))
        reader.brecv("1248sSpfm",
                     byref(number1), byref(number2), byref(number4), byref(number8),
                     byref(string), byref(longstr),
                     byref(pointer),
                     byref(frame._as_parameter_), byref(msg._as_parameter_))
        self.assertEquals(number1.value, 123)
        self.assertEquals(number2.value, 123 * 123)
        self.assertEquals(number4.value, 123 * 123 * 123)
        self.assertEquals(number8.value, 123 * 123 * 123 * 123)
        self.assertEquals(string_at(string), "Hello, World")
        self.assertEquals(string_at(longstr), "Goodbye cruel World!")
        self.assertEquals(pointer.value, addressof(original))
        libc.free(longstr)
        #zstr_free (byref(longstr))
        #del chunk
        del frame
        del msg

        # Check that we can send a zproto format message
        #writer.bsend("1111sS4", 0xAA, 0xA0, 0x02, 0x01, "key", "value", 1234)
        #gossip = zgossip_msg_new ()
        #gossip.recv(reader)
        #self.assertEquals(gossip.id(), ZGOSSIP_MSG_PUBLISH)
        #del gossip

if __name__ == '__main__':
    unittest.main()
