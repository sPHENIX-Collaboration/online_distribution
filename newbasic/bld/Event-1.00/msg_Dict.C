// Do NOT change. Changes will be lost next time file is generated

#define R__DICTIONARY_FILENAME msg_Dict
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
#include "../msg_control.h"
#include "../date_filter_msg_buffer.h"
#include "../filter_msg_buffer.h"
#include "../remote_msg_buffer.h"

// Header files passed via #pragma extra_include

// The generated code does not explicitly qualify STL entities
namespace std {} using namespace std;

namespace ROOT {
   static TClass *msg_control_Dictionary();
   static void msg_control_TClassManip(TClass*);
   static void *new_msg_control(void *p = nullptr);
   static void *newArray_msg_control(Long_t size, void *p);
   static void delete_msg_control(void *p);
   static void deleteArray_msg_control(void *p);
   static void destruct_msg_control(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::msg_control*)
   {
      ::msg_control *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::msg_control));
      static ::ROOT::TGenericClassInfo 
         instance("msg_control", "", 118,
                  typeid(::msg_control), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &msg_control_Dictionary, isa_proxy, 3,
                  sizeof(::msg_control) );
      instance.SetNew(&new_msg_control);
      instance.SetNewArray(&newArray_msg_control);
      instance.SetDelete(&delete_msg_control);
      instance.SetDeleteArray(&deleteArray_msg_control);
      instance.SetDestructor(&destruct_msg_control);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::msg_control*)
   {
      return GenerateInitInstanceLocal((::msg_control*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::msg_control*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *msg_control_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::msg_control*)nullptr)->GetClass();
      msg_control_TClassManip(theClass);
   return theClass;
   }

   static void msg_control_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *msg_buffer_Dictionary();
   static void msg_buffer_TClassManip(TClass*);
   static void *new_msg_buffer(void *p = nullptr);
   static void *newArray_msg_buffer(Long_t size, void *p);
   static void delete_msg_buffer(void *p);
   static void deleteArray_msg_buffer(void *p);
   static void destruct_msg_buffer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::msg_buffer*)
   {
      ::msg_buffer *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::msg_buffer));
      static ::ROOT::TGenericClassInfo 
         instance("msg_buffer", "msg_buffer.h", 67,
                  typeid(::msg_buffer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &msg_buffer_Dictionary, isa_proxy, 3,
                  sizeof(::msg_buffer) );
      instance.SetNew(&new_msg_buffer);
      instance.SetNewArray(&newArray_msg_buffer);
      instance.SetDelete(&delete_msg_buffer);
      instance.SetDeleteArray(&deleteArray_msg_buffer);
      instance.SetDestructor(&destruct_msg_buffer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::msg_buffer*)
   {
      return GenerateInitInstanceLocal((::msg_buffer*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::msg_buffer*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *msg_buffer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::msg_buffer*)nullptr)->GetClass();
      msg_buffer_TClassManip(theClass);
   return theClass;
   }

   static void msg_buffer_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *filter_msg_buffer_Dictionary();
   static void filter_msg_buffer_TClassManip(TClass*);
   static void *new_filter_msg_buffer(void *p = nullptr);
   static void *newArray_filter_msg_buffer(Long_t size, void *p);
   static void delete_filter_msg_buffer(void *p);
   static void deleteArray_filter_msg_buffer(void *p);
   static void destruct_filter_msg_buffer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::filter_msg_buffer*)
   {
      ::filter_msg_buffer *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::filter_msg_buffer));
      static ::ROOT::TGenericClassInfo 
         instance("filter_msg_buffer", "filter_msg_buffer.h", 17,
                  typeid(::filter_msg_buffer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &filter_msg_buffer_Dictionary, isa_proxy, 3,
                  sizeof(::filter_msg_buffer) );
      instance.SetNew(&new_filter_msg_buffer);
      instance.SetNewArray(&newArray_filter_msg_buffer);
      instance.SetDelete(&delete_filter_msg_buffer);
      instance.SetDeleteArray(&deleteArray_filter_msg_buffer);
      instance.SetDestructor(&destruct_filter_msg_buffer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::filter_msg_buffer*)
   {
      return GenerateInitInstanceLocal((::filter_msg_buffer*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::filter_msg_buffer*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *filter_msg_buffer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::filter_msg_buffer*)nullptr)->GetClass();
      filter_msg_buffer_TClassManip(theClass);
   return theClass;
   }

   static void filter_msg_buffer_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *date_filter_msg_buffer_Dictionary();
   static void date_filter_msg_buffer_TClassManip(TClass*);
   static void *new_date_filter_msg_buffer(void *p = nullptr);
   static void *newArray_date_filter_msg_buffer(Long_t size, void *p);
   static void delete_date_filter_msg_buffer(void *p);
   static void deleteArray_date_filter_msg_buffer(void *p);
   static void destruct_date_filter_msg_buffer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::date_filter_msg_buffer*)
   {
      ::date_filter_msg_buffer *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::date_filter_msg_buffer));
      static ::ROOT::TGenericClassInfo 
         instance("date_filter_msg_buffer", "", 177,
                  typeid(::date_filter_msg_buffer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &date_filter_msg_buffer_Dictionary, isa_proxy, 3,
                  sizeof(::date_filter_msg_buffer) );
      instance.SetNew(&new_date_filter_msg_buffer);
      instance.SetNewArray(&newArray_date_filter_msg_buffer);
      instance.SetDelete(&delete_date_filter_msg_buffer);
      instance.SetDeleteArray(&deleteArray_date_filter_msg_buffer);
      instance.SetDestructor(&destruct_date_filter_msg_buffer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::date_filter_msg_buffer*)
   {
      return GenerateInitInstanceLocal((::date_filter_msg_buffer*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::date_filter_msg_buffer*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *date_filter_msg_buffer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::date_filter_msg_buffer*)nullptr)->GetClass();
      date_filter_msg_buffer_TClassManip(theClass);
   return theClass;
   }

   static void date_filter_msg_buffer_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   static TClass *remote_msg_buffer_Dictionary();
   static void remote_msg_buffer_TClassManip(TClass*);
   static void *new_remote_msg_buffer(void *p = nullptr);
   static void *newArray_remote_msg_buffer(Long_t size, void *p);
   static void delete_remote_msg_buffer(void *p);
   static void deleteArray_remote_msg_buffer(void *p);
   static void destruct_remote_msg_buffer(void *p);

   // Function generating the singleton type initializer
   static TGenericClassInfo *GenerateInitInstanceLocal(const ::remote_msg_buffer*)
   {
      ::remote_msg_buffer *ptr = nullptr;
      static ::TVirtualIsAProxy* isa_proxy = new ::TIsAProxy(typeid(::remote_msg_buffer));
      static ::ROOT::TGenericClassInfo 
         instance("remote_msg_buffer", "", 327,
                  typeid(::remote_msg_buffer), ::ROOT::Internal::DefineBehavior(ptr, ptr),
                  &remote_msg_buffer_Dictionary, isa_proxy, 3,
                  sizeof(::remote_msg_buffer) );
      instance.SetNew(&new_remote_msg_buffer);
      instance.SetNewArray(&newArray_remote_msg_buffer);
      instance.SetDelete(&delete_remote_msg_buffer);
      instance.SetDeleteArray(&deleteArray_remote_msg_buffer);
      instance.SetDestructor(&destruct_remote_msg_buffer);
      return &instance;
   }
   TGenericClassInfo *GenerateInitInstance(const ::remote_msg_buffer*)
   {
      return GenerateInitInstanceLocal((::remote_msg_buffer*)nullptr);
   }
   // Static variable to force the class initialization
   static ::ROOT::TGenericClassInfo *_R__UNIQUE_DICT_(Init) = GenerateInitInstanceLocal((const ::remote_msg_buffer*)nullptr); R__UseDummy(_R__UNIQUE_DICT_(Init));

   // Dictionary for non-ClassDef classes
   static TClass *remote_msg_buffer_Dictionary() {
      TClass* theClass =::ROOT::GenerateInitInstanceLocal((const ::remote_msg_buffer*)nullptr)->GetClass();
      remote_msg_buffer_TClassManip(theClass);
   return theClass;
   }

   static void remote_msg_buffer_TClassManip(TClass* ){
   }

} // end of namespace ROOT

