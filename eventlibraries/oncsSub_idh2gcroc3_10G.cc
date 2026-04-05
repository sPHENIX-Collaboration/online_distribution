#include "oncsSub_idh2gcroc3_10G.h"

#include <string.h>
#include "buffer.h"

using namespace std;

// this is the clock value for "contiguous samples. 
#define CONTIGUOUS_CLOCK  164

oncsSub_idh2gcroc3_10G::oncsSub_idh2gcroc3_10G(subevtdata_ptr data)
  :oncsSubevent_w4 (data)
{

  is_decoded = 0;
  _broken = 0;
  old_timestamp   = 0xffffffffffffffff;
  old_trigger_in  = 0xffffffff;
  old_trigger_out = 0xffffffff;

  _sample =0;
  _nr_channels = 0;
  _nr_samples = 0;
  _nr_events = 0;
}


int oncsSub_idh2gcroc3_10G::decode_line( uint8_t d[4*48])
{

      
  //  int o,c;

  if ( d[0] != 0xaa && d[1] != 0x5a) return 0;

  // cout << "new line " << hex << " " ;
  // for (int i = 0; i < 32; i++)
  //   {
  //     if ( i > 0 && i%4 == 0) cout << " ";
  //     cout << setw(3) << (unsigned int) d[i];
  //   }
  // cout << dec << endl;
  

  //      int ifpga = ((u4swap(d[0])>>16) & 0xff );
  //coutfl << " d[2] " << (unsigned int) d[2] << endl;
  int ichip = d[2] & 0xf;
  int group = d[3] - 36;  // needs check...
  
  uint32_t trigger_in = u4frombyte(&d[4]);
  uint32_t trigger_out = u4frombyte(&d[8]);
  uint32_t evt_ctr = u4frombyte(&d[12]);
  uint64_t timestamp = u8frombyte(&d[16]);
  //uint64_t spare = u8frombyte(&d[24]);
  
  
  // cout << hex << " ichip = " << ichip
  //      << " group " << group
  //      << " trigger_in " << trigger_in
  //      << " trigger_out " << trigger_out
  //      << " evt_ctr " << evt_ctr
  //      << " timestamp " << timestamp
  //      << " spare " << spare 
  //      << dec << endl;
  
  uint32_t ROC_HDR;
  uint32_t CMN;
  uint32_t CALIB;
  uint32_t CRC;
  uint32_t ch[36];
  
  ROC_HDR = u4frombyte(&d[32 + 0]);
  CMN     = u4frombyte(&d[32 + 4]);
  
  // coutfl << "ROC_HDR " << hex << ROC_HDR << " CMN: " << CMN << dec << endl;
  
  for (int n = 0; n < 18; n++)
    {
      ch[n] = u4frombyte(&d[32 + 8 + n*4]);
    }
  
  CALIB = u4frombyte(&d[112]);
  
  for (int n = 0; n < 18; n++)
    {
      ch[n+18] = u4frombyte(&d[112 + 4 + n*4]);
    }
  
  CRC = u4frombyte(&d[188]);
  
  // on the first invocation, old_timestamp is -1 (or 0xfff... as an unsigned)
  // initializing it this way will make the next check succeed.

  if ( ichip >=MAX_CHIPS) return 0;

  //let's see if we need to update our nr of channels
  if ( (ichip+1) *72 > _nr_channels) _nr_channels = (ichip+1) *72;
  
  if ( old_trigger_in == 0xffffffff || old_trigger_out == 0xffffffff) // first data
    {
      old_timestamp = timestamp;
      old_trigger_in = trigger_in;
      old_trigger_out = trigger_out;
      _sample = new sample;
      memset (_sample, 0 , sizeof(sample) );
      _sample->timestamp = timestamp;

    }
  
  // if the time stamp changes, we are done with this smaple.
  // me are pushing this on the stack and move on to the next. 
  //if ( old_trigger_out != trigger_out || old_trigger_in != trigger_in)

  //coutfl << "new, old timestamp " << timestamp << "  " <<  old_timestamp << " diff " << timestamp - old_timestamp  << endl;

  if (  ( timestamp >= old_timestamp && timestamp - old_timestamp >  5 ) || ( timestamp < old_timestamp && old_timestamp - timestamp >  5 ))
    {
      waveform.push_back(_sample);
      _nr_samples++;
      
      //      coutfl << "*** complete, size of waveform " << waveform.size() << " timestamp: " << _sample->timestamp  << " sample nr " << _nr_samples << endl;
      _sample = new sample;
      memset (_sample, 0 , sizeof(sample) );
      _sample->timestamp = timestamp;
      
      // if ( timestamp - old_timestamp > CONTIGUOUS_CLOCK) // we jump to a new trigger here
      // 	{
      // 	  _nr_events++;
      // 	}
      
    }
  old_timestamp = timestamp;
  old_trigger_in = trigger_in;
  old_trigger_out = trigger_out;
  
  int chip_offset =0;
  chip_offset = ichip * 72;
  
  int group_offset =0;
  if ( group) group_offset = 36;
  
  //for the next section, we continue to fill the various vlaues of that "sample" structure
  for ( int c = 0; c< 36; c++)
    {
      _sample->ADC[chip_offset + group_offset + c] = (ch[c] >> 20) & 0x3ff;
      _sample->tot[chip_offset + group_offset + c] = (ch[c] >> 10) & 0x3f;
      _sample->toa[chip_offset + group_offset + c] = ch[c] & 0x3ff;
      
      // coutfl << setw(4) << chip_offset + group_offset + c << " " << hex
	  // 		 << " ADC " << _sample->ADC[chip_offset + group_offset + c]
	  // 		 << " TOT " << _sample->tot[chip_offset + group_offset + c]
	  // 		 << " TOA " << _sample->toa[chip_offset + group_offset + c]
	  // 		 << dec << endl;
    }
  
  
  _sample->trigger_in   = trigger_in;
  _sample->trigger_out  = trigger_out;
  _sample->event_counter  = evt_ctr;
  _sample->cmn          = CMN;
  _sample->calib        = CALIB;
  _sample->crc32        = CRC;
  
  
  return 0;
}


