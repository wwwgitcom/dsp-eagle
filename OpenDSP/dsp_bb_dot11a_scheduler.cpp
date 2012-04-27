#include "dsp_bb_dot11a_scheduler.h"
#include "dsp_task.h"
namespace OpenDSP
{
    namespace dot11a
    {


        scheduler::scheduler()
            : dsp_scheduler(NULL), m_enabled(true), m_cs_task(*this), m_header_task(*this),
            m_source_task(*this), m_decode_task(*this)
        {
            
        }
        scheduler::~scheduler()
        {
            stop();

            delete m_fg;

            delete m_source;
            delete m_cs;
            delete m_remove_dc;
            delete m_boundary;
            delete m_foe;
            delete m_foc;
            delete m_channel_estimation;
            delete m_channel_compensation;
            delete m_remove_gi;
            delete m_fft;
            delete m_demap;
            delete m_deinterleave;
            delete m_viterbi;
            delete m_header_parser;
            delete m_pilot;
            delete m_descramble;
            delete m_crc;
            delete m_dummy;
        }

        dsp_scheduler_ptr scheduler::make()
        {
            return dsp_scheduler_ptr(new scheduler());
        }


        void scheduler::build_flow_graph()
        {
            //m_source                      = new dsp_file_source("D:\\FANGJi\\KuaiPan\\OpenDSP\\11ndump\\11a\\16qam\\machine1\\1.dmp");
            m_source                     = new dsp_file_source("C:\\DiskD\\Projects\\OpenDSP\\11ndump\\11g\\cable1\\MiMoRx_1_30047401_1220202304.dmp");
            //m_source                     = new dsp_file_source("C:\\DiskD\\Danial\\KuaiPan\\OpenDSP\\11ndump\\11a\\16qam\\machine3\\2.dmp");
            //m_source                   = new dsp_file_source("mimo_0.dmp");
            //m_source                   = new dsp_file_source("mimorx_0.dmp");
            m_cs                         = new dot11a::carrier_sense();
            m_remove_dc                  = new remove_dc();
            m_boundary                   = new dot11a::ofdm_symbol_boundary_check();
            m_foe                        = new dot11a::carrier_frequency_estimation();
            m_foc                        = new dot11a::carrier_frequency_compensation();
            m_channel_estimation         = new dot11a::channel_estimation();
            m_channel_compensation       = new dot11a::channel_compensation();
            m_remove_gi                  = new dot11a::remove_gi();
            m_fft                        = new dot11a::fft();
            m_deinterleave               = new deinterleave();
            m_demap                      = new demap();
            m_viterbi                    = new viterbi_decoder();
            m_header_parser              = new header_parser();
            m_pilot                      = new pilot_tracking();
            m_crc                        = new crc_check();
            m_descramble                 = new descramble();
            m_dummy                      = new dot11a::dummy();

            m_source->set_decimation(2);

            m_fg = dsp_make_flat_flowgraph();

            dsp_vector_endpoint shared_connections;

            m_fg->connect((dsp_basic_block_ptr)m_source, 0, (dsp_basic_block_ptr)m_remove_dc, 0);

            m_fg->connect((dsp_basic_block_ptr)m_remove_dc, 0, (dsp_basic_block_ptr)m_cs, 0);
            m_fg->connect((dsp_basic_block_ptr)m_remove_dc, 0, (dsp_basic_block_ptr)m_boundary, 0);
            m_fg->connect((dsp_basic_block_ptr)m_remove_dc, 0, (dsp_basic_block_ptr)m_foe, 0);
            m_fg->connect((dsp_basic_block_ptr)m_remove_dc, 0, (dsp_basic_block_ptr)m_foc, 0);
            m_fg->connect((dsp_basic_block_ptr)m_foc, 0, (dsp_basic_block_ptr)m_remove_gi, 0);
            m_fg->connect((dsp_basic_block_ptr)m_foc, 0, (dsp_basic_block_ptr)m_fft, 0);
            m_fg->connect((dsp_basic_block_ptr)m_fft, 0, (dsp_basic_block_ptr)m_channel_estimation, 0);
            m_fg->connect((dsp_basic_block_ptr)m_fft, 0, (dsp_basic_block_ptr)m_channel_compensation, 0);
            m_fg->connect((dsp_basic_block_ptr)m_channel_compensation, 0, (dsp_basic_block_ptr)m_pilot, 0);
            m_fg->connect((dsp_basic_block_ptr)m_channel_compensation, 0, (dsp_basic_block_ptr)m_demap, 0);
            m_fg->connect((dsp_basic_block_ptr)m_demap, 0, (dsp_basic_block_ptr)m_deinterleave, 0);
            m_fg->connect((dsp_basic_block_ptr)m_deinterleave, 0, (dsp_basic_block_ptr)m_viterbi, 0);
            m_fg->connect((dsp_basic_block_ptr)m_viterbi, 0, (dsp_basic_block_ptr)m_header_parser, 0);
            m_fg->connect((dsp_basic_block_ptr)m_viterbi, 0, (dsp_basic_block_ptr)m_descramble, 0);
            m_fg->connect((dsp_basic_block_ptr)m_descramble, 0, (dsp_basic_block_ptr)m_crc, 0);

            m_fg->validate();

            m_fg->setup_connections();
            //////////////////////////////////////////////////////////////////////////
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_cs, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_boundary, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_foe, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_foc, 0));
            m_fg->shared_connect(dsp_endpoint((dsp_basic_block_ptr)m_remove_dc, 0), shared_connections);

