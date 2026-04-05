#ifndef __ONCSSUB_IDASTROV1_H__
#define __ONCSSUB_IDASTROV1_H__

#include "oncsSubevent.h"

#include <vector>
#include <set>


#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idastrov1 : public  oncsSubevent_w1 {
#else
class  oncsSub_idastrov1 : public  oncsSubevent_w1 {
#endif

public:
  oncsSub_idastrov1( subevtdata_ptr);
  ~oncsSub_idastrov1();

  int    iValue(const int dummy); // returns NR_HITS
  int    iValue(const int unit, const int field);
  int    iValue(const int unit, const char * what);
  long long  lValue(const int unit);    // returns fpga_timestamp
  
  void  dump ( OSTREAM& os = COUT);  

protected:
  int decode ();
  int decode_line (unsigned char *);
  unsigned int u4swap(const unsigned int v);

  int is_decoded;
  
  int _broken;


  
struct AstroStruct
{
  unsigned int layer;
  unsigned int id;
  unsigned int payload;
  unsigned int location;
  unsigned int iscol;
  unsigned int timestamp;
  unsigned int tot;
  unsigned long long fpga_ts;
};


struct CompareByTimestamp
{
  bool operator()(const AstroStruct& a, const AstroStruct& b) const
  {
    if ( a.layer < b.layer)
      {
	return true;
      }
    else if ( a.fpga_ts == b.fpga_ts)
      {
        if ( a.fpga_ts < b.fpga_ts)
	  {
	    return true;  //easy case
	  }
	else if ( a.fpga_ts == b.fpga_ts) // timestamps are equal, then go column, row, like we read a book
	  {
	    if ( a.iscol > b.iscol)
	      {
		return true;
	      }
	  }
      }
    return false;
  }
};

  std::multiset<AstroStruct, CompareByTimestamp> _TheSet;
  

};


#endif /* __ONCSSUB_IDASTROV1_H__ */
