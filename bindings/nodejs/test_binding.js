/*  =========================================================================
    test_binding.js - hand-written test cases for NodeJS binding

    -------------------------------------------------------------------------
    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

var czmq = require ('bindings')('czmq')
var zstr = new czmq.Zstr ();

var server = new czmq.Zsock ('server')
server.bind ("inproc://test-endpoint")
var client = new czmq.Zsock (ZMQ_CLIENT)
client.connect ("inproc://test-endpoint")

zstr.send (client, "Hello")
var request = zstr.recv (server)
console.log (request)

zstr.send (server, "World")
var reply = zstr.recv (client)
console.log (reply)

server.destroy ()
client.destroy ()
