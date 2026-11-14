#ifndef __BZ2BUFFER_H
#define __BZ2BUFFER_H

#include "oncsBuffer.h"

#ifndef __CINT__
class WINDOWSEXPORT bz2buffer : public buffer {
#else
class  bz2buffer : public buffer {
#endif

public:

  //** Constructors

  bz2buffer( PHDWORD *array, const int length);
  ~bz2buffer();

  Event * getEvent();

  int getBufferSequence() const;
  int * getEventData() { return ( theBuffer ? theBuffer->getEventData() : 0); };
  int isGood() const { return is_good; };

protected:

  PHDWORD  *bufferarray;
  buffer *theBuffer;
  int is_good;

  int _broken;

};

#endif
