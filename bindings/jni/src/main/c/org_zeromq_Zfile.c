#include <jni.h>
#include <stdio.h>
#include "org_zeromq_Zfile.h"
#include "czmq.h"

JNIEXPORT void JNICALL Java_org_zeromq_Zfile_create (JNIEnv *env, jobject thisObj) {
}

JNIEXPORT void JNICALL Java_org_zeromq_Zfile_destroy (JNIEnv *env, jobject thisObj) {
}

//  Duplicate a file item, returns a newly constructed item. If the file
//  is null, or memory was exhausted, returns null.                     
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_dup (JNIEnv *env, jobject thisObj) {
}

//  Return file name, remove path if provided
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_filename (JNIEnv *env, jobject thisObj) {
}

//  Refresh file properties from disk; this is not done automatically   
//  on access methods, otherwise it is not possible to compare directory
//  snapshots.                                                          
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_restat (JNIEnv *env, jobject thisObj) {
}

//  Return when the file was last modified. If you want this to reflect the
//  current situation, call zfile_restat before checking this property.    
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_modified (JNIEnv *env, jobject thisObj) {
}

//  Return the last-known size of the file. If you want this to reflect the
//  current situation, call zfile_restat before checking this property.    
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_cursize (JNIEnv *env, jobject thisObj) {
}

//  Return true if the file is a directory. If you want this to reflect   
//  any external changes, call zfile_restat before checking this property.
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_isDirectory (JNIEnv *env, jobject thisObj) {
}

//  Return true if the file is a regular file. If you want this to reflect
//  any external changes, call zfile_restat before checking this property.
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_isRegular (JNIEnv *env, jobject thisObj) {
}

//  Return true if the file is readable by this process. If you want this to
//  reflect any external changes, call zfile_restat before checking this    
//  property.                                                               
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_isReadable (JNIEnv *env, jobject thisObj) {
}

//  Return true if the file is writeable by this process. If you want this 
//  to reflect any external changes, call zfile_restat before checking this
//  property.                                                              
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_isWriteable (JNIEnv *env, jobject thisObj) {
}

//  Check if file has stopped changing and can be safely processed.
//  Updates the file statistics from disk at every call.           
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_isStable (JNIEnv *env, jobject thisObj) {
}

//  Return true if the file was changed on disk since the zfile_t object
//  was created, or the last zfile_restat() call made on it.            
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_hasChanged (JNIEnv *env, jobject thisObj) {
}

//  Remove the file from disk
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_remove (JNIEnv *env, jobject thisObj) {
}

//  Open file for reading                             
//  Returns 0 if OK, -1 if not found or not accessible
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_input (JNIEnv *env, jobject thisObj) {
}

//  Open file for writing, creating directory if needed               
//  File is created if necessary; chunks can be written to file at any
//  location. Returns 0 if OK, -1 if error.                           
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_output (JNIEnv *env, jobject thisObj) {
}

//  Read chunk from file at specified position. If this was the last chunk,
//  sets the eof property. Returns a null chunk in case of error.          
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_read (JNIEnv *env, jobject thisObj) {
}

//  Returns true if zfile_read() just read the last chunk in the file.
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_eof (JNIEnv *env, jobject thisObj) {
}

//  Write chunk to file at specified position
//  Return 0 if OK, else -1                  
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_write (JNIEnv *env, jobject thisObj) {
}

//  Read next line of text from file. Returns a pointer to the text line,
//  or NULL if there was nothing more to read from the file.             
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_readln (JNIEnv *env, jobject thisObj) {
}

//  Close file, if open
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_close (JNIEnv *env, jobject thisObj) {
}

//  Return file handle, if opened
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_handle (JNIEnv *env, jobject thisObj) {
}

//  Calculate SHA1 digest for file, using zdigest class.
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_digest (JNIEnv *env, jobject thisObj) {
}

//  Self test of this class.
JNIEXPORT void JNICALL Java_org_zeromq_Zfile_test (JNIEnv *env, jobject thisObj) {
}
