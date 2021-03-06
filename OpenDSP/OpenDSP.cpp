// OpenDSP.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "dsp_sysconfig.h"
#include "dsp_scheduler.h"
#include <typeinfo>
namespace OpenDSP
{
  extern void global_init();
}


//////////////////////////////////////////////////////////////////////////

int dsp_testunit(int argc, _TCHAR* argv[]);

OpenDSP::dsp_scheduler_ptr _scheduler = NULL;

BOOL WINAPI HandlerRoutine(__in  DWORD dwCtrlType)
{
  std::cout << "Console event " << dwCtrlType;
  switch (dwCtrlType)
  {
  case CTRL_C_EVENT:
  case CTRL_BREAK_EVENT:
  case CTRL_CLOSE_EVENT:
    std::cout << " stop scheduler" << endl;
    exit(0);
    _scheduler->stop ();
    return true;// we handle the msg
  default:
    return false;
  }
}

int viterbi_check();

int viterbi_test(float EbN0);

int _tmain(int argc, _TCHAR* argv[])
{
  OpenDSP::global_init();

  if (argc == 2)
  {
    viterbi_test(atof(argv[1]));
  }
  else
  {
    viterbi_test(2.0f);
  }
  
  return 1;

  //viterbi_check();



  //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
  //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
  //SetThreadAffinityMask(GetCurrentThread(), 0x1);


  SetConsoleCtrlHandler(HandlerRoutine, true);

  return dsp_testunit(argc, argv);
}