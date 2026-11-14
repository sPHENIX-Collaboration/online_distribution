#ifndef __LZOBUFFER_H
#define __LZOBUFFER_H

#include "oncsBuffer.h"
#include <lzo/lzo1x.h>
#include <lzo/lzo1c.h>
#include <lzo/lzo2a.h>


#ifndef __CINT__
class WINDOWSEXPORT lzobuffer : public buffer {
#else
class  lzobuffer : public buffer {
#endif

public:

  //** Constructors

  lzobuffer( PHDWORD *array, const int length);
  ~lzobuffer();

  Event * getEvent();

  int getBufferSequence() const;
  int * getEventData() { return ( theBuffer ? theBuffer->getEventData() : 0); };
  int isGood() const { return is_good; };

protected:
  static int lzo_initialized;

  PHDWORD  *bufferarray;
  buffer *theBuffer;
  int is_good;
  lzo_byte *wrkmem;

  int _broken;

};

#endif
