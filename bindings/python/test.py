from __future__ import print_function
import unittest
import sys, os, time
from ctypes import *
from czmq import *

# http://python3porting.com/problems.html#common-migration-problems
PY3 = sys.version_info > (3,)

if PY3:
    # Python 3 does not have a "cmp" builtin.
    def cmp(a, b):
        return (a > b) - (a < b)

# Part test, part example of how to use python bindings.
# Note the del statements are just to explicitly test functionality, it is generally not required.

class TestCZMQ(unittest.TestCase):
    verbose = int(os.environ.get('VERBOSE', '0'))

    def test_zdir(self):
        older = Zdir(b'.', None)
        self.assertTrue(older)
        if self.verbose:
            older.print(0)

        newer = Zdir(b'..', None)
        self.assertTrue(newer)

        patches = Zdir.diff(older, newer, b'/')
        self.assertTrue(patches)

        while patches.size():
            patch = ZdirPatch(patches.pop(), True)

        nosuch = Zdir(b"does-not-exist", None)
        self.assertFalse(nosuch)

        # zdir_watch test:
        watch = Zactor(zactor_fn(lib.zdir_watch), None)
        self.assertTrue(watch)

        if self.verbose:
            watch.sock().send(b"s", b"VERBOSE")
            self.assertEqual(watch.sock().wait(), 0)

        # need to create a file in the test directory we're watching
        # in order to ensure the directory exists
        initfile = Zfile(b"./zdir-test-dir", b"initial_file")
        self.assertTrue(initfile)
        initfile.output()
        if PY3:
            with initfile.handle() as hdl:  # Avoid ResourceWarning: unclosed file.
                hdl.write(b"initial file\n")
        else:
            initfile.handle().write(b"initial file\n")
        initfile.close()

        try:
            stable_age = float(file_stable_age_msec()) / 1000.0 + 0.050
        except Exception:
            stable_age = 5.050
        time.sleep(stable_age) # wait for initial file to become 'stable'

        watch.sock().send(b"si", b"TIMEOUT", 100)
        self.assertEqual(watch.sock().wait(), 0)

        watch.sock().send(b"ss", b"SUBSCRIBE", b"zdir-test-dir")
        self.assertEqual(watch.sock().wait(), 0)

        watch.sock().send(b"ss", b"UNSUBSCRIBE", b"zdir-test-dir")
        self.assertEqual(watch.sock().wait(), 0)

        watch.sock().send(b"ss", b"SUBSCRIBE", b"zdir-test-dir")
        self.assertEqual(watch.sock().wait(), 0)

        newfile = Zfile(b"zdir-test-dir", b"test_abc")
        newfile.output()
        if PY3:
            with newfile.handle() as hdl:  # Avoid ResourceWarning: unclosed file.
                hdl.write(b"test file\n")
        else:
            newfile.handle().write(b"test file\n")
        newfile.close()

        #watch_poll = Zpoller(watch, None)

        # poll for a certain timeout before giving up and failing the test.
        #assert(watch_poll.wait(1001) == watch)

        # wait for notification of the file being added
        path = c_char_p()
        patches = zlist_p()
        rc = watch.sock().recv(b"sp", byref(path), byref(patches))
        self.assertEqual(rc, 0)
        patches = Zlist(patches, True)

        self.assertEqual(string_at(path), b"zdir-test-dir")
        libc.free (path)

        self.assertEqual(patches.size(), 1)

        patch = ZdirPatch(patches.pop(), True)
        self.assertEqual(patch.path(), b"zdir-test-dir")

        patch_file = patch.file()
        self.assertEqual(patch_file.filename(b""), b"zdir-test-dir/test_abc")

        del patch
        del patches

        # remove the file
        newfile.remove()
        del newfile

        # poll for a certain timeout before giving up and failing the test.
        #assert(watch_poll.wait(1001) == watch)

        # wait for notification of the file being removed
        path = c_char_p()
        patches = zlist_p()
        rc = watch.sock().recv(b"sp", byref(path), byref(patches))
        self.assertEqual(rc, 0)
        patches = Zlist(patches, True)

        self.assertEqual(string_at(path), b"zdir-test-dir")
        libc.free (path)

        self.assertEqual(patches.size(), 1)

        patch = ZdirPatch(patches.pop(), True)
        self.assertEqual(patch.path(), b"zdir-test-dir")

        patch_file = patch.file()
        self.assertEqual(patch_file.filename(b""), b"zdir-test-dir/test_abc")

        del patch
        del patches

        #zpoller_destroy (&watch_poll)
        del watch

        # clean up by removing the test directory.
        testdir = Zdir(b"zdir-test-dir", None)
        testdir.remove(True)
        del testdir

    def test_zdir_patch(self):
        file = Zfile(b".", b"bilbo")
        self.assertTrue(file)
        patch = ZdirPatch(b".", file, ZdirPatch.CREATE, b"/")
        self.assertTrue(patch)
        del file

        file = patch.file()
        self.assertTrue(file)
        self.assertEqual(file.filename(b"."), b"bilbo")
        self.assertEqual(patch.vpath(), b"/bilbo")
        del patch

    def test_zfile(self):
        file = Zfile(None, b"bilbo")
        self.assertTrue(file)
        self.assertEqual(file.filename(b"."), b"bilbo")
        self.assertFalse(file.is_readable())
        del file

        # Create a test file in some random subdirectory
        file = Zfile(b"./this/is/a/test", b"bilbo")
        self.assertTrue(file)
        self.assertEqual(file.output(), 0)
        #chunk = Zchunk.new(None, 100)
        #self.assertTrue(chunk)
        #chunk.fill(0, 100)

        # Write 100 bytes at position 1,000,000 in the file
        #self.assertEqual(file.write(chunk, 1000000), 0)
        #del chunk
        file.close()
        self.assertTrue(file.is_readable())
        #self.assertEqual(file.cursize(), 1000100)
        #self.assertFalse(file.is_stable())
        self.assertTrue(file.digest())

        # Now truncate file from outside
        f = open('./this/is/a/test/bilbo', 'wb')
        f.truncate()
        f.write(b"Hello, World\n")
        f.close()
        del f

        try:
            stable_age = float(file_stable_age_msec()) / 1000.0 + 0.050
        except Exception:
            stable_age = 5.050

        self.assertTrue(file.has_changed())
        self.assertFalse(file.is_stable())
        time.sleep(stable_age) # just over a threshold value passes...
        self.assertTrue(file.has_changed())
        self.assertFalse(file.is_stable())

        file.restat() # restat.
        self.assertTrue(file.is_stable())
        self.assertFalse(file.has_changed())

        self.assertEqual(file.digest(), b"4AB299C8AD6ED14F31923DD94F8B5F5CB89DFB54")

        # Check we can read from file
        self.assertEqual(file.input(), 0)
        #chunk = file.read(1000100, 0)
        #self.assertTrue(chunk)
        #self.assertEqual(chunk.size(), 13)
        #del chunk
        file.close()

        # Check we can read lines from file
        self.assertEqual(file.input(), 0)
        line = file.readln()
        self.assertEqual(line, b"Hello, World")
        line = file.readln()
        self.assertIsNone(line)
        file.close()

        # Try some fun with symbolic links
        link = Zfile(b"./this/is/a/test", b"bilbo.ln")
        self.assertTrue(link)
        self.assertEqual(link.output(), 0)
        if PY3:
            with link.handle() as hdl:  # Avoid ResourceWarning: unclosed file.
                hdl.write(b"./this/is/a/test/bilbo\n")
        else:
            link.handle().write(b"./this/is/a/test/bilbo\n")

        del link

        link = Zfile(b"./this/is/a/test", b"bilbo.ln")
        self.assertTrue(link)
        self.assertEqual(link.input(), 0)
        #chunk = link.read(1000100, 0)
        #self.assertTrue(chunk)
        #self.assertEqual(chunk.size(), 13)
        #del chunk
        del link

        # Remove file and directory
        dir = Zdir(b"./this", None)
        self.assertTrue(dir)
        self.assertEqual(dir.cursize(), 26)
        dir.remove(True)
        self.assertEqual(dir.cursize(), 0)
        del dir

        # Check we can no longer read from file
        self.assertTrue(file.is_readable())
        file.restat()
        self.assertFalse(file.is_readable())
        self.assertEqual(file.input(), -1)
        del file

    def test_zframe(self):
        # Create two PAIR sockets and connect over inproc
        output = Zsock.new_pair (b"@inproc://zframe.test")
        self.assertTrue(output)
        input = Zsock.new_pair (b">inproc://zframe.test")
        self.assertTrue(input)

        # Send five different frames, test Zframe.MORE
        for frame_nbr in range(5):
            frame = Zframe(b"Hello", 5)
            self.assertTrue(frame)
            self.assertEqual(Zframe.send(frame, output, Zframe.MORE), 0)

        # Send same frame five times, test ZFRAME_REUSE
        frame = Zframe(b"Hello", 5)
        self.assertTrue(frame)
        for frame_nbr in range(5):
            self.assertEqual(Zframe.send(frame, output, Zframe.MORE + Zframe.REUSE), 0)
        self.assertTrue(frame)
        copy = frame.dup()
        self.assertTrue(frame.eq(copy))
        self.assertEqual(copy.size(), 5)
        del copy
        self.assertFalse(frame.eq(None))

        # Test zframe_new_empty
        frame = Zframe.new_empty()
        self.assertTrue(frame)
        self.assertEqual(frame.size(), 0)
        del frame

        # Send END frame
        frame = Zframe(b"NOT", 3)
        self.assertTrue(frame)
        frame.reset(b"END", 3)
        string = frame.strhex()
        self.assertEqual(string, b"454E44")
        string = frame.strdup()
        self.assertEqual(string, b"END")
        self.assertEqual(Zframe.send(frame, output, 0), 0)

        # Read and count until we receive END
        frame_nbr = 0
        while True:
            frame = Zframe.recv(input)
            if frame.streq(b"END"):
                break

            self.assertTrue(frame.more())
            frame.set_more(0)
            self.assertEqual(frame.more(), 0)
            frame_nbr += 1
        self.assertEqual(frame_nbr, 10)

    def test_zhash(self):
        hash = Zhash()
        self.assertTrue(hash)
        self.assertEqual(hash.size(), 0)
        self.assertFalse(hash.first())
        self.assertFalse(hash.cursor())

        # Insert some items
        # Note the bindings for zhash are pretty pointless for actually storing
        # things - we have to keep our ctypes buffer objects around to keep the
        # memory alive
        buffer1 = create_string_buffer(b"dead beef")
        self.assertEqual(hash.insert(b"DEADBEEF", buffer1), 0)
        self.assertEqual(string_at(hash.first()), b"dead beef")
        self.assertEqual(hash.cursor(), b"DEADBEEF")

        buffer2 = create_string_buffer(b"a bad cafe")
        self.assertEqual(hash.insert(b"ABADCAFE", buffer2), 0)
        buffer3 = create_string_buffer(b"coded bad")
        self.assertEqual(hash.insert(b"C0DEDBAD", buffer3), 0)
        buffer4 = create_string_buffer(b"dead food")
        self.assertEqual(hash.insert(b"DEADF00D", b"dead food"), 0)
        self.assertEqual(hash.size(), 4)

        # Look for existing items
        self.assertEqual(string_at(hash.lookup(b"DEADBEEF")), b"dead beef")
        self.assertEqual(string_at(hash.lookup(b"ABADCAFE")), b"a bad cafe")
        self.assertEqual(string_at(hash.lookup(b"DEADF00D")), b"dead food")

        # Look for non-existent items
        self.assertFalse(hash.lookup(b"foo"))

        # Try to insert duplicate items
        self.assertEqual(hash.insert(b"DEADBEEF", b"foo"), -1)
        self.assertEqual(string_at(hash.lookup(b"DEADBEEF")), b"dead beef")

        # Some rename tests

        # Valid rename, key is now LIVEBEEF
        self.assertEqual(hash.rename(b"DEADBEEF", b"LIVEBEEF"), 0)
        self.assertEqual(string_at(hash.lookup(b"LIVEBEEF")), b"dead beef")

        # Trying to rename an unknown item to a non-existent key
        self.assertEqual(hash.rename(b"WHATBEEF", b"NONESUCH"), -1)

        # Trying to rename an unknown item to an existing key
        self.assertEqual(hash.rename(b"WHATBEEF", b"LIVEBEEF"), -1)
        self.assertEqual(string_at(hash.lookup(b"LIVEBEEF")), b"dead beef")

        # Trying to rename an existing item to another existing item
        self.assertEqual(hash.rename(b"LIVEBEEF", b"ABADCAFE"), -1)
        self.assertEqual(string_at(hash.lookup(b"LIVEBEEF")), b"dead beef")
        self.assertEqual(string_at(hash.lookup(b"ABADCAFE")), b"a bad cafe")

        # Test keys method
        keys = hash.keys()
        self.assertEqual(keys.size(), 4)
        del keys

        # Test dup method
        copy = hash.dup()
        self.assertEqual(copy.size(), 4)
        self.assertEqual(string_at(copy.lookup(b"LIVEBEEF")), b"dead beef")
        del copy

        # Test pack/unpack methods
        frame = hash.pack()
        copy = Zhash.unpack(frame)
        del frame
        self.assertEqual(copy.size(), 4)
        self.assertEqual(string_at(copy.lookup(b"LIVEBEEF")), b"dead beef")
        del copy

        # Test save and load
        hash.comment(b"This is a test file")
        hash.comment(b"Created by %s", b"czmq_selftest")
        hash.save(b".cache")
        copy = Zhash()
        self.assertTrue(copy)
        copy.load(b".cache")
        self.assertEqual(string_at(copy.lookup(b"LIVEBEEF")), b"dead beef")
        del copy
        os.remove(b".cache")

        # Delete a item
        hash.delete(b"LIVEBEEF")
        self.assertFalse(hash.lookup(b"LIVEBEEF"))
        self.assertEqual(hash.size(), 3)

        # Test autofree automatically copies and frees string values
        # This means the python string doesn't need to persist, like above
        hash = Zhash()
        self.assertTrue(hash)
        hash.autofree()
        self.assertEqual(hash.insert(b"key1", b"This is a string"), 0)
        self.assertEqual(hash.insert(b"key2", b"Ring a ding ding"), 0)
        self.assertEqual(string_at(hash.lookup(b"key1")), b"This is a string")
        self.assertEqual(string_at(hash.lookup(b"key2")), b"Ring a ding ding")
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
        self.assertEqual(list.size(), 0)

        # Three items we'll use as test data
        # List items are void *, not particularly strings
        cheese = create_string_buffer(b"boursin")
        bread = create_string_buffer(b"baguette")
        wine = create_string_buffer(b"bordeaux")

        list.append(cheese)
        self.assertEqual(list.size(), 1)
        list.append(bread)
        self.assertEqual(list.size(), 2)
        list.append(wine)
        self.assertEqual(list.size(), 3)

        self.assertEqual(list.head().value, addressof(cheese))
        self.assertEqual(list.next().value, addressof(cheese))

        self.assertEqual(list.first().value, addressof(cheese))
        self.assertEqual(list.tail().value, addressof(wine))
        self.assertEqual(list.next().value, addressof(bread))

        self.assertEqual(list.first().value, addressof(cheese))
        self.assertEqual(list.next().value, addressof(bread))
        self.assertEqual(list.next().value, addressof(wine))
        self.assertFalse(list.next())
        # After we reach end of list, next wraps around
        self.assertEqual(list.next().value, addressof(cheese))
        self.assertEqual(list.size(), 3)

        list.remove(wine)
        self.assertEqual(list.size(), 2)

        self.assertEqual(list.first().value, addressof(cheese))
        list.remove(cheese)
        self.assertEqual(list.size(), 1)
        self.assertEqual(list.first().value, addressof(bread))

        list.remove(bread)
        self.assertEqual(list.size(), 0)

        list.append(cheese)
        list.append(bread)
        self.assertEqual(list.last().value, addressof(bread))
        list.remove(bread)
        self.assertEqual(list.last().value, addressof(cheese))
        list.remove(cheese)
        self.assertFalse(list.last())

        list.push(cheese)
        self.assertEqual(list.size(), 1)
        self.assertEqual(list.first().value, addressof(cheese))

        list.push(bread)
        self.assertEqual(list.size(), 2)
        self.assertEqual(list.first().value, addressof(bread))
        self.assertEqual(list.item().value, addressof(bread))

        list.append(wine)
        self.assertEqual(list.size(), 3)
        self.assertEqual(list.first().value, addressof(bread))

        sub_list = list.dup()
        self.assertTrue(sub_list)
        self.assertEqual(sub_list.size(), 3)

        list.sort(zlist_compare)
        self.assertEqual(list.pop().value, addressof(bread))
        self.assertEqual(list.pop().value, addressof(wine))
        self.assertEqual(list.pop().value, addressof(cheese))
        self.assertEqual(list.size(), 0)

        self.assertEqual(sub_list.size(), 3)
        list.push(sub_list)
        sub_list_2 = sub_list.dup()
        list.append(sub_list_2)

        ret = list.freefn(sub_list, zlist_free, False)
        sub_list.allow_destruct = False # Can't double-free
        self.assertEqual(ret.value, cast(sub_list._as_parameter_, c_void_p).value)

        ret = list.freefn(sub_list_2, zlist_free, True)
        sub_list_2.allow_destruct = False # Can't double-free
        self.assertEqual(ret.value, cast(sub_list_2._as_parameter_, c_void_p).value)
        del list

        # Test autofree functionality
        list = Zlist()
        self.assertTrue(list)
        list.autofree()
        list.push(b"bread")
        list.append(b"cheese")
        self.assertEqual(list.size(), 2)
        self.assertEqual(string_at(list.first()), b"bread")
        item = list.pop()
        self.assertEqual(string_at(item), b"bread")
        libc.free(item)
        item = list.pop()
        self.assertEqual(string_at(item), b"cheese")
        libc.free(item)

    def test_zloop(self):
        def _cancel_timer_event(loop, timer_id, arg):
            # We are handling timer 2, and will cancel timer 1
            self.assertIsInstance(arg, int)
            return Zloop(loop, False).timer_end(arg)
        cancel_timer_event = zloop_timer_fn(_cancel_timer_event)

        def _timer_event(loop, timer_id, arg):
            output.send(b's', b"PING")
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
        output = Zsock.new_pair(b"@inproc://zloop.test")
        self.assertTrue(output)
        input = Zsock.new_pair(b">inproc://zloop.test")
        self.assertTrue(input)

        loop = Zloop()
        self.assertTrue(loop)
        loop.set_verbose(self.verbose)

        # Create a timer that will be cancelled
        timer_id = loop.timer(1000, 1, timer_event, None)
        loop.timer(5, 1, cancel_timer_event, timer_id)

        # After 20 msecs, send a ping message to output3
        loop.timer(20, 1, timer_event, output)

        # Set up some tickets that will never expire
        loop.set_ticket_delay(10000)
        ticket1 = loop.ticket(timer_event, None)
        ticket2 = loop.ticket(timer_event, None)
        ticket3 = loop.ticket(timer_event, None)

        # When we get the ping message, end the reactor
        self.assertEqual(loop.reader(input, socket_event, None), 0)
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
        output = Zsock.new_pair(b"@inproc://zmsg.test")
        self.assertTrue(output)
        input_ = Zsock.new_pair(b">inproc://zmsg.test")
        self.assertTrue(input_)

        # Test send and receive of single-frame message
        msg = Zmsg()
        self.assertTrue(msg)
        frame = Zframe(b"Hello", 5)
        self.assertTrue(frame)
        msg.prepend(frame)
        self.assertEqual(msg.size(), 1)
        self.assertEqual(msg.content_size(), 5)
        self.assertEqual(Zmsg.send(msg, output), 0)
        self.assertFalse(msg)

        msg = Zmsg.recv(input_)
        self.assertTrue(msg)
        self.assertEqual(msg.size(), 1)
        self.assertEqual(msg.content_size(), 5)
        del msg

        # Test send and receive of multi-frame message
        msg = Zmsg()
        self.assertTrue(msg)
        self.assertEqual(msg.addmem(b"Frame0", 6), 0)
        self.assertEqual(msg.addmem(b"Frame1", 6), 0)
        self.assertEqual(msg.addmem(b"Frame2", 6), 0)
        self.assertEqual(msg.addmem(b"Frame3", 6), 0)
        self.assertEqual(msg.addmem(b"Frame4", 6), 0)
        self.assertEqual(msg.addmem(b"Frame5", 6), 0)
        self.assertEqual(msg.addmem(b"Frame6", 6), 0)
        self.assertEqual(msg.addmem(b"Frame7", 6), 0)
        self.assertEqual(msg.addmem(b"Frame8", 6), 0)
        self.assertEqual(msg.addmem(b"Frame9", 6), 0)
        copy = msg.dup()
        self.assertTrue(copy)
        self.assertEqual(Zmsg.send(copy, output), 0)
        self.assertEqual(Zmsg.send(msg, output), 0)

        copy = Zmsg.recv(input_)
        self.assertTrue(copy)
        self.assertEqual(copy.size(), 10)
        self.assertEqual(copy.content_size(), 60)
        del copy

        msg = Zmsg.recv(input_)
        self.assertTrue(msg)
        self.assertEqual(msg.size(), 10)
        self.assertEqual(msg.content_size(), 60)

        # Save to a file, read back
        f = open("zmsg.test", "w")
        self.assertEqual(msg.save(f), 0)
        f.close()

        f = open ("zmsg.test", "r")
        self.assertEqual(msg.save(f), -1)
        f.close()
        del msg

        f = open ("zmsg.test", "r")
        msg = Zmsg.load(f)
        self.assertTrue(msg)
        f.close()
        os.remove ("zmsg.test")
        self.assertEqual(msg.size(), 10)
        self.assertEqual(msg.content_size(), 60)

        # Remove all frames except first and last
        for frame_nbr in range(8):
            msg.first()
            frame = msg.next()
            msg.remove(frame)

        # Test message frame manipulation
        self.assertEqual(msg.size(), 2)
        frame = msg.last()
        self.assertTrue(frame.streq(b"Frame9"))
        self.assertEqual(msg.content_size(), 12)
        frame = Zframe(b"Address", 7)
        self.assertTrue(frame)
        msg.prepend(frame)
        self.assertEqual(msg.size(), 3)
        self.assertEqual(msg.addstr(b"Body"), 0)
        self.assertEqual(msg.size(), 4)
        frame = msg.pop()
        del frame
        self.assertEqual(msg.size(), 3)
        body = msg.popstr()
        self.assertEqual(body, b"Frame0")
        del msg

        # Test encoding/decoding
        msg = Zmsg()
        self.assertTrue(msg)
        blank = create_string_buffer(100000)
        self.assertEqual(msg.addmem(blank, 0), 0)
        self.assertEqual(msg.addmem(blank, 1), 0)
        self.assertEqual(msg.addmem(blank, 253), 0)
        self.assertEqual(msg.addmem(blank, 254), 0)
        self.assertEqual(msg.addmem(blank, 255), 0)
        self.assertEqual(msg.addmem(blank, 256), 0)
        self.assertEqual(msg.addmem(blank, 65535), 0)
        self.assertEqual(msg.addmem(blank, 65536), 0)
        self.assertEqual(msg.addmem(blank, 65537), 0)
        del blank
        self.assertEqual(msg.size(), 9)
        frame = msg.encode()
        del msg
        msg = Zmsg.decode(frame)
        self.assertTrue(msg)
        del msg

        # Test submessages
        msg = Zmsg()
        self.assertTrue(msg)
        submsg = Zmsg()
        msg.pushstr(b"matr")
        submsg.pushstr(b"joska")
        self.assertEqual(msg.addmsg(submsg), 0)
        self.assertFalse(submsg)
        submsg = msg.popmsg()
        self.assertFalse(submsg)   # string "matr" is not encoded zmsg_t, so was discarded
        submsg = msg.popmsg()
        self.assertTrue(submsg)
        body = submsg.popstr()
        self.assertEqual(body, b"joska")
        del submsg
        frame = msg.pop()
        self.assertFalse(frame)
        del msg

        # Test comparison of two messages
        msg = Zmsg()
        msg.addstr(b"One")
        msg.addstr(b"Two")
        msg.addstr(b"Three")
        msg_other = Zmsg()
        msg_other.addstr(b"One")
        msg_other.addstr(b"Two")
        msg_other.addstr(b"One-Hundred")
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
        self.assertEqual(msg.signal(), 0)
        del msg
        msg = Zmsg.new_signal(-1)
        self.assertEqual(msg.signal(), 255)
        del msg

        # Now try methods on an empty message
        msg = Zmsg()
        self.assertTrue(msg)
        self.assertEqual(msg.size(), 0)
        self.assertFalse(msg.first())
        self.assertFalse(msg.last())
        self.assertFalse(msg.next())
        self.assertFalse(msg.pop())
        # Sending an empty message is valid and destroys the message
        self.assertEqual(Zmsg.send(msg, output), 0)
        self.assertFalse(msg)

    def test_zsock(self):
        writer = Zsock.new_push (b"@tcp://127.0.0.1:5560")
        self.assertTrue(writer)
        self.assertTrue(Zsock.resolve(writer) != writer)
        self.assertEqual(writer.type_str(), b"PUSH")

        # Check unbind
        self.assertEqual(writer.unbind(b"tcp://127.0.0.1:%d", 5560), 0)

        # In some cases and especially when running under Valgrind, doing
        # a bind immediately after an unbind causes an EADDRINUSE error.
        # Even a short sleep allows the OS to release the port for reuse.
        time.sleep(0.1)

        # Bind again
        self.assertEqual(writer.bind(b"tcp://127.0.0.1:%d", 5560), 5560)
        self.assertEqual(writer.endpoint(), b"tcp://127.0.0.1:5560")
