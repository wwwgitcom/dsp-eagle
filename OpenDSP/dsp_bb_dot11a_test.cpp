#include "dsp_bb_dot11a.h"
#include "dsp_bb_dot11a_scheduler.h"

using namespace OpenDSP;
using namespace OpenDSP::dot11a;

extern OpenDSP::dsp_scheduler_ptr _scheduler;

void dsp_dot11a_test()
{
    SetThreadAffinityMask(GetCurrentThread(), 1);
    _scheduler = dot11a::scheduler::make();
    _scheduler->run();
    delete _scheduler;
}