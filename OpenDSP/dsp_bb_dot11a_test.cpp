#include "dsp_bb_dot11a.h"
#include "dsp_bb_dot11a_scheduler.h"
#include "dsp_dot11a_2.h"
using namespace OpenDSP;
using namespace OpenDSP::dot11a;

extern OpenDSP::dsp_scheduler_ptr _scheduler;

void dsp_dot11a_test()
{
    SetThreadAffinityMask(GetCurrentThread(), 1);
    //_scheduler = dot11a::scheduler::make();
    //_scheduler->run();
    //delete _scheduler;
    dot11a_entry *entry = new dot11a_entry();

    entry->allocate_flow_graph();
    entry->build_flow_graph();
    entry->schedule_flow_graph();

    delete entry;
}