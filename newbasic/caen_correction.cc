#include <iostream>
#include <iomanip>
#include <fstream>

#include "caen_correction.h"
#include "packet.h"

using namespace std;

caen_correction::caen_correction ( const char *calibdata) 
{ 

  int nsample[9];

  _broken = 0;

  int index;
  ifstream IN;

  int adccorr[9];
  float timecorr;

  IN.open(calibdata,ios_base::in);

  int chip;
  int i;
  if ( ! IN.is_open())
    {
      _broken = 1;
    }
  else 
    {
      for  ( chip = 0; chip < 4; chip++)
	{
	  for ( i = 0; i < 1024; i++)
	    {
	      IN >> index
		    >> adccorr[0]
		    >> adccorr[1]
		    >> adccorr[2]
		    >> adccorr[3]
		    >> adccorr[4]
		    >> adccorr[5]
		    >> adccorr[6]
		    >> adccorr[7]
		    >> adccorr[8]
		    >> timecorr
		    >> nsample[0]
		    >> nsample[1]
		    >> nsample[2]
		    >> nsample[3]
		    >> nsample[4]
		    >> nsample[5]
		    >> nsample[6]
		    >> nsample[7]
		  >> nsample[8];
	      
	      base [index][chip*9 + 0] = adccorr[0];
	      base [index][chip*9 + 1]= adccorr[1];
	      base [index][chip*9 + 2]= adccorr[2];
	      base [index][chip*9 + 3]= adccorr[3];
	      base [index][chip*9 + 4]= adccorr[4];
	      base [index][chip*9 + 5]= adccorr[5];
	      base [index][chip*9 + 6]= adccorr[6];
	      base [index][chip*9 + 7]= adccorr[7];
	      base [index][chip*9 + 8]= adccorr[8];
	      timevec[index][chip] = timecorr;
	    }
	}
      IN.close();
    }
}

int caen_correction::init (Packet *p)
{
  int chip,c,i,idx;

  for ( chip = 0; chip < 4; chip++)
    {

      int cell = p->iValue(chip,"INDEXCELL");

      //correct time for each chip
      idx = cell;
      for ( i = 0; i < 1024; i++)
	{
	  current_time[i][chip] = timevec[idx][chip];
	  idx++;
	  if (idx >=1024) idx=0;
	}

      // the adc samples
      for ( c = 0; c < 8; c++)  
	{
	  idx = cell;
	  for ( i = 0; i < 1024; i++)
	    {
	      current_wave[i][chip*8+c] = p->iValue(i,chip*8+c) - base[idx][chip*9+c];
	      idx++;
	      if (idx >=1024) idx=0;
	    }
	}

      //      // the TR cells
      // idx = cell;
      //for ( i = 0; i < 1024; i++)
      //	{
      //	  if (chip < 2)	  current_wave[i][chip*8+8] = p->iValue(i,"TR0") - base[idx][chip*9+8];
      //	  else  current_wave[i][chip*9+8] = p->iValue(i,"TR1") - base[idx][chip*9+8];
      //	  idx++;
      //	  if (idx >=1024) idx=0;
      //	}
    }
  return 0;
}
	  
float caen_correction::caen_corrected(const int sample, const int channel) const
{
  if ( sample < 0 || sample >1023 || channel < 0 || channel > 31) return 0;
  return current_wave[sample][channel];
}

float caen_correction::caen_time(const int sample, const int channel) const
{
  if ( sample < 0 || sample >1023 || channel < 0 || channel > 31) return 0;
  return current_time[sample][channel/8];
}
