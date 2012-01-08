#include "dsp_block.h"
using namespace OpenDSP;

class myblock : dsp_block
{
public:
    myblock()
        : dsp_block("myblock", dsp_io_signature(1, sizeof(int)), dsp_io_signature(1, sizeof(float)))
    {

    }

    state general_work()
    {
        return dsp_block::READY;
    }

    // reset internal data
    void reset()
    {
        return;
    }
};

myblock myblk;
dsp_block_executor myblkexe((dsp_block_ptr)&myblk);

void dsp_block_test()
{
    //dsp_block srcblock("source", dsp_io_signature(1, vin), dsp_io_signature(1, vout));

    myblkexe.run_one_iteration();    
}