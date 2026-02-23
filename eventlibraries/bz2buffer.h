#ifndef __BZ2BUFFER_H
#define __BZ2BUFFER_H

#include "prdfBuffer.h"

#ifndef __CINT__
class WINDOWSEXPORT bz2buffer : public prdfBuffer{
#else
class  bz2buffer : public prdfBuffer{
#endif

public:

  //** Constructors

  bz2buffer( PHDWORD *array, const int length);
  ~bz2buffer();

  Event * getEvent();

  int getBufferSequence() const;

protected:

  PHDWORD  *bufferarray;
  buffer *theBuffer;

  int _broken;

};

#endif
