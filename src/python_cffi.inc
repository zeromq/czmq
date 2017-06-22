czmq_cdefs.append ('''
typedef int time_t;
typedef int off_t;

typedef unsigned char   byte;           //  Single unsigned byte = 8 bits
typedef unsigned short  dbyte;          //  Double byte = 16 bits
typedef unsigned int    qbyte;          //  Quad byte = 32 bits
typedef int SOCKET;
typedef struct sockaddr_in inaddr_t;

//  -- destroy an item
typedef void (czmq_destructor) (void **item);
//  -- duplicate an item
typedef void *(czmq_duplicator) (const void *item);
//  - compare two items, for sorting
typedef int (czmq_comparator) (const void *item1, const void *item2);
''')
