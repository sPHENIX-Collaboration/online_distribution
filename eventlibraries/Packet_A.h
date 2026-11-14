#ifndef __PACKET_A_H__
#define __PACKET_A_H__
// -*- c++ -*-


#include "packet.h"
#include "decoding_routines.h"
#include "oncsStructures.h"
#include "oncsSubConstants.h"

#include <stddef.h>


#ifndef __CINT__
class WINDOWSEXPORT Packet_A : public Packet {
#else
class  Packet_A : public Packet {
#endif

public:

  Packet_A(subevtdata_ptr data);
  virtual ~Packet_A();

  // access to information 
  int   getLength() const;
  int   getIdentifier() const;
  int   getHitFormat() const;
  int   getPadding() const;

  int	getDataLength() const;
  int	getStructure() const { return 0;};
  int	getDebugLength() const { return 0;};
  int	getErrorLength() const { return 0;};
  int   getStatus() const { return 0;};

  // debugging-type information
  void  identify( OSTREAM& =COUT) const;

  int setIdentifier(const int newid);
  
  // getting decoded values
  int    iValue(const int);
  int    iValue(const int,const char *);
  int    iValue(const int,const int);

  int    iValue(const int,const int, const char *){return 0;};
  int    iValue(const int channel,const int iy, const int iz) {return 0;};

  int    iValue(const int channel,const int iy, const int iz, const char *what) {return 0;};

  long long    lValue(const int channel,const int iy, const int iz) {return 0;};

  float  rValue(const int);
  float  rValue(const int,const char *);
  float  rValue(const int,const int);

  int    getArraylength(const char *);
  int    fillIntArray (int [], const int, int *,const char *what="");
  int    fillFloatArray (float [], const int, int *,const char *what="");
  int*   getIntArray (int *,const char *what="");
  float* getFloatArray (int *,const char *what="");

  // pointer or data based handling
  int is_pointer_type() const;
  int convert();
  int   copyMe(int [],  const int maxlength) const;

  int setInternalParameter ( const int p1=0, const int p2=0, const char *what = "") {return 0;};

protected:

  virtual int *decode(int *) =0;


  subevtdata_ptr SubeventHdr;
  int data1_length;
  int data2_length;
  int data3_length;
  int data4_length;

  int *decoded_data1;
  int *decoded_data2;
  int *decoded_data3;
  int *decoded_data4;
  int is_data_type;
};

// --------------------------------------------------------------------------
#ifndef __CINT__
class WINDOWSEXPORT Packet_w1 : public Packet_A {
#else
class  Packet_w1 : public Packet_A {
#endif
public:
  Packet_w1(subevtdata_ptr);

  virtual void  dump ( OSTREAM& os = COUT) ;

  virtual void  gdump (const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const;

protected:
  inline virtual int *decode (int *) {return 0;};

};

// ----------------------------------------------------

#ifndef __CINT__
class WINDOWSEXPORT Packet_w2 : public Packet_A {
#else
class  Packet_w2 : public Packet_A {
#endif
public:
  Packet_w2(subevtdata_ptr);

  virtual void  dump ( OSTREAM& os = COUT) ;

  virtual void  gdump (const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const;

protected:
  inline virtual int *decode (int *) {return 0;};

};

// ----------------------------------------------------


#ifndef __CINT__
class WINDOWSEXPORT Packet_w4 : public Packet_A {
#else
class  Packet_w4 : public Packet_A {
#endif
public:
  Packet_w4(subevtdata_ptr);

  virtual void  dump ( OSTREAM& os = COUT) ;

  virtual void  gdump (const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const;

protected:
  inline virtual int *decode (int *) {return 0;};

};

// ----------------------------------------------------


#ifndef __CINT__
class WINDOWSEXPORT Packet_w8 : public Packet_A {
#else
class  Packet_w8 : public Packet_A {
#endif
public:
  Packet_w8(subevtdata_ptr);

  virtual void  dump ( OSTREAM& os = COUT) ;

  virtual void  gdump (const int how=EVT_HEXADECIMAL, OSTREAM& os = COUT) const;

protected:
  inline virtual int *decode (int *) {return 0;};

};


#endif /* __PACKET_A_H__ */
