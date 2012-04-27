#pragma once
#include "dsp_vector1.h"
#include "dsp_block.h"
#include "dsp_scheduler.h"
#include "dsp_freqest.h"
#include "dsp_fft.h"
#include "dsp_draw.h"
#include "dsp_mapper.h"
#include "dsp_interleave.h"
#include "dsp_viterbi.h"
#include "dsp_math.h"
#include "dsp_scramble.h"
#include "dsp_crc.h"

namespace OpenDSP
{
    namespace dot11a
    {
        enum rate
        {
            DOT11A_RATE_6M  = 0xB, // 1-011
            DOT11A_RATE_9M  = 0XF, // 1-111
            DOT11A_RATE_12M = 0xA, // 1-010
            DOT11A_RATE_18M = 0xE, // 1-110
            DOT11A_RATE_24M = 0x9, // 1-001
            DOT11A_RATE_36M = 0xD, // 1-101
            DOT11A_RATE_48M = 0x8, // 1-000
            DOT11A_RATE_54M = 0xC // 1-100
        };
        class contex
        {
        public:

        };

        //////////////////////////////////////////////////////////////////////////
        class remove_dc : public dsp_block
        {
        public:
            remove_dc ();
            ~remove_dc ();
            state general_work();
            void reset();
        private:
            unsigned int m_counter;
            unsigned int m_length;
            complex32 m_dc;
            dsp_draw_window* m_pBefore;
            dsp_draw_window* m_pAfter;
        };

        class carrier_sense : public dsp_block
        {
        public:
            carrier_sense();
            ~carrier_sense();
            dsp_block::state general_work();
            void reset();
        protected:
            int decision();
            enum {corr_none, corr_up, corr_down, corr_error};
        private:
            static const int m_corr_length = 32;
            
            complex16  m_his_sample[m_corr_length];
            complex32  m_his_corr[m_corr_length];
            int        m_his_energy[m_corr_length];
            int        m_his_index;
            complex32  m_corr_sum;
            int        m_energy_sum;
            __int64    m_energy_sqr[m_corr_length];
            __int64    m_corr_sqr[m_corr_length];
            int        m_corr_direction;
            int        m_accumulate_samples;
            int        m_pos_cnt;
        };

        //////////////////////////////////////////////////////////////////////////
        
        class ofdm_symbol_boundary_check : public dsp_block
        {
        public:
            ofdm_symbol_boundary_check();
            ~ofdm_symbol_boundary_check();
            dsp_block::state general_work();
            void reset();
        protected:
            int decision();
        private:
            static const int m_corr_length = 32;
            complex32  m_his_corr[m_corr_length];
            complex32  m_corr_sum;
            int        m_his_index;
        };
        
        //////////////////////////////////////////////////////////////////////////
        class carrier_frequency_estimation : public dsp_block
        {
        public:
            carrier_frequency_estimation();
            ~carrier_frequency_estimation();
            dsp_block::state general_work();
            void reset();

            short frequency_offset(){return m_frequency_offset;}
        protected:
            
        private:
            static const int m_estimation_length = 64;
            dsp_freq      m_frequency_estimator;
            short         m_frequency_offset;
        };
        //////////////////////////////////////////////////////////////////////////
        class carrier_frequency_compensation : public dsp_block
        {
        public:
            carrier_frequency_compensation();
            ~carrier_frequency_compensation();
            dsp_block::state general_work();
            void reset();
            void set_frequency_offset(short frequency_offset);
            void set_phase(short phase){m_cfo_theta_i = phase;}
            void set_compensate_length(int length){m_compensate_length = length;}
        protected:

        private:
            int           m_compensate_length;
            dsp_freq      m_frequency_compensator;

            short m_cfo_delta_i;
            short m_cfo_step_i;
            short m_cfo_theta_i;
            v_align(16) complex16 m_cfo_coeff_i[128];
        };
        //////////////////////////////////////////////////////////////////////////
        class channel_estimation : public dsp_block
        {
        public:
            channel_estimation();
            ~channel_estimation();
            dsp_block::state general_work();
            void reset();
            complex16* channel_state(){return m_channel_state;}
        protected:

            //! SISO channel estimation & compensation

            void channel_estimation_i(complex16* pcsin, complex16* pcschannel, int count);
            
