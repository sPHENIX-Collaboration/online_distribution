#ifndef __ONCSSUB_IDRTCLOCK_H__
#define __ONCSSUB_IDRTCLOCK_H__

#include "oncsSubevent.h"

// this packet holds 3 "struct timespec" values (3x32 bits each)
// The first is the event time derived from CLOCK_MONOTONIC_RAW
// the second holds the same value from the previous event
// the third is the value derived from CLOCK_MONOTONIC (for good mesure)

// The first two allows us to calculate the time difference in ns to the previous event.


#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idrtclock : public  oncsSubevent_w4 {
#else
class  oncsSub_idrtclock : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_idrtclock( subevtdata_ptr);

  int iValue(const int n);
  int iValue(const int n, const char * what);
  long long  lValue(const int hit,const char * what);

  
  void  dump ( OSTREAM& os = COUT);  

  

protected:
  int decode ();

  int is_decoded;
  time_t tv_sec;
  long long tv_nsec;
  long long total;
  
  time_t tv_sec_prev;
  long long tv_nsec_prev;
  long long total_prev;

  time_t tv_sec_monotonic;
  long long tv_nsec_monotonic;
  long long total_monotonic;

  int diff_sec;
  unsigned int diff_nsec;
  long long diff_totalnsec;
  int _broken;
  
  unsigned int array[9];
};


#endif /* __ONCSSUB_ID4EVT_H__ */
