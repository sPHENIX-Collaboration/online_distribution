#ifndef __A_EVENT_H
#define __A_EVENT_H

#include "Event.h"
#include "EvtConstants.h"
#include "phenixOnline.h"
#include "EvtStructures.h"

#if !defined(SunOS) && !defined(OSF1)
#include <map>
#endif

#ifndef __CINT__
class WINDOWSEXPORT A_Event : public Event {
#else
class  A_Event : public Event {
#endif

public:
  // constructors and destructors
  A_Event( PHDWORD *);
  A_Event( int *);
  ~A_Event();

  // info & debug utils

  unsigned int getEvtLength () const;
  int getEvtType() const;
  int getEvtSequence() const;
  int getRunNumber() const;

  void identify(std::ostream& os = std::cout) const;

  void listFrame( const int id=0, OSTREAM& os=COUT ) const;

  void listHistory( const int id=0, OSTREAM& os=COUT ) const;

  void listError( const int id=0, OSTREAM& os=COUT ) const;

  unsigned int getFrameEntry(  const char *what, const int id=0, const int index =0) const;

  // packet handling
  Packet* getPacket(const int);

  Packet* getPacket(const int, const int hitFormat);

  int getPacketList(Packet*[], const int);
  std::vector<Packet *> getPacketVector();

  int existPacket (const int);

  //event copying
  int Copy ( int *, const unsigned int, int *, const char *what ="");



  int getErrorCode();
  unsigned int getTagWord( const int i=0) const 
    { 
      if ( i) return EventData->reserved[1]; 
      return EventData->reserved[0]; 
    };

  int is_pointer_type() const;
  int convert ();

  static void dumpFrame(PHDWORD *fp, OSTREAM &os = COUT);
  static void dumpErrorBlock(PHDWORD *fp, OSTREAM &os = COUT);

  static void dumpBlock(PHDWORD *p, UINT len, OSTREAM &os = COUT, const int how=EVT_HEXADECIMAL);

  int getDate() { return 0;};
  time_t getTime() const;

  static Packet *makePacket(PHDWORD *pp, const int hitformat=0);

  void setOriginBuffer(const int n) {originBuffer =n;};
  int getOriginBuffer() const {return originBuffer;};

protected:
  virtual int updateFramelist();

  virtual unsigned int getFrameValue(const char *what, PHDWORD *fp, const unsigned int index =0) const;


#if !defined(SunOS) && !defined(OSF1)
  virtual int createMap();
#endif
  int is_data_type;  // 0 is pointer based --  1 is data based
  evtdata_ptr EventData;
  PHDWORD **framelist;
  int NumberFrames;
  int hasMap;
  int errorcode;
  int originBuffer;

#if !defined(SunOS) && !defined(OSF1)
  std::map <int, PHDWORD *> pmap;

#endif

};

#endif



