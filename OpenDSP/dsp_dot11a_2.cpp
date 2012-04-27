#include "dsp_dot11a_2.h"

namespace OpenDSP
{
    namespace dot11a
    {
        dot11a_entry::dot11a_entry()
        {

        }

        dot11a_entry::~dot11a_entry()
        {

        }

        void dot11a_entry::allocate_flow_graph()
        {
            //m_source                      = new dsp_file_source("D:\\FANGJi\\KuaiPan\\OpenDSP\\11ndump\\11a\\16qam\\machine1\\1.dmp");
            //m_source                        = new dsp_file_source("C:\\DiskD\\Projects\\OpenDSP\\11ndump\\11g\\cable1\\MiMoRx_1_30047401_1220202304.dmp");
            m_source                      = new dsp_file_source("C:\\DiskD\\Projects\\OpenDSP\\11ndump\\11a\\16qam\\machine1\\1.dmp");
            //m_source                      = new dsp_file_source("mimo_0.dmp");
            //m_source                      = new dsp_file_source("mimorx_0.dmp");
            m_plcp_switch                   = new bb_switch();
            m_cs                            = new dot11a::carrier_sense();
            m_remove_dc                     = new remove_dc();
            m_boundary                      = new dot11a::ofdm_symbol_boundary_check();
            m_foe                           = new dot11a::carrier_frequency_estimation();
            m_foc                           = new dot11a::carrier_frequency_compensation();
            m_channel_estimation            = new dot11a::channel_estimation();
            m_channel_compensation          = new dot11a::channel_compensation();
            m_remove_gi                     = new dot11a::remove_gi();
            m_fft                           = new dot11a::fft();
            m_deinterleave                  = new deinterleave();
            m_demap                         = new demap();
            m_viterbi                       = new viterbi_decoder();
            m_header_parser                 = new header_parser();
            m_pilot                         = new pilot_tracking();
            m_crc                           = new crc_check();
            m_descramble                    = new descramble();
            m_dummy                         = new dot11a::dummy();

            m_source->set_decimation(2);
        }

        void dot11a_entry::build_flow_graph()
        {
            m_fg = dsp_make_flat_flowgraph();
            m_context = new dot11a_context();

            carrier_frequency_compensation * foc2 = new carrier_frequency_compensation();
            foc2->set_compensate_length(80);

            m_fg->connect_d((dsp_basic_block_ptr)m_source, 0, (dsp_basic_block_ptr)m_remove_dc, 0);
            m_fg->connect_d((dsp_basic_block_ptr)m_remove_dc, 0, (dsp_basic_block_ptr)m_plcp_switch, 0);

            m_foc->set_compensate_length(128);
            m_fg->connect_c((dsp_basic_block_ptr)m_plcp_switch, 0, (dsp_basic_block_ptr)m_cs, 0);
            m_fg->connect_c((dsp_basic_block_ptr)m_plcp_switch, 1, (dsp_basic_block_ptr)m_boundary, 0);
            m_fg->connect_c((dsp_basic_block_ptr)m_plcp_switch, 2, (dsp_basic_block_ptr)m_foe, 0);
            m_fg->connect_c((dsp_basic_block_ptr)m_plcp_switch, 3, (dsp_basic_block_ptr)m_foc, 0);
            m_fg->connect_c((dsp_basic_block_ptr)m_plcp_switch, 4, (dsp_basic_block_ptr)foc2, 0);
            m_plcp_switch->switch_to(0, 0);

            dsp_basic_block_ptr fft2 = new fft();

            m_fg->connect_d((dsp_basic_block_ptr)m_foc, 0, (dsp_basic_block_ptr)m_fft, 0);
            m_fg->connect_d((dsp_basic_block_ptr)m_fft, 0, (dsp_basic_block_ptr)m_channel_estimation, 0);

            m_fg->connect_d((dsp_basic_block_ptr)foc2, 0, (dsp_basic_block_ptr)m_remove_gi, 0);
            m_fg->connect_d((dsp_basic_block_ptr)m_remove_gi, 0, (dsp_basic_block_ptr)fft2, 0);
            m_fg->connect_d((dsp_basic_block_ptr)fft2, 0, (dsp_basic_block_ptr)m_channel_compensation, 0);
            m_fg->connect_d((dsp_basic_block_ptr)m_channel_compensation, 0, (dsp_basic_block_ptr)m_pilot, 0);

            bb_switch* switch2 = new bb_switch();

            m_fg->connect_d((dsp_basic_block_ptr)m_pilot, 0, (dsp_basic_block_ptr)m_dummy, 0);
            //m_fg->connect_c((dsp_basic_block_ptr)switch2, 0, (dsp_basic_block_ptr)m_dummy, 0);
            //switch2->switch_to(0, 0);

            //m_fg->connect((dsp_basic_block_ptr)m_foc, 0, (dsp_basic_block_ptr)m_remove_gi, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_foc, 0, (dsp_basic_block_ptr)m_fft, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_fft, 0, (dsp_basic_block_ptr)m_channel_estimation, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_fft, 0, (dsp_basic_block_ptr)m_channel_compensation, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_channel_compensation, 0, (dsp_basic_block_ptr)m_pilot, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_channel_compensation, 0, (dsp_basic_block_ptr)m_demap, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_demap, 0, (dsp_basic_block_ptr)m_deinterleave, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_deinterleave, 0, (dsp_basic_block_ptr)m_viterbi, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_viterbi, 0, (dsp_basic_block_ptr)m_header_parser, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_viterbi, 0, (dsp_basic_block_ptr)m_descramble, 0);
            //m_fg->connect((dsp_basic_block_ptr)m_descramble, 0, (dsp_basic_block_ptr)m_crc, 0);

            m_fg->bind(m_context);
        }

        void dot11a_entry::schedule_flow_graph()
        {
            while ( true ) m_source->internal_work();
        }
    }
}