int oncsSub_idh2gcroc3_10G::decode()
{
  if (is_decoded) return 0;
  is_decoded = 1;

  
  //unsigned int *buffer = ( unsigned int *)  &SubeventHdr->data;
  uint8_t *cbuffer = ( uint8_t *)  &SubeventHdr->data;

  
  //int current_index  = 0;
  int current_index_bytes =0;
  
  int lines = 0;
  int pos;
  
  std::vector<unsigned int>  incomplete_line;
  
  
  while ( current_index_bytes < 4*getDataLength() )
    {

      unsigned int packetstart = current_index_bytes; // we remember where the packet began
      
      unsigned int packetlength = u4swapfrombyte(&cbuffer[current_index_bytes + 4]);


      packetlength += 8;
      

      //unsigned int hdr0 = u4swap(buffer[current_index+2]);
      // coutfl << "current_index_bytes = " << current_index_bytes << "  marker " << hex <<  setw(2) << cbuffer[current_index_bytes] << setw(2) << cbuffer[current_index_bytes+1]<< dec << " Length " << packetlength <<  endl;

      int packet_end_bytes = current_index_bytes + packetlength -4;

      current_index_bytes += 22;

      // coutfl << " incomplete_line_size " << incomplete_line.size() << endl;
      
      // if ( incomplete_line.size() )
      // 	{
      // 	  long unsigned int n = 0;
      // 	  for ( ; n < 10 - incomplete_line.size() ; n++)
      // 	    {
      // 	      incomplete_line.push_back(buffer[current_index + n]);
      // 	    }
      // 	  coutfl << " incomplete_line_size " << incomplete_line.size() << endl;
      // 	  decode_line(&incomplete_line[0]);
      // 	  incomplete_line.clear();
      // 	  current_index =n;
      // 	}



      
      // for ( n = current_index_bytes; n < packet_end_bytes ; n++)
      // 	{
      // 	  if ( (cbuffer[n] & 0xffff) == 0x00a0 || (buffer[n] & 0xffff) == 0x00a1)
      // 	    {
      // 	      coutfl << " found line start at " << n - current_index << " value " << hex << buffer[n] << dec << endl;
      // 	      break;
      // 	    }

      // 	}

      lines = 0;
      // here we are at the beginning of an "a0 or "a1" line 

      for ( pos = current_index_bytes; pos < packet_end_bytes; pos += 4*48)
	{
	  // coutfl << "line start " << hex << (unsigned int) cbuffer[pos] << dec << " at index " << pos << endl;
	  decode_line(&cbuffer[pos]);
	  lines++;
	}

      // current_index = pos;
      // for ( pos = current_index; pos < packet_end; pos++)
      // 	{
      // 	  incomplete_line.push_back(buffer[pos]);
      // 	}
      
      
      current_index_bytes = packetstart + packetlength;
      //      coutfl << " number of lines: " << lines << endl;
    }

  parse_timeline ();
  
  return 0;  
}


