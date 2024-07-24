#ifndef __BUFFER_CONSTANTS_H
#define __BUFFER_CONSTANTS_H

/* the header length values */ 
#define BUFFERHEADERLENGTH 4U
#define EOBLENGTH 4U
#define BUFFERSIZE (1024U*1024U)

#define BUFFERMARKER          0xffffffc0U
#define ONCSBUFFERMARKER      0xffffc0c0U
#define GZBUFFERMARKER        0xfffffafeU
#define LZO1XBUFFERMARKER     0xffffbbfeU
#define LZO1CBUFFERMARKER     0xffffbcfeU
#define ONCSLZO1XBUFFERMARKER 0xffffbbc0U

#define BUFFERBLOCKSIZE 8192U



#endif
