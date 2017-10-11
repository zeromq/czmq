//
// bjornwennberg71@gmail.com
//
// super-simple test-server that reads a string from test-client
//
// build.sh first
// macosx: node ./test-server.js
// linux:  nodejs ./test-server.js
//
//var czmq = require ('bindings')('czmq')
var czmq = require ('czmq')
var zstr = new czmq.Zstr ();

var host = "tcp://127.0.0.1:5678";
var server = new czmq.Zsock ('server')
server.bind (host);

console.log("server[" + host + "]: ");
 
reply = zstr.recv (server)
console.log("400: " + reply);

server.destroy ()
