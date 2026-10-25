#ifndef __CAEN_CALIBRATION_H__
#define __CAEN_CALIBRATION_H__

// Applies the factory calibration of a CAEN V1742 to its data packets
// (IDCAENV1742), like caen_correction, but sets itself up from the
// calibration packet (IDCAENV1742CALIB) that the device_CAENcalibration
// device writes into the begin-run event, instead of a calib_*.dat file.
// It holds the tables for all three speeds and picks the one matching the
// speed of each data packet.
//
//   Packet *pc = beginrun_event->getPacket(901);
//   caen_calibration *cc = new caen_calibration(pc);
//   delete pc;   // the tables are copied
//   ...
//   cc->init(data_packet);
//   float v = cc->caen_corrected(sample, channel);
//   float t = cc->caen_time(sample, channel);

#include <iostream>
#include <string>
#include <vector>

class Packet;

class caen_calibration {

public:

  // the packet is only read here, the caller still owns (and deletes) it
  caen_calibration(Packet *calibpacket);
  virtual ~caen_calibration() {};

  // returns 0 if the data packet could be corrected
  int init(Packet *p);

  float caen_corrected(const int sample, const int channel) const;
  float caen_time(const int sample, const int channel) const;
  float caen_rollover(const int channel) const;

  int isBroken() const { return _broken; };
  int getSerialNumber() const { return _serial; };
  int getReadoutId() const { return _readout_id; };   // the data packet this calibration was taken for

  virtual void identify(std::ostream& os) const;

 protected:

  static const int NCELLS = 1024;
  static const int NCHANNELS = 36;   // 32 + the 4 trigger channels
  static const int NCHIPS = 4;
  static const int NSPEEDS = 3;

  int _broken;
  int _serial;
  int _readout_id;
  int _warned_id;
  int _status[NSPEEDS];

  // [speed][cell][channel], [speed][sample][channel] and [speed][cell][chip]
  std::vector<short> _offset;
  std::vector<short> _nsample;
  std::vector<float> _timevec;

  int _samples;
  int _speed;                         // of the last data packet
  std::vector<float> _current_wave;   // [sample][channel]
  std::vector<float> _current_time;   // [sample][chip]

  short offset(const int speed, const int cell, const int ch) const
  { return _offset[ (speed*NCELLS + cell) * NCHANNELS + ch]; };
  short nsample(const int speed, const int sample, const int ch) const
  { return _nsample[ (speed*NCELLS + sample) * NCHANNELS + ch]; };
  float timevec(const int speed, const int cell, const int chip) const
  { return _timevec[ (speed*NCELLS + cell) * NCHIPS + chip]; };

};

#endif
