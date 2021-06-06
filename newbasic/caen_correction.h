
#ifndef __CAEN_CORRECTION_H__
#define __CAEN_CORRECTION_H__

#include <string>

class Packet;
class caen_correction {

public:

  //  caen_correction ( const std::string chip0, const std::string  chip1, const std::string chip2, const std::string chip3);
  //  caen_correction ( const char *chip0, const char *chip1, const char *chip2, const char *chip3);
  caen_correction ( const char *calibdata);
  virtual ~caen_correction () {};

  int init(Packet *p);

  float caen_corrected(const int sample, const int channel) const;
  float caen_time(const int sample, const int channel) const;

  //  virtual void identify(std::ostream& os = std::cout) const;

 protected:
  Packet *_p;
  int _broken;

  int base[1024][4*9];
  float timevec[1024][4];

  int current_wave[1024][4*9];
  float current_time[1024][4];
 
};

#endif
