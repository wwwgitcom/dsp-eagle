#include "dsp_scheduler.h"

namespace OpenDSP
{
    dsp_scheduler::dsp_scheduler(dsp_flat_flowgraph_ptr ffg)
        : m_ffg(ffg)
    {}

    dsp_scheduler::~dsp_scheduler(){}

    //////////////////////////////////////////////////////////////////////////


    dsp_scheduler_ptr
        dsp_scheduler_sts::make(dsp_flat_flowgraph_ptr ffg)
    {
        return dsp_scheduler_ptr(new dsp_scheduler_sts(ffg));
    }

    dsp_scheduler_sts::dsp_scheduler_sts(dsp_flat_flowgraph_ptr ffg)
        : dsp_scheduler(ffg)
    {        
    }

    dsp_scheduler_sts::~dsp_scheduler_sts()
    {
        stop();

        for (size_t i = 0; i < m_schedulers.size(); i++)
        {
            delete m_schedulers[0];
        }
    }

    void dsp_scheduler_sts::stop()
    {
        for (unsigned int i = 0; i < m_schedulers.size(); i++)
        {
            m_schedulers[i]->stop();
        }
    }

    void dsp_scheduler_sts::wait()
    {
    }

    void dsp_scheduler_sts::run()
    {
        // Split the flattened flow graph into discrete partitions, each
        // of which is topologically sorted.

        std::vector<dsp_basic_block_vector_t> graphs = m_ffg->partition();

        // For each partition, create a thread to evaluate it using
        // an instance of the dsp_single_threaded_scheduler
        int iLoop = 10;
        for (std::vector<dsp_basic_block_vector_t>::iterator p = graphs.begin(); p != graphs.end(); p++) 
        {
            dsp_block_vector_t blocks = dsp_flat_flowgraph::make_block_vector(*p);

            m_schedulers.push_back(dsp_make_single_threaded_scheduler(blocks));
        }

        // we only impl. single graph scheduling now.
        assert(m_schedulers.size() == 1);

        m_schedulers[0]->run();
    }

    //////////////////////////////////////////////////////////////////////////

    static int which_scheduler  = 0;

    dsp_single_threaded_scheduler_ptr
        dsp_make_single_threaded_scheduler (std::vector<dsp_block_ptr> &blocks)
    {
        return dsp_single_threaded_scheduler_ptr (new dsp_single_threaded_scheduler (blocks));
    }

    dsp_single_threaded_scheduler::dsp_single_threaded_scheduler (
        std::vector<dsp_block_ptr> &blocks)
        : m_blocks (blocks), m_enabled (true)
    {
    }

    dsp_single_threaded_scheduler::~dsp_single_threaded_scheduler ()
    {
        stop ();
    }

    void dsp_single_threaded_scheduler::run ()
    {
        main_loop ();
    }

    void dsp_single_threaded_scheduler::stop ()
    { 
        if (1)
            std::cout << "dsp_singled_threaded_scheduler::stop() "
            << this << std::endl;
        m_enabled = false;
    }

    void dsp_single_threaded_scheduler::main_loop ()
    {
        dsp_block::state st;

        std::cout << "=====dsp_blocks to schedule======" << endl;
        for (dsp_block_viter_t t = m_blocks.begin(); t != m_blocks.end(); t++)
        {
            std::cout << (*t)->name() << endl;
        }
        std::cout << "==================================" << endl;

        dsp_block_viter_t block;
        do 
        {
            block = m_blocks.begin();
            do 
            {
                // try to consume as much as possible
                st = (*block)->general_work();
                //std::cout << "block "<< (*t)->name() << " ret: " << st << endl;
                block++;
            } while (block != m_blocks.end());
        } while (m_enabled);
    }
}