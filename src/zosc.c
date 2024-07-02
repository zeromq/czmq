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

#if defined( WIN32 )
  #include <winsock2.h>           //  needed for ntohll/htonll
#endif
#include "czmq_classes.h"
#if defined(__MINGW32__)
  #define htonll(x) ((((uint64_t)htonl(x&0xFFFFFFFF)) << 32) + htonl(x >> 32))
  #define ntohll(x) ((((uint64_t)ntohl(x&0xFFFFFFFF)) << 32) + ntohl(x >> 32))
#elif defined(__UTYPE_FREEBSD) || defined(__UTYPE_NETBSD)
  #include <sys/endian.h>
  #define htonll(x) htobe64(x)
  #define ntohll(x) be64toh(x)
#elif defined(__UTYPE_OSX) || defined(__UTYPE_IOS)
  #include <machine/endian.h>
#elif defined(__UNIX__)
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
    int         cursor_index;   //  cursor position of the iterator
    size_t      *data_indexes;  //  data offset positions of the elements in the message
};

// appends data to the chunk returning the new size, returns -1 on failure
static size_t
s_append_data(zchunk_t *chunk, const char *format, va_list argptr)
{
    size_t size = (size_t)-1;
    while (*format)
    {
        char typetag = *format;
        switch (typetag)
        {
            case 'b':
            {
                // todo handle bundles
                zsys_error("bundles or blobs not yet implemented");
                break;
            }
            case 'i' :
            {
                uint32_t int_v = va_arg( argptr, uint32_t );
                int_v = htonl(int_v);
                size = zchunk_extend( chunk, &int_v, sizeof(uint32_t) );
                break;
            }
            case 'h':
            {
                uint64_t int_v = va_arg( argptr, uint64_t );
                int_v = htonll(int_v);
                size = zchunk_extend( chunk, &int_v, sizeof(uint64_t) );
                break;
            }
            case 'f':
            {
                float flt_v = (float)va_arg( argptr, double );
                uint32_t flt_int = htonl(*((uint32_t *) &flt_v));
                size = zchunk_extend( chunk, &flt_int, sizeof(float) );
                break;
            }
            case 'd':
            {
                double dbl_v = (double)va_arg( argptr, double );
                uint64_t dbl_int = htonll(*((uint64_t *) &dbl_v));
                size = zchunk_extend( chunk, &dbl_int, sizeof(double) );
                break;
            }
            case 's':
            {
                //  not sure if the double pointer is the way to go
                char *str = va_arg( argptr, char * );
                assert(str);
                if (str)
                {
                    size = zchunk_extend( chunk, str, strlen(str)+1);
                    //  osc dictates we need to supply data in multitudes of 4 bytes so append \0's if needed
                    size_t newsize = (size + 3) & (size_t)~0x03;
                    if (newsize-size)
                        size = zchunk_extend(chunk, "\x00\x00\x00\x00", newsize-size);
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
                char v[5] = "\x00\x00\x00\x00"; // a char array on windows always wants a terminating null so we need size 5 :|
                v[3] = char_v;
                size = zchunk_extend( chunk, v, 4 );
                break;
            }
            case 'm': // midi data
            {
                uint32_t midi_v = va_arg( argptr, uint32_t);
                size = zchunk_extend( chunk, &midi_v, sizeof(uint32_t) );
                break;
            }
            case 'T':
            case 'F':
            case 'N': // never used???
            case 'I': // never used???
            {
                // booleans and infinite are not added as data but only as type tag
                // just return the current size
                size = zchunk_size(chunk);
                break;
            }
            default:
                zsys_error("format identifier '%c' not matched", typetag);
        }
        format++;
    }
    static bool warn_jumbo_once = false;
    static bool warn_ether_once = false;
    if ( size > 8192 && !warn_jumbo_once )
    {
        warn_jumbo_once = true;
        zsys_debug("The packet size exceeds 8192 bytes. It's fine for ZMTP but for DGRAM(UDP) it only works on rare networks. This warning is only echoed once");
    }
    else
    if ( size > 508 && !warn_ether_once )
    {
        warn_ether_once = true;
        zsys_debug("The packet size exceeds 508 bytes. It's fine for ZMTP but for DGRAM(UDP) it might not work. This warning is only echoed once");
    }
    return size;
}

//  --------------------------------------------------------------------------
//  Create a new zosc

zosc_t *
zosc_new (const char *address)
{
    zosc_t *self = (zosc_t *) zmalloc (sizeof (zosc_t));
    assert (self);
    //  Initialize class properties here
    self->format = "";
    self->chunk = zchunk_new(address, strlen(address) + 1);
    self->address = (char *)zchunk_data(self->chunk);
    assert(self->address);
    assert(self->format);
    self->data_begin = 0;
    self->data_indexes = NULL;
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
        if (self->data_indexes)
            free(self->data_indexes);

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
zosc_fromstring (const char *oscstr)
{
    // split spaces
    char *input = strdup(oscstr);
    char *address = strtok(input, " ");
    if (address == NULL)
    {
        // no spaces return
        zstr_free(&input);
        return NULL;
    }
    char *format = strtok(NULL, " ");
    if (format == NULL)
    {
        // no format
        zstr_free(&input);
        return NULL;
    }
    zosc_t *oscm = zosc_new(address);
    int i = 0;
    char type = *format;
    while( type != 0 )
    {
        char *value = NULL;
        if (type != 'F' && type != 'T' )
        {
            value = strtok(NULL, " ");
            if (value == NULL)
                goto error;
        }

        switch (type)
        {
            case('i'):
            {
                int32_t v = atoi(value);
                zosc_append(oscm, "i", v);
                break;
            }
            case('h'):
            {
                int64_t v = atol(value);
                zosc_append(oscm, "h", v);
                break;
            }
            case('f'):
            {
                float v = (float)atof(value);
                zosc_append(oscm, "f", v);
                break;
            }
            case('d'):
            {
                double v = atof(value);
                zosc_append(oscm, "d", v);
                break;
            }
            case('s'):
            {
                zosc_append(oscm, "s", value);
                break;
            }
            case('c'):
            {
                zosc_append(oscm, "c", value[0]);
                break;
            }
            case('m'):
            {
                // TODO midi
                //zosc_append(oscm, "m", value);
                break;
            }
            case('F'):
            {
                zosc_append(oscm, "F", NULL);
                break;
            }
            case('T'):
            {
                zosc_append(oscm, "T", NULL);
                break;
            }
            default:
            {
                zsys_info("type tag is %c", type);
                break;
            }
        }
        i++;                // get next character
        type = *(format+i);
    }
    zstr_free(&input);
    return oscm;

error:
    zstr_free(&input);
    zosc_destroy(&oscm);
    return NULL;
}


zosc_t *
zosc_create (const char *address, const char *format, ...)
{
    assert(address);
    assert(format);
    // create our instance
    zosc_t *self = (zosc_t *) zmalloc (sizeof (zosc_t));
    assert (self);

    size_t init_size = strlen(address) + strlen(format) * 10; // 10 times format string as almost all types are 4 bytes, just a guess
    init_size += 2;       // to count for the two terminating \0's
    self->chunk = zchunk_new(NULL, init_size);
    self->data_indexes = NULL;

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
    s_append_data(self->chunk, format, argptr);
    va_end(argptr);
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
zosc_append(zosc_t *self, const char *format, ...)
{
    assert(self);
    assert(format);

    // create new format string (format_cur + format)
    unsigned long formatlen = strlen(format)+ strlen(self->format) + 1; // +1 for the 0 byte
    unsigned int aligned = (unsigned)(((int)formatlen + 3) &~0x03);
    // for safety we'd better do this on the heap??? which is slower.
#ifdef _MSC_VER
    char *new_format = (char *)_malloca(aligned);  // VLA's char new_format[aligned] not supported on MSVC
    snprintf(new_format, sizeof(char) * aligned, "%s%s", self->format, format);
#else
    char new_format[aligned];
    snprintf(new_format, sizeof(new_format), "%s%s", self->format, format);
#endif
    // create a new chunk
    size_t init_size = 2 + strlen(self->address) + aligned * 10; // 10 times format string as almost all types are 4 bytes and we need space
    zchunk_t *newchunk = zchunk_new(NULL, init_size);
    // insert the address
    size_t size = zchunk_extend(newchunk, self->address, strlen(self->address) + 1);
    size_t newsize = (size + 3) & (size_t)~0x03;
    // extend the chunk to a multitude of 4
    if (newsize-size)
        size = zchunk_extend(newchunk, "\x00\x00\x00\x00", newsize-size);
    // insert the new format
    size = zchunk_extend(newchunk, ",", sizeof (char));
    size_t format_start = size; // save the start position of the format string
    size = zchunk_extend(newchunk, new_format, strlen(new_format) + 1 );
    // extend the chunk to a multitude of 4
    newsize = (size + 3) & (size_t)~0x03;
    if (newsize-size)
        size = zchunk_extend(newchunk, "\x00\x00\x00\x00", newsize-size);
#ifdef _MSC_VER
    _freea(new_format);
#endif
    // save the pointer to where the data starts
    size_t new_data_begin = size;

    // copy current data from data begin to end if there is any data
    if (self->data_begin)
        size = zchunk_extend(newchunk, zchunk_data(self->chunk)+self->data_begin, zchunk_size(self->chunk)-self->data_begin);

    // now append the new data
    va_list argptr;
    va_start(argptr, format);
    s_append_data(newchunk, format, argptr);
    va_end(argptr);

    zchunk_destroy(&self->chunk);
    self->chunk = newchunk;
    self->data_begin = new_data_begin;
    // the chunk data starts with the address string
    self->address =  (char *)zchunk_data(self->chunk);
    // set the format string to the pointer in the chunk of data
    self->format = (char *)zchunk_data(self->chunk)+format_start;
    // release the indexes
    if (self->data_indexes)
    {
        free(self->data_indexes);
        self->data_indexes = NULL;
    }

    return 0;
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
    if (self)
    {
        byte* data = (byte *)zmalloc( zchunk_size(self->chunk) );
        memcpy(data, zchunk_data(self->chunk), zchunk_size( self->chunk ));
        return zosc_frommem( (char *)data, zchunk_size( self->chunk ));
    }
    else
        return NULL;
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

//  Dump OSC message to stdout, for debugging and tracing.
char *
zosc_dump (zosc_t *self)
{
    assert(self);
    assert(self->format);
    char *out = (char *)malloc(1024);

    size_t needle = self->data_begin;
    int i=0;
    sprintf(out, "%s %s", self->address, self->format);
    while(self->format[i])
    {
        switch (self->format[i])
        {
            case 'i' :
            {
                void *data = zchunk_data( self->chunk ) + needle;
                int32_t int_v = (int32_t)ntohl(*(uint32_t *)data);
                sprintf(out, "%s %i", out, int_v);
                needle += sizeof (uint32_t);
                break;
            }
            case 'h':
            {
                uint64_t int_v = ntohll(*(uint64_t*)(zchunk_data( self->chunk ) + (needle * sizeof (char) )));
                sprintf(out, "%s %ld", out, (long)int_v);
                needle += sizeof (uint64_t);
                break;
            }
            case 'f':
            {
                uint32_t flt_v = ntohl(*(uint32_t*)(zchunk_data( self->chunk ) + needle));
                float *v = (float *)&flt_v;  // hackish
                sprintf(out, "%s %.6f", out, (double)*v);
                needle += sizeof (float);
                break;
            }
            case 'd':
            {
                uint64_t dbl_v = ntohll(*(uint64_t*)(zchunk_data( self->chunk ) + needle));
                double *v = (double *)&dbl_v;
                sprintf(out, "%s %f", out, *v);
                needle += sizeof (double);
                break;
            }
            case 's':
            {
                //  not sure if the double pointer is the way to go
                char *str = (char*)(zchunk_data( self->chunk ) + needle);
                sprintf(out, "%s %s", out, str);
                size_t l = strlen((char*)(zchunk_data( self->chunk ) + needle));
                needle += l + 1;
                needle = (needle + 3) & (size_t)~0x03;
                break;
            }
            case 'S': // never used???
                break;
            case 'c':
            {
                char chr = (*(char*)(zchunk_data( self->chunk ) +
                                        needle + 3));
                sprintf(out, "%s %c", out, chr);
                needle += sizeof (int);  // advance multitude of 4!
                break;
            }
            case 'm':
            {
                uint32_t midi = ntohl(*(uint32_t *)(zchunk_data( self->chunk ) + needle));
                sprintf(out, "%s 0x%08x", out, midi);
                needle += sizeof (uint32_t);
                break;
            }
            case 'T':
            {
                // value only determined based on the format!
                //sprintf(out, "%s True", out);
                break;
            }
            case 'F':
            {
                //sprintf(out, "%s False", out);
                break;
            }
            case 'N': // never used???
            case 'I': // never used???
            {
                needle++;
                break;
            }
        default:
            zsys_error("format identifier '%c' not matched", self->format[i]);


        }
        i++;
    }
    return out;
}

void
zosc_print (zosc_t *self)
{
    char *s = zosc_dump(self);
    zsys_debug("%s", s);
    zstr_free(&s);
}


bool
zosc_is (void *self)
{
    zosc_t *test = (zosc_t *)self;
    // for now just test if there's an address and format string
    return strlen( test->address ) && strlen( test->format );
}

static void
s_require_indexes(zosc_t *self)
{
    assert(self);
    assert(self->data_begin);
    if (self->data_indexes) return;
    self->data_indexes = (size_t *)zmalloc( sizeof(size_t) * (strlen(self->format)+1) );
    self->data_indexes[0] = self->data_begin;
    // generate the indexes
    size_t needle = self->data_begin;
    int stridx = 0;
    while ( self->format[stridx] )
    {
        switch (self->format[stridx])
        {
            case 'i' :
            {
                needle += sizeof (uint32_t);
                break;
            }
            case 'h':
            {
                needle += sizeof (uint64_t);
                break;
            }
            case 'f':
            {
                needle += sizeof (float);
                break;
            }
            case 'd':
            {
                needle += sizeof (double);
                break;
            }
            case 's':
            {
                size_t l = strlen((char*)(zchunk_data( self->chunk ) + needle));
                needle += l + 1;
                needle = (needle + 3) & (size_t)~0x03;
                break;
            }
            case 'S': // never used???
                break;
            case 'c':
            {
                needle += sizeof (int);  // advance multitude of 4!
                break;
            }
            case 'm':
            {
                needle += sizeof (uint32_t);
                break;
            }
            case 'T':
            {
                break;
            }
            case 'F':
            {
                break;
            }
            case 'N': // never used???
            case 'I': // never used???
            {
                needle++;
                break;
            }
            default:
                zsys_error("format identifier '%c' not matched", *self->format+stridx);
        }
        // save current element bytearray index
        stridx++;
        self->data_indexes[stridx] = needle;
    }
}

// Return a pointer to the item at the head of the OSC data.
// Sets the given char argument to the type tag of the data.
// If the message is empty, returns NULL.
const void *
zosc_first (zosc_t *self, char *type)
{
    assert(self);
    if (self->data_begin == 0 )
        return NULL;    // empty message

    s_require_indexes(self);
    self->cursor_index = 0;
    *type = self->format[0];
    return zchunk_data(self->chunk) + self->data_indexes[0];
}

// Return the next item of the OSC message. If the list is empty, returns
// NULL. To move to the start of the OSC message call zosc_first ().
const void *
zosc_next (zosc_t *self, char *type)
{
    assert(self);
    if (self->data_begin == 0 )
        return NULL;    // empty message

    s_require_indexes(self);
    self->cursor_index++;
    if ( self->cursor_index > (int)strlen(self->format)-1 )
        return NULL;    //  no more elements

    *type = self->format[self->cursor_index];
    return zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
}

// Return a pointer to the item at the tail of the OSC message.
// Sets the given char argument to the type tag of the data. If
// the message is empty, returns NULL.
const void *
zosc_last (zosc_t *self, char *type)
{
    assert(self);
    if (self->data_begin == 0 )
        return NULL;    // empty message
    s_require_indexes(self);

    self->cursor_index = (int)strlen(self->format)-1;
    *type = self->format[self->cursor_index];
    return zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
}

int
zosc_pop_int32(zosc_t *self, int *val)
{
    assert(self);
    if (self->format[self->cursor_index] != 'i')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    *val = (int32_t)ntohl(*((uint32_t *)data));
    return 0;
}

int
zosc_pop_int64(zosc_t *self, int64_t *val)
{
    assert(self);
    if (self->format[self->cursor_index] != 'h')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    *val = (int64_t)ntohll(*((uint64_t *)data));
    return 0;
}

int
zosc_pop_float(zosc_t *self, float *val)
{
    assert(self);
    if (self->format[self->cursor_index] != 'f')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    uint32_t flt_v = ntohl(*((uint32_t *)data));
    *val = *((float *)&flt_v);  // tricky but it works, a normal cast gets it wrong
    return 0;
}

int
zosc_pop_double(zosc_t *self, double *val)
{
    assert(self);
    if (self->format[self->cursor_index] != 'd')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    uint64_t dbl_v = ntohll(*((uint64_t *)data));
    *val = *((double *)&dbl_v);  // tricky but it works, a normal cast gets it wrong
    return 0;
}

int
zosc_pop_string(zosc_t *self, char **val)
{
    assert(self);
    if (self->format[self->cursor_index] != 's')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    *val = strdup((char *)data);
    return 0;
}

int
zosc_pop_char(zosc_t *self, char *val)
{
    assert(self);
    if (self->format[self->cursor_index] != 'c')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    *val = *((char*)data+3);  // network order with alignment!
    return 0;
}

int
zosc_pop_bool(zosc_t *self, bool *val)
{
    assert(self);
    if ( ! (self->format[self->cursor_index] == 'F' || self->format[self->cursor_index] == 'T' ) )
        return -1; //  wrong type
    *val = self->format[self->cursor_index] == 'F' ? false : true;
    return 0;
}

int
zosc_pop_midi(zosc_t *self, uint32_t *val)
{
    assert(self);
    if (self->format[self->cursor_index] != 'm')
        return -1; //  wrong type

    void *data = zchunk_data(self->chunk) + self->data_indexes[self->cursor_index];
    *val = ntohl(*((uint32_t *)data));
    return 0;
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
    zosc_t *cd = zosc_new("/createdestroy");
    assert(cd);
    zosc_destroy(&cd);
    assert(cd == NULL);

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
    assert(streq ( zosc_format(testmsg), "ihTfds" ) );
    // test duplicate
    zosc_t *testmsgdup = zosc_dup(NULL);
    assert( testmsgdup == NULL );
    testmsgdup = zosc_dup(testmsg);
    assert( testmsgdup );
    assert(streq ( zosc_address(testmsgdup), "/test"));
    assert(streq ( zosc_format(testmsgdup), "ihTfds" ) );
    zosc_destroy(&testmsgdup);
    // check value
    int itest = -1;
    int64_t ltest = -1;
    bool btest = false;
    float ftest = -1.f;
    double dtest = -1.;
    char *stest;
    const char *format = zosc_format(testmsg);
    zosc_retr(testmsg, format, &itest, &ltest, &btest, &ftest, &dtest, &stest);
    assert( itest == 2 );
    assert( ltest == 1844674407370 );
    assert( btest );
    assert( fabsf(ftest - 3.14f) < FLT_EPSILON ); // float equal
    assert( fabs(dtest - 3.1415926535897932) < DBL_EPSILON );
    assert( streq(stest, "hello") );
    zstr_free(&stest);
    zosc_destroy(&testmsg);

    // test contructing from string
    zosc_t* strm = zosc_fromstring("/stringmsg ihfdscF 32 64 1.100000 3.140000 hello q");
    assert(strm);
    assert(streq(zosc_address(strm), "/stringmsg"));
    assert(streq(zosc_format(strm), "ihfdscF"));
    char *sd = zosc_dump(strm);
    assert(streq(sd, "/stringmsg ihfdscF 32 64 1.100000 3.140000 hello q"));
    zstr_free(&sd);
    {
        int32_t test = 0;
        int64_t test64 = 0;
        float testf = 0.0f;
        double testd = 0.0;
        char * tests = "robots";
        char c = 'a';
        bool b = true;
        zosc_retr(strm, zosc_format(strm), &test, &test64, &testf, &testd, &tests, &c, &b);
        assert( test == 32 );
        assert( test64 == 64 );
        assert( fabsf(testf-1.1f) < FLT_EPSILON );
        assert( fabs(testd-3.14) < DBL_EPSILON );
        assert( streq(tests, "hello") );
        zstr_free( &tests );
        assert( c == 'q' );
    }
    // test print
    zosc_print(strm);
    zosc_destroy(&strm);

    // test constructing messages
    int64_t prez = 3;
    zosc_t* conm = zosc_create("/construct", "iihfdscF", -2,2, prez, 3.14,6.283185307179586, "greetings", 'q');
    assert(conm);
    assert(streq(zosc_address(conm), "/construct"));
    assert(streq(zosc_format(conm), "iihfdscF"));
    if (verbose) zosc_print(conm);
    int x,y;
    int64_t z;
    float zz;
    double zzz;
    char *ss = "aliens";
    char q = 'z';
    bool Ftest = true;
    zosc_retr(conm, "iihfdscF", &x, &y, &z, &zz, &zzz, &ss, &q, &Ftest);
    assert( x==-2 );
    assert( y==2 );
    assert( z==3 );
    assert( fabsf(zz-3.14f) < FLT_EPSILON );
    assert( fabs(zzz-6.283185307179586) < DBL_EPSILON );
    assert( streq( ss, "greetings") );
    assert( q == 'q');
    assert( !Ftest );
    zstr_free(&ss);

    // test iterating
    char type = '0';
    const void *data = zosc_first(conm, &type);
    while ( data )
    {
        if (verbose)
            zsys_info("type tag is %c", type);

        switch (type)
        {
        case('i'):
        {
            int32_t test = 9;
            int rc = zosc_pop_int32(conm, &test);
            assert(rc == 0);
            assert( test == -2 || test == 2);
            break;
        }
        case('h'):
        {
            int32_t fail = 9;
            int rc = zosc_pop_int32(conm, &fail); // this will fail
            assert(rc == -1);
            int64_t test = 9;
            rc = zosc_pop_int64(conm, &test);
            assert(rc == 0);
            assert( test == 3 );
            break;
        }
        case('f'):
        {
            float flt_v = 0.f;
            int rc = zosc_pop_float(conm, &flt_v);
            assert(rc == 0);
            assert( fabsf(flt_v-3.14f) < FLT_EPSILON );
            break;
        }
        case 'd':
        {
            double dbl_v = 0.;
            int rc = zosc_pop_double(conm, &dbl_v);
            assert(rc == 0);
            assert( fabs(dbl_v-6.283185307179586) < DBL_EPSILON );
            break;
        }
        case 's':
        {
            char *str;
            int rc = zosc_pop_string(conm, &str);
            assert(rc == 0);
            assert(streq(str, "greetings"));
            zstr_free(&str);
            break;
        }
        case 'c':
        {
            char chr;
            int rc = zosc_pop_char(conm, &chr);
            assert(rc == 0);
            assert(chr == 'q');
            break;
        }
        case 'F':
        {
            bool bl;
            int rc = zosc_pop_bool(conm, &bl);
            assert(rc == 0);
            assert( !bl );
            break;
        }
        default:
            assert(0);
        }
        data = zosc_next(conm, &type);
    }

    // test append
    zosc_t *cappd = zosc_new("/createappenddestroy");
    zosc_append(cappd, "s", "addedstring");
    zosc_destroy(&cappd);

    int64_t preal = -80000;
    zosc_append(conm, "ih", -200, preal);
    int ax,ay;
    int64_t az;
    float azz;
    double azzz;
    char *ass = "aliens";
    char aq = 'z';
    bool aFtest = true;
    int ai = 0;
    int64_t al = 0;
    zosc_retr(conm, "iihfdscFih", &ax, &ay, &az, &azz, &azzz, &ass, &aq, &aFtest, &ai, &al);
    assert( ax==-2 );
    assert( ay==2 );
    assert( az==3 );
    assert( fabsf(azz-3.14f) < FLT_EPSILON );
    assert( fabs(azzz-6.283185307179586) < DBL_EPSILON );
    assert( streq( ass, "greetings") );
    assert( aq == 'q');
    assert( !aFtest );
    assert( ai==-200 );
    assert( al==-80000 );
    zstr_free(&ass);


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
