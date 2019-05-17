#include <Event/testEventiterator.h>
#include <Event/fileEventiterator.h>
#include <Event/listEventiterator.h>
#include <Event/oncsEventiterator.h>
#include <Event/rcdaqEventiterator.h>

#include <string>
#include <iostream>
#include <sstream>
#include <unistd.h>

#include "pmonitor.h"
#include "pmonstate.h"
//#include "pmondisplay.h"
//#include "pmongui.h"

#include <TThread.h>
#include <TGClient.h>
#include <pMutex.h>
#include <pthread.h>

#ifdef HAVE_FROG
#include "FROG.h"
#endif

static Eventiterator *theIterator = 0;
//static pmongui *theGui = 0;

int stopcondition;
int displaystopcondition;

static pmonstate theState;

static TThread *main_thread = 0;
//static TThread *display_thread = 0;

static pMutex TM;
static pMutex runningTM;

using namespace std;

pthread_mutex_t pmonmutex;

int pstatus()
{
  cout << theState;
  if ( theState.streamOpened() )
    {
      theIterator->identify();
    }
  else
    cout << endl;

  return 0;
}

int pexitstatus()
{
  return theState.getloopStatus();
}


//-----------------------------------------

int ptestopen()
{

  if ( theState.streamOpened() )
    {
      cout << "Stream in already open" << endl;
      return 1;
    }

  theIterator = new testEventiterator();
  theState.setTestOpened();
  //  if (theGui)
  //   theGui->setStreamLabel(theIterator->getIdTag() );
  return 0;
}

//-----------------------------------------

int pfileopen(const char * filename)
{
  int status = 0;
  if ( theState.streamOpened() )
    {
      cout << "Stream in already open" << endl;
      return 1;
    }

#ifdef HAVE_FROG
  FROG f;
  theIterator = new fileEventiterator(f.location(filename), status);
#else
  theIterator = new fileEventiterator( filename, status);
#endif

  if (status)
    {
      delete theIterator;
      theIterator = 0;
      cout << "Could not open file " << filename << endl;
      return 2;
    }
  theState.setFileOpened(filename);
  //  if (theGui)
  //  theGui->setStreamLabel(theIterator->getIdTag() );
  return 0;
}
//-----------------------------------------

int poncsopen(const char * filename)
{
  int status = 0;
  if ( theState.streamOpened() )
    {
      cout << "Stream in already open" << endl;
      return 1;
    }
  theIterator = new oncsEventiterator(filename, status);
  if (status)
    {
      delete theIterator;
      theIterator = 0;
      cout << "Could not open file " << filename << endl;
      return 2;
    }
  theState.setFileOpened(filename);
  //  if (theGui)
  //  theGui->setStreamLabel(theIterator->getIdTag() );
  return 0;
}
//-----------------------------------------


int rcdaqopen(const char * ip)
{
  int status = 0;
  if ( theState.streamOpened() )
    {
      cout << "Stream in already open" << endl;
      return 1;
    }

  std::string host;

  if ( ip)
    {
      host = ip;
    }
  else
    {
      if ( getenv("RCDAQHOST")  )
	{
	  host = getenv("RCDAQHOST");
	}
      else
	{
	  host = "localhost";
	}
    }
  
  theIterator = new rcdaqEventiterator(host.c_str(), status);
  if (status)
    {
      delete theIterator;
      theIterator = 0;
      cout << "Could not connect to server " << host.c_str() << endl;
      return 2;
    }
  theState.setRCDAQOpened(host.c_str());
  //if (theGui)
  //  theGui->setStreamLabel(theIterator->getIdTag() );
  return 0;
}
//-----------------------------------------

int plistopen(const char * filename)
{
  int status = 0;
  if ( theState.streamOpened() )
    {
      cout << "Stream in already open" << endl;
      return 1;
    }
  theIterator = new listEventiterator(filename, status);
  if (status)
    {
      delete theIterator;
      theIterator = 0;
      cout << "Could not open list " << filename << endl;
      return 2;
    }
  theState.setFileOpened(filename);
  //if (theGui)
  //  theGui->setStreamLabel(theIterator->getIdTag() );
  return 0;
}

int pidentify(const int n)
{

  if (n > 0)
    {
      theState.setIdentifyFlag(n);
      return 0;
    }
  else if (n == 0)
    {
      theState.setIdentifyFlag();
      return 0;
    }
  else
    {
      cout << "must be a positive number" << endl;
      return 1;
    }
}

int pidentify()
{
  theState.setIdentifyFlag(1);
  return 0;
}

int pclearidentify()
{
  theState.setIdentifyFlag(0);
  return 0;
}


//-----------------------------------------