#endif

        reader = Zsock.new_pull (b">tcp://127.0.0.1:5560")
        self.assertTrue(reader)
        self.assertTrue(Zsock.resolve(reader) != reader)
        self.assertEqual(reader.type_str(), b"PULL")

        # Basic Hello, World
        writer.send(b's', b"Hello, World")
        msg = Zmsg.recv(reader)
        self.assertTrue(msg)
        string = msg.popstr()
        self.assertEqual(string, b"Hello, World")
        del msg

        # Test resolve FD
        #fd = Zsock.fd(reader)
        #self.assertEqual(zsock_resolve (fd), None)

        # Test binding to ephemeral ports, sequential and random
        DYNAMIC_FIRST = 0xc000
        DYNAMIC_LAST = 0xffff
        port = writer.bind(b"tcp://127.0.0.1:*")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= DYNAMIC_LAST)
        port = writer.bind(b"tcp://127.0.0.1:*[50000-]")
        self.assertTrue(port >= 50000 and port <= DYNAMIC_LAST)
        port = writer.bind(b"tcp://127.0.0.1:*[-50001]")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= 50001)
        port = writer.bind(b"tcp://127.0.0.1:*[60000-60050]")
        self.assertTrue(port >= 60000 and port <= 60050)

        port = writer.bind(b"tcp://127.0.0.1:!")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= DYNAMIC_LAST)
        port = writer.bind(b"tcp://127.0.0.1:![50000-]")
        self.assertTrue(port >= 50000 and port <= DYNAMIC_LAST)
        port = writer.bind(b"tcp://127.0.0.1:![-50001]")
        self.assertTrue(port >= DYNAMIC_FIRST and port <= 50001)
        port = writer.bind(b"tcp://127.0.0.1:![60000-60050]")
        self.assertTrue(port >= 60000 and port <= 60050)

        # Test zsock_attach method
        server = Zsock.new_dealer(b"")
        self.assertTrue(server)
        self.assertEqual(server.attach(b"@inproc://myendpoint,tcp://127.0.0.1:5556,inproc://others", True), 0)
        self.assertEqual(server.attach(b"", False), 0)
        self.assertEqual(server.attach(None, True), 0)
        self.assertEqual(server.attach(b">a,@b, c,, ", False), -1)
        del server

        # Test zsock_endpoint method
        self.assertEqual(writer.bind(b"inproc://test.%s", b"writer"), 0)
        self.assertEqual(writer.endpoint(), b"inproc://test.writer")

        # Test error state when connecting to an invalid socket type
        # ('txp://' instead of 'tcp://', typo intentional)
        self.assertEqual(reader.connect(b"txp://127.0.0.1:5560"), -1)

        # Test signal/wait methods
        self.assertEqual(writer.signal(123), 0)
        self.assertEqual(reader.wait(), 123)

        # Test zsock_send/recv pictures
        number1 = 123
        number2 = 123 * 123
        number4 = 123 * 123 * 123
        number8 = 123 * 123 * 123 * 123

        #chunk = zchunk_new (b"HELLO", 5)
        #self.assertTrue(chunk)
        frame = Zframe(b"WORLD", 5)
        self.assertTrue(frame)
        #hash = zhashx_new ()
        #self.assertTrue(hash)
        #hash.autofree()
        #hash.insert(b"1", b"value A")
        #hash.insert(b"2", b"value B")
        original = create_string_buffer(b"pointer")

        # Test zsock_recv into each supported type
        #writer.send(b"i1248zsbcfhp",
        #            -12345, number1, number2, number4, number8,
        #            b"This is a string", b"ABCDE", 5, chunk, frame, hash, original)
        writer.send(b"i1248zsbfp",
                    -12345, number1, number2, number4, c_longlong(number8),
                    b"This is a string", b"ABCDE", 5, frame, original)
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
        #rc = reader.recv(b"i1248zsbcfhp",
        #                 byref(integer), byref(number1), byref(number2), byref(number4), byref(number8),
        #                 byref(string), byref(data), byref(size), byref(chunk), byref(frame), byref(hash), byref(pointer))
        rc = reader.recv(b"i1248zsbfp",
                         byref(integer), byref(number1), byref(number2), byref(number4), byref(number8),
                         byref(string), byref(data), byref(size), byref(frame._as_parameter_), byref(pointer))
        self.assertEqual(rc, 0)
        self.assertEqual(integer.value, -12345)
        self.assertEqual(number1.value, 123)
        self.assertEqual(number2.value, 123 * 123)
        self.assertEqual(number4.value, 123 * 123 * 123)
        self.assertEqual(number8.value, 123 * 123 * 123 * 123)
        self.assertEqual(string_at(string), b"This is a string")
        self.assertEqual(string_at(data, size.value), b"ABCDE")
        self.assertEqual(size.value, 5)
        #self.assertEqual(memcmp (chunk.data(), b"HELLO", 5), 0)
        #self.assertEqual(chunk.size(), 5)
        self.assertTrue(frame.streq(b"WORLD"))
        self.assertEqual(frame.size(), 5)
        #value = string_at(hash.lookup(b"1"))
        #self.assertEqual(value, b"value A")
        #value = string_at(hash.lookup(b"2"))
        #self.assertEqual(value, b"value B")
        self.assertEqual(addressof(original), pointer.value)
        libc.free (string)
        libc.free (data)
        del frame
        #del chunk
        #del hash

        # Test zsock_recv of short message this lets us return a failure
        # with a status code and then nothing else the receiver will get
        # the status code and None/zero for all other values
        writer.send(b"i", -1)
        #reader.recv(b"izsbcfp",
        #    byref(integer), byref(string), byref(data), byref(size), byref(chunk), byref(frame), byref(pointer))
        frame = Zframe(c_void_p(), True)
        reader.recv(b"izsbfp",
            byref(integer), byref(string), byref(data), byref(size), byref(frame._as_parameter_), byref(pointer))
        self.assertEqual(integer.value, -1)
        self.assertEqual(string.value, None)
        self.assertFalse(data)
        self.assertEqual(size.value, 0)
        #self.assertEqual(chunk, None)
        self.assertFalse(frame)
        self.assertEqual(pointer.value, None)

        msg = Zmsg()
        msg.addstr(b"frame 1")
        msg.addstr(b"frame 2")
        writer.send(b"szm", b"header", msg)
        del msg

        msg = Zmsg(c_void_p(), True)
        reader.recv(b"szm", byref(string), byref(msg._as_parameter_))

        self.assertEqual(string_at(string), b"header")
        self.assertEqual(msg.size(), 2)
        self.assertEqual(msg.popstr(), b"frame 1")
        self.assertEqual(msg.popstr(), b"frame 2")
        #zstr_free (byref(string))
        libc.free(string)
        del msg

        # Test zsock_recv with null arguments
        #chunk = zchunk_new (b"HELLO", 5)
        #self.assertTrue(chunk)
        frame = Zframe(b"WORLD", 5)
        self.assertTrue(frame)
        #writer.send(b"izsbcfp",
        #            -12345, b"This is a string", b"ABCDE", 5, chunk, frame, original)
        writer.send(b"izsbfp",
                    -12345, b"This is a string", b"ABCDE", 5, frame, original)
        del frame
        #del chunk
        #reader.recv(b"izsbcfp", byref(integer), None, None, None, byref(chunk), None, None)
        reader.recv(b"izsbcfp", byref(integer), None, None, None, None, None, None)
        self.assertEqual(integer.value, -12345)
        #self.assertEqual(chunk.data(), b"HELLO", 5), 0)
        #self.assertEqual(chunk.size(), 5)
        #del chunk

        # Test zsock_bsend/brecv pictures with binary encoding
        frame = Zframe(b"Hello", 5)
        #chunk = Zchunk(b"World", 5)

        msg = Zmsg()
        msg.addstr(b"Hello")
        msg.addstr(b"World")

        #writer.bsend(b"1248sSpcfm",
        #             number1, number2, number4, number8,
        #             b"Hello, World",
        #             b"Goodbye cruel World!",
        #             original,
        #             chunk, frame, msg)
        writer.bsend(b"1248sSpfm",
                     number1, number2, number4, number8,
                     b"Hello, World",
                     b"Goodbye cruel World!",
                     original,
                     frame, msg)
        #del chunk
        del frame
        del msg

        frame = Zframe(c_void_p(), True)
        msg = Zmsg(c_void_p(), True)
        longstr = c_char_p()
        #reader.brecv(b"1248sSpcfm",
        #             byref(number1), byref(number2), byref(number4), byref(number8),
        #             byref(string), byref(longstr),
        #             byref(pointer),
        #             byref(chunk), byref(frame._as_parameter_), byref(msg._as_parameter_))
        reader.brecv(b"1248sSpfm",
                     byref(number1), byref(number2), byref(number4), byref(number8),
                     byref(string), byref(longstr),
                     byref(pointer),
                     byref(frame._as_parameter_), byref(msg._as_parameter_))
        self.assertEqual(number1.value, 123)
        self.assertEqual(number2.value, 123 * 123)
        self.assertEqual(number4.value, 123 * 123 * 123)
        self.assertEqual(number8.value, 123 * 123 * 123 * 123)
        self.assertEqual(string_at(string), b"Hello, World")
        self.assertEqual(string_at(longstr), b"Goodbye cruel World!")
        self.assertEqual(pointer.value, addressof(original))
        libc.free(longstr)
        #zstr_free (byref(longstr))
        #del chunk
        del frame
        del msg

        # Check that we can send a zproto format message
        #writer.bsend(b"1111sS4", 0xAA, 0xA0, 0x02, 0x01, b"key", b"value", 1234)
        #gossip = zgossip_msg_new ()
        #gossip.recv(reader)
        #self.assertEqual(gossip.id(), ZGOSSIP_MSG_PUBLISH)
        #del gossip

    def test_zactor(self):
        def _echo_actor(pipe, arg):
            # Do some initialization
            pipe = Zsock(pipe, False) # We don't own the pipe, so False.
            arg = string_at(arg)
            self.assertEqual(arg, b"Hello, World")
            pipe.signal(0) # startup ok

            terminated = False
            while not terminated:
                msg = Zmsg.recv(pipe)
                if not msg:
                    break # Interrupted

                command = msg.popstr()
                if command == b"$TERM":
                    # All actors must handle $TERM in this way
                    terminated = True
                elif command == b"ECHO":
                    # This is an example command for our test actor
                    Zmsg.send(msg, pipe)
                else:
                    self.fail(b"Unexpected message " + command)
        echo_actor = zactor_fn(_echo_actor) # ctypes function reference must live as long as the actor.

        arg_buffer = create_string_buffer(b"Hello, World") # argument buffer must live as long as the actor.
        actor = Zactor(echo_actor, arg_buffer)
        self.assertTrue(actor)

        actor.sock().send(b"ss", b"ECHO", b"This is a string");

        result = c_char_p()
        actor.sock().recv(b"s", byref(result))

        self.assertEqual(string_at(result), b"This is a string")

        libc.free(result)

if __name__ == '__main__':
    unittest.main()
