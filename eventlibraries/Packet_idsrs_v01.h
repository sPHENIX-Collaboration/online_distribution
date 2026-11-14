#ifndef __PACKET_IDSRS_V01_H__
#define __PACKET_IDSRS_V01_H__

#include <vector>

#include "Packet_A.h"

#ifndef __CINT__
class WINDOWSEXPORT Packet_idsrs_v01 : public  Packet_w4 {
#else
class  Packet_idsrs_v01 : public  Packet_w4 {
#endif

public:
  Packet_idsrs_v01( subevtdata_ptr);
  ~Packet_idsrs_v01();


  int    iValue(const int channel, const char *);
  int    iValue(const int channel, const int hybrid, const char *);
  int    iValue(const int channel, const int tsample, const int hybrid);

  void  dump ( OSTREAM& os = COUT) ;

protected:
  int *decode (int *);


  typedef struct {
    int n;
    int address;
    int error;
    int adc[128];
  } report;

  typedef struct {
    int framecounter;
    int hdmi;
    char desc[5];
    int words;
    std::vector<int> adc;
    std::vector<report *> rowdata;
  } hybriddata;


  std::vector<hybriddata*> hybridlist;
  int nhybrids;

  int analyze ( hybriddata * hd);
  int add_report ( hybriddata * hd, const int start, const int nreport);


};



#endif /* __PACKET_IDUPPETDATA_V104_H__ */
