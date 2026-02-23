#include "oncsSub_idastrov1.h"

#include <string.h>
#include "buffer.h"

using namespace std;


oncsSub_idastrov1::oncsSub_idastrov1(subevtdata_ptr data)
  :oncsSubevent_w1 (data)
{

  is_decoded = 0;
  _broken = 0;

}


int oncsSub_idastrov1::decode_line( unsigned char *cp)
{
  struct AstroStruct *as = new AstroStruct;
  
  as->layer = cp[1];
  
  as->id = cp[2] >> 3;
  as->payload = cp[2] & 0x7;

  as->location = cp[3] & 0x3f;
  as->iscol = cp[3] >>7 &1;
  as->timestamp = cp[4];
  as->tot = cp[5] & 0xf;
  as->tot <<=8;
  as->tot |= cp[6];
  
  unsigned int x0,x1,x2,x3;
  x0 = cp[10];
  x1 = cp[9];
  x2 = cp[8];
  x3 = cp[7];

  as->fpga_ts = (x0<<24) | (x1<< 16) | (x2 <<8) | x3;

  _unitlist.push_back(as);
  
  return 0;
}


int oncsSub_idastrov1::decode()
{
  //coutfl << " calling decode" << endl;


  if (is_decoded) return 0;
  is_decoded = 1;

  
  unsigned char *buffer = ( unsigned char *)  &SubeventHdr->data;

  // for ( int x = 0; x < 11; x++)
  //   {
  //     cout << hex << setw(2) << (unsigned short) buffer[x] <<" ";
  //   }
  // cout << endl;
  
  
  int current_index  = 0;
  
  //coutfl << " len= " << getDataLength() << " padding: " << getPadding() << endl;

  while ( current_index < getDataLength()*4 -getPadding()  )  
    {
      int l = buffer[current_index]+1;

      if ( l + current_index <= getDataLength()*4-getPadding() )
	{
	  if ( l == 11)
	    {
	      
	      decode_line (&buffer[current_index]);
	      // for ( int x = 0; x < l; x++)
	      // 	{
	      // 	  cout << hex << setw(2) << (unsigned short)buffer[current_index+x] <<" ";
	      // 	}
	    }
	}	
      current_index +=l;
    }
  return 0;  
}


int oncsSub_idastrov1::iValue(const int unit, const int field)
{

  decode();
  
  // 0 - length
  // 1 - layer
  // 2 - id
  // 3 - payload
  // 4 - location
  // 5 - iscol
  // 6 - timestamp
  // 7 - tot
  // 8 - fpga_ts
  
  unsigned int un = unit; // the size() is unsigned

  // here we gop through the individual units

  if ( un >= _unitlist.size()) return 0;
  
  switch (field)
    { 
    case 0:
      break;

    case 1:
      return _unitlist[un]->layer;
      break;

    case 2:
      return _unitlist[un]->id;
      break;
     
    case 3:
      return _unitlist[un]->payload;
      break;
     
    case 4:
      return _unitlist[un]->location;
      break;
     
    case 5:
      return _unitlist[un]->iscol;
      break;
     
    case 6:
      return _unitlist[un]->timestamp;
      break;
     
    case 7:
      return _unitlist[un]->tot;
      break;
     
    case 8:
      return _unitlist[un]->fpga_ts;
      break;
    }
  
  return 0;
}
  
int oncsSub_idastrov1::iValue(const int unit, const char *what)
{
  decode();

  if ( strcmp(what,"NR_UNITS") == 0 )
    {
      return _unitlist.size(); 
    }
  
  if ( strcmp(what,"LAYER") == 0 )
    {
      return iValue(unit,1);
    }

  else if ( strcmp(what,"ID") == 0 )
    {
      return iValue(unit,2);
    }

  else if ( strcmp(what,"PAYLOAD") == 0 )
    {
      return iValue(unit,3);
    }
  else if ( strcmp(what,"LOCATION") == 0 )
    {
      return iValue(unit,4);
    }
  else if ( strcmp(what,"ISCOL") == 0 )
    {
      return iValue(unit,5);
    }
  else if ( strcmp(what,"TIMESTAMP") == 0 )
    {
      return iValue(unit,6);
    }
  else if ( strcmp(what,"TOT") == 0 )
    {
      return iValue(unit,7);
    }
  else if ( strcmp(what,"FPGA_TS") == 0 )
    {
      return iValue(unit,8);
    }


  return 0;
}
  
  

void oncsSub_idastrov1::dump(std::ostream &os)
{
  identify(os);
  if ( _broken) 
    {
      os << " ** corrupt packet **" << endl;
      return;
    }

  decode();

  
  os << "Nr of units: " << iValue(0, "NR_UNITS") << endl;



  os << "    # |   layer    id payload location iscol   ts    ToT fpga_ts" << endl;

  for (int s = 0; s < iValue(0,"NR_UNITS"); s++)
    {
      os << setw(5) << s << " | ";
      for (int i = 1; i <=8; i++) 
	{
	  os << setw(6) << iValue(s, i) << " ";
	}
      os << endl;
    }
  
  
}



oncsSub_idastrov1::~oncsSub_idastrov1()
{

  for (auto itr =  _unitlist.begin(); itr != _unitlist.end(); ++itr)
    {
      delete *itr;
    }
}