unsigned int oncsSub_idh2gcroc3_10G::u4swapfrombyte(uint8_t* v)
{
  unsigned int r = (v[3] << 24)
	|  (v[2] << 16)
	|  (v[1] << 8)
	|  (v[0]);

  return r;
}

unsigned int oncsSub_idh2gcroc3_10G::u4frombyte(uint8_t* v)
{

  unsigned int r = (v[0] << 24)
	|  (v[1] << 16)
	|  (v[2] << 8)
	|  (v[3]);

  return r;
}

unsigned long long oncsSub_idh2gcroc3_10G::u8frombyte(uint8_t* v)
{

  unsigned long long r = 0;
  
  for ( int i = 0; i < 8; i++)
    {
      unsigned long long x = v[i];
      x <<= 8*(7-i);
      r |= x;
    }
	
  return r;
}




int oncsSub_idh2gcroc3_10G::iValue(const int n, const char *what)
{
  decode();
  unsigned int un = n;
  
  if ( strcmp(what,"CHANNELS") == 0 )
    {
      return _nr_channels; // might be dynamic in the future
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


int oncsSub_idh2gcroc3_10G::iValue(const int sample, const int ch)
{
  decode();
  if ( ch < 0 || ch >=_nr_channels || sample < 0) return 0;

  unsigned int un = sample;
  
  if ( un >= waveform.size() ) return 0;

  return waveform[sample]->ADC[ch];
}

long long   oncsSub_idh2gcroc3_10G::lValue(const int wf, const int sample, const int ch)
{
  decode();
  if ( ch < 0 || ch >= _nr_channels || sample < 0) return 0;
  
  unsigned int ue = wf;  //preventing a warning as size() is unsigned
  if ( wf < 0 || ue > _eventlist.size() ) return 0;

  unsigned int un = sample;   //preventing a warning as size() is unsigned
  
  if ( un >= waveform.size() ) return 0;

  event_bounds *eb = _eventlist[wf];
  int n = eb->first;
  int l = eb->length;
  if (sample >= l)  return 0;   // this event has only l samnples
  if ( (unsigned int) (n+sample) >= waveform.size())
    {
      coutfl << "element out of bounds " << n + sample << "   size = " <<  waveform.size() << endl;
      return 0;
    }
  return (long long) waveform[n+sample]->timestamp;
}



int oncsSub_idh2gcroc3_10G::iValue(const int wf, const int sample, const int ch)
{
  decode();
  if ( ch < 0 || ch >=_nr_channels || sample < 0) return 0;

  // only known once we decode
  
  unsigned int ue = wf;  //preventing a warning as size() is unsigned
  if ( wf < 0 || ue > _eventlist.size() ) return 0;

  unsigned int un = sample;   //preventing a warning as size() is unsigned
  
  if ( un >= waveform.size() ) return 0;

  event_bounds *eb = _eventlist[wf];
  int n = eb->first;
  int l = eb->length;
  if (sample >= l)  return 0;   // this event has only l samnples
  if ( (unsigned int) (n+sample) >= waveform.size())
    {
      coutfl << "element out of bounds " << n + sample << "   size = " <<  waveform.size() << endl;
      return 0;
    }
  return waveform[n+sample]->ADC[ch];
}
  
int oncsSub_idh2gcroc3_10G::iValue(const int sample, const int ch, const char *what)
{
  decode();
  if ( ch < 0 || ch >=_nr_channels || sample < 0) return 0;

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
  
// this is for the TOT and TOA values
int oncsSub_idh2gcroc3_10G::iValue(const int wf, const int sample, const int ch, const char *what)
{
  // these are the easy checks we can do w/o decoding the data
  if ( ch < 0 || ch >=_nr_channels || sample < 0) return 0;
  if ( wf < 0) return 0;

  // now the ones where we need the decoded data
  decode();

  
  
  if ( (unsigned int ) wf  >= _eventlist.size()) return 0; // no such wf number
  event_bounds *eb = _eventlist[wf];
  if ( (unsigned int ) sample >= eb->length) return 0;

  int n = eb->first;
  
  if ( (unsigned int) (n+sample) >= waveform.size())
    {
      coutfl << "element out of bounds " << n + sample << "   size = " <<  waveform.size() << endl;
      return 0;
    }

 
  if ( strcmp(what,"TOT") == 0 )
    {
      return waveform[n+sample]->tot[ch];
    }
  else if ( strcmp(what,"TOA") == 0 )
    {
      return waveform[n+sample]->toa[ch];
    }
  else if ( strcmp(what,"TRIGGER_IN") == 0 )
    {
      return waveform[n+sample]->trigger_in;
    }
  else if ( strcmp(what,"TRIGGER_OUT") == 0 )
    {
      return waveform[n+sample]->trigger_out;
    }
  else if ( strcmp(what,"EVENT_COUNTER") == 0 )
    {
      return waveform[n+sample]->event_counter;
    }

  return 0;
}
  
  

void oncsSub_idh2gcroc3_10G::dump(std::ostream &os)
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
  os << " Number of Channels:  " << iValue(0, "CHANNELS") << endl;
  os << " Number of Waveforms: " << e << endl << endl;

  for ( int n = 0; n < e; n++)
    {
      os << " ----- Waveform  " << n << " Samples: " << iValue(n, "SAMPLESIZE") << "  Timestamp: 0x" << hex << lValue(n,0,0) << dec  << endl;
      for ( int ic =0; ic < iValue(0,"CHANNELS"); ic++)
	{

	  int show_this = 1;
	  for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
	    {
	      if ( iValue (n, is, ic) || iValue (n, is, ic, "TOT") || iValue (n, is, ic, "TOA") ) show_this =1;
	    }
	  
	  if ( show_this)
	    {
	      os << setw(4) << ic << " | ADC | " << hex;
	      for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
		{
		  os << " " << setw(4)  << iValue (n, is, ic);
		}
	      os << dec << endl;

	      os << setw(4) << ic << " | TOT | " << hex;
	      for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
		{
		  os << " " << setw(4) << iValue (n, is, ic, "TOT");
		}
	      os << dec << endl;
	      
	      os << setw(4) << ic << " | TOA | " << hex;
	      for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
		{
		  os << " " << setw(4) << iValue (n, is, ic, "TOA");
		}
	      os << dec << endl;
	      
	      // os << "ECTR" << " | " ;
	      // for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
	      //   {
	      //     os << " " << setw(4) << iValue (n, is, ic, "EVENT_COUNTER");
	      //   }
	      // os << dec << endl;
	      // os << "TOUT" << " | " ;
	      // for ( int is = 0; is < iValue(n, "SAMPLESIZE") ; is++)
	      //   {
	      //     os << " " << setw(4) << iValue (n, is, ic, "TRIGGER_OUT");
	      //   }
	      // os << dec << endl;
	      os << dec << endl;
	    }
	}
    }
  
  
}

int oncsSub_idh2gcroc3_10G::parse_timeline ()
{
  //coutfl << "Waveform size " << waveform.size() << endl; // first one
  auto itr =  waveform.begin();  // first one
  if ( itr == waveform.end()) return -1;
  
  uint64_t old_timestamp = (*itr)->timestamp;  // get the initial values
  //  unsigned int old_trigger_in = (*itr)->trigger_in;  // get the initial values
  //unsigned int old_trigger_out = (*itr)->trigger_out;  // get the initial values

  int pos = 0;
  event_bounds *eb = new event_bounds;
  eb->first = pos;
  eb->length=1;

  ++itr;

  for (; itr != waveform.end(); ++itr)
    {
      // coutfl << "sample trigger " << (*itr)->trigger_out << " pos " << pos << " diff: " << (*itr)->timestamp - old_timestamp << endl;

      //      if ( (*itr)->trigger_in  != old_trigger_in ||  (*itr)->trigger_out  != old_trigger_out)
      if ( (*itr)->timestamp - old_timestamp > CONTIGUOUS_CLOCK )
	{
	  eb->length--;
	  // coutfl << "adding new event boundary at index " << eb->first << " with length " <<  eb->length << " pos is " << pos
	  //  	 << " old ts = " << old_timestamp << " new = " << (*itr)->timestamp << " diff: " << (*itr)->timestamp - old_timestamp << endl;
	  _eventlist.push_back(eb);
	  eb = new event_bounds;
	  eb->first = pos;
	  eb->length=1;
	}
      old_timestamp = (*itr)->timestamp;
      //old_trigger_in = (*itr)->trigger_in;  // get the initial values
      //old_trigger_out = (*itr)->trigger_out;  // get the initial values
      eb->length++;
      pos++;
    }
  
  eb->length--;
  //  coutfl << "adding new event boundary at index " << eb->first << " with length " <<  eb->length << " pos is " << pos << endl;
  _eventlist.push_back(eb);
  return 0;
}

oncsSub_idh2gcroc3_10G::~oncsSub_idh2gcroc3_10G()
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


