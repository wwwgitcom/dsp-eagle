#include "dsp_log.h"
#include "dsp_sysconfig.h"
namespace OpenDSP
{
    dsp_log& log = dsp_log::Instance();
    dsp_sysconfig* config = dsp_sysconfig::Instance();

    void global_init()
    {
        log.add_include_filter("perf");
        log.add_exclude_filter("file_source");
        log.add_exclude_filter("remove_dc");
        log.add_exclude_filter("carrier_sense");
    }

    
}