#ifndef __ONCSSUB_IDASTROV1_H__
#define __ONCSSUB_IDASTROV1_H__

#include "oncsSubevent.h"

#include<vector>


#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idastrov1 : public  oncsSubevent_w1 {
#else
class  oncsSub_idastrov1 : public  oncsSubevent_w1 {
#endif

public:
  oncsSub_idastrov1( subevtdata_ptr);
  ~oncsSub_idastrov1();

  int    iValue(const int unit, const int field);
  int    iValue(const int unit, const char * what);

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
  unsigned int fpga_ts;
};

  std::vector<AstroStruct *> _unitlist;


};


#endif /* __ONCSSUB_IDASTROV1_H__ */