namespace ROOT {
   // Wrappers around operator new
   static void *new_msg_control(void *p) {
      return  p ? new(p) ::msg_control : new ::msg_control;
   }
   static void *newArray_msg_control(Long_t nElements, void *p) {
      return p ? new(p) ::msg_control[nElements] : new ::msg_control[nElements];
   }
   // Wrapper around operator delete
   static void delete_msg_control(void *p) {
      delete ((::msg_control*)p);
   }
   static void deleteArray_msg_control(void *p) {
      delete [] ((::msg_control*)p);
   }
   static void destruct_msg_control(void *p) {
      typedef ::msg_control current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::msg_control

namespace ROOT {
   // Wrappers around operator new
   static void *new_msg_buffer(void *p) {
      return  p ? new(p) ::msg_buffer : new ::msg_buffer;
   }
   static void *newArray_msg_buffer(Long_t nElements, void *p) {
      return p ? new(p) ::msg_buffer[nElements] : new ::msg_buffer[nElements];
   }
   // Wrapper around operator delete
   static void delete_msg_buffer(void *p) {
      delete ((::msg_buffer*)p);
   }
   static void deleteArray_msg_buffer(void *p) {
      delete [] ((::msg_buffer*)p);
   }
   static void destruct_msg_buffer(void *p) {
      typedef ::msg_buffer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::msg_buffer

namespace ROOT {
   // Wrappers around operator new
   static void *new_filter_msg_buffer(void *p) {
      return  p ? new(p) ::filter_msg_buffer : new ::filter_msg_buffer;
   }
   static void *newArray_filter_msg_buffer(Long_t nElements, void *p) {
      return p ? new(p) ::filter_msg_buffer[nElements] : new ::filter_msg_buffer[nElements];
   }
   // Wrapper around operator delete
   static void delete_filter_msg_buffer(void *p) {
      delete ((::filter_msg_buffer*)p);
   }
   static void deleteArray_filter_msg_buffer(void *p) {
      delete [] ((::filter_msg_buffer*)p);
   }
   static void destruct_filter_msg_buffer(void *p) {
      typedef ::filter_msg_buffer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::filter_msg_buffer

namespace ROOT {
   // Wrappers around operator new
   static void *new_date_filter_msg_buffer(void *p) {
      return  p ? new(p) ::date_filter_msg_buffer : new ::date_filter_msg_buffer;
   }
   static void *newArray_date_filter_msg_buffer(Long_t nElements, void *p) {
      return p ? new(p) ::date_filter_msg_buffer[nElements] : new ::date_filter_msg_buffer[nElements];
   }
   // Wrapper around operator delete
   static void delete_date_filter_msg_buffer(void *p) {
      delete ((::date_filter_msg_buffer*)p);
   }
   static void deleteArray_date_filter_msg_buffer(void *p) {
      delete [] ((::date_filter_msg_buffer*)p);
   }
   static void destruct_date_filter_msg_buffer(void *p) {
      typedef ::date_filter_msg_buffer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::date_filter_msg_buffer

namespace ROOT {
   // Wrappers around operator new
   static void *new_remote_msg_buffer(void *p) {
      return  p ? new(p) ::remote_msg_buffer : new ::remote_msg_buffer;
   }
   static void *newArray_remote_msg_buffer(Long_t nElements, void *p) {
      return p ? new(p) ::remote_msg_buffer[nElements] : new ::remote_msg_buffer[nElements];
   }
   // Wrapper around operator delete
   static void delete_remote_msg_buffer(void *p) {
      delete ((::remote_msg_buffer*)p);
   }
   static void deleteArray_remote_msg_buffer(void *p) {
      delete [] ((::remote_msg_buffer*)p);
   }
   static void destruct_remote_msg_buffer(void *p) {
      typedef ::remote_msg_buffer current_t;
      ((current_t*)p)->~current_t();
   }
} // end of namespace ROOT for class ::remote_msg_buffer

namespace {
  void TriggerDictionaryInitialization_msg_Dict_Impl() {
    static const char* headers[] = {
"0",
nullptr
    };
    static const char* includePaths[] = {
nullptr
    };
    static const char* fwdDeclCode = R"DICTFWDDCLS(
#line 1 "msg_Dict dictionary forward declarations' payload"
#pragma clang diagnostic ignored "-Wkeyword-compat"
#pragma clang diagnostic ignored "-Wignored-attributes"
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
extern int __Cling_AutoLoading_Map;
class msg_control;
class __attribute__((annotate("$clingAutoload$msg_buffer.h")))  __attribute__((annotate("$clingAutoload$filter_msg_buffer.h")))  msg_buffer;
class __attribute__((annotate("$clingAutoload$filter_msg_buffer.h")))  filter_msg_buffer;
class date_filter_msg_buffer;
class remote_msg_buffer;
)DICTFWDDCLS";
    static const char* payloadCode = R"DICTPAYLOAD(
#line 1 "msg_Dict dictionary payload"

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
#ifndef __MSG_CONTROL_H__
#define __MSG_CONTROL_H__

#include "msg_profile.h"
#include "event_io.h"
#include <string.h>

/** 
Objects of the msg\_control class maintain the profiling information 
for your messages. They add a few tokens to the output stream which can be
parsed by the msg\_buffer's routines, which so learns about the profile. 
Use it, for example, like this:
\begin{verbatim}
  msg_control *mvd_warning = new msg_control(MSG_TYPE_CODEDEBUG, 
					     MSG_SOURCE_MVD, 
					     MSG_SEV_WARNING);
  COUT <<  *mvd_info    << " this is a info message from MVD" << endl;
\end{verbatim}

The mvd\_info's "<<" operator adds now the profiling information to the message,
which is then parsed again and stripped off by our custom streambuf.

If we do not have a custom streambuf, the msg\_control objects maintain silence.
They have to be activated by one of our custom streambufs first.

* @VERSION: 2.0
* ejd add string for component source of message
*/

class msg_control
{
   friend class msg_buffer;

  friend OSTREAM& operator<< (OSTREAM& , msg_control &);

protected:

  int msg_type;
  int msg_source;
  int msg_severity;
  int storedseverity;
  char * msg_sourcecomponent;


  int activate();
  int deactivate();

public:


  msg_control(const int mtype = MSG_TYPE_DEFAULT
	      , const int source = MSG_SOURCE_DEFAULT
	      , const int severity = MSG_SEV_DEFAULT
	      , const char *sourcecomponent = "ONLINE");

  virtual ~msg_control();

  virtual void set_severity(const int severity); 
  virtual int get_severity() const { return msg_severity;};
  virtual void reset_severity() {msg_severity = storedseverity; };
  // set the message source id
  virtual void set_source(const int source) {msg_source = source; };
  virtual int  get_source() const { return msg_source; };
  virtual void set_sourcecomponent(const char * msgsourcecomponent = "ONLINE");
  virtual const char * get_sourcecomponent(){ return msg_sourcecomponent; };

  static int xx_active;

};

#endif /* __MSG_CONTROL_H__ */
#ifndef __DATE_FILTER_MSG_BUFFER_CC__
#define __DATE_FILTER_MSG_BUFFER_CC__



#include "filter_msg_buffer.h"

#define ON 1
#define OFF 0

/** This is the "date\_filter" msg\_buffer class which allows you to
filter messages based on their profile. Itacts much like the
filter\_msg\_buffer, but it also prepends a date tag to messages which have
a profile.
*/

class date_filter_msg_buffer : public   filter_msg_buffer {
  

public:

  /** The msglen parameter specifies the initial length of the 
      message string which is kept internally. If you exceed the length, 
      it is automautically extended.
  */
  date_filter_msg_buffer (const int msglen=256);

  /** This constructor defines a custom-sized matrix of 
      type/source/severities.  */
  date_filter_msg_buffer (const int type_max, const int source_max, 
		     const int sev_max, const int msglen=256);

  /// the virtual destructor
  virtual ~date_filter_msg_buffer();


  virtual int sync ();

};

#endif /* __DATE_FILTER_MSG_BUFFER_CC__ */



#ifndef __FILTER_MSG_BUFFER_H__
#define __FILTER_MSG_BUFFER_H__



#include "msg_buffer.h"

#define ON 1
#define OFF 0

/** This is the "filter" msg\_buffer class which allows you to filter
messages based on their profile. Its default behavior is to let all messages pass. 
You can use the member functions to tailor the kind of messages filtered and passed on.

*/

class filter_msg_buffer : public   msg_buffer {

#ifndef __CINT__
protected:
  STREAMBUF * original_streambuf;
  int ***state;
  int msg_type_max;
  int msg_source_max;
  int msg_sev_max;
#endif


  //  int state[MSG_TYPE_MAX][MSG_SOURCE_MAX][MSG_SEV_MAX];

public:

  /** The msglen parameter specifies the initial length of the 
      message string which is kept internally. If you exceed the length, 
      it is automautically extended.
  */
  filter_msg_buffer (const int msglen=256);

  /** This constructor defines a custom-sized matrix of 
      type/source/severities.  */
  filter_msg_buffer (const int type_max, const int source_max, 
		     const int sev_max, const int msglen=256);

  /// the virtual destructor
  virtual ~filter_msg_buffer();

  /// the sync function overrides the streambuf's sync function

  // mlp -- the pubsync is what's needed for the new
  // iostream libraries - sync will no longer be a public method. 
  // for now we leave it as it was.


  virtual int sync ();


  /** the set function defines (ON or OFF) which messages are passed and
      which ones are filtered. This gives very fine-grained control. There 
      are other functions which give you more global control.
  */


  virtual int set (const int msg_type, 
	  const int msg_source,
	  const int msg_severity,
	  const int value = OFF);

  /// Globally set all messages below a certain severity threshold to ON or OFF 
  virtual int set_severity_below_threshold (const int threshold, 
					    const int value =OFF); 

  /// Globally switch ON or OFF all messages of a certain type
  virtual int set_type (const int type, 
			const int value =OFF); 

  /// Globally switch ON or OFF all messages from a certain source
  virtual int set_source (const int source, 
			const int value =OFF); 

  /// Globally switch all messages off
  virtual int all_off();

  /// Globally switch all messages on
  virtual int all_on();

};


#endif /* __FILTER_MSG_BUFFER_H__ */
#ifndef __REMOTE_MSG_BUFFER_H__
#define __REMOTE_MSG_BUFFER_H__



#include "msg_buffer.h"

#ifndef __CINT__

#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#endif



/** This is the "filter" msg\_buffer class which allows you to filter
messages based on their profile. Its default behavior is to let all messages pass. 
You can use the member functions to tailor the kind of messages filtered and passed on.

*/

class remote_msg_buffer : public   msg_buffer {

protected:

#ifndef __CINT__
  STREAMBUF * original_streambuf;
  int ThePort;
  char *TheHost;

  int sockfd;
  struct sockaddr_in server_addr;
  struct hostent *p_host;

  void send_string(const char *x, const int len);
  int writen (int fd, const char *ptr, int nbytes);
#endif


public:


  /** The msglen parameter specifies the initial length of the 
      message string which is kept internally. If you exceed the length, 
      it is automautically extended.
  */
  remote_msg_buffer (const char *host = "va010.phenix.bnl.gov",
		     const int port = 8400, const int msglen=256);

  /// the virtual destructor
  virtual ~remote_msg_buffer();

  /// the sync function overrides the streambuf's sync function

  // mlp -- the pubsync is what's needed for the new
  // iostream libraries - sync will no longer be a public method. 
  // for now we leave it as it was.

#ifdef STREAMBUF_NEW_IOSTREAM
  virtual int pubsync ();
#else
  virtual int sync ();
#endif


};


#endif /* __REMOTE_MSG_BUFFER_H__ */

#undef  _BACKWARD_BACKWARD_WARNING_H
)DICTPAYLOAD";
    static const char* classesHeaders[] = {
"date_filter_msg_buffer", payloadCode, "@",
"filter_msg_buffer", payloadCode, "@",
"msg_buffer", payloadCode, "@",
"msg_control", payloadCode, "@",
"remote_msg_buffer", payloadCode, "@",
nullptr
};
    static bool isInitialized = false;
    if (!isInitialized) {
      TROOT::RegisterModule("msg_Dict",
        headers, includePaths, payloadCode, fwdDeclCode,
        TriggerDictionaryInitialization_msg_Dict_Impl, {}, classesHeaders, /*hasCxxModule*/false);
      isInitialized = true;
    }
  }
  static struct DictInit {
    DictInit() {
      TriggerDictionaryInitialization_msg_Dict_Impl();
    }
  } __TheDictionaryInitializer;
}
void TriggerDictionaryInitialization_msg_Dict() {
  TriggerDictionaryInitialization_msg_Dict_Impl();
}