            shared_connections.clear();
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_fft, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_remove_gi, 0));
            m_fg->shared_connect(dsp_endpoint((dsp_basic_block_ptr)m_foc, 0), shared_connections);

            shared_connections.clear();
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_channel_estimation, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_channel_compensation, 0));
            m_fg->shared_connect(dsp_endpoint((dsp_basic_block_ptr)m_fft, 0), shared_connections);

            shared_connections.clear();
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_demap, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_pilot, 0));
            m_fg->shared_connect(dsp_endpoint((dsp_basic_block_ptr)m_channel_compensation, 0), shared_connections);

            shared_connections.clear();
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_header_parser, 0));
            shared_connections.push_back(dsp_endpoint((dsp_basic_block_ptr)m_descramble, 0));
            m_fg->shared_connect(dsp_endpoint((dsp_basic_block_ptr)m_viterbi, 0), shared_connections);
            //////////////////////////////////////////////////////////////////////////
            m_fg->dump();
            //////////////////////////////////////////////////////////////////////////
            m_pipeline0.add((dsp_block_ptr)m_source);
            m_pipeline0.add((dsp_block_ptr)m_remove_dc);
            m_pipeline0.add((dsp_block_ptr)m_cs);

            m_pipeline1.add((dsp_block_ptr)m_source);
            m_pipeline1.add((dsp_block_ptr)m_remove_dc);
            m_pipeline1.add((dsp_block_ptr)m_boundary);

            m_pipeline2.add((dsp_block_ptr)m_source);
            m_pipeline2.add((dsp_block_ptr)m_remove_dc);
            m_pipeline2.add((dsp_block_ptr)m_foe);

            m_pipeline3.add((dsp_block_ptr)m_source);
            m_pipeline3.add((dsp_block_ptr)m_remove_dc);
            m_pipeline3.add((dsp_block_ptr)m_foc);
            m_pipeline3.add((dsp_block_ptr)m_fft);
            m_pipeline3.add((dsp_block_ptr)m_channel_estimation);

            m_pipeline4.add((dsp_block_ptr)m_source);
            m_pipeline4.add((dsp_block_ptr)m_remove_dc);
            m_pipeline4.add((dsp_block_ptr)m_foc);
            m_pipeline4.add((dsp_block_ptr)m_remove_gi);
            m_pipeline4.add((dsp_block_ptr)m_fft);
            m_pipeline4.add((dsp_block_ptr)m_channel_compensation);
            m_pipeline4.add((dsp_block_ptr)m_pilot);
            m_pipeline4.add((dsp_block_ptr)m_demap);
            m_pipeline4.add((dsp_block_ptr)m_deinterleave);
            m_pipeline4.add((dsp_block_ptr)m_viterbi);
            m_pipeline4.add((dsp_block_ptr)m_header_parser);

            m_pipeline5.add((dsp_block_ptr)m_source);
            m_pipeline5.add((dsp_block_ptr)m_remove_dc);
            m_pipeline5.add((dsp_block_ptr)m_foc);
            m_pipeline5.add((dsp_block_ptr)m_remove_gi);
            m_pipeline5.add((dsp_block_ptr)m_fft);
            m_pipeline5.add((dsp_block_ptr)m_channel_compensation);
            m_pipeline5.add((dsp_block_ptr)m_pilot);
            m_pipeline5.add((dsp_block_ptr)m_demap);
            m_pipeline5.add((dsp_block_ptr)m_deinterleave);

            m_pipeline6.add((dsp_block_ptr)m_viterbi);
            m_pipeline6.add((dsp_block_ptr)m_descramble);
            m_pipeline6.add((dsp_block_ptr)m_crc);
            //////////////////////////////////////////////////////////////////////////
