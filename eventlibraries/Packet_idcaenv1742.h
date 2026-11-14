
#ifndef __PACKET_IDCAENV1742_H__
#define __PACKET_IDCAENV1742_H__


#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idcaenv1742 : public  Packet_w4 {
#else
class  Packet_idcaenv1742 : public  Packet_w4 {
#endif

public:
  Packet_idcaenv1742( subevtdata_ptr);

  int    iValue(const int ch);
  int    iValue(const int sample, const int ch);
  int    iValue(const int,const char *);
  long long  lValue(const int,const char *);
  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);
  int channels;
  int samples;
  int dlength;
  int evnr;
  int freq;
  int group_mask;
  int LVDSPattern;
  int index_cell[4];
  int tr_present[4];
  unsigned long long GroupTriggerTime[4];
  unsigned int EvtTimeTag;

};



#endif /* __PACKET_IDCAENV1742_H__ */
