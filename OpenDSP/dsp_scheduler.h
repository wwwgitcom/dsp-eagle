#pragma once
#include "dsp_flowgraph.h"

namespace OpenDSP
{
    class dsp_scheduler
    {
    protected:
        dsp_flat_flowgraph_ptr m_ffg;
    public:
        /*!
        * \brief Construct a scheduler and begin evaluating the graph.
        *
        * The scheduler will continue running until all blocks until they
        * report that they are done or the stop method is called.
        */
        dsp_scheduler(dsp_flat_flowgraph_ptr ffg);

        virtual ~dsp_scheduler();

        /*!
        * \brief Tell the scheduler to stop executing.
        */
        virtual void stop() = 0;

        /*!
        * \brief Block until the graph is done.
        */
        virtual void wait() = 0;

        virtual void run() = 0;
    };

    /*!
    * \brief Concrete scheduler that uses the single_threaded_scheduler
    */
    class dsp_scheduler_sts : public dsp_scheduler
    {
    protected:
        
        /*!
        * \brief Construct a scheduler and begin evaluating the graph.
        *
        * The scheduler will continue running until all blocks until they
        * report that they are done or the stop method is called.
        */
        dsp_scheduler_sts(dsp_flat_flowgraph_ptr ffg);

        std::vector<dsp_single_threaded_scheduler_ptr> m_schedulers;

    public:
        static dsp_scheduler_ptr make(dsp_flat_flowgraph_ptr ffg);

        ~dsp_scheduler_sts();

        /*!
        * \brief Tell the scheduler to stop executing.
        */
        void stop();

        /*!
        * \brief Block until the graph is done.
        */
        void wait();

        void run();
    };



    /*!
    * \brief Simple scheduler for stream computations.
    * \ingroup internal
    */

    class dsp_single_threaded_scheduler
    {
    public:
        ~dsp_single_threaded_scheduler ();

        void run ();
        void stop ();

    private:
        std::vector<dsp_block_ptr>	 m_blocks;
        volatile bool				 m_enabled;        

        dsp_single_threaded_scheduler (std::vector<dsp_block_ptr> &blocks);

        void main_loop ();

        friend dsp_single_threaded_scheduler_ptr
            dsp_make_single_threaded_scheduler (std::vector<dsp_block_ptr> &blocks);
    };

    dsp_single_threaded_scheduler_ptr
        dsp_make_single_threaded_scheduler (std::vector<dsp_block_ptr> &blocks);
}