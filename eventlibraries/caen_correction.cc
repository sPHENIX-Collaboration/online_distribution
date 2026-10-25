#include <iostream>
#include <iomanip>
#include <fstream>

#include "caen_correction.h"
#include "packet.h"

using namespace std;

// the time of one turn of the 1024-cell DRS4 ring, in ns, from the
// FREQUENCY of the data packet (0 = 5GS, 1 = 2.5GS, 2 = 1GS, 3 = 750MS)
static double ring_period(const int frequency)
{
  switch (frequency)
    {
    case 0: return 1024 * 0.2;
    case 1: return 1024 * 0.4;
    case 2: return 1024 * 1.0;
    }
  return 1024 * 1000.0/750.0;
}

caen_correction::caen_correction ( const char *calibdata) 
{ 

  int nsample[9];

  _broken = 0;
  _samples = 1024;
  
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
	      // the nsample correction goes by sample number, not by cell
	      for ( int c = 0; c < 9; c++) nsamp[index][chip*9 + c] = nsample[c];
	      //std::cout << __FILE__ << " " << __LINE__ << setw(3) << chip << setw(5) << index << " " << setw(5) << timecorr << endl;
	    }
	}
      IN.close();
    }
}

int caen_correction::init (Packet *p)
{
  int chip,c,i, idx;

  const double period = ring_period(p->iValue(0,"FREQUENCY"));

  for ( chip = 0; chip < 4; chip++)
    {

      _samples = p->iValue(chip,"SAMPLES");
      int cell = p->iValue(chip,"INDEXCELL");
      idx = cell;
      
      // correct time for each chip, as CAEN's ApplyDataCorrection: past
      // cell 1023 the readout wraps around to cell 0, one full turn of the
      // ring later
      double tstart = timevec[cell][chip];
      
      for ( i = 0; i < _samples; i++)
	{
	  if ( i+cell < 1024)
	    {
	      current_time[i][chip] = timevec[i+cell][chip] - tstart;
	    }
	  else
	    {
	      current_time[i][chip] = timevec[i+cell-1024][chip] + period - tstart;
	    }
	}

      
      // the adc samples
      for ( c = 0; c < 8; c++)  
	{
	  idx = cell;
	  for ( i = 0; i < _samples; i++)
	    {
	      current_wave[i][chip*8+c] = p->iValue(i,chip*8+c) - base[idx][chip*9+c] - nsamp[i][chip*9+c];
	      idx++;
	      if (idx >=1024) idx=0;
	    }
	}

      // Trigger cells;
      {
	idx = cell;
	for ( i = 0; i < _samples; i++)
	  {
	    current_wave[i][32+chip] = p->iValue(i,32+chip) - base[idx][chip*9+8] - nsamp[i][chip*9+8];
	    idx++;
	    if (idx >=1024) idx=0;
	  }
      }
    }
  return 0;
}
	  
float caen_correction::caen_corrected(const int sample, const int channel) const
{
  if ( sample < 0 || sample >=_samples || channel < 0 || channel > 35) return 0;
  return current_wave[sample][channel];
}

float caen_correction::caen_time(const int sample, const int channel) const
{
  //if ( sample < 0 || sample >1023 || channel < 0 || channel > 31) return 0;
  //return current_time[sample][channel/8];
  if ( sample < 0 || sample >1023 || channel < 0 || channel > 35) return 0;
  return current_time[sample][channel < 32 ? channel/8 : channel-32];
}
