#ifndef __ONCSSUB_IDCAENV1742CALIB_H__
#define __ONCSSUB_IDCAENV1742CALIB_H__

#include "oncsSubevent.h"

// The factory calibration of a CAEN V1742/DT5742 (the DRS4 correction
// tables for all three speeds), as captured at begin-run by the
// device_CAENcalibration device of the CAENdrs plugin.
//
// The channels are numbered as in the IDCAENV1742 data packet: 0-31 are
// chip*8 + channel, 32-35 the trigger channel of chips 0-3. The speed is
// numbered as the data packet's iValue(0,"FREQUENCY"): 0 = 5GS, 1 = 2.5GS, 2 = 1GS.
//
//   iValue(cell, ch, speed, "OFFSET")   cell offset (ADC counts)
//   iValue(cell, ch, speed, "NSAMPLE")  correction by position in the readout window
//   dValue(cell, ch, speed)             time of the cell (ns)
//   iValue(speed, "STATUS")             0 if the tables for this speed were read ok
//   iValue(0, "SERIAL"), "READOUTID", "MODEL", "VERSION"
//   iValue(0, "CHANNELS"), "CELLS", "SPEEDS"

#ifndef __CINT__
class WINDOWSEXPORT oncsSub_idcaenv1742calib : public  oncsSubevent_w4 {
#else
class  oncsSub_idcaenv1742calib : public  oncsSubevent_w4 {
#endif

public:
  oncsSub_idcaenv1742calib( subevtdata_ptr);

  using oncsSubevent::iValue;
  using oncsSubevent::dValue;
  int    iValue(const int, const char *);
  int    iValue(const int cell, const int ch, const int speed, const char *what);
  double dValue(const int cell, const int ch, const int speed);

  void  dump ( OSTREAM& os = COUT) ;

protected:
  int valid() const;
  int *word(const int cell, const int ch, const int speed) const;
  int *timeword(const int cell, const int ch, const int speed) const;
  int header(const int i) const;
  std::string text(const int first, const int nwords) const;
};


#endif /* __ONCSSUB_IDCAENV1742CALIB_H__ */
