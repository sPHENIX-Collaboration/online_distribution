#ifndef __PACKET_IDH2GCROC3_H__
#define __PACKET_IDH2GCROC3_H__

#include "Packet_A.h"

#include<vector>


#ifndef __CINT__
class WINDOWSEXPORT Packet_idh2gcroc3 : public  Packet_w4 {
#else
class  Packet_idh2gcroc3 : public  Packet_w2 {
#endif

public:
  Packet_idh2gcroc3( subevtdata_ptr);
  ~Packet_idh2gcroc3();

  int    iValue(const int sample, const int ch);
  int    iValue(const int sample, const int ch, const char *what);
  int    iValue(const int ,const char * what);
  int    iValue(const int event, const int sample, const int ch);
  int    iValue(const int wf, const int sample, const int ch, const char *what);
  long long   lValue(const int wf, const int sample, const int ch);
  
  void  dump ( OSTREAM& os = COUT);  

protected:
  int decode ();
  int decode_line (unsigned int *);
  int parse_timeline ();
  unsigned int u4swap(const unsigned int v);

  int is_decoded;
  
  int _broken;


  unsigned int old_timestamp;

  // this structure is the main datatset after we are done.
  // it holds all ADC values for a given timestamp.
  // once we see the timestamp move on, we declare that sample "closed"
  
  struct sample {
    unsigned int ADC[144];
    unsigned int tot[144];
    unsigned int toa[144];
    unsigned int timestamp;
    unsigned int cmn;
    unsigned int calib;
    unsigned int crc32;
  } ;

  sample *_sample;
  
  std::vector<sample *> waveform;

  int _nr_events;

  int _nr_samples;

  // this structure is meant to parse the timeline into event boundaries.
  struct event_bounds
  {
    unsigned int first;
    unsigned int length;
  };
  
  std::vector<event_bounds *> _eventlist;
    
};


#endif /* __PACKET_IDH2GCROC3_H__ */
