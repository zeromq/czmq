#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zsock.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zsock_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zsock_destroy (JNIEnv *env, jobject thisObj) {
}

//  Create a PUB socket. Default action is bind.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newPub (JNIEnv *env, jobject thisObj) {
}

//  Create a SUB socket, and optionally subscribe to some prefix string. Default
//  action is connect.                                                          
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newSub (JNIEnv *env, jobject thisObj) {
}

//  Create a REQ socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newReq (JNIEnv *env, jobject thisObj) {
}

//  Create a REP socket. Default action is bind.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newRep (JNIEnv *env, jobject thisObj) {
}

//  Create a DEALER socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newDealer (JNIEnv *env, jobject thisObj) {
}

//  Create a ROUTER socket. Default action is bind.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newRouter (JNIEnv *env, jobject thisObj) {
}

//  Create a PUSH socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newPush (JNIEnv *env, jobject thisObj) {
}

//  Create a PULL socket. Default action is bind.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newPull (JNIEnv *env, jobject thisObj) {
}

//  Create an XPUB socket. Default action is bind.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newXpub (JNIEnv *env, jobject thisObj) {
}

//  Create an XSUB socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newXsub (JNIEnv *env, jobject thisObj) {
}

//  Create a PAIR socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newPair (JNIEnv *env, jobject thisObj) {
}

//  Create a STREAM socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newStream (JNIEnv *env, jobject thisObj) {
}

//  Create a SERVER socket. Default action is bind.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newServer (JNIEnv *env, jobject thisObj) {
}

//  Create a CLIENT socket. Default action is connect.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_newClient (JNIEnv *env, jobject thisObj) {
}

//  Bind a socket to a formatted endpoint. For tcp:// endpoints, supports   
//  ephemeral ports, if you specify the port number as "*". By default      
//  zsock uses the IANA designated range from C000 (49152) to FFFF (65535). 
//  To override this range, follow the "*" with "[first-last]". Either or   
//  both first and last may be empty. To bind to a random port within the   
//  range, use "!" in place of "*".                                         
//                                                                          
//  Examples:                                                               
//      tcp://127.0.0.1:*           bind to first free port from C000 up    
//      tcp://127.0.0.1:!           bind to random port from C000 to FFFF   
//      tcp://127.0.0.1:*[60000-]   bind to first free port from 60000 up   
//      tcp://127.0.0.1:![-60000]   bind to random port from C000 to 60000  
//      tcp://127.0.0.1:![55000-55999]                                      
//                                  bind to random port from 55000 to 55999 
//                                                                          
//  On success, returns the actual port number used, for tcp:// endpoints,  
//  and 0 for other transports. On failure, returns -1. Note that when using
//  ephemeral ports, a port may be reused by different services without     
//  clients being aware. Protocols that run on ephemeral ports should take  
//  this into account.                                                      
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_bind (JNIEnv *env, jobject thisObj) {
}

//  Returns last bound endpoint, if any.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_endpoint (JNIEnv *env, jobject thisObj) {
}

//  Unbind a socket from a formatted endpoint.                     
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.                                               
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_unbind (JNIEnv *env, jobject thisObj) {
}

//  Connect a socket to a formatted endpoint        
//  Returns 0 if OK, -1 if the endpoint was invalid.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_connect (JNIEnv *env, jobject thisObj) {
}

//  Disconnect a socket from a formatted endpoint                  
//  Returns 0 if OK, -1 if the endpoint was invalid or the function
//  isn't supported.                                               
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_disconnect (JNIEnv *env, jobject thisObj) {
}

//  Attach a socket to zero or more endpoints. If endpoints is not null,     
//  parses as list of ZeroMQ endpoints, separated by commas, and prefixed by 
//  '@' (to bind the socket) or '>' (to connect the socket). Returns 0 if all
//  endpoints were valid, or -1 if there was a syntax error. If the endpoint 
//  does not start with '@' or '>', the serverish argument defines whether   
//  it is used to bind (serverish = true) or connect (serverish = false).    
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_attach (JNIEnv *env, jobject thisObj) {
}

//  Returns socket type as printable constant string.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_typeStr (JNIEnv *env, jobject thisObj) {
}

//  Send a 'picture' message to the socket (or actor). The picture is a   
//  string that defines the type of each frame. This makes it easy to send
//  a complex multiframe message in one call. The picture can contain any 
//  of these characters, each corresponding to one or two arguments:      
//                                                                        
//      i = int (signed)                                                  
//      1 = uint8_t                                                       
//      2 = uint16_t                                                      
//      4 = uint32_t                                                      
//      8 = uint64_t                                                      
//      s = char *                                                        
//      b = byte *, size_t (2 arguments)                                  
//      c = zchunk_t *                                                    
//      f = zframe_t *                                                    
//      h = zhashx_t *                                                    
//      U = zuuid_t *                                                     
//      p = void * (sends the pointer value, only meaningful over inproc) 
//      m = zmsg_t * (sends all frames in the zmsg)                       
//      z = sends zero-sized frame (0 arguments)                          
//      u = uint (deprecated)                                             
//                                                                        
//  Note that s, b, c, and f are encoded the same way and the choice is   
//  offered as a convenience to the sender, which may or may not already  
//  have data in a zchunk or zframe. Does not change or take ownership of 
//  any arguments. Returns 0 if successful, -1 if sending failed for any  
//  reason.                                                               
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_send (JNIEnv *env, jobject thisObj) {
}

//  Send a 'picture' message to the socket (or actor). This is a va_list 
//  version of zsock_send (), so please consult its documentation for the
//  details.                                                             
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_vsend (JNIEnv *env, jobject thisObj) {
}

