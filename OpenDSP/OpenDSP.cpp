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

class CBase
{
public:
    virtual __forceinline void work()
    {
        cout << GetCurrentThreadId() << " ºº×Ö" << endl;
    }
    
protected:
private:
};

class CDerived : public CBase
{
public:
    __forceinline void work0()
    {
        cout << GetCurrentThreadId() << " ×ÓÀàºº×Ö" << endl;
        CBase::work();
    }

protected:
private:
};




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

int _tmain(int argc, _TCHAR* argv[])
{
    //OpenDSP::global_init();
    
    dsp_testunit(argc, argv);



    //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    //SetThreadAffinityMask(GetCurrentThread(), 0x2);

    
    SetConsoleCtrlHandler(HandlerRoutine, true);

    return dsp_testunit(argc, argv);
}