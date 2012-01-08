#pragma once

#include "dsp_bb_dot11a.h"
#include "dsp_file_source.h"
#include "dsp_flowgraph.h"
#include "dsp_scheduler.h"
#include "dsp_pipeline.h"
namespace OpenDSP
{
    namespace dot11a
    {
        class scheduler : public dsp_scheduler
        {
        protected:
            dsp_file_source *m_source;
            remove_dc *m_remove_dc;
            carrier_sense *m_cs;
            ofdm_symbol_boundary_check *m_boundary;
            carrier_frequency_estimation *m_foe;
            carrier_frequency_compensation *m_foc;
            channel_estimation *m_channel_estimation;
            channel_compensation *m_channel_compensation;
            remove_gi *m_remove_gi;
            fft *m_fft;
            demap *m_demap;
            deinterleave *m_deinterleave;
            viterbi_decoder *m_viterbi;
            header_parser *m_header_parser;
            pilot_tracking *m_pilot;
            descramble *m_descramble;
            crc_check *m_crc;
            dummy *m_dummy;

            dsp_flat_flowgraph_ptr m_fg;
            dsp_vector_endpoint shared_connections;

            void build_flow_graph();
            void schedule_flow_graph();
            void schedule_flow_graph_pipeline();
            void schedule_flow_graph_parallel();

            class cs_task : public dsp_task::task
            {
            private:
                scheduler& m_sc;
            public:
                cs_task(scheduler &sc) : m_sc(sc){}
                ~cs_task(){}
                __forceinline virtual task* execute()
                {
                    dsp_block::state st;
                    do 
                    {
                        while ( (st = m_sc.m_cs->general_work()) == dsp_block::READY );
                    } while (st != dsp_block::DONE && m_sc.m_enabled);
                    return this;
                }
            };
            class header_task : public dsp_task::task
            {
            private:
                scheduler& m_sc;
            public:
                header_task(scheduler &sc) : m_sc(sc){}
                ~header_task(){}
                __forceinline virtual task* execute()
                {
                    dsp_block::state st;
                    do 
                    {
                        if ((st = m_sc.m_foc->general_work()) == dsp_block::READY)
                        {
                            st =  m_sc.m_remove_gi->general_work();
                            st =  m_sc.m_fft->general_work();
                            st =  m_sc.m_channel_compensation->general_work();
                            st =  m_sc.m_pilot->general_work();
                            m_sc.m_foc->set_phase( m_sc.m_pilot->phase());// feedback incremental phase
                            st =  m_sc.m_demap->general_work();
                            st =  m_sc.m_deinterleave->general_work();
                            st =  m_sc.m_viterbi->general_work();
                            st =  m_sc.m_header_parser->general_work();

                            break;
                        }
                    } while (st != dsp_block::DONE &&  m_sc.m_enabled);
                    return this;
                }
            };

            class source_task : public dsp_task::task
            {
            private:
                scheduler& m_sc;
            public:
                source_task(scheduler &sc) : m_sc(sc){}
                ~source_task(){}
                __forceinline virtual task* execute()
                {
                    dsp_block::state st;
                    do 
                    {
                        st = m_sc.m_source->general_work();
                        st = m_sc.m_remove_dc->general_work();
                    } while (st != dsp_block::DONE && m_sc.m_enabled);
                    return this;
                }
            };

            class decode_task : public dsp_task::task
            {
            private:
                scheduler& m_sc;
            public:
                decode_task(scheduler &sc) : m_sc(sc){}
                ~decode_task(){}
                __forceinline virtual task* execute()
                {
                    dsp_block::state st;
                    do 
                    {
                        if ((st = m_sc.m_foc->general_work()) == dsp_block::READY)
                        {
                            st = m_sc.m_remove_gi->general_work();
                            st = m_sc.m_fft->general_work();
                            st = m_sc.m_channel_compensation->general_work();
                            st = m_sc.m_pilot->general_work();
                            m_sc.m_foc->set_phase(m_sc.m_pilot->phase());// feedback incremental phase
                            st = m_sc.m_demap->general_work();
                            st = m_sc.m_deinterleave->general_work();
                            if (m_sc.m_viterbi->general_work() == dsp_block::READY)
                            {
                                st = m_sc.m_descramble->general_work();
                                st = m_sc.m_crc->general_work();
                            }
                        }

                    } while (st != dsp_block::DONE && m_sc.m_enabled);
                    return this;
                }
            };

            source_task m_source_task;

            cs_task m_cs_task;
            header_task m_header_task;

            decode_task m_decode_task;


            dsp_pipeline m_pipeline0;
            dsp_pipeline m_pipeline1;
            dsp_pipeline m_pipeline2;
            dsp_pipeline m_pipeline3;
            dsp_pipeline m_pipeline4;
            dsp_pipeline m_pipeline5;
            dsp_pipeline m_pipeline6;
            
            /*!
            * \brief Construct a scheduler and begin evaluating the graph.
            *
            * The scheduler will continue running until all blocks until they
            * report that they are done or the stop method is called.
            */
            scheduler();
            bool m_enabled;
            dsp_task::cpu_manager* m_cpu_manager;
        public:
            static dsp_scheduler_ptr make();

            ~scheduler();

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
    }
}