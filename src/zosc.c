/*  =========================================================================
    zosc - work with Open Sound Control messages

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.
    =========================================================================
*/

/*
@header
    zosc - work with Open Sound Control messages
@discuss
@end
*/

#ifdef WIN32
#include <winsock2.h>           //  needed for ntohll/htonll
#endif
#include "czmq_classes.h"
#if __unix__ && !__APPLE__
#include <endian.h>
#define htonll(x) htobe64(x)
#define ntohll(x) be64toh(x)
#endif
//  Structure of our class

struct _zosc_t {
    char        *address;       //  the osc address string
    char        *format;        //  the string containing type hints
    zchunk_t    *chunk;         //  zchunk containing the complete OSC data
    size_t      data_begin;     //  start position of actual OSC data
    // time tag???
};


//  --------------------------------------------------------------------------
//  Create a new zosc

zosc_t *
zosc_new (const char *address)
{
    zosc_t *self = (zosc_t *) zmalloc (sizeof (zosc_t));
    assert (self);
    //  Initialize class properties here
    self->address = strdup(address);
    assert(self->address);
    self->chunk = zchunk_new(NULL, 0);
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zosc

void
zosc_destroy (zosc_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zosc_t *self = *self_p;
        //  Free class properties here
        self->address = NULL;
        self->format = NULL;
        zchunk_destroy(&self->chunk);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}

zosc_t *
zosc_fromframe (zframe_t *frame)
{
    assert (frame);
    assert (zframe_is(frame));
    size_t data_len = zframe_size(frame);
    assert (data_len);
    //  unfortunately we need to copy the data
    //  otherwise we cannot destroy the zframe
    byte *data = (byte *)zmalloc( data_len );
    memcpy(data, zframe_data(frame), data_len);
    assert(data);
    zframe_destroy(&frame);
    return zosc_frommem((char *)data, data_len);
}

zosc_t *
zosc_frommem (char *data, size_t size)
{
    assert(data);
    size_t needle = 0;
    size_t format_start = 0;

    // an OSC message always starts with a null terminated string
    while (data[needle] != '\0' )
        needle++;
    if ( needle >= size )
    {
        zsys_error("invalid OSC message we can't find an address string");
        return NULL;
    }
    needle++;

    // find the format string
    while( data[needle] != ',' )
        needle++;
    format_start = needle;   // save needle location for later
    // find end of format string
    while (needle < size && data[needle] != '\0' )
        needle++;
    if ( needle >= size )
    {
        zsys_error("invalid OSC message we can't find the format string");
        return NULL;
    }
    needle++;           // include terminating 0
    format_start++;     // we forget the first ','

    // create our instance
    zosc_t *self = (zosc_t *) zmalloc (sizeof (zosc_t));
    assert (self);

    // copy the address string
    self->address = data; // without copy
    //self->address = zmalloc( address_end * sizeof(char) );
    //memcpy(self->address, data, address_end * sizeof(char));

    // copy the format string
    self->format = data+format_start; // without copy
    //size_t format_size = needle - format_start;
    //self->format = zmalloc( format_size * sizeof(char) );
    //memcpy(self->format, data+format_start, format_size * sizeof(char));

    needle = (needle + 3) & (size_t)~0x03; // advance to the next multiple of 4 after trailing '\0'
    assert(needle <= size);
    self->chunk = zchunk_frommem(data, size, NULL, NULL);  // without copy
    //self->chunk = zchunk_new(data+needle, (size - needle) * sizeof(char));
    self->data_begin = needle;
    assert(self->chunk);
    return self;
}

zosc_t *
zosc_create (const char *address, const char *format, ...)
{
    assert(address);
    assert(format);
    // create our instance
    zosc_t *self = (zosc_t *) zmalloc (sizeof (zosc_t));
    assert (self);

    size_t init_size = strlen(address) + strlen(format) * 5; // 5 times format string as almost all types are 4 bytes
    init_size += 2;       // to count for the two terminating \0's
    self->chunk = zchunk_new(NULL, init_size);

    size_t size = zchunk_extend(self->chunk, address, strlen(address) + 1);
    size_t newsize = (size + 3) & (size_t)~0x03;
    // extend the chunk to a multitude of 4
    if (newsize-size)
        size = zchunk_extend(self->chunk, "\x00\x00\x00\x00", newsize-size);
    size = zchunk_extend(self->chunk, ",", sizeof (char));
    size_t format_start = size; // save the start position of the format string
    size = zchunk_extend(self->chunk, format, strlen(format) + 1 );

    // extend the chunk to a multitude of 4
    newsize = (size + 3) & (size_t)~0x03;
    if (newsize-size)
        size = zchunk_extend(self->chunk, "\x00\x00\x00\x00", newsize-size);

    // save the pointer to where the data starts
    self->data_begin = size;

    va_list argptr;
    va_start(argptr, format);
    while(*format)
    {
        switch (*format)
        {
            case 'b':
            {
                // todo handle bundles

            }
            case 'i' :
            {
                uint32_t int_v = va_arg( argptr, uint32_t );
                int_v = htonl(int_v);
                zchunk_extend( self->chunk, &int_v, sizeof(uint32_t) );
                break;
            }
            case 'h':
            {
                uint64_t int_v = va_arg( argptr, uint64_t );
                int_v = htonll(int_v);
                zchunk_extend( self->chunk, &int_v, sizeof(uint64_t) );
                break;
            }
            case 'f':
            {
                float flt_v = (float)va_arg( argptr, double );
                uint32_t flt_int = htonl(*((uint32_t *) &flt_v));
                zchunk_extend( self->chunk, &flt_int, sizeof(float) );
                break;
            }
            case 'd':
            {
                double dbl_v = (double)va_arg( argptr, double );
                uint64_t dbl_int = htonll(*((uint64_t *) &dbl_v));
                zchunk_extend( self->chunk, &dbl_int, sizeof(double) );
                break;
            }
            case 's':
            {
                //  not sure if the double pointer is the way to go
                char *str = va_arg( argptr, char * );
                assert(str);
                if (str)
                {
                    size = zchunk_extend( self->chunk, str, strlen(str)+1);
                    //  osc dictates we need to supply data in multitudes of 4 bytes so append \0's if needed
                    newsize = (size + 3) & (size_t)~0x03;
                    if (newsize-size)
                        size = zchunk_extend(self->chunk, "\x00\x00\x00\x00", newsize-size);
                }
                break;
            }
            case 'S': // never used???
                break;
            case 'c': // is this ever used?
            {
                char char_v = (char)va_arg( argptr, int); // the standard dictates to use int for smaller vars
                //  osc dictates we need to supply data in multitudes of 4 bytes so create a 0 array to hold our char
                //  Not sure if this correct on big endian machines!!!
                char v[5] = "\x00\x00\x00\x00";
                v[3] = char_v;
                size = zchunk_extend( self->chunk, v, sizeof(v) );
                break;
            }
            case 'm': // midi data
            {
                uint32_t midi_v = va_arg( argptr, uint32_t);
                zchunk_extend( self->chunk, &midi_v, sizeof(uint32_t) );
                break;
            }
            case 'T':
            case 'F':
            case 'N': // never used???
            case 'I': // never used???
            {
                break;
            }
            default:
                zsys_error("format identifier '%c' not matched", *format);
        }
        format++;
    }
    // the chunk data starts with the address string
    self->address =  (char *)zchunk_data(self->chunk);
    // set the format string to the pointer in the chunk of data
    self->format = (char *)zchunk_data(self->chunk)+format_start;
    return self;
}

size_t
zosc_size (zosc_t *self)
{
    assert(self);
    return zchunk_size(self->chunk);
}

byte *
zosc_data (zosc_t *self)
{
    assert(self);
    return zchunk_data(self->chunk);
}

const char *
zosc_address (zosc_t *self)
{
    assert(self);
    assert(self->address);
    return self->address;
}

//  *** Draft method, for development use, may change without warning ***
//  Return the OSC format of the message.
//    i - 32bit integer
//    h - 64bit integer
//    f - 32bit floating point number
//    d - 64bit (double) floating point number
//    s - string
//    S - symbol
//    c - char
//    m - 4 byte midi packet (8 digits hexadecimal)
//    T - TRUE (no value required)
//    F - FALSE (no value required)
//    N - NIL (no value required)
//    I - INFINITUM (no value required)
const char *
zosc_format (zosc_t *self)
{
    assert(self);
    assert(self->format);
    return self->format;
}

int
zosc_retr(zosc_t *self, const char *format, ...)
{
    assert(self);
    assert(format);
    assert(self->format);

    va_list argptr;
    va_start(argptr, format);
    int rc = 0;
    size_t needle = self->data_begin;
    //zchunk_print(self->chunk);
    while(*format)
    {
        switch (*format)
        {
            case 'i' :
            {
                uint32_t *int_p = va_arg( argptr, uint32_t * );
                if ( int_p )
                    *int_p = ntohl(*(uint32_t*)(zchunk_data( self->chunk ) + needle));
                needle += sizeof (uint32_t);
                break;
            }
            case 'h':
            {
                uint64_t *int_p = va_arg( argptr, uint64_t * );
                if ( int_p )
                    *int_p = ntohll(*(uint64_t*)(zchunk_data( self->chunk ) + (needle * sizeof (char) )));
                needle += sizeof (uint64_t);
                break;
            }
            case 'f':
            {
                float *flt_p = va_arg( argptr, float * );
                if ( flt_p )
                    *(uint32_t*)flt_p = ntohl(*(uint32_t*)(zchunk_data( self->chunk ) + needle));
                needle += sizeof (float);
                break;
            }
            case 'd':
            {
                double *dbl_p = va_arg( argptr, double * );
                if ( dbl_p )
                    *(uint64_t*)dbl_p = ntohll(*(uint64_t*)(zchunk_data( self->chunk ) + needle));
                needle += sizeof (double);
                break;
            }
            case 's':
            {
                //  not sure if the double pointer is the way to go
                char **str_pp = va_arg( argptr, char ** );
                if (str_pp)
                {
                    //  this was tricky
                    char *str_p = *str_pp;
                    str_p = strdup((char*)(zchunk_data( self->chunk ) + needle));
                    *str_pp = str_p;
                }
                size_t l = strlen((char*)(zchunk_data( self->chunk ) + needle));
                needle += l + 1;
                needle = (needle + 3) & (size_t)~0x03;
                break;
            }
            case 'S': // never used???
                break;
            case 'c':
            {
                char *char_p = va_arg( argptr, char * );
                if ( char_p )
                    //  we receive 4 btes of which the last one hold our
                    //  char therefore we need to access needle + 3
                    //  not sure this is valid on big endian machines
                    *char_p = (*(char*)(zchunk_data( self->chunk ) +
                                        needle + 3));
                needle += sizeof (int);  // advance multitude of 4!
                break;
            }
            case 'm':
            {
                uint32_t *midi_p = va_arg( argptr, uint32_t * );
                if ( midi_p )
                    *midi_p = ntohl(*(uint32_t *)(zchunk_data( self->chunk ) + needle));
                needle += sizeof (uint32_t);
                break;
            }
            case 'T':
            {
                // value only determined based on the format!
                bool *bool_p = va_arg( argptr, bool * );
                if ( bool_p )
                    *bool_p = true;
                break;
            }
            case 'F':
            {
                bool *bool_p = va_arg( argptr, bool * );
                if ( bool_p )
                    *bool_p = false;
                break;
            }
            case 'N': // never used???
            case 'I': // never used???
            {
                needle++;
                break;
            }
        default:
            zsys_error("format identifier '%c' not matched", *format);


        }
        format++;
    }
    return rc;
}

//  Create copy of the message, as new chunk object. Returns a fresh zosc_t
//  object, or null if there was not enough heap memory. If chunk is null,
//  returns null.
//  Caller owns return value and must destroy it when done.
zosc_t *
zosc_dup (zosc_t *self)
{
    byte* data = (byte *)zmalloc( zchunk_size(self->chunk) );
    memcpy(data, zchunk_data(self->chunk), zchunk_size( self->chunk ));
    return zosc_frommem( (char *)data, zchunk_size( self->chunk ));
}

//  Transform zosc into a zframe that can be sent in a message.
//  Caller owns return value and must destroy it when done.
zframe_t *
zosc_pack (zosc_t *self)
{
    assert(self);
    assert(self->chunk);
    return zchunk_pack (self->chunk);
}

//  Transform zosc into a zframe that can be sent in a message.
//  Take ownership of the chunk.
//  Caller owns return value and must destroy it when done.
zframe_t *
zosc_packx (zosc_t **self_p)
{
    assert(self_p);
    //  detach the chunk from the zosc struct
    zchunk_t *data = (*self_p)->chunk;
    (*self_p)->chunk = NULL;
    zosc_destroy(self_p);
    return zchunk_packx( &data );
}

//  Transform a zframe into a zosc.
//  Caller owns return value and must destroy it when done.
zosc_t *
zosc_unpack (zframe_t *frame)
{
    return zosc_fromframe(frame);
}

//  Dump OSC message to stderr, for debugging and tracing.
void
zosc_print (zosc_t *self)
{
    zchunk_print(self->chunk);
}

bool
zosc_is (void *self)
{
    zosc_t *test = (zosc_t *)self;
    // for now just test if there's an address and format string
    return strlen( test->address ) && strlen( test->format );
}

//  --------------------------------------------------------------------------
//  Self test of this class

// If your selftest reads SCMed fixture data, please keep it in
// src/selftest-ro; if your test creates filesystem objects, please
// do so under src/selftest-rw.
// The following pattern is suggested for C selftest code:
//    char *filename = NULL;
//    filename = zsys_sprintf ("%s/%s", SELFTEST_DIR_RO, "mytemplate.file");
//    assert (filename);
//    ... use the "filename" for I/O ...
//    zstr_free (&filename);
// This way the same "filename" variable can be reused for many subtests.
#define SELFTEST_DIR_RO "src/selftest-ro"
#define SELFTEST_DIR_RW "src/selftest-rw"

void
zosc_test (bool verbose)
{
    printf (" * zosc: ");
#ifdef __WINDOWS__
    zsys_init();
#endif
    // $ oscsend localhost 7777 /sample/address iTfs 1 3.14 hello
    /*     16 wide
    0000   2f 73 61 6d 70 6c 65 2f 61 64 64 72 65 73 73 00   /sample/address.
    0010   2c 69 54 66 73 00 00 00 00 00 00 01 40 48 f5 c3   ,iTfs.......@H..
    0020   68 65 6c 6c 6f 00 00 00                           hello...
    */
    // /sample/address,iTfs@HõÃhello
    //2f73616d706c652f61646472657373002c69546673000000000000014048f5c368656c6c6f000000
    char oscpacket[41] = "\x2f\x73\x61\x6d\x70\x6c" \
    "\x65\x2f\x61\x64\x64\x72\x65\x73\x73\x00\x2c\x69\x54\x66\x73\x00" \
    "\x00\x00\x00\x00\x00\x01\x40\x48\xf5\xc3\x68\x65\x6c\x6c\x6f\x00" \
    "\x00\x00";

    // oscsend localhost 7777 /test ihTfds 2 1844674407370 3.14 3.1415926535897932 hello
    char testpack[49] = "\x2f\x74\x65\x73\x74\x00\x00\x00\x2c\x69\x68\x54\x66\x64\x73\x00" \
    "\x00\x00\x00\x02\x00\x00\x01\xad\x7f\x29\xab\xca\x40\x48\xf5\xc3" \
    "\x40\x09\x21\xfb\x54\x44\x2d\x18\x68\x65\x6c\x6c\x6f\x00\x00\x00";

    //  @selftest
    // we need to have packets on the heap
    char *p1 = (char *)malloc(40);
    memcpy(p1, oscpacket, 40);
    zosc_t *self = zosc_frommem(p1, 40);
    assert (self);
    assert ( zosc_is( self ));
    assert(streq ( zosc_address(self), "/sample/address"));
    assert(streq ( zosc_format(self), "iTfs"));
    // check value
    uint32_t test = 0;
    bool b = false;
    float f = 0.f;
    char *s = "BLA";
    zosc_retr(self, "iTfs", &test, &b, &f, &s);
    assert(test == 1);
    assert(b);
    assert(fabsf(f - 3.14f) < FLT_EPSILON); // float equal
    assert(streq(s, "hello"));
    zstr_free(&s);

    zosc_destroy (&self);

    zframe_t *frame = zframe_new(oscpacket, 40);
    assert(frame);
    assert(zframe_size(frame) == 40);
    zosc_t *fosc = zosc_fromframe(frame);
    assert (fosc);
    assert(streq ( zosc_address(fosc), "/sample/address"));
    assert(streq ( zosc_format(fosc), "iTfs"));
    // test skipping retrieving values
    zosc_retr(fosc, "iTfs", NULL, NULL, NULL, NULL);
    char *fstr = "";
    zosc_retr(fosc, "iTfs", NULL, NULL, NULL, &fstr);
    assert( streq(fstr, "hello") );
    zstr_free(&fstr);
    zosc_destroy (&fosc);

    // to the heap, otherwise we can't destroy
    char *p2 = (char *)malloc(48);
    memcpy(p2, testpack, 48);
    zosc_t *testmsg = zosc_frommem(p2, 48);
    assert( testmsg );
    assert(streq ( zosc_address(testmsg), "/test"));
    const char *format = zosc_format(testmsg);
    assert(streq ( zosc_format(testmsg), "ihTfds" ) );
    // check value
    int itest = -1;
    int64_t ltest = -1;
    bool btest = false;
    float ftest = -1.f;
    double dtest = -1.;
    char *stest;
    zosc_retr(testmsg, format, &itest, &ltest, &btest, &ftest, &dtest, &stest);
    assert( itest == 2 );
    assert( ltest == 1844674407370 );
    assert( btest );
    assert( fabsf(ftest - 3.14f) < FLT_EPSILON ); // float equal
    assert( fabs(dtest - 3.1415926535897932) < DBL_EPSILON );
    assert( streq(stest, "hello") );
    zstr_free(&stest);
    zosc_destroy(&testmsg);

    // test constructing messages
    int64_t prez = 3;
    zosc_t* conm = zosc_create("/construct", "iihfdscF", 1,2, prez, 3.14,6.283185307179586, "greetings", 'q');
    assert(conm);
    assert(streq(zosc_address(conm), "/construct"));
    assert(streq(zosc_format(conm), "iihfdscF"));
    int x,y;
    int64_t z;
    float zz;
    double zzz;
    char *ss = "aliens";
    char q = 'z';
    bool Ftest = true;
    zosc_retr(conm, "iihfdscF", &x, &y, &z, &zz, &zzz, &ss, &q, &Ftest);
    assert( x==1 );
    assert( y==2 );
    assert( z==3 );
    assert( fabsf(zz-3.14f) < FLT_EPSILON );
    assert( fabs(zzz-6.283185307179586) < DBL_EPSILON );
    assert( streq( ss, "greetings") );
    assert( q == 'q');
    assert( !Ftest );
    zstr_free(&ss);

#ifdef ZMQ_DGRAM
    zsock_t *dgrams = zsock_new_dgram("udp://*:*");
    assert (dgrams);
    zframe_t *t = zosc_pack(conm);
    zstr_sendm( dgrams, "127.0.0.1:7777" );
    zframe_send( &t, dgrams, 0);
    zclock_sleep(10); // slow down, otherwise we've cleaned up before async send
    zsock_destroy( &dgrams );
#else
    int sockfd;
    struct sockaddr_in     servaddr;

    // Creating socket file descriptor
    if ( (sockfd = (int)socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        zsys_error("socket creation failed");
    }

    memset(&servaddr, 0, sizeof(servaddr));
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(7777);
    servaddr.sin_addr.s_addr = INADDR_ANY;
#ifdef __WINDOWS__
    sendto(sockfd, (const char *)zosc_data(conm), (int)zosc_size(conm),
            0, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));
#else
    sendto(sockfd, zosc_data(conm), zosc_size(conm),
            0, (const struct sockaddr *) &servaddr,
                sizeof(servaddr));
#endif
#endif
    zosc_destroy(&conm);
#ifdef __WINDOWS__
    zsys_shutdown();
#endif
    //  @end
    printf ("OK\n");
}
