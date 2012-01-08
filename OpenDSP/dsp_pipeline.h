#pragma once
#include "dsp_task.h"
#include "dsp_block.h"
#include "dsp_tickcount.h"
#include <vector>
#include <stack>
using namespace OpenDSP::dsp_task;
using namespace std;

namespace OpenDSP
{
    class dsp_pipeline : public dsp_task::task
    {
    public:
        dsp_pipeline();
        ~dsp_pipeline();
        void add(dsp_block_ptr block);
        void clear();
        task* execute();
        dsp_block::state run_block(dsp_block_ptr block);
    protected:

    private:
        tick_count m_start_tick;
        tick_count m_end_tick;
        tick_count m_interval;
        vector<dsp_block_ptr> m_blocks;
        stack<vector<dsp_block_ptr>::iterator> m_stack;
    };
}