int totalevents;
void pprocess (void * ptr)
{
  theState.setloopStatus(0); //clear loop status field
  int nevents = totalevents;
  stopcondition = 0;

  runningTM.Lock();

  theState.setRunning();
  theState.clearNoevt();
  Event *evt;
  evt = theIterator->getNextEvent();
  int ncount = 0;

  // if (theGui)
  //  theGui->setStatusLabel("Running");
  while (evt )
    {
      // if (theGui)
      //  theGui->setEvtnrLabel(evt->getEvtSequence() );
      if (theState.isIdentifyFlag())
        evt->identify();
      theState.incrementNoevt();
      TM.Lock();
      int istat = process_event(evt);
      theState.setloopStatus(istat);
      delete evt;
      TM.Release();
      if (istat || stopcondition)
        {
          runningTM.Release();
          theState.clearRunning();
          //if (theGui)
          //  theGui->setStatusLabel("Stopped");
          return ;
        }
      if ( ( nevents > 0 && ++ncount >= nevents) )
        break;
      evt = theIterator->getNextEvent();
    }
  theState.clearRunning();
  if (!evt)
    pclose();
  //if (theGui)
  //  theGui->setEvtnrLabel(0);
  //if (theGui)
  //  theGui->setStatusLabel("Stopped");
  runningTM.Release();
  return ;
}


//-----------------------------------------

int pstart (const int nevents)
{
  if (! theState.streamOpened() )
    {
      cout << "No input Stream open" << endl;
      return 1;
    }

  if (theState.isRunning() )
    {
      cout << "Already running" << endl;
      return 1;
    }

  totalevents = nevents;

  if (!main_thread)
    main_thread = new TThread ( pprocess);
  main_thread->Run();
  return 0;
}

int pstart ()
{
  return pstart(0);
}

void prun ()
{
  prun(0);
}

void prun (const int nevents)
{
  if (! theState.streamOpened() )
    {
      cout << "No input Stream open" << endl;
      return ;
    }

  stopcondition = 0;

  theState.setRunning();
  theState.clearNoevt();
  Event *evt;
  evt = theIterator->getNextEvent();
  int ncount = 0;

  while (evt )
    {
      if (theState.isIdentifyFlag())
        evt->identify();
      theState.incrementNoevt();
      int istat = process_event(evt);
      theState.setloopStatus(istat);
      delete evt;
      if (istat || stopcondition)
        {
          theState.clearRunning();
          return ;
        }
      if ( ( nevents > 0 && ++ncount >= nevents) )
        break;
      evt = theIterator->getNextEvent();
    }
  theState.clearRunning();
  if (! evt)
    pclose();
  return ;
}

//-----------------------------------------


int pstop()
{
  if (! theState.isRunning() )
    {
      cout << "Not running" << endl;
      return 1;
    }

  stopcondition = 1;
  //if (theGui)
  //  theGui->setStatusLabel("Stopped");
  return 0;
}

//-----------------------------------------

int pclose()
{

  if (! theState.streamOpened() )
    {
      cout << "No input Stream open" << endl;
      return 1;
    }

  if ( theState.isRunning() )
    {
      cout << "Still running, stop first" << endl;
      return 1;
    }
  delete theIterator;
  theIterator = 0;
  theState.clearOpened();
  //if (theGui)
  //  theGui->setStreamLabel("No stream open");

  return 0;
}

int mylock = 0;
int plock()
{
  if (!mylock)
    {
      pthread_mutex_init(&pmonmutex, NULL);
    }
  if (mylock)
    {
      cout << "Locked already" << endl;
      return 0;
    }
  pthread_mutex_lock(&pmonmutex);
  mylock = 1;
  return 0;
}

int prelease()
{
  if (!mylock)
    {
      cout << "not locked" << endl;
      return 0;
    }
  else
    {
      pthread_mutex_unlock(&pmonmutex);
    }
  mylock = 0;
  return 0;
}

int pwait()
{
  runningTM.Lock();
  return runningTM.Release();
}



// int timeinterval;
// void pdisplay (void * ptr)
// {


//   char string[40];
//   TThread::Lock();
//   pmondisplay *pm = new pmondisplay();
//   TThread::UnLock();

//   int seconds = timeinterval;
//   displaystopcondition = 0;

//   while ( displaystopcondition == 0 )
//     {
//       if (theState.streamOpened() )
//         {
//           char *c = string;
//           for (int i = 0; i++ < 40; *c++ = 0)
//             ;
//           ostream *os = new ostringstream(string);
//           theIterator->identify(*os);
//           cout << "---" << string << endl;
//           delete os;
//         }
//       else
//         {
//           strcpy(string, "none");
//         }
//       TThread::Lock();
//       pm->setStream(string);

//       pm->setEvtnr(theState.getNoevt());
//       pm->setStatus(theState.isRunning());
//       pm->Update();
//       TThread::UnLock();
//       sleep(seconds);
//     }

//   TThread::Lock();
//   delete pm;
//   TThread::UnLock();


//   return ;
// }