#if 0
            std::vector<dsp_basic_block_vector_t> graphs = m_fg->partition();
            for (int i = 0; i < graphs.size() ; i++)
            {
                cout << "graph " << i << endl;
            	dsp_basic_block_vector_t vt = graphs[0];
                for (int j = 0; j < vt.size() ; j++)
                {
                	cout << " " << vt[j]->name() << endl;
                }
            }
#endif
        }

        void scheduler::schedule_flow_graph_pipeline()
        {
            m_cpu_manager = dsp_task::cpu_manager::Instance();
            do 
            {
                m_cs->reset();
                m_boundary->reset();

                m_pipeline0.execute();
                m_pipeline1.execute();

                /////////////////////
                m_foe->general_work();
                m_foc->set_frequency_offset(
                    m_foe->frequency_offset()
                    );
                m_foc->set_compensate_length(64);

                m_pipeline3.execute();

                m_channel_compensation->set_channel_state(
                    m_channel_estimation->channel_state()
                    );

                m_foc->set_compensate_length(80);
                m_remove_gi->set_symbol_count(1);

                m_pipeline6.Wait();

                m_viterbi->reset();
                m_demap->reset();
                m_deinterleave->reset();

                m_pipeline4.execute();

                if (m_header_parser->header_ok())
                {
                    m_remove_gi->set_symbol_count(m_header_parser->symbol_count());
                    m_demap->set_rate(m_header_parser->data_rate());
                    m_deinterleave->set_rate(m_header_parser->data_rate());
                    m_viterbi->setup_param(m_header_parser->data_rate(), 
                        (m_header_parser->data_length() << 3) + 16 + 6);
                    m_descramble->reset();
                    m_crc->reset();
                    m_crc->set_length(m_header_parser->data_length());
                }
                else
                {
                    // go on carrier sensing....
                    continue;
                }
                
                m_cpu_manager->run_task(&m_pipeline6);
                m_pipeline5.execute();
            }while(m_enabled);
        }


        void scheduler::schedule_flow_graph_parallel()
        {
            dsp_block::state st;
            m_cpu_manager = dsp_task::cpu_manager::Instance();
            do 
            {
                m_cs->reset();
                m_boundary->reset();
                m_viterbi->reset();
                m_demap->reset();
                m_deinterleave->reset();
                
#if 1
                do 
                {
                    m_cpu_manager->run_task((task*)m_source);
                    m_cpu_manager->run_task((task*)m_remove_dc);
                    m_cpu_manager->run_task((task*)m_cs);

                } while (m_enabled);
#endif
                //m_cpu_manager->run_task(&m_cs_task);
                //m_cs_task.Wait();
                

                do 
                {
#if 1
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
#endif
                    while ( (st = m_boundary->general_work()) == dsp_block::READY );
                } while (st != dsp_block::DONE && m_enabled);

                m_foe->general_work();
                /*
                do 
                {
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
                    while ( (st = m_foe->general_work()) == dsp_block::READY );
                } while (st != dsp_block::DONE && m_enabled);
                */
                m_foc->set_frequency_offset(
                    m_foe->frequency_offset()
                    );
                m_foc->set_compensate_length(64);

                do 
                {
                    if (m_foc->general_work() == dsp_block::READY)
                    {
                        st = m_fft->general_work();
                        if (m_channel_estimation->general_work() == dsp_block::DONE)
                            break;
                    }
#if 1
                    st = m_source->general_work();
                    m_remove_dc->general_work();
#endif
                    
                } while (st != dsp_block::DONE && m_enabled);
                
                m_foc->set_compensate_length(80);

                m_channel_compensation->set_channel_state(
                    m_channel_estimation->channel_state()
                    );

#if 1
                do 
                {
                    if ((st = m_foc->general_work()) == dsp_block::READY)
                    {
                        st = m_remove_gi->general_work();
                        st = m_fft->general_work();
                        st = m_channel_compensation->general_work();
                        st = m_pilot->general_work();
                        m_foc->set_phase(m_pilot->phase());// feedback incremental phase
                        st = m_demap->general_work();
                        st = m_deinterleave->general_work();
                        st = m_viterbi->general_work();
                        st = m_header_parser->general_work();

                        break;
                    }
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
                } while (st != dsp_block::DONE && m_enabled);
#endif
                //m_cpu_manager->run_task(&m_header_task);
                //m_header_task.Wait();
                if (m_header_parser->header_ok())
                {
                    m_demap->set_rate(m_header_parser->data_rate());
                    m_deinterleave->set_rate(m_header_parser->data_rate());
                    m_viterbi->setup_param(m_header_parser->data_rate(), 
                        (m_header_parser->data_length() << 3) + 16 + 6);
                    m_descramble->reset();
                    m_crc->reset();
                    m_crc->set_length(m_header_parser->data_length());
                }
                else
                {
                    // go on carrier sensing....
                    continue;
                }

#if 1
                do 
                {
                    while ((st = m_foc->general_work()) == dsp_block::READY)
                    {
                        st = m_remove_gi->general_work();
                        st = m_fft->general_work();
                        st = m_channel_compensation->general_work();
                        st = m_pilot->general_work();
                        m_foc->set_phase(m_pilot->phase());// feedback incremental phase
                        st = m_demap->general_work();
                        st = m_deinterleave->general_work();
                        if (m_viterbi->general_work() == dsp_block::READY)
                        {
                            st = m_descramble->general_work();
                            st = m_crc->general_work();
                        }
                        if (st == dsp_block::DONE)
                        {
                            break;
                        }
                    }
                    if (st == dsp_block::DONE)
                    {
                        break;
                    }
#if 0
                    do 
                    {
                        m_source->general_work();
                        st = m_remove_dc->general_work();
                    } while (st != dsp_block::READY);
#endif
                    
                } while (m_enabled);
#endif

                //m_cpu_manager->run_task(&m_decode_task);
                //m_decode_task.Wait();
            } while (m_enabled);
        }






        void scheduler::schedule_flow_graph()
        {
            dsp_block::state st;


            //m_cpu_manager->run_task(&m_source_task);
            do 
            {
                m_cs->reset();
                m_boundary->reset();
                m_viterbi->reset();
                m_demap->reset();
                m_deinterleave->reset();
                
#if 1
                do 
                {
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
                    while ( (st = m_cs->general_work()) == dsp_block::READY );

                } while (st != dsp_block::DONE && m_enabled);
#endif
                //m_cpu_manager->run_task(&m_cs_task);
                //m_cs_task.Wait();
                

                do 
                {
#if 1
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
#endif
                    while ( (st = m_boundary->general_work()) == dsp_block::READY );
                } while (st != dsp_block::DONE && m_enabled);

                m_foe->general_work();
                /*
                do 
                {
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
                    while ( (st = m_foe->general_work()) == dsp_block::READY );
                } while (st != dsp_block::DONE && m_enabled);
                */
                m_foc->set_frequency_offset(
                    m_foe->frequency_offset()
                    );
                m_foc->set_compensate_length(64);

                do 
                {
                    if (m_foc->general_work() == dsp_block::READY)
                    {
                        st = m_fft->general_work();
                        if (m_channel_estimation->general_work() == dsp_block::DONE)
                            break;
                    }
#if 1
                    st = m_source->general_work();
                    m_remove_dc->general_work();
#endif
                    
                } while (st != dsp_block::DONE && m_enabled);
                
                m_foc->set_compensate_length(80);

                m_channel_compensation->set_channel_state(
                    m_channel_estimation->channel_state()
                    );

#if 1
                do 
                {
                    if ((st = m_foc->general_work()) == dsp_block::READY)
                    {
                        st = m_remove_gi->general_work();
                        st = m_fft->general_work();
                        st = m_channel_compensation->general_work();
                        st = m_pilot->general_work();
                        m_foc->set_phase(m_pilot->phase());// feedback incremental phase
                        st = m_demap->general_work();
                        st = m_deinterleave->general_work();
                        st = m_viterbi->general_work();
                        st = m_header_parser->general_work();

                        break;
                    }
                    st = m_source->general_work();
                    if (m_remove_dc->general_work() == dsp_block::BLKD_IN)
                    {
                        continue;
                    }
                } while (st != dsp_block::DONE && m_enabled);
#endif
                //m_cpu_manager->run_task(&m_header_task);
                //m_header_task.Wait();
                if (m_header_parser->header_ok())
                {
                    m_demap->set_rate(m_header_parser->data_rate());
                    m_deinterleave->set_rate(m_header_parser->data_rate());
                    m_viterbi->setup_param(m_header_parser->data_rate(), 
                        (m_header_parser->data_length() << 3) + 16 + 6);
                    m_descramble->reset();
                    m_crc->reset();
                    m_crc->set_length(m_header_parser->data_length());
                }
                else
                {
                    // go on carrier sensing....
                    continue;
                }

#if 1
                do 
                {
                    while ((st = m_foc->general_work()) == dsp_block::READY)
                    {
                        st = m_remove_gi->general_work();
                        st = m_fft->general_work();
                        st = m_channel_compensation->general_work();
                        st = m_pilot->general_work();
                        m_foc->set_phase(m_pilot->phase());// feedback incremental phase
                        st = m_demap->general_work();
                        st = m_deinterleave->general_work();
                        if (m_viterbi->general_work() == dsp_block::READY)
                        {
                            st = m_descramble->general_work();
                            st = m_crc->general_work();
                        }
                        if (st == dsp_block::DONE)
                        {
                            break;
                        }
                    }
                    if (st == dsp_block::DONE)
                    {
                        break;
                    }
#if 0
                    do 
                    {
                        m_source->general_work();
                        st = m_remove_dc->general_work();
                    } while (st != dsp_block::READY);
#endif
                    
                } while (m_enabled);
#endif

                //m_cpu_manager->run_task(&m_decode_task);
                //m_decode_task.Wait();
            } while (m_enabled);
        }

        void scheduler::stop()
        {
            m_enabled = false;
        }

        void scheduler::wait()
        {
        }

        void scheduler::run()
        {
            build_flow_graph();
            //schedule_flow_graph_parallel();
            schedule_flow_graph_pipeline();
            //schedule_flow_graph();
        }
    }
}