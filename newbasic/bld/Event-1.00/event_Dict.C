// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME event_Dict
#define R__NO_DEPRECATION

/*******************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define G__DICTIONARY
#include "RConfig.h"
#include "TClass.h"
#include "TDictAttributeMap.h"
#include "TInterpreter.h"
#include "TROOT.h"
#include "TBuffer.h"
#include "TMemberInspector.h"
#include "TInterpreter.h"
#include "TVirtualMutex.h"
#include "TError.h"

#ifndef G__ROOT
#define G__ROOT
#endif

#include "RtypesImp.h"
#include "TIsAProxy.h"
#include "TFileMergeInfo.h"
#include <algorithm>
#include "TCollectionProxyInfo.h"
/*******************************************************************/

#include "TDataMember.h"

// Header files passed as explicit arguments
#include "ioselect.h"
#include "../Event.h"
#include "../Eventiterator.h"
#include "../fileEventiterator.h"
#include "../listEventiterator.h"
#include "../oncsEventiterator.h"
#include "../rcdaqEventiterator.h"
#include "../packet.h"
#include "../testEventiterator.h"
#include "../eventReceiverClient.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *Packet_Dictionary();
   static void Packet_TClassManip(TClass*);
   static void delete_Packet(void *p);
   static void deleteArray_Packet(void *p);
   static void destruct_Packet(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Packet*)
   {
      ::Packet *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Packet));
      static ::ROOT::TGenericClassInfo 
         instance("Packet", "packet.h", 20,
                  typeid(::Packet), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Packet_Dictionary, isa_proxy, 3,
                  sizeof(::Packet) );
      instance.SetDelete(&delete_Packet);
      instance.SetDeleteArray(&deleteArray_Packet);
      instance.SetDestructor(&destruct_Packet);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Packet*)
   {
      return GenerateInitInstanceLocal((::Packet*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Packet*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Packet_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Packet*)nullptr)->GetClass();
      Packet_TClassManip(theClass);
   return theClass;
   }

   static void Packet_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Event_Dictionary();
   static void Event_TClassManip(TClass*);
   static void delete_Event(void *p);
   static void deleteArray_Event(void *p);
   static void destruct_Event(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Event*)
   {
      ::Event *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Event));
      static ::ROOT::TGenericClassInfo 
         instance("Event", "", 115,
                  typeid(::Event), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Event_Dictionary, isa_proxy, 3,
                  sizeof(::Event) );
      instance.SetDelete(&delete_Event);
      instance.SetDeleteArray(&deleteArray_Event);
      instance.SetDestructor(&destruct_Event);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Event*)
   {
      return GenerateInitInstanceLocal((::Event*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Event*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Event_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Event*)nullptr)->GetClass();
      Event_TClassManip(theClass);
   return theClass;
   }

   static void Event_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *Eventiterator_Dictionary();
   static void Eventiterator_TClassManip(TClass*);
   static void delete_Eventiterator(void *p);
   static void deleteArray_Eventiterator(void *p);
   static void destruct_Eventiterator(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::Eventiterator*)
   {
      ::Eventiterator *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::Eventiterator));
      static ::ROOT::TGenericClassInfo 
         instance("Eventiterator", "", 293,
                  typeid(::Eventiterator), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &Eventiterator_Dictionary, isa_proxy, 3,
                  sizeof(::Eventiterator) );
      instance.SetDelete(&delete_Eventiterator);
      instance.SetDeleteArray(&deleteArray_Eventiterator);
      instance.SetDestructor(&destruct_Eventiterator);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::Eventiterator*)
   {
      return GenerateInitInstanceLocal((::Eventiterator*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::Eventiterator*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *Eventiterator_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::Eventiterator*)nullptr)->GetClass();
      Eventiterator_TClassManip(theClass);
   return theClass;
   }

   static void Eventiterator_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *fileEventiterator_Dictionary();
   static void fileEventiterator_TClassManip(TClass*);
   static void delete_fileEventiterator(void *p);
   static void deleteArray_fileEventiterator(void *p);
   static void destruct_fileEventiterator(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::fileEventiterator*)
   {
      ::fileEventiterator *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::fileEventiterator));
      static ::ROOT::TGenericClassInfo 
         instance("fileEventiterator", "", 358,
                  typeid(::fileEventiterator), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &fileEventiterator_Dictionary, isa_proxy, 3,
                  sizeof(::fileEventiterator) );
      instance.SetDelete(&delete_fileEventiterator);
      instance.SetDeleteArray(&deleteArray_fileEventiterator);
      instance.SetDestructor(&destruct_fileEventiterator);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::fileEventiterator*)
   {
      return GenerateInitInstanceLocal((::fileEventiterator*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::fileEventiterator*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *fileEventiterator_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::fileEventiterator*)nullptr)->GetClass();
      fileEventiterator_TClassManip(theClass);
   return theClass;
   }

   static void fileEventiterator_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *listEventiterator_Dictionary();
   static void listEventiterator_TClassManip(TClass*);
   static void delete_listEventiterator(void *p);
   static void deleteArray_listEventiterator(void *p);
   static void destruct_listEventiterator(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::listEventiterator*)
   {
      ::listEventiterator *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::listEventiterator));
      static ::ROOT::TGenericClassInfo 
         instance("listEventiterator", "", 443,
                  typeid(::listEventiterator), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &listEventiterator_Dictionary, isa_proxy, 3,
                  sizeof(::listEventiterator) );
      instance.SetDelete(&delete_listEventiterator);
      instance.SetDeleteArray(&deleteArray_listEventiterator);
      instance.SetDestructor(&destruct_listEventiterator);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::listEventiterator*)
   {
      return GenerateInitInstanceLocal((::listEventiterator*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::listEventiterator*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *listEventiterator_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::listEventiterator*)nullptr)->GetClass();
      listEventiterator_TClassManip(theClass);
   return theClass;
   }

   static void listEventiterator_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *oncsEventiterator_Dictionary();
   static void oncsEventiterator_TClassManip(TClass*);
   static void delete_oncsEventiterator(void *p);
   static void deleteArray_oncsEventiterator(void *p);
   static void destruct_oncsEventiterator(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::oncsEventiterator*)
   {
      ::oncsEventiterator *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::oncsEventiterator));
      static ::ROOT::TGenericClassInfo 
         instance("oncsEventiterator", "", 505,
                  typeid(::oncsEventiterator), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &oncsEventiterator_Dictionary, isa_proxy, 3,
                  sizeof(::oncsEventiterator) );
      instance.SetDelete(&delete_oncsEventiterator);
      instance.SetDeleteArray(&deleteArray_oncsEventiterator);
      instance.SetDestructor(&destruct_oncsEventiterator);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::oncsEventiterator*)
   {
      return GenerateInitInstanceLocal((::oncsEventiterator*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::oncsEventiterator*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *oncsEventiterator_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::oncsEventiterator*)nullptr)->GetClass();
      oncsEventiterator_TClassManip(theClass);
   return theClass;
   }

   static void oncsEventiterator_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *rcdaqEventiterator_Dictionary();
   static void rcdaqEventiterator_TClassManip(TClass*);
   static void *new_rcdaqEventiterator(void *p = nullptr);
   static void *newArray_rcdaqEventiterator(Long_t size, void *p);
   static void delete_rcdaqEventiterator(void *p);
   static void deleteArray_rcdaqEventiterator(void *p);
   static void destruct_rcdaqEventiterator(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::rcdaqEventiterator*)
   {
      ::rcdaqEventiterator *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::rcdaqEventiterator));
      static ::ROOT::TGenericClassInfo 
         instance("rcdaqEventiterator", "", 557,
                  typeid(::rcdaqEventiterator), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &rcdaqEventiterator_Dictionary, isa_proxy, 3,
                  sizeof(::rcdaqEventiterator) );
      instance.SetNew(&new_rcdaqEventiterator);
      instance.SetNewArray(&newArray_rcdaqEventiterator);
      instance.SetDelete(&delete_rcdaqEventiterator);
      instance.SetDeleteArray(&deleteArray_rcdaqEventiterator);
      instance.SetDestructor(&destruct_rcdaqEventiterator);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::rcdaqEventiterator*)
   {
      return GenerateInitInstanceLocal((::rcdaqEventiterator*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::rcdaqEventiterator*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *rcdaqEventiterator_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::rcdaqEventiterator*)nullptr)->GetClass();
      rcdaqEventiterator_TClassManip(theClass);
   return theClass;
   }

   static void rcdaqEventiterator_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *testEventiterator_Dictionary();
   static void testEventiterator_TClassManip(TClass*);
   static void *new_testEventiterator(void *p = nullptr);
   static void *newArray_testEventiterator(Long_t size, void *p);
   static void delete_testEventiterator(void *p);
   static void deleteArray_testEventiterator(void *p);
   static void destruct_testEventiterator(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::testEventiterator*)
   {
      ::testEventiterator *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::testEventiterator));
      static ::ROOT::TGenericClassInfo 
         instance("testEventiterator", "", 945,
                  typeid(::testEventiterator), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &testEventiterator_Dictionary, isa_proxy, 3,
                  sizeof(::testEventiterator) );
      instance.SetNew(&new_testEventiterator);
      instance.SetNewArray(&newArray_testEventiterator);
      instance.SetDelete(&delete_testEventiterator);
      instance.SetDeleteArray(&deleteArray_testEventiterator);
      instance.SetDestructor(&destruct_testEventiterator);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::testEventiterator*)
   {
      return GenerateInitInstanceLocal((::testEventiterator*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::testEventiterator*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *testEventiterator_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::testEventiterator*)nullptr)->GetClass();
      testEventiterator_TClassManip(theClass);
   return theClass;
   }

   static void testEventiterator_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *eventReceiverClient_Dictionary();
   static void eventReceiverClient_TClassManip(TClass*);
   static void delete_eventReceiverClient(void *p);
   static void deleteArray_eventReceiverClient(void *p);
   static void destruct_eventReceiverClient(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::eventReceiverClient*)
   {
      ::eventReceiverClient *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::eventReceiverClient));
      static ::ROOT::TGenericClassInfo 
         instance("eventReceiverClient", "", 995,
                  typeid(::eventReceiverClient), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &eventReceiverClient_Dictionary, isa_proxy, 3,
                  sizeof(::eventReceiverClient) );
      instance.SetDelete(&delete_eventReceiverClient);
      instance.SetDeleteArray(&deleteArray_eventReceiverClient);
      instance.SetDestructor(&destruct_eventReceiverClient);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::eventReceiverClient*)
   {
      return GenerateInitInstanceLocal((::eventReceiverClient*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::eventReceiverClient*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *eventReceiverClient_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::eventReceiverClient*)nullptr)->GetClass();
      eventReceiverClient_TClassManip(theClass);
   return theClass;
   }

   static void eventReceiverClient_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrapper around operator delete
   static void delete_Packet(void *p) {
      delete ((::Packet*)p);
   }
   static void deleteArray_Packet(void *p) {
      delete [] ((::Packet*)p);
   }
   static void destruct_Packet(void *p) {
      typedef ::Packet current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Packet

namespace ROOT {
   // Wrapper around operator delete
   static void delete_Event(void *p) {
      delete ((::Event*)p);
   }
   static void deleteArray_Event(void *p) {
      delete [] ((::Event*)p);
   }
   static void destruct_Event(void *p) {
      typedef ::Event current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Event

namespace ROOT {
   // Wrapper around operator delete
   static void delete_Eventiterator(void *p) {
      delete ((::Eventiterator*)p);
   }
   static void deleteArray_Eventiterator(void *p) {
      delete [] ((::Eventiterator*)p);
   }
   static void destruct_Eventiterator(void *p) {
      typedef ::Eventiterator current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::Eventiterator

namespace ROOT {
   // Wrapper around operator delete
   static void delete_fileEventiterator(void *p) {
      delete ((::fileEventiterator*)p);
   }
   static void deleteArray_fileEventiterator(void *p) {
      delete [] ((::fileEventiterator*)p);
   }
   static void destruct_fileEventiterator(void *p) {
      typedef ::fileEventiterator current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::fileEventiterator

namespace ROOT {
   // Wrapper around operator delete
   static void delete_listEventiterator(void *p) {
      delete ((::listEventiterator*)p);
   }
   static void deleteArray_listEventiterator(void *p) {
      delete [] ((::listEventiterator*)p);
   }
   static void destruct_listEventiterator(void *p) {
      typedef ::listEventiterator current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::listEventiterator

namespace ROOT {
   // Wrapper around operator delete
   static void delete_oncsEventiterator(void *p) {
      delete ((::oncsEventiterator*)p);
   }
   static void deleteArray_oncsEventiterator(void *p) {
      delete [] ((::oncsEventiterator*)p);
   }
   static void destruct_oncsEventiterator(void *p) {
      typedef ::oncsEventiterator current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::oncsEventiterator

namespace ROOT {
   // Wrappers around operator new
   static void *new_rcdaqEventiterator(void *p) {
      return  p ? new(p) ::rcdaqEventiterator : new ::rcdaqEventiterator;
   }
   static void *newArray_rcdaqEventiterator(Long_t nElements, void *p) {
      return p ? new(p) ::rcdaqEventiterator[nElements] : new ::rcdaqEventiterator[nElements];
   }
   // Wrapper around operator delete
   static void delete_rcdaqEventiterator(void *p) {
      delete ((::rcdaqEventiterator*)p);
   }
   static void deleteArray_rcdaqEventiterator(void *p) {
      delete [] ((::rcdaqEventiterator*)p);
   }
   static void destruct_rcdaqEventiterator(void *p) {
      typedef ::rcdaqEventiterator current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::rcdaqEventiterator

namespace ROOT {
   // Wrappers around operator new
   static void *new_testEventiterator(void *p) {
      return  p ? new(p) ::testEventiterator : new ::testEventiterator;
   }
   static void *newArray_testEventiterator(Long_t nElements, void *p) {
      return p ? new(p) ::testEventiterator[nElements] : new ::testEventiterator[nElements];
   }
   // Wrapper around operator delete
   static void delete_testEventiterator(void *p) {
      delete ((::testEventiterator*)p);
   }
   static void deleteArray_testEventiterator(void *p) {
      delete [] ((::testEventiterator*)p);
   }
   static void destruct_testEventiterator(void *p) {
      typedef ::testEventiterator current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::testEventiterator

namespace ROOT {
   // Wrapper around operator delete
   static void delete_eventReceiverClient(void *p) {
      delete ((::eventReceiverClient*)p);
   }
   static void deleteArray_eventReceiverClient(void *p) {
      delete [] ((::eventReceiverClient*)p);
   }
   static void destruct_eventReceiverClient(void *p) {
      typedef ::eventReceiverClient current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::eventReceiverClient

namespace {
  void TriggerDictionaryInitialization_event_Dict_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "event_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class __attribute__((annotate("$clingAutoload$packet.h")))  Packet;
class Event;
class Eventiterator;
class fileEventiterator;
class listEventiterator;
class oncsEventiterator;
class rcdaqEventiterator;
class testEventiterator;
class eventReceiverClient;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "event_Dict dictionary payload"

#ifndef PACKAGE_NAME
  #define PACKAGE_NAME "Event"
#endif
#ifndef PACKAGE_TARNAME
  #define PACKAGE_TARNAME "event"
#endif
#ifndef PACKAGE_VERSION
  #define PACKAGE_VERSION "1.00"
#endif
#ifndef PACKAGE_STRING
  #define PACKAGE_STRING "Event 1.00"
#endif
#ifndef PACKAGE_BUGREPORT
  #define PACKAGE_BUGREPORT ""
#endif
#ifndef PACKAGE_URL
  #define PACKAGE_URL ""
#endif
#ifndef STDC_HEADERS
  #define STDC_HEADERS 1
#endif
#ifndef HAVE_SYS_TYPES_H
  #define HAVE_SYS_TYPES_H 1
#endif
#ifndef HAVE_SYS_STAT_H
  #define HAVE_SYS_STAT_H 1
#endif
#ifndef HAVE_STDLIB_H
  #define HAVE_STDLIB_H 1
#endif
#ifndef HAVE_STRING_H
  #define HAVE_STRING_H 1
#endif
#ifndef HAVE_MEMORY_H
  #define HAVE_MEMORY_H 1
#endif
#ifndef HAVE_STRINGS_H
  #define HAVE_STRINGS_H 1
#endif
#ifndef HAVE_INTTYPES_H
  #define HAVE_INTTYPES_H 1
#endif
#ifndef HAVE_STDINT_H
  #define HAVE_STDINT_H 1
#endif
#ifndef HAVE_UNISTD_H
  #define HAVE_UNISTD_H 1
#endif
#ifndef HAVE_DLFCN_H
  #define HAVE_DLFCN_H 1
#endif
#ifndef LT_OBJDIR
  #define LT_OBJDIR ".libs/"
#endif
#ifndef PACKAGE
  #define PACKAGE "Event"
#endif
#ifndef VERSION
  #define VERSION "1.00"
#endif
#ifndef STDC_HEADERS
  #define STDC_HEADERS 1
#endif
#ifndef HAVE_GETOPT_H
  #define HAVE_GETOPT_H 1
#endif
#ifndef HAVE_SETENV
  #define HAVE_SETENV 1
#endif
#ifndef HAVE_ROOT6
  #define HAVE_ROOT6 1
#endif
#ifndef HAVE_STRPTIME_PROTOTYPE
  #define HAVE_STRPTIME_PROTOTYPE 1
#endif
#ifndef RDBUF_ACCEPTS_STREAMBUF
  #define RDBUF_ACCEPTS_STREAMBUF 1
#endif

#define _BACKWARD_BACKWARD_WARNING_H
// Inline headers
// *** this is a generated file. Do not commit, do not edit
#ifndef __IOSELECT_H__
#define __IOSELECT_H__

#define STREAMBUF_NEW_IOSTREAM

#endif
// -*- c++ -*-
#ifndef __EVENT_H__
#define __EVENT_H__

//#include <stddef.h>
#include <vector>

#include "packet.h"
#include <ctime>

// virtual base class for an "event"

const char *get_evt_mnemonic(const int);

/** 
This is the abstract Event class.

It defines all interfaces to deal with Event data. It is a pure
virtual class which does not define any implementation details (it has
only member function defintions, no data members). It is meant to be
subclassed by the classes which define the actual implementation.
*/

//class PHLeanTimeStamp;

#ifndef __CINT__
class WINDOWSEXPORT Event {
#else
class  Event {
#endif

public:
  inline virtual ~Event(){};

  // **** info & debug utils ******
  /** getEvtLength() returns the length of the event raw data
   in longwords (32bit). If you want to copy the event somewhere,
   you can find out whether or not you have enough space left.
  */
  virtual unsigned int getEvtLength() const =0;

  /**
      getEvtType() returns the type of the event. Most of the
   events have the type DATAEVENT, but there are also
   BEGRUNEVENT, SPILLONEVENT, SPILLOFFEVENT, ENDRUNEVENT,
   and so on.
  */
  virtual int getEvtType() const =0;

  /**
   getEvtSequence() returns the number of the event in a 
   particular run. This number is a property of the event. Its run number and the 
   sequence number uniquely indentify an event. It has nothing to do with the position
   of the event in any given data file.
  */
  virtual int getEvtSequence() const =0;
  
  /**
   getRunNumber() returns the number of the run to which this
   event belongs.
  */
  virtual int getRunNumber() const =0;

  /**
   identify will write a short identification message to 
   standard output or to the ostream provided. 
	*/


  virtual void identify(std::ostream& os = std::cout) const = 0;

  /**
   listFrame will write a short descriptor of the frame header to the output stream 
   provided. If id == 0, it will list all frame headers. Otherwise it will list
   the header of the frame in which the packet with id resides. 
  */


  virtual void listFrame(  const int /*id*/=0, std::ostream& /*os*/=std::cout) const {};

  virtual void listHistory(  const int /*id*/=0, std::ostream& /*os*/=std::cout) const {};
  virtual void listError(  const int /*id*/=0, std::ostream& /*os*/=std::cout) const {};

  /**
   getFrameEntry will return a particular entry from the frame header which contains
   the packet with the specidied id. If id == 0, the first frame will be selected.

   keywords to return are:
   
   FRAMELENGTH
   FRAMEMARK
   FRAMEHDRVERSION
   FRAMEHDRLENGTH
   FRAMESTATUS
   FRAMESOURCEID
   FRAMEDATATYPE
   FRAMETYPE
   FRAMEALIGNLENGTH
   FRAMEALIGNMENTWORD[index]

   The last  FRAMEALIGNMENTWORD is an array, you need to call 
   getFrameEntry(id,"FRAMEALIGNMENTWORD",1) to get the index 1, and so on. 
   Asking for a word beyond the number of such words will return 0.
  */

  virtual unsigned int getFrameEntry(  const char * /*what*/, const int /*id*/=0, const int /*index */=0) const { return 0; };


  // ***** packet handling *****
  /**
   getPacket creates and returns a pointer to the
   packet object with the specified packet id (think of it 
   as a house number). 
  */
  virtual Packet* getPacket(const int)=0;

  /**
   This interface allows to override the hitformat of the packet.
   For debugging purposes mostly.
    
  */
  virtual Packet* getPacket(const int, const int /*hitFormat*/)=0;

  /**
    getPacketList returns a simple-minded array of pointers
    to the packets in the given event. It returns the number 
    of packets returned in the array. The second parameter tells
    the packet how long the array is.
  */
  virtual int getPacketList(Packet*[], const int /*length*/) =0;

  /**
    getPacketVector returns a stdd:vector of the packet pointers. 
  */
  virtual std::vector<Packet *> getPacketVector() =0;

  /**
   existPacket returns 1 if such a packet exists in the
   event. 0 else.
  */
  virtual int existPacket (const int /*packetid*/)=0;

  // **** event copying *****
  /**
   the event's raw data are copied to destination. length is the
   size of destination, and nw is the number of words
   actually copied.

   if what = "RAW" then we just copy the payload data without the event header.
  */
  virtual int Copy ( int *destination, const unsigned int /*length*/, int *nw, const char * /*what */="" )=0;


  /**
   getErrorCode() returns a non-zero error code if something in the event
   structure is wrong. Test for 0 to see that the event passes some 
   consistency checks.
  */
  virtual int getErrorCode() {return 0;};


  /**
   getTagWord() returns the bit pattern that the event builder may have put into
   the header for fast event selection purposes. This is mainly used by the
   ET system to distribute events based on its properties.
  */
  virtual unsigned int getTagWord( const int /*i*/ =0) const { return 0;};

  virtual int is_pointer_type() const =0;

  // **** convert event from pointer- to data-based *****
  /**
   converting the Event object means that the actual data it manages 
   are copied to an internal location and are thus safe from being overwritten. 
   For efficiency,  most Event objects initially maintain a pointer to the 
   external raw data only. They are referred to as being pointer-based. 
   If an Event object has already been converted, a second convert operation 
   has no effect.  
  */
  virtual int convert ()=0;

  virtual int getDate() = 0;
  virtual time_t getTime() const = 0;

  virtual void setOriginBuffer(const int n) = 0;
  virtual int getOriginBuffer() const =0;


};

#endif /* __EVENT_H__ */
// -*- c++ -*-
#ifndef __EVENTITERATOR_H__
#define __EVENTITERATOR_H__

#include "Event.h"


/**
   The Eventiterator creates and returns pointers to Event objects.
   This is the virtual base class of a variety of different iterators.
*/
#ifndef __CINT__
class WINDOWSEXPORT Eventiterator {
#else
class  Eventiterator {
#endif
public:
  /// The virtual destructor.
  inline virtual ~Eventiterator(){};

  virtual const char * getIdTag() const =0;

  virtual void identify(std::ostream& os = std::cout) const = 0;


/**
   this member function returns a pointer to the Event object, or
   NULL if there no events left.
*/   
  virtual Event *getNextEvent() = 0;


  /** the next member functions do nothing for most iterators
  except the etEventiterator. It's still easier to give the parent
  class the interfaces
  */

  virtual int *getNextEventData() {return 0;};
  virtual int releaseEventData() {return 0;};

  virtual void setBlockingMode(const int /*mode*/) {};
  virtual int getBlockingMode() const {return 0;};

  virtual void setSelectMode(const int /*mode*/) {};
  virtual int getSelectMode() const {return 0;};

  virtual void setSelectWords(const int , const int , const int , const int  ) {};
  virtual void getSelectWords(int []) const {};

  virtual const char * getCurrentFileName() const { return " "; };

  virtual int  setVerbosity(const int ) { return -1; }; // most iterators don't have the concept
  virtual int  getVerbosity() const { return -1; };


};

#endif /* __EVENTITERATOR_H__ */
// -*- c++ -*-
#ifndef __FILEEVENTITERATOR_H__
#define __FILEEVENTITERATOR_H__

#include <stdio.h>

#include "Eventiterator.h"
#include "Event.h"

#include "gzbuffer.h"
#include "oncsBuffer.h"

/**
   The fileEventiterator reads the event data from a data file on disk. 
   It creates and returns pointers to Event objects. At the end of the file 
   it returns 0 when there are no events left.
*/
#ifndef __CINT__
class WINDOWSEXPORT fileEventiterator : public Eventiterator {
#else
class  fileEventiterator : public Eventiterator {
#endif
public:

  virtual ~fileEventiterator();

  /// This simple constructor just needs the file name of the data file.
  fileEventiterator(const char *filename);

  /**
  This constructor gives you a status so you can learn that the creation
  of the fileEventiterator object was successful. If the status is not 0,
  something went wrong and you should delete the object again.
  */
  fileEventiterator(const char *filename, int &status);

  const char * getIdTag() const;

  virtual void identify(std::ostream& os = std::cout) const;

  virtual const char * getCurrentFileName() const;

/**
   this member function returns a pointer to the Event object, or
   NULL if there are no events left.
*/   
  Event *getNextEvent();

  int  setVerbosity(const int v) 
  { 
    verbosity=v;
    return 0; 
  }; 

  int  getVerbosity() const 
  { 
    return verbosity; 
  };


private:
  int open_file(const char *filename);
  int read_next_buffer();

  char *thefilename;
  int fd;
  
  PHDWORD *bp;
  unsigned int allocatedsize;

  int current_index;
  int last_read_status;
  unsigned int buffer_size;
  buffer *bptr;

  int events_so_far;
  int verbosity;
  int _defunct;
};

#endif /* __FILEEVENTITERATOR_H__ */

#ifndef __LISTEVENTITERATOR_H__
#define __LISTEVENTITERATOR_H__

#include <cstdio>

#ifndef __CINT__
#include <fstream>
#endif

#include "fileEventiterator.h"
#include "Event.h"

#define FEMAXFILENAMELENGTH 256

/**
   The listEventiterator reads the event data from a list of data files
   on disk.  It creates and returns pointers to Event objects. At the
   end of the file it returns 0 when there are no events left.  */

#ifndef __CINT__
class WINDOWSEXPORT listEventiterator : public Eventiterator {
#else
class  listEventiterator : public Eventiterator {
#endif
public:

  virtual ~listEventiterator();

  /// This simple constructor just needs the file name of the file 
  /// that contains, line by line, teh filenames.

  listEventiterator(const char *filename);

  /**
  This constructor gives you a status so you can learn that the creation
  of the fileEventiterator object was successful. If the status is not 0,
  something went wrong and you should delete the object again.
  */
  listEventiterator(const char *filename, int &status);

  const char * getIdTag() const;

  virtual void identify(std::ostream& os = std::cout) const;

  virtual const char * getCurrentFileName() const;

/**
   this member function returns a pointer to the Event object, or
   NULL if there are no events left.
*/   
  Event *getNextEvent();


private:

  Eventiterator * getNextIterator();

  char thefilename[FEMAXFILENAMELENGTH];
  char * listfilename;

#ifndef __CINT__
  std::ifstream *liststream;
#endif

  Eventiterator *it;
  int defunct;
  int finished;

};



#endif /* __LISTEVENTITERATOR_H__ */
// -*- c++ -*-
#ifndef __ONCSEVENTITERATOR_H__
#define __ONCSEVENTITERATOR_H__

#include "Eventiterator.h"
#include "oncsBuffer.h"

#ifndef __CINT__
class WINDOWSEXPORT oncsEventiterator : public Eventiterator {
#else
class  oncsEventiterator : public Eventiterator {
#endif
public:

  virtual ~oncsEventiterator();
  oncsEventiterator(const char *filename);
  oncsEventiterator(const char *filename, int &status);

  const char * getIdTag() const;
  virtual void identify(std::ostream& os = std::cout) const;


  Event *getNextEvent();

private:
  int read_next_buffer();
  
  char * thefilename;
  int fd;
  unsigned int initialbuffer[BUFFERSIZE];
  int *bp;
  int allocatedsize;

  int current_index;
  int last_read_status;
  int buffer_size;
  oncsBuffer *bptr;

};

#endif /* __ONCSEVENTITERATOR_H__ */

// -*- c++ -*-
#ifndef __RCDAQEVENTITERATOR_H__
#define __RCDAQEVENTITERATOR_H__

#include "Eventiterator.h"
#include "buffer.h"

#ifndef __CINT__
#include <string>

#include <arpa/inet.h>
#include <stdio.h>

#endif


#ifndef __CINT__
class WINDOWSEXPORT rcdaqEventiterator : public Eventiterator {
#else
class  rcdaqEventiterator : public Eventiterator {
#endif
public:

  virtual ~rcdaqEventiterator();
  rcdaqEventiterator();
  rcdaqEventiterator(const char *ip );
  rcdaqEventiterator(const char *ip, int &status);

  const char * getIdTag() const;
  virtual void identify(std::ostream& os = std::cout) const;


  Event *getNextEvent();

protected:
  int read_next_buffer();

  int setup(const char *ip, int &status);

  int readn (int fd, char *ptr, int nbytes);
  int writen (int fd, char *ptr, int nbytes);

  
  std::string _theIP;

  int _sockfd;
  PHDWORD initialbuffer[BUFFERSIZE];
  PHDWORD *bp;
  int allocatedsize;

  int current_index;
  int last_read_status;
  int buffer_size;
  buffer *bptr;
  struct sockaddr_in server;
  int _defunct;

};

#endif /* __RCDAQEVENTITERATOR_H__ */

#ifndef __PACKET_H__
#define __PACKET_H__


#include "generalDefinitions.h"

#include "event_io.h"

#define WINDOWSEXPORT

// --------------------------------------------------
// the virtual base base class for all Packets.
// --------------------------------------------------


/**
    This is the abstract packet class.
*/
#ifndef __CINT__
class WINDOWSEXPORT Packet
#else
class  Packet
#endif
  {

   /* @name Description of the interface routines
      these are the interface routines to get at decoded values.
      iValue and rValue return int's and float's, respectively.
      the optional "what" parameter is needed for some devices
      which have more than one type of information, such as the
      MIZAR board, which gets ADC and TDC values.
      iValue(0), iValue(0,""), and iValue("ADC") all
      return what we call "natural data", which means ADC information
      (I decided that), while iValue(0,"TDC") gives you the TDC
      value of channel 0. All Packet classes accept "RAW" as a type,
      directing them to return unprocessed/undecoded packet raw data.

      The array-type interface iValue(const int channel,const int iy)
      is just a convenient call for devices which inherently have
      two-dimensional arrays of data, such as the Hammond flash ADC
      board, where you get 8 channels with 512 time samples each.
      Another example is a group of several, say, LC2249W ADC's read out
      in one packet, so that iValue(0,2) refers to channel 0 of the
      second ADC in the group.
   */




  public:
  /// the virtual destructor
  inline virtual ~Packet() {};

  // **** getting decoded values ****



  /// iValue returns the value of a given channel as an int.
  virtual int    iValue(const int /*channel*/) =0;

  /** with the "what" parameter you can decide which aspect
      of the data you want to see (for devices which have more than one)
  */
  virtual int    iValue(const int /*channel*/, const char * /*what*/) =0;

  /** we have a few recent devices which have one more dimension
      (such as card, time sample, channel)
  */
  virtual int    iValue(const int /*channel*/, const int /*y*/, const char * /*what*/) =0;

  /** this supports devices which are inherently organized as two-dimensional
      data, such as flash ADC's (channel vs time slice)
  */
  virtual int    iValue(const int /*channel*/,const int /*iy*/) =0;

  /** this supports devices  organized as three-dimensional
      data (card vs channel vs time slice )
  */
  virtual int    iValue(const int /*channel*/,const int /*iy*/, const int /*iz*/) =0;

  /** this supports devices  organized as three-dimensional
      data (card vs channel vs time slice, with a "what" selection )
  */
  virtual int    iValue(const int /*channel*/,const int /*iy*/, const int /*iz*/, const char */*what*/) =0;

  /** rValue returns the value of a given channel as a float */
  virtual float  rValue(const int /*channel*/) =0;

  /** dValue returns the value of a given channel as a double */
  virtual double  dValue(const int channel)
  {return rValue(channel);};

  virtual double  dValue(const int channel, const char *what)
  {return iValue(channel, what);};

  virtual double  dValue(const int channel, const int iy)
  {return iValue(channel, iy);};

  /** lValue returns the value of a given channel as a long long */
  virtual long long  lValue(const int channel)
  {return iValue(channel);};

  virtual long long  lValue(const int channel, const char *what)
  {return iValue(channel,what);};

  virtual long long  lValue(const int channel, const int iy)
  {return iValue(channel, iy);};

  virtual long long  lValue(const int channel, const int iy, const char *what)
  {return iValue(channel, iy, what);};

  /** with the "what" parameter you can decide which aspect
      of the data you want to see (for devices which have more than one)
  */
  virtual float  rValue(const int /*channel*/, const char * /*what*/) =0;


  /** this supports devices which are inherently organized as two-dimensional
      data, such as flash ADC's (channel vs time slice)
  */
  virtual float  rValue(const int /*channel*/, const int /*iy*/) =0;

  virtual void * pValue(const int /*channel*/)
  {
    return 0;
  }

  virtual void * pValue(const int /*channel*/, const char * /*what*/)
  {
    return 0;
  }

  virtual void * pValue(const int /*chan*/, const int /*iy*/)
  {
    return 0;
  }




  // *** now getting all the decoded values in one go ***
  // these routines get you all the decoded values into an array
  // of float's or int's. The what parameter has the usual meaning.

  /** getArraylength returns the length of the array needed to store the
      decoded values.
  */
  virtual int    getArraylength(const char * what ="") =0;

  /** fillIntArray and fillFloatArray fill existing (user-supplied) arrays
      with the decoded data
  */
  virtual int    fillIntArray (int destination[],    // the data go here
			       const int /*length*/,      // space we have in destination
			       int * /*nw*/,              // words actually used
			       const char * what="") = 0; // type of data (see above)

  ///  fillFloatArray fills an array of floats
  virtual int    fillFloatArray (float destination[],    // the data go here
				 const int /*length*/,      // space we have in destination
				 int * /*nw*/,              // words actually used
				 const char * what="") = 0; // type of data (see above)

  /** getIntArray and getFloatArray create a new array of the approriate size
      fill it with the decoded values, and return a pointer to the array.
      nw is the length of the array created.
  */
  virtual int*   getIntArray (int * /*nw*/,const char * ="") =0;

  ///  getFloatArray creates and returns an array of floats
  virtual float* getFloatArray (int * /*nw*/,const char * ="") =0;


  /// find out what type (pointer- or data based) packet object we have
  virtual int is_pointer_type() const = 0;

  /// convert from pointer- to data based object, if it is already data-based, do nothing.
  virtual int convert() =0;


  // access to envelope information:

  /** getLength() returns the length of the raw packet data. If you were to copy the data
      somewhere, the destination must be able to hold as many words.
  */
  virtual int   getLength() const = 0;

  //  virtual int   getType() const = 0;
  //  virtual int   getDecoding() const = 0;

  // some more header fields which are not yet implemented, marked "//*"
  //* virtual int   gethdrVersion() const = 0; // Version of header definition
  //* virtual int   getHdrLength() const = 0;     // inclusive of alignment data
  virtual int   getStatus() const = 0;	       // returns 0 for all ok 

  virtual int   getErrorLength() const = 0;    // Length of error block in Dwords

  /// get the length of the debug block
  virtual int	getDebugLength() const = 0;// Length of debug block in Dwords

  /// get the packet identifier
  virtual int	getIdentifier() const = 0; // Identifier

  //* virtual int	getEndianism() const = 0;  // Big/little endian indicator

  /// get the number of padding units in the packet data.
  virtual int	getPadding() const = 0;    // number of padding units

  /// get the structure of the packet data; unformatted, hitlist, etc.
  virtual int	getStructure() const = 0;  // Structure of packet

  //* virtual int	getWordSize() const = 0;   // "Word" size used to store packet data
  //* virtual int	getAddrLength() const = 0; // number of bytes used for channel address
  //* virtual int	getHitLength() const = 0;  // Length of a single "hit" in bytes

  /// get the hit format; in case of unformatted get the encoding scheme.
  virtual int	getHitFormat() const = 0;  // Format of a single hit
  //* virtual int	getNumEntries() const = 0; // Number of "objects" stored in packet

  /// get what the name says...
  virtual int	getDataLength() const = 0;  // Format of a single hit

  // debugging-type information
  // identify will write a short identification message to
  // standard output or to the iOSTREAM provided. Nice to see.


  //#ifdef LVL2_WINNT
  ///see below for comments
  //  virtual void  identify( std::OSTREAM&os =std::COUT ) const = 0;
  //  virtual void  fullIdentify( std::OSTREAM& os =std::COUT ) const
  //    { identify(os);};
  //  virtual void  dump ( std::OSTREAM& =std::COUT )  = 0;
  //  virtual void  gdump (const int how = EVT_HEXADECIMAL, std::OSTREAM& =std::COUT) const = 0;


  /// write an identification message to the supplied OSTREAM.

  virtual void identify(std::ostream& os = std::cout) const = 0;

  /// set a new packet identifier
  virtual int	setIdentifier(const int /*newid*/) = 0; // Identifier

  /// write an indepth identification message to the supplied OSTREAM.
  virtual void fullIdentify(std::ostream& os = std::cout) const
  { identify(os);};

  /**dump (either to standard output or the specified OSTREAM)
     the packet's data in some packet-specific way.
  */

  virtual void dump(std::ostream& os = std::cout)  = 0;


  /** Since dump()
      requires the packet data to be consistent, gdump ("generic" dump)
      dumps the data without making assumptions about the integrity of
      the packet data. For many Packet classes dump just calls gdump
      because it is good enough. The "how" parameter for gdump specifies
      decimal, octal, or hexadecimal (0,1,2) dump.
  */

  virtual void gdump(const int how = EVT_HEXADECIMAL, std::ostream& os = std::cout) const  = 0;

  /** This functiion returns the status of the DCM transfer checksum.
      0  = cannot be calculated for this packet
      1  = ok
      -1  = fail

  */
  virtual int getCheckSumStatus() const { return 0;};
  virtual int   copyMe(int [],  const int /*maxlength*/) const { return 0;};

  virtual int setInternalParameter ( const int p1=0, const int p2=0, const char *what = "") = 0;


  };

// some structures for the LVL triggers

// emcal

struct emcChannelLongList
{
  int channel;
  int time;
  int highpost;
  int lowpost;
  int highpre;
  int lowpre;
};


#define RICH_TIME 0
#define RICH_POSTSAMPLE 1
#define RICH_PRESAMPLE 2

struct richChannelList
{
  int channel;
  int time;
  int post;
  int pre;
};

struct emcChannelShortList
{
  int channel;
  int gain;  // 0 = low, else high
  int time;
  int post;
  int pre;

};

struct tecChannelList
{
  int channel;
  int time;
  int value;

};


#endif /* __PACKET_H__ */
// -*- c++ -*-
#ifndef __TESTEVENTITERATOR_H__
#define __TESTEVENTITERATOR_H__

#include "Eventiterator.h"

#include "simpleRandom.h"

/** The testEventiterator creates Event objects with known properties
   from scratch. The events contain 1 frame and 3 packets. The first 2
   packets with id's 1001 and 1002 look like ADC's whose channel i
   contains the value i, which can be used to verify that a certain
   channel actually ends up where it should. The third packet 1003
   has four channels which give values according to gaussian distributions 
   with RMS 10,100,1000, and 10000, respectively. 
*/
#ifndef __CINT__
class WINDOWSEXPORT testEventiterator : public Eventiterator {
#else
class  testEventiterator : public Eventiterator {
#endif
public:
  /// The constructor.
  testEventiterator();
  /// The destructor.
  ~testEventiterator();

  const char * getIdTag() const;
  //#ifdef WIN32
  virtual void identify(std::ostream& os = std::cout) const;


/**
   this member function creates returns a pointer to the Event object
   (this iterator does not become empty).
*/   
  Event *getNextEvent();

private:
#ifndef WIN32
  simpleRandom *R;
#endif
  int current_event;
  
  // these are the spots where we change anything per event later
  int *e_sequence;
  int *e_time;
  int *e_1003values;
  
  // we get somememory to generate the event
  int e_data[256]; 
  
};

#endif /* __TESTEVENTITERATOR_H__ */

#ifndef  __EVENTRECEIVERCLIENT_H__
#define  __EVENTRECEIVERCLIENT_H__


#include <string>
#include <netinet/in.h>


class Event;

class eventReceiverClient
{
 public:
  eventReceiverClient( const std::string hostname, const int flags=0, const int port =8080);
  ~eventReceiverClient();

  Event *getEvent(const int eventnumber, const int flag = 0);
  void setVerbosity(const int v) { _verbosity = v;};
  int getVerbosity() const { return _verbosity;};
  int getStatus() const { return _broken;};
  int setUserTimeout(const int t) { _user_timeout = t; return 0;};
  int getUserTimeout() const { return _user_timeout;};
  int hadTimeout() const { return _had_timeout;};
  
 protected:

  int _sockfd;
  int _flags;
  int _broken;
  int _verbosity;
  int _timeout;
  int _user_timeout;
  int _had_timeout;
  int _port;
  
  std::string _hostname;
  struct sockaddr_in _serveraddr; 

  struct hostent *_phost;


  
};

#endif

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"Event", payloadCode, "@",
"Eventiterator", payloadCode, "@",
"Packet", payloadCode, "@",
"eventReceiverClient", payloadCode, "@",
"fileEventiterator", payloadCode, "@",
"listEventiterator", payloadCode, "@",
"oncsEventiterator", payloadCode, "@",
"rcdaqEventiterator", payloadCode, "@",
"testEventiterator", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("event_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_event_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_event_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_event_Dict() {
  TriggerDictionaryInitialization_event_Dict_Impl();
}