// //-----------------------------------------

// int pcontrol ()
// {
//   pcontrol(10);
//   return 0;
// }

// int pcontrol (const int seconds)
// {
//   if (!display_thread)
//     {
//       display_thread = new TThread("Display", pdisplay);
//     }
//   timeinterval = seconds;
//   display_thread->Run();
//   return 0;
// }

// const char *pname()
// {

//   if (! theState.isRunning() )
//     {
//       return " ";
//     }
//   return theIterator->getCurrentFileName();

// }

// int pgui ()
// {
//   if (theGui)
//     {
//       return -1;
//     }
//   theGui = new pmongui(gClient->GetRoot(), 400, 120 );
//   return 0;
// }

// int prmgui ()
// {
//   if (!theGui)
//     {
//       return -1;
//     }
//   delete theGui;
//   theGui = 0;
//   return 0;
// }

// int phsave (const char *filename)
// {

//       return -1;
// }

void phelp()
{

  cout << " -- pmonitor commands" << endl;
  cout << endl;
  cout << " pstatus()                 gives a brief status of pmonitor" << endl;
  cout << " pfileopen(\"filename\")     opens a PRDF file" << endl;
  cout << " plistopen(\"filename\")     opens a file with a list of prdfs" << endl;
  cout << " ptestopen()               opens a test input stream" << endl;
  cout << " pclose()                  closes the open stream" << endl;
  cout << " pstart(nevt)              starts event loop for nevt events in background (nevt=0 -> all events)" << endl;
  cout << " prun(nevt)                run nevt events (nevt=0 -> all events)" << endl;
  cout << endl;
  cout << " pidentify()               identify the next event read from the data stream" << endl;
  cout << " pidentify(5)              identify the next 5 events read from the data stream" << endl;
  cout << " pidentify(0)              identify all events read from the data stream (lots of output!)" << endl;
  cout << " pclearidentify()          resets pidentify(0)" << endl;
  cout << endl;
#ifdef HAVE_ROOT6
  cout << " pupdate(TCanvas *c1, int seconds)    starts periodic updates of Canvas c1" << endl;
  cout << " pendupdate(TCanvas *c1)              stop updating Canvas c1" << endl;
  cout << " pendupdate()                         stop updating all Canvases that are updating" << endl;
  cout << endl;
#endif  
  //  cout << " phsave(\"filename\")        saves all histograms made the histogram factory to the file" << endl;
  cout << "--" << endl;

}


unsigned int stop_display = 0;

struct threadargument
{
  TVirtualPad *thePad;
  unsigned int refreshinterval;
};

pMutex mondisplay_started;

static TThread *mon_thread = 0;

unsigned int refreshinterval = 5;

// if stop update is true, we look for the thread that updates pad_to_stop
// if that is 0, we stop all threads
unsigned int stop_update = 0;
TVirtualPad * pad_to_stop = 0;

void update_process (void * ptr)
{
  int highest_subpad = 0;

  // our argument
  threadargument *ta = ( threadargument *) ptr; 

  TVirtualPad *myPad = ta->thePad;
  unsigned int my_refreshinterval = ta->refreshinterval;

  // and done with it.
  delete ta;

  time_t last_time = time(0) + my_refreshinterval + 1;  //force an update on start

  TVirtualPad * s = 0;
  int i;

  for ( i = 0; i < 128; i++)
    {
      if ( s == myPad->cd(i)) break;
      s = myPad->cd(i);
    }
  highest_subpad = i-1;


  while ( 1)
    {
      if ( stop_update)
	{
	  if ( pad_to_stop == 0)
	    {
	      break;
	    }
	  else
	    {
	      if (pad_to_stop == myPad) break;
	    }
	}
      
      time_t x = time(0);
      if ( x - last_time > my_refreshinterval)
	{
	  last_time = x;
	  
	  for ( i = 0; i <= highest_subpad; i++)
	    {
	      s = myPad->cd(i);
	      s->Modified();
	      s->Update();
	    }
	  sleep(1);
	}
    }
  
}


void pupdate(TVirtualPad * pad, const unsigned int refresh)
{

#ifdef HAVE_ROOT6
  // this structure is getting deleted in the thread after
  // we got everytthing off it. 
  threadargument *ta = new threadargument;
  
  // parameters for the thread - the pad, and the waitinterval
  ta->thePad = pad;
  ta->refreshinterval = refresh;

  stop_update = 0;
  pad_to_stop = 0;
  mon_thread = new TThread ( update_process, ta);
  mon_thread->Run();
#else
  cout << "this is implemented in root v6 only" << endl; 
#endif
  
}

void pend_update(TVirtualPad * pad)
{  
#ifdef HAVE_ROOT6
  stop_update = 1;
  pad_to_stop = pad;
#else
  cout << "this is implemented in root v6 only" << endl; 
#endif

}

