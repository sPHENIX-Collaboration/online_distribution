#include "oncsSub_idh2gcroc3.h"

#include <string.h>
#include "buffer.h"

using namespace std;

// this is the clock value for "contiguous samples. 
#define CONTIGUOUS_CLOCK  41

oncsSub_idh2gcroc3::oncsSub_idh2gcroc3(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{

  is_decoded = 0;
  _broken = 0;
  old_timestamp = 0xffffffff;
  _sample =0;
  _nr_samples = 0;
  _nr_events = 0;
}


int oncsSub_idh2gcroc3::decode_line( unsigned int d[10])
{

      
  int o,c;
  //coutfl << "new line " << hex << u4swap(d[0]) << endl;;
  
  if ( (u4swap(d[0]) >> 28 ) == 0xa)
    {
      
      
      int ichip = ((u4swap(d[0])>>24) & 0xf );
      //      int ifpga = ((u4swap(d[0])>>16) & 0xff );
      int group = ((u4swap(d[0])>>8) & 0xff ) - 36;
      int line_number = (u4swap(d[0])) & 0xff;

      // cout << " ifpga = " << ifpga
      // 	   << " ichip = " << ichip
      // 	   << " line number " << line_number
      // 	   << " group " << group
      // 	   << " timestamp " << u4swap(d[1])
      // 	   << dec << endl;

      //      if ( ichip == 0 && line_number == 0 && group == 0)
      //	{

      // for (int i = 0; i < 10; i++)
      // 	{
      // 	  cout << hex << std::setfill('0')
      // 	       << setw(2) << ((d[i]     ) & 0xff) << " "
      // 	       << setw(2) << ((d[i] >>8 ) & 0xff) << " "
      // 	       << setw(2) << ((d[i] >>16) & 0xff) << " "
      // 	       << setw(2) << ((d[i] >>24) & 0xff) << " "
      // 	       << dec;
      // 	}
      // cout << std::setfill(' ') << endl;
      //	}
      

      // on the first invocation, old_timestamp is -1 (or 0xfff... as an unsigned)
      // initializing it this way will make the nexty check succeed.
      
      if ( old_timestamp == 0xffffffff )
	{
	  old_timestamp = u4swap(d[1]);
	  _sample = new sample;
	  memset (_sample, 0 , sizeof(sample) );
	  _sample->timestamp = u4swap(d[1]);
	}

      // if the time stamp changes, we are done with this smaple.
      // me are pushing this on the stack and mobe on to the next. 
      if ( old_timestamp != u4swap(d[1]))
	{
	  waveform.push_back(_sample);
	  //cout << endl;
	  _sample = new sample;
	  memset (_sample, 0 , sizeof(sample) );
	  _sample->timestamp = u4swap(d[1]);
	  _nr_samples++;
	  //coutfl << "*** complete, size of waveform " << waveform.size() << " timestamp: " << _sample->timestamp  << " sample nr " << nr_samples << endl;

	  if ( u4swap(d[1]) - old_timestamp > CONTIGUOUS_CLOCK) // we jump to a new trigger here
	    {
	      _nr_events++;
	    }

	}
      old_timestamp = u4swap(d[1]);

      int chip_offset =0;
      if ( ichip) chip_offset = 72;

      int group_offset =0;
      if ( group) group_offset = 36;

      int calib;

      unsigned int val;
      
      //for the next section, we continue to fill the various vlaues of that "sample" structure
      
      switch ( line_number)
	{
	case 0:
	  // header = u4swap(buffer[2]);
	  // cmn    = u4swap(buffer[3]); 
	  
	  c = 0;
	  o = 4;

	  // 6 values in line "0"
	  for ( int j = 0; j < 6; j++)
	    {
	      val = u4swap(d[o]);
	      _sample->ADC[chip_offset + group_offset + c] = (val >> 20) & 0x3ff;
	      _sample->tot[chip_offset + group_offset + c] = (val >> 10) & 0x3ff;
	      _sample->toa[chip_offset + group_offset + c] = val & 0x3ff;
	      // coutfl << hex << "Value: 0x" << val << dec << "  ADC " << _sample->ADC[chip_offset + group_offset + c]
	      // 	     << " TOT " << _sample->tot[chip_offset + group_offset + c]
	      // 	     << " TOA " << _sample->toa[chip_offset + group_offset + c]
	      // 	     << endl;
	      o++;
	      c++;
	    }

	  break; 
	  
	case 1:
	  c = 6;
	  o = 2;
	  

	  // 8 values in line "1"
	  for ( int j = 0; j < 8; j++)
	    {
	      val = u4swap(d[o]);
	      _sample->ADC[chip_offset + group_offset + c] = (val >> 20) & 0x3ff;
	      _sample->tot[chip_offset + group_offset + c] = (val >> 10) & 0x3ff;
	      _sample->toa[chip_offset + group_offset + c] = val & 0x3ff;
	      o++;
	      c++;
	    }
	  
	  break; 
	  
	case 2:
	  c = 14;
	  o = 2;
	  
	  // 3 values in line "2"
	  for ( int j = 0; j < 3; j++)
	    {
	      val = u4swap(d[o]);
	      _sample->ADC[chip_offset + group_offset + c] = (val >> 20) & 0x3ff;
	      _sample->tot[chip_offset + group_offset + c] = (val >> 10) & 0x3ff;
	      _sample->toa[chip_offset + group_offset + c] = val & 0x3ff;
	      o++;
	      c++;
	    }

	  calib        = (u4swap(d[o++]) >> 20) & 0x3ff;
	  
	  // and 4 more values
	  for ( int j = 0; j < 4; j++)
	    {
	      val = u4swap(d[o]);
	      _sample->ADC[chip_offset + group_offset + c] = (val >> 20) & 0x3ff;
	      _sample->tot[chip_offset + group_offset + c] = (val >> 10) & 0x3ff;
	      _sample->toa[chip_offset + group_offset + c] = val & 0x3ff;
	      o++;
	      c++;
	    }


	  
	  break; 
	  
	case 3:
	  c = 21;
	  o = 2;
	  
	  // 8 values in line "3"
	  for ( int j = 0; j < 8; j++)
	    {
	      val = u4swap(d[o]);
	      _sample->ADC[chip_offset + group_offset + c] = (val >> 20) & 0x3ff;
	      _sample->tot[chip_offset + group_offset + c] = (val >> 10) & 0x3ff;
	      _sample->toa[chip_offset + group_offset + c] = val & 0x3ff;
	      o++;
	      c++;
	    }
	  	  
	  break; 
	  
	case 4:
	  c = 29;
	  o = 2;
	  
	  // 7 values in line "3"
	  for ( int j = 0; j < 7; j++)
	    {
	      val = u4swap(d[o]);
	      _sample->ADC[chip_offset + group_offset + c] = (val >> 20) & 0x3ff;
	      _sample->tot[chip_offset + group_offset + c] = (val >> 10) & 0x3ff;
	      _sample->toa[chip_offset + group_offset + c] = val & 0x3ff;
	      o++;
	      c++;
	    }
	  	  
	  _sample->crc32        = u4swap(d[o++]);
	  break; 
	  
	default:
	  //cout << " invalid line number " << endl;
	  break;
	  	  
	}
    }
  return 0;
}


int oncsSub_idh2gcroc3::decode()
{
  if (is_decoded) return 0;
  is_decoded = 1;

  
  unsigned int *buffer = ( unsigned int *)  &SubeventHdr->data;

  
  int current_index  = 0;
  int lines = 0;
  int pos;
  
  std::vector<unsigned int>  incomplete_line;
  
  
  while ( current_index < getDataLength() )
    {

      int packetlength = buffer[current_index+1]/4 + 2;

      //unsigned int hdr0 = u4swap(buffer[current_index+2]);
      //coutfl << "marker " << hex << buffer[current_index] << dec << " Length " << buffer[current_index+1] << " counter " << (hdr0 & 0xffff) << endl;
      int packet_end = current_index + packetlength -1;


      

      current_index += 5;

      // coutfl << " incomplete_line_size " << incomplete_line.size() << endl;
      
      if ( incomplete_line.size() )
	{
	  long unsigned int n = 0;
	  for ( ; n < 10 - incomplete_line.size() ; n++)
	    {
	      incomplete_line.push_back(buffer[current_index + n]);
	    }
	  coutfl << " incomplete_line_size " << incomplete_line.size() << endl;
	  decode_line(&incomplete_line[0]);
	  incomplete_line.clear();
	  current_index =n;
	}



      
      // for ( n = current_index; n < packet_end ; n++)
      // 	{
      // 	  if ( (buffer[n] & 0xffff) == 0x00a0 || (buffer[n] & 0xffff) == 0x00a1)
      // 	    {
      // 	      coutfl << " found line start at " << n - current_index << " value " << hex << buffer[n] << dec << endl;
      // 	      break;
      // 	    }

      // 	}

      lines = 0;
      // here we are at the beginning of an "a0 or "a1" line 

      for ( pos = current_index; pos < packet_end; pos += 10)
	{
	  //cout << "line start " << hex << buffer[pos] << dec << " at index " << pos << endl;
	  decode_line(&buffer[pos]);
	  lines++;
	}

      current_index = pos;
      for ( pos = current_index; pos < packet_end; pos++)
	{
	  incomplete_line.push_back(buffer[pos]);
	}
      
      
      current_index = packet_end+1;
      //      coutfl << " number of lines: " << lines << endl;
    }

  parse_timeline ();
  
  return 0;  
}


unsigned int oncsSub_idh2gcroc3::u4swap(const unsigned int in)
{
  union 
  {
    unsigned int i4;
    char c[4];
  } i,o;

  i.i4 = in;
  o.c[0] = i.c[3];
  o.c[1] = i.c[2];
  o.c[2] = i.c[1];
  o.c[3] = i.c[0];
  return o.i4;
}


int oncsSub_idh2gcroc3::iValue(const int n, const char *what)
{
  decode();
  unsigned int un = n;
  
  if ( strcmp(what,"CHANNELS") == 0 )
    {
      return 144; // might be dynamic in the future
    }
  
  if ( strcmp(what,"SAMPLES") == 0 )
    {
      return (int) waveform.size();
    }
  
  if ( strcmp(what,"NR_WF") == 0 )
    {
      return (int) _eventlist.size();
    }

  if ( strcmp(what,"SAMPLESIZE") == 0 )
    {
      if ( un > _eventlist.size()) return 0; // no such event number
      event_bounds *eb = _eventlist[n];
      return eb->length;
    }
  
  else if ( strcmp(what,"TIMESTAMP") == 0 )
    {
      if (n < 0 ||  un >= waveform.size() ) return 0;
      return waveform[n]->timestamp;
    }

  else if ( strcmp(what,"CMN") == 0 )
    {
      if (n < 0 ||  un >= waveform.size() ) return 0;
      return waveform[n]->cmn;
    }

  else if ( strcmp(what,"CALIB") == 0 )
    {
      if (n < 0 ||  un >= waveform.size() ) return 0;
      return waveform[n]->calib;
    }


  return 0;
}


int oncsSub_idh2gcroc3::iValue(const int ch, const int sample)
{
  if ( ch < 0 || ch >=144 || sample < 0) return 0;
  decode();

  unsigned int un = sample;
  
  if ( un >= waveform.size() ) return 0;

  return waveform[sample]->ADC[ch];
}

int oncsSub_idh2gcroc3::iValue(const int event, const int ch, const int sample)
{
  if ( ch < 0 || ch >=144 || sample < 0) return 0;
  decode();

  // only known once we decode
  
  unsigned int ue = event;  //preventing a warning as size() is unsigned
  if ( event < 0 || ue > _eventlist.size() ) return 0;

  unsigned int un = sample;   //preventing a warning as size() is unsigned
  
  if ( un >= waveform.size() ) return 0;

  event_bounds *eb = _eventlist[event];
  int n = eb->first;
  int l = eb->length;
  if (sample >= l)  return 0;   // this event has only l samnples
  if ( (unsigned int) (n+sample) >= waveform.size())
    {
      coutfl << "elemoent out of bounds " << n + sample << "   size = " <<  waveform.size() << endl;
      return 0;
    }
  return waveform[n+sample]->ADC[ch];
}
  
int oncsSub_idh2gcroc3::iValue(const int ch, const int sample, const char *what)
{
  if ( ch < 0 || ch >=144 || sample < 0) return 0;
  decode();

  unsigned int un = sample;
  if ( un >= waveform.size() ) return 0;

  if ( strcmp(what,"TOT") == 0 )
    {
      return waveform[sample]->tot[ch];
    }

  else if ( strcmp(what,"TOA") == 0 )
    {
      return waveform[sample]->toa[ch];
    }

  return 0;
}
  
  

void oncsSub_idh2gcroc3::dump(std::ostream &os)
{
  identify(os);
  if ( _broken) 
    {
      os << " ** corrupt packet **" << endl;
      return;
    }

  decode();

  /*  
  os << "Nr of samples: " << iValue(0, "SAMPLES") << endl;

  for (int s = 0; s < iValue(0,"SAMPLES"); s++)
    {
      os << iValue(s,"TIMESTAMP") << " ";
    }
  os << endl;
  
  for ( int i = 0; i < 144; i++)
    {
      os << setw(3) << i  << " | ";
      
      for (int s = 0; s < iValue(0,"SAMPLES"); s++)
	{
	  os << setw(4) << iValue(i,s);
	}
      os << endl;
    }

  os << endl << "---------------------------------------------------------------------" << endl << endl;
  */
  
  int e = iValue(0, "NR_WF");
  os << " Number of Waveforms: " << e << endl;
  for ( int n = 0; n < e; n++)
    {
      os << "----- Event " << n << " size: " << iValue(n, "SAMPLESIZE") << endl;
      for ( int ic =72; ic < iValue(0,"CHANNELS"); ic++)
	{
	  os << setw(4) << ic << " | " ;
	  for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
	    {
	      os << setw(4) << iValue (n, ic, is) << " ";
	    }
	  os << endl;
	}
    }
  
  
}

int oncsSub_idh2gcroc3::parse_timeline ()
{
  auto itr =  waveform.begin();  // first one
  if ( itr == waveform.end()) return -1;
  
  unsigned int old_timestamp = (*itr)->timestamp;  // get the initial timestamp

  int pos = 0;
  event_bounds *eb = new event_bounds;
  eb->first = pos;
  eb->length=1;
    
  ++itr;

  for (; itr != waveform.end(); ++itr)
    {
      pos++;
      if ( (*itr)->timestamp - old_timestamp !=  CONTIGUOUS_CLOCK)
	{
	  // coutfl << "adding new event boundary at index " << eb->first << " with length " <<  eb->length << " pos is " << pos
	  // 	 << " old ts = " << old_timestamp << " new = " << (*itr)->timestamp << " diff: " << (*itr)->timestamp - old_timestamp << endl;
	  _eventlist.push_back(eb);
	  eb = new event_bounds;
	  eb->first = pos;
	  eb->length=1;
	}
      old_timestamp = (*itr)->timestamp;
      eb->length++;
    }
  eb->length--;
  // coutfl << "adding new event boundary at index " << eb->first << " with length " <<  eb->length << " pos is " << pos << endl;
  //   _eventlist.push_back(eb);
  return 0;
}

oncsSub_idh2gcroc3::~oncsSub_idh2gcroc3()
{

  for (auto itr =  waveform.begin(); itr != waveform.end(); ++itr)
    {
      delete *itr;
    }
   for (auto itr =  _eventlist.begin(); itr != _eventlist.end(); ++itr)
    {
      delete *itr;
    }
 

}


