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
        class dot11a_entry
        {
        protected:
            dsp_file_source *m_source;
            remove_dc *m_remove_dc;
            carrier_sense *m_cs;
            bb_switch *m_plcp_switch;
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

            dot11a_context *m_context;
            dsp_flat_flowgraph_ptr m_fg;
        public:
            dot11a_entry();
            ~dot11a_entry();
            void allocate_flow_graph();
            void build_flow_graph();
            void schedule_flow_graph();

        };
    }
}