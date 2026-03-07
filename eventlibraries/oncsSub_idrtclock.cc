#include "oncsSub_idrtclock.h"
#include <cstring>

using namespace std;

oncsSub_idrtclock::oncsSub_idrtclock(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{
  is_decoded = 0;
  _broken = 0;
}
  
int oncsSub_idrtclock::decode ()
{
  if ( is_decoded) return 0;
  is_decoded = 1;
  

  int dlength = ( getLength()-4) - getPadding();
  if (dlength < 9) _broken = 1;
  
  //  int i;
  unsigned int *d = (unsigned int *) &SubeventHdr->data;
  for ( int i = 0; i < 9; i++)
    {
      array[i] = d[i];
    }

  // since the upper 32 bits of the 64 bit "ns" in the structure are always 0,
  // we only need to look at the lower 32 bits
  tv_sec  = d[0];
  tv_nsec = d[1]; 
  total = tv_sec * 1000000000 + tv_nsec;

  tv_sec_prev  = d[3];
  tv_nsec_prev = d[4];
  total_prev = tv_sec_prev * 1000000000 + tv_nsec_prev;
  
  tv_sec_monotonic  = d[6];
  tv_nsec_monotonic = d[7];
  total_monotonic = tv_sec_monotonic * 1000000000 + tv_nsec_monotonic;

  
  diff_totalnsec = total - total_prev; ;

  return 0;
}

int oncsSub_idrtclock::iValue(const int n)
{
  decode();
  if (_broken) return 0;

  if ( n < 0 || n >=9) return 0;
  return array[n];
}

  int oncsSub_idrtclock::iValue(const int n, const char * what)
{
  decode();
  if (_broken) return 0;

  if ( n >= 3) return 0;
  
  if (strcmp (what, "SECONDS") == 0)
    {
      if ( n == 0) return tv_sec;
      else if ( n == 1) return tv_sec_prev;
      else if ( n == 2) return tv_sec_monotonic;
    }
  
  else if (strcmp (what, "NSECONDS") == 0)
    {
      if ( n == 0) return tv_nsec;
      else if ( n == 1) return tv_nsec_prev;
      else if ( n == 2) return tv_nsec_monotonic;
    }
  return 0;
}

long long oncsSub_idrtclock::lValue(const int n, const char * what)
{
  decode();
  if (_broken) return 0;
  
  if (strcmp (what, "TIMEDIFF") == 0)
    {
      return diff_totalnsec;
    }
  else if (strcmp (what, "TIMEDIFF_S") == 0)
    {
      return diff_sec;
    }
  else if (strcmp (what, "TIMEDIFF_NS") == 0)
    {
      return diff_nsec;
    }
  else if (strcmp (what, "TOTAL") == 0)
    {
      if ( n == 0) return total;
      else if ( n == 1) return total_prev;
      else if ( n == 2) return total_monotonic;
    }

  return 0;
}

void  oncsSub_idrtclock::dump ( OSTREAM& os)
{
  identify();
  if (_broken)
    {
      os << " broken " << endl;
      return;
    }
  
  os << " evt  time       s: " << setw(8) << iValue (0, "SECONDS")
     << " ns: " << setw(9) << iValue(0,"NSECONDS") 
     << " total " << lValue (0, "TOTAL")<< endl;

  os << " monotonic time  s: " << setw(8) << iValue (2, "SECONDS")
     << " ns: " << setw(9) << iValue(2,"NSECONDS")
     << " total " << lValue (2, "TOTAL")<< endl;

  os << " diff to prev. evt:" <<  setw(9) << lValue (0, "TIMEDIFF") << " ns" << endl;
  os << endl;
}


  
