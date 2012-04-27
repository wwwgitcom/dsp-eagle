#include "dsp_log.h"
#include "dsp_sysconfig.h"
namespace OpenDSP
{
    dsp_log& log = dsp_log::Instance();
    dsp_sysconfig* config = dsp_sysconfig::Instance();

    void global_init()
    {
    }

    
}