            static const char m_preamble[64];
        private:
            static const int m_estimation_length = 64;
            fft_i<64>        m_fft_executor;
            v_align(16) complex16        m_frequency_domain[64];
            v_align(16) complex16        m_channel_state[64];
            dsp_draw_window             *m_draw;
        };
        //////////////////////////////////////////////////////////////////////////
        class channel_compensation : public dsp_block
        {
        public:
            channel_compensation();
            ~channel_compensation();
            dsp_block::state general_work();
            void reset();
            void set_channel_state(complex16* channel_state){
                memcpy(m_channel_state, channel_state, sizeof(complex16) * m_compensation_length);
            }

        protected:

            //! SISO channel compensation
            void channel_compensation_i(complex16* pcsin, complex16* pcschannel, complex16* pcsout);
            
        private:
            static const int m_compensation_length = 64;
            complex16        m_channel_state[64];
            dsp_draw_window *m_draw;
        };
        //////////////////////////////////////////////////////////////////////////
        class remove_gi : public dsp_block
        {
        public:
            remove_gi();
            ~remove_gi();
            dsp_block::state general_work();
            void reset();
            void set_gi_length(int gi_length){m_gi_length = gi_length;}
            void set_symbol_count(unsigned int symbol_count){m_symbol_count = symbol_count;}
        protected:
        private:
            int m_gi_length;
            unsigned int m_symbol_count;
        };
        //////////////////////////////////////////////////////////////////////////
        class fft: public dsp_block
        {
        public:
            fft();
            ~fft();
            dsp_block::state general_work();
            void reset();
        protected:
        private:
            fft_i<64>  m_fft_executor;
        };

        //////////////////////////////////////////////////////////////////////////
        class dummy : public dsp_block
        {
        public:
            dummy () : dsp_block("dummy sink", dsp_io_signature(1, sizeof(unsigned __int8)), dsp_io_signature(0)){}
            ~dummy (){};
            state general_work()
            {
                consume(0, ninput(0));
                return DONE;
            }
            void reset(){}
        };
        //////////////////////////////////////////////////////////////////////////
        class demap : public dsp_block
        {
        public:
        private:
            rate m_rate;
            demapper::dsp_demapper m_demapper;
        protected:
            template<int low, int high, typename fncallback>
            void _demap(complex16* pInput, unsigned __int8* pOutput, fncallback &fcb)
            {
                int i;

                for (i = 64 + low; i < 64; i++)
                {
                    if (i == 64 - 21 || i == 64 - 7)
                        continue;
                    fcb(pInput[i], pOutput);
                }

                for (i = 1; i <= high; i++)
                {
                    if (i == 7 || i == 21)
                        continue;

                    fcb(pInput[i], pOutput);
                }
            }

        public:
            demap ();
            ~demap ();
            void set_rate(rate r){m_rate = r;}
            dsp_block::state general_work();
            void reset();
        };
        //////////////////////////////////////////////////////////////////////////
        class deinterleave : public dsp_block
        {
        private:
            deinterleaver::dot11a_deinterleaver m_deinterleaver;
            rate m_rate;
        public:
            void set_rate(rate r) {m_rate = r;}
            deinterleave();
            ~deinterleave();
            void reset();
            dsp_block::state general_work();
        };
        //////////////////////////////////////////////////////////////////////////
        class viterbi_decoder : public dsp_block
        {
        public:
            enum status{header, frame};
        private:
            typedef struct _viterbi_fifo
            {
#define LVB1_DCSIZE 48
#define LVB1_DCCOUNT 128
#define LVB2_DCSIZE 96
#define LVB2_DCCOUNT 128
#define LVB3_DCSIZE 192
#define LVB3_DCCOUNT 128
#define LVB4_DCSIZE 288
#define LVB4_DCCOUNT 128

#define HTVB1_DCSIZE 104
#define HTVB1_DCCOUNT 128
#define HTVB2_DCSIZE 208
#define HTVB2_DCCOUNT 128
#define HTVB3_DCSIZE 416
#define HTVB3_DCCOUNT 128
#define HTVB4_DCSIZE 624
#define HTVB4_DCCOUNT 128
                union
                {
                    typedef VBUFFER::VB<LVB1_DCSIZE, LVB1_DCCOUNT> LVB1;
                    typedef VBUFFER::VB<LVB2_DCSIZE, LVB2_DCCOUNT> LVB2;
                    typedef VBUFFER::VB<LVB3_DCSIZE, LVB3_DCCOUNT> LVB3;
                    typedef VBUFFER::VB<LVB4_DCSIZE, LVB4_DCCOUNT> LVB4;
                    LVB1 lvb1;
                    LVB2 lvb2;
                    LVB3 lvb3;
                    LVB4 lvb4;

                    typedef VBUFFER::VB<HTVB1_DCSIZE, HTVB1_DCCOUNT> HTVB1;
                    typedef VBUFFER::VB<HTVB2_DCSIZE, HTVB2_DCCOUNT> HTVB2;
                    typedef VBUFFER::VB<HTVB3_DCSIZE, HTVB3_DCCOUNT> HTVB3;
                    typedef VBUFFER::VB<HTVB4_DCSIZE, HTVB4_DCCOUNT> HTVB4;
                    HTVB1 htvb1;
                    HTVB2 htvb2;
                    HTVB3 htvb3;
                    HTVB4 htvb4;
                };
            } viterbi_fifo;
            viterbi_fifo                            m_viterbi_fifo;
            dsp_viterbi                             m_viterbi;
            dsp_viterbi::input_param                m_viterbi_param;
            dsp_viterbi::Trellis<5000 * 8, 12>      m_viterbi_trellis;
            rate                                    m_rate;
            status                                  m_status;
            v_align(16) unsigned __int8             m_buffer[216];
            int                                     m_decoded_bytes;

        public:
            void setup_param(rate r, int totalbits);
            viterbi_decoder();
            ~viterbi_decoder();
            void decode_sequential();
            void decode_parallel();
            dsp_block::state general_work();
            void reset();
            void set_rate(rate r){m_rate = r;}
        };
        //////////////////////////////////////////////////////////////////////////
        class header_parser : public dsp_block
        {
        private:
            unsigned int m_signal;
            unsigned int m_length;
            unsigned int m_rate;
            unsigned int m_dbps;
            unsigned int m_symbol_count;
            bool         m_ok;
        protected:
            static const unsigned int DBPSLookUp[16];
            bool parse();
        public:
        	header_parser::header_parser();
        	header_parser::~header_parser();
        	dsp_block::state general_work();
        	void reset();
            rate data_rate() const {return rate(m_rate);}
            unsigned int data_dbps() const {return m_dbps;}
            unsigned int data_length() const {return m_length;}
            unsigned int symbol_count() const {return m_symbol_count;}
            bool header_ok() const {return m_ok;}
        };
        //////////////////////////////////////////////////////////////////////////
        class pilot_tracking : public dsp_block
        {
        private:
            short m_theta;
        protected:
            short tracking(complex16* pc);
        public:
        	pilot_tracking::pilot_tracking();
        	pilot_tracking::~pilot_tracking();
        	dsp_block::state general_work();
        	void reset();
            short phase() const {return m_theta;}
        };
        //////////////////////////////////////////////////////////////////////////
        class descramble : public dsp_block
        {
        private:
            descrambler::dot11a_descrambler m_descrambler;
            int m_counter;
        protected:
        
        public:
        	descramble::descramble();
        	descramble::~descramble();
        	dsp_block::state general_work();
        	void reset();
        };
        //////////////////////////////////////////////////////////////////////////
        class crc_check : public dsp_block
        {
        private:
            crc::crc32 m_crc;
            int m_length;
            int m_counter;
            bool m_crc_check_passed;
        protected:
        
        public:
        	crc_check::crc_check();
        	crc_check::~crc_check();
        	dsp_block::state general_work();
        	void reset();
            bool crc_check_passed();
            void set_length(int length){m_length = length;}
        };

        class bb_switch : public dsp_switch_block
        {
        public:
        	bb_switch::bb_switch();
        	bb_switch::~bb_switch();
        	dsp_block::state general_work();
        	void reset();
        };

        class dot11a_context
        {
        public:
            dot11a_context(){memset(this, 0, sizeof(dot11a_context));}
            int plcp_state;
            short frequency_offset;
            short frequency_offset_delta;
            v_align(16) complex16        m_channel_state[64];
        };
    }
}