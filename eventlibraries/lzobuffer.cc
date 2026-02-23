#include "lzobuffer.h"
#include "lzo/lzoutil.h"
#include "oncsBuffer.h"

int lzobuffer::lzo_initialized = 0;


// the constructor first ----------------
lzobuffer::lzobuffer (PHDWORD *array , const int length )

{

  if ( !  lzo_initialized )
    {
      if (lzo_init() != LZO_E_OK)
	{
	  COUT << "Could not initialize LZO" << std::endl;
	  _broken = 1;
	}
      
      lzo_initialized = 1;
    }
      
  
  is_good =1;
  bufferarray=0;
  theBuffer=0;

  lzo_uint bytes; 
  lzo_uint outputlength_in_bytes;
  if (array[1] == LZO1XBUFFERMARKER || 
      array[1] == LZO1CBUFFERMARKER ||
      array[1] == LZO2ABUFFERMARKER )
    {
      bytes = array[0]-4*BUFFERHEADERLENGTH;
      outputlength_in_bytes = array[3];
    }
  else if ( u4swap(array[1]) == LZO1XBUFFERMARKER 
	    || u4swap(array[1]) == LZO1CBUFFERMARKER
	    || u4swap(array[1]) == LZO2ABUFFERMARKER)
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

  
  int outputlength =  (outputlength_in_bytes+3)/4;
  bufferarray = new PHDWORD[outputlength];
  lzo_uint olen;

  //  std::cout << __FILE__ << "  " << __LINE__ << " safe!!! array length before is " << array[-1] << std::endl;
  olen = outputlength_in_bytes;

  if (array[1] == LZO1XBUFFERMARKER || u4swap(array[1]) == LZO1XBUFFERMARKER )
    {
      lzo1x_decompress_safe ( (lzo_byte *)  &array[4], bytes,
			      (lzo_byte *)  bufferarray, &olen, NULL );
    }
  else if (array[1] == LZO1CBUFFERMARKER || u4swap(array[1]) == LZO1CBUFFERMARKER )
    {

      lzo1c_decompress_safe ( (lzo_byte *)  &array[4], bytes,
			      (lzo_byte *)  bufferarray, &olen, NULL );

    }
  else 
    {

      lzo2a_decompress_safe ( (lzo_byte *)  &array[4], bytes,
			      (lzo_byte *)  bufferarray, &olen, NULL );

    }

  //  std::cout << __FILE__ << "  " << __LINE__ << " array length after is " << array[-1] << std::endl;

    if (  olen != outputlength_in_bytes)
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
Event * lzobuffer::getEvent()
{
  if ( theBuffer) return theBuffer->getEvent();
  return 0;
}

// ---------------------------------------------------------
int lzobuffer::getBufferSequence() const
{
  if ( !theBuffer) return 0;
  return theBuffer->getBufferSequence();
}

// ---------------------------------------------------------

lzobuffer::~lzobuffer()
{
  if ( theBuffer) delete theBuffer;
  if ( bufferarray) delete [] bufferarray;
}


