#ifndef __GZBUFFER_H
#define __GZBUFFER_H

#include "oncsBuffer.h"

#ifndef __CINT__
class WINDOWSEXPORT gzbuffer : public buffer {
#else
class  gzbuffer : public buffer {
#endif

public:

  //** Constructors

  gzbuffer( PHDWORD *array, const int length);
  ~gzbuffer();

  Event * getEvent();
  int getBufferSequence() const;
  int * getEventData() { return ( theBuffer ? theBuffer->getEventData() : 0); };
  int isGood() const { return is_good; };

protected:

  PHDWORD  *bufferarray;
  buffer *theBuffer;
  int is_good;


};

#endif
