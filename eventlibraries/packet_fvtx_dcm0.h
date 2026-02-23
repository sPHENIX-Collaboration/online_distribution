#ifndef __PACKET_FVTX_DCM0_H__
#define __PACKET_FVTX_DCM0_H__


#include <packet_w124.h>

/**
   This is the packet which deals with data in PXL\_DCM0 format.
   It inherits from Packet\_w4 because the data are 32bit entities.
*/
#ifndef __CINT__
class WINDOWSEXPORT Packet_fvtx_dcm0 : public Packet_w4 {
#else
class  Packet_fvtx_dcm0 : public Packet_w4 {
#endif

public:
  Packet_fvtx_dcm0();
  Packet_fvtx_dcm0(PACKET_ptr);
  ~Packet_fvtx_dcm0();

/** with the "what" parameter you can decide which aspect of
the data is made available. This class is one of those which have
several different "kinds" of data; we use this to bring up 
the misc. items in the FEM headers and trailers.


In addition, there is 
\begin{verbatim}
  packet->iValue(0,"EVTNR")
  packet->iValue(0,"DETID")
  packet->iValue(0,"MODADDR")
  packet->iValue(0,"FLAG")
  packet->iValue(0,"BCLCK")
  packet->iValue(0,"CHECKPARITY")

  packet->iValue(0,"HITS")
  packet->iValue(i,"FEMID")   ==  packet->iValue(i,0)
  packet->iValue(i,"CHIPID")  ==  packet->iValue(i,1)
  packet->iValue(i,"CHANNEL") ==  packet->iValue(i,2)
  packet->iValue(i,"ADC")     ==  packet->iValue(i,3)


\end{verbatim}
*/


  virtual int    iValue(const int channel,const char *what);
  virtual int    iValue(const int chip,const int what);
  virtual void  dump ( OSTREAM& ) ;


protected:
  virtual int *decode (int *);
  virtual int *decode_misc (int *);
  int calculate_parity();

  int calculated_parity; 
  int parity_comparison; 
  int number_of_hits;

  int error_code;


};

#endif /* __PACKET_FVTX_DCM0_H__ */



