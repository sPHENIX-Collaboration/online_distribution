#include "bz2buffer.h"
#include "oncsBuffer.h"
#include <bzlib.h>


// the constructor first ----------------
bz2buffer::bz2buffer (PHDWORD *array , const int length )

{

  is_good =1;
  bufferarray=0;
  theBuffer=0;

  unsigned int bytes; 
  unsigned int outputlength_in_bytes;
  if (array[1] == BZ2BUFFERMARKER)
    {
      bytes = array[0]-4*BUFFERHEADERLENGTH;
      outputlength_in_bytes = array[3];
    }
  else if ( u4swap(array[1]) == BZ2BUFFERMARKER )
    {
      bytes = i4swap(array[0])-16;
      outputlength_in_bytes = i4swap(array[3]);
    }
  
  else
    {
      COUT << __FILE__ << " " << __LINE__ << " wrong buffer marker = " << std::hex << array[1] << std::dec << std::endl;
      is_good = 0;
      return;
    }
  
  
  unsigned int outputlength =  (outputlength_in_bytes+3)/4;
  bufferarray = new PHDWORD[outputlength];

  unsigned int olen = 4*outputlength;
    
  int status = BZ2_bzBuffToBuffDecompress ( (char *) bufferarray, &olen,
			       ( char *) &array[4], bytes, 0, 0 );

  if ( status || olen != outputlength_in_bytes)
      {
        COUT << __FILE__ << "  " << __LINE__ << " wrong-sized buffer:  " << olen << " should be " <<  outputlength_in_bytes << std::endl;
  	is_good = 0;
	//  	delete [] bufferarray;
	//	bufferarray = 0;
	//	return;
     }
    if ( bufferarray[1]== BUFFERMARKER || buffer::u4swap(bufferarray[1])== BUFFERMARKER )
      {
	theBuffer = new prdfBuffer(bufferarray, outputlength);
      }
    else if ( bufferarray[1]== ONCSBUFFERMARKER || buffer::u4swap(bufferarray[1])== ONCSBUFFERMARKER )
      {
	theBuffer = new oncsBuffer(bufferarray, outputlength);
      }
    else
      {
	theBuffer = 0;
      }
}

// ---------------------------------------------------------
Event * bz2buffer::getEvent()
{
  if ( theBuffer) return theBuffer->getEvent();
  return 0;
}

// ---------------------------------------------------------
int bz2buffer::getBufferSequence() const
{
  if ( !theBuffer) return 0;
  return theBuffer->getBufferSequence();
}

// ---------------------------------------------------------

bz2buffer::~bz2buffer()
{
  if ( theBuffer) delete theBuffer;
  if ( bufferarray) delete [] bufferarray;
}


