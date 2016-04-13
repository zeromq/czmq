/*
    CZMQ exported constants and other initializations

    Copyright (c) the Contributors as noted in the AUTHORS file.       
    This file is part of CZMQ, the high-level C binding for 0MQ:       
    http://czmq.zeromq.org.                                            
                                                                       
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.           
 */

var czmq = require('bindings')('czmq')

/**
 * Expose the czmq bindings as the module.
 */
module.exports = czmq;

var types = {
    ZMQ_PAIR: 0,
    ZMQ_PUB: 1,
    ZMQ_SUB: 2,
    ZMQ_REQ: 3,
    ZMQ_REP: 4,
    ZMQ_DEALER: 5,
    ZMQ_ROUTER: 6,
    ZMQ_PULL: 7,
    ZMQ_PUSH: 8,
    ZMQ_XPUB: 9,
    ZMQ_XSUB: 10,
    ZMQ_STREAM: 11,
    ZMQ_SERVER: 12,
    ZMQ_CLIENT: 13,
    ZMQ_RADIO: 14,
    ZMQ_DISH: 15,
};

Object.assign(module.exports, types);
