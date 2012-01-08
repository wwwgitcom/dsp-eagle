#include "dsp_pipeline.h"
#include "dsp_log.h"
namespace OpenDSP
{
    dsp_pipeline::dsp_pipeline()
    {

    }

    dsp_pipeline::~dsp_pipeline()
    {

    }

    void dsp_pipeline::add(dsp_block_ptr block)
    {
        m_blocks.push_back(block);
    }

    void dsp_pipeline::clear()
    {
        m_blocks.clear();
    }

    dsp_block::state dsp_pipeline::run_block(dsp_block_ptr block)
    {
        dsp_block::state st;
        m_start_tick = tick_count::now();

        st = block->general_work();

        m_end_tick = tick_count::now();

        m_interval = m_end_tick - m_start_tick;
        if (st == dsp_block::READY)
        log("[perf][%s] %f us\n", block->name().c_str(), m_interval.us());
        return st;
    }

    task* dsp_pipeline::execute()
    {
        dsp_block::state st;

        vector<dsp_block_ptr>::iterator it = m_blocks.begin();

        assert(!m_blocks.empty());
        int iSucc = 0;

        do
        {
            //log("[perf]%s \n", (*it)->name().c_str());
            st = run_block(*it);
            if (st == dsp_block::READY)
            {
                if ( it != m_blocks.end() - 1 )
                {
                    ++it;
                }
            }
            else if (st == dsp_block::BLKD_IN)
            {
                if ( it != m_blocks.begin() )
                    --it;
            }
        }while (st != dsp_block::DONE);

        return this;
    }
}