//  Receive a 'picture' message to the socket (or actor). See zsock_send for
//  the format and meaning of the picture. Returns the picture elements into
//  a series of pointers as provided by the caller:                         
//                                                                          
//      i = int * (stores signed integer)                                   
//      4 = uint32_t * (stores 32-bit unsigned integer)                     
//      8 = uint64_t * (stores 64-bit unsigned integer)                     
//      s = char ** (allocates new string)                                  
//      b = byte **, size_t * (2 arguments) (allocates memory)              
//      c = zchunk_t ** (creates zchunk)                                    
//      f = zframe_t ** (creates zframe)                                    
//      U = zuuid_t * (creates a zuuid with the data)                       
//      h = zhashx_t ** (creates zhashx)                                    
//      p = void ** (stores pointer)                                        
//      m = zmsg_t ** (creates a zmsg with the remaing frames)              
//      z = null, asserts empty frame (0 arguments)                         
//      u = uint * (stores unsigned integer, deprecated)                    
//                                                                          
//  Note that zsock_recv creates the returned objects, and the caller must  
//  destroy them when finished with them. The supplied pointers do not need 
//  to be initialized. Returns 0 if successful, or -1 if it failed to recv  
//  a message, in which case the pointers are not modified. When message    
//  frames are truncated (a short message), sets return values to zero/null.
//  If an argument pointer is NULL, does not store any value (skips it).    
//  An 'n' picture matches an empty frame; if the message does not match,   
//  the method will return -1.                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_recv (JNIEnv *env, jobject thisObj) {
}

//  Receive a 'picture' message from the socket (or actor). This is a    
//  va_list version of zsock_recv (), so please consult its documentation
//  for the details.                                                     
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_vrecv (JNIEnv *env, jobject thisObj) {
}

//  Send a binary encoded 'picture' message to the socket (or actor). This 
//  method is similar to zsock_send, except the arguments are encoded in a 
//  binary format that is compatible with zproto, and is designed to reduce
//  memory allocations. The pattern argument is a string that defines the  
//  type of each argument. Supports these argument types:                  
//                                                                         
//   pattern    C type                  zproto type:                       
//      1       uint8_t                 type = "number" size = "1"         
//      2       uint16_t                type = "number" size = "2"         
//      4       uint32_t                type = "number" size = "3"         
//      8       uint64_t                type = "number" size = "4"         
//      s       char *, 0-255 chars     type = "string"                    
//      S       char *, 0-2^32-1 chars  type = "longstr"                   
//      c       zchunk_t *              type = "chunk"                     
//      f       zframe_t *              type = "frame"                     
//      u       zuuid_t *               type = "uuid"                      
//      m       zmsg_t *                type = "msg"                       
//      p       void *, sends pointer value, only over inproc              
//                                                                         
//  Does not change or take ownership of any arguments. Returns 0 if       
//  successful, -1 if sending failed for any reason.                       
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_bsend (JNIEnv *env, jobject thisObj) {
}

//  Receive a binary encoded 'picture' message from the socket (or actor).  
//  This method is similar to zsock_recv, except the arguments are encoded  
//  in a binary format that is compatible with zproto, and is designed to   
//  reduce memory allocations. The pattern argument is a string that defines
//  the type of each argument. See zsock_bsend for the supported argument   
//  types. All arguments must be pointers; this call sets them to point to  
//  values held on a per-socket basis. Do not modify or destroy the returned
//  values. Returns 0 if successful, or -1 if it failed to read a message.  
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_brecv (JNIEnv *env, jobject thisObj) {
}

//  Return socket routing ID if any. This returns 0 if the socket is not
//  of type ZMQ_SERVER or if no request was already received on it.     
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_routingId (JNIEnv *env, jobject thisObj) {
}

//  Set routing ID on socket. The socket MUST be of type ZMQ_SERVER.        
//  This will be used when sending messages on the socket via the zsock API.
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_setRoutingId (JNIEnv *env, jobject thisObj) {
}

//  Set socket to use unbounded pipes (HWM=0); use this in cases when you are
//  totally certain the message volume can fit in memory. This method works  
//  across all versions of ZeroMQ. Takes a polymorphic socket reference.     
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_setUnbounded (JNIEnv *env, jobject thisObj) {
}

//  Send a signal over a socket. A signal is a short message carrying a   
//  success/failure code (by convention, 0 means OK). Signals are encoded 
//  to be distinguishable from "normal" messages. Accepts a zock_t or a   
//  zactor_t argument, and returns 0 if successful, -1 if the signal could
//  not be sent. Takes a polymorphic socket reference.                    
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_signal (JNIEnv *env, jobject thisObj) {
}

//  Wait on a signal. Use this to coordinate between threads, over pipe  
//  pairs. Blocks until the signal is received. Returns -1 on error, 0 or
//  greater on success. Accepts a zsock_t or a zactor_t as argument.     
//  Takes a polymorphic socket reference.                                
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_wait (JNIEnv *env, jobject thisObj) {
}

//  If there is a partial message still waiting on the socket, remove and    
//  discard it. This is useful when reading partial messages, to get specific
//  message types.                                                           
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_flush (JNIEnv *env, jobject thisObj) {
}

//  Probe the supplied object, and report if it looks like a zsock_t.
//  Takes a polymorphic socket reference.                            
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_is (JNIEnv *env, jobject thisObj) {
}

//  Probe the supplied reference. If it looks like a zsock_t instance, return
//  the underlying libzmq socket handle; else if it looks like a file        
//  descriptor, return NULL; else if it looks like a libzmq socket handle,   
//  return the supplied value. Takes a polymorphic socket reference.         
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_resolve (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class
JNIEXPORT void JNICALL Java_org_zeromq_Zsock_test (JNIEnv *env, jobject thisObj) {
}
