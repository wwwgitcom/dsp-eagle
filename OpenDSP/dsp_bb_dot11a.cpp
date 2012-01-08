#include "dsp_bb_dot11a.h"
#include "dsp_log.h"
#include <iostream>
using namespace std;

namespace OpenDSP
{
    extern dsp_log& log;

    namespace dot11a
    {

        //////////////////////////////////////////////////////////////////////////
        remove_dc::remove_dc()
            : dsp_block("remove_dc", dsp_io_signature(1, sizeof(complex16)), dsp_io_signature(1, sizeof(complex16))),
            m_counter(0), m_length(64), m_dc(0)
        {
            //m_pBefore = new dsp_draw_window("before remove dc", 0, 0, 100, 100);
            //m_pAfter  = new dsp_draw_window("before remove dc", 0, 0, 100, 100);
        }
        remove_dc::~remove_dc()
        {
            //delete m_pBefore;
            //delete m_pAfter;
        }
        void remove_dc::reset()
        {
        }
        dsp_block::state remove_dc::general_work()
        {
            complex16* ip = input<complex16>(0);
            complex16* op = output<complex16>(0);

            //printf("%s: in: %d, out: %d\n", name().c_str(), ninput(0), noutput(0));

            if (ninput(0) < m_length)
            {
                //SleepEx(100, true);
                //cout << name() << " blocked on input" << endl;
                return BLKD_IN;
            }
            if (noutput(0) < m_length)
            {
                log("%s: blocked on output %d\n", name().c_str(), noutput(0));
                //SleepEx(100, true);
                
                return BLKD_OUT;
            }

            //m_pBefore->DrawScatter(ip, m_length);

            m_dc = 0;
            for (unsigned int i = 0; i < m_length; i++)
            {
                m_dc += ip[i];
            }

            m_dc >>= 6;

            //cout << "dc " << m_dc.re << ", " << m_dc.im << endl;

            for (unsigned int i = 0; i < m_length; i++)
            {
                op[i].re = ip[i].re - (__int16)m_dc.re;
                op[i].im = ip[i].im - (__int16)m_dc.im;
            }

            //m_pAfter->DrawScatter(op, m_length);

            consume(0, m_length);
            produce(0, m_length);
            return READY;
        }
        //////////////////////////////////////////////////////////////////////////
        carrier_sense::carrier_sense()
            : dsp_block(
            "dsp_dot11a_carrier_sense",
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(0)),
            m_corr_direction(corr_none), m_accumulate_samples(0)
        {
            reset();
        }

        carrier_sense::~carrier_sense()
        {

        }
        
        int carrier_sense::decision()
        {
            int i = 0;
            if (m_corr_direction == corr_none)
            {
                for (; i < m_corr_length; i++)
                {
                    if (m_corr_sqr[i]  > (m_energy_sqr[i] >> 1))
                    {
                        m_corr_direction = corr_up;
                        m_pos_cnt  = 0;
                        log("%s: sync up: %d\n", name().c_str(), i);
                        
                        break;
                    }
                }
            }
            if (m_corr_direction == corr_up)
            {
                for (; i < m_corr_length; i++)
                {
                    if (m_corr_sqr[i] < (m_energy_sqr[i] >> 3))
                    {
                        if (m_pos_cnt > 96 && m_pos_cnt < 160)
                        {
                            m_corr_direction = corr_down;
                            reset();
                            log("%s: sync down: %d\n", name().c_str(), i);
                            
                            return i;
                        }
                        else
                        {
                            m_corr_direction = corr_error;
                            m_pos_cnt  = 0;
                        }
                    }
                    else
                    {
                        m_pos_cnt++;

                        if (m_pos_cnt > 320)
                        {
                            m_pos_cnt = 0;
                            m_corr_direction = corr_error;
                            break;
                        }
                    }
                }
            }
            
            return m_corr_length;
        }

        dsp_block::state carrier_sense::general_work()
        {
            dsp_block::state st = dsp_block::READY;

            complex16* ip = input<complex16>(0);

            if (ninput(0) < m_accumulate_samples + m_corr_length)
            {
                return BLKD_IN;
            }

            ip += m_accumulate_samples;

            complex32 fc;
            complex32 corr;
            int energy;
            for (int i = 0; i < m_corr_length; i++)
            {
                fc.re = (int)ip[i].re;
                fc.im = (int)ip[i].im;

                corr.re = fc.re * m_his_sample[m_his_index].re + fc.im * m_his_sample[m_his_index].im;
                corr.im = fc.im * m_his_sample[m_his_index].re - fc.re * m_his_sample[m_his_index].im;

                energy        = fc.re * fc.re + fc.im * fc.im;
                energy       >>= 5;
                m_energy_sum -= m_his_energy[m_his_index];
                m_energy_sum += energy;

                corr >>= 5;
                m_corr_sum -= m_his_corr[m_his_index];
                m_corr_sum += corr;

                m_his_sample[m_his_index]   = ip[i];
                m_his_corr[m_his_index]   = corr;
                m_his_energy[m_his_index] = energy;

                m_energy_sqr[i] = ((__int64)m_energy_sum * (__int64)m_energy_sum);
                m_corr_sqr[i]   = ((__int64)m_corr_sum.re * (__int64)m_corr_sum.re + (__int64)m_corr_sum.im * (__int64)m_corr_sum.im);

                //cout << energy << "\t" << m_energy_sum << endl;
                //if (m_sense_ok)
                //printf("%d. %d \t %I64d \t %I64d\n", i, energy, m_energy_sqr[i], m_corr_sqr[i]);
                //printf("energy %I64d <=> corr %I64d\n", m_energy_sqr[i] >> 15, m_corr_sqr[i] >> 15);

                m_his_index++;
                m_his_index %= m_corr_length;
            }
            
            int n = decision();


            if (m_corr_direction == corr_up)
            {
                m_accumulate_samples += n;
                //cout << name() << " up acc " << m_accumulate_samples << endl;
            }
            else if (m_corr_direction == corr_down)
            {
                n &= ~0x3;
                m_accumulate_samples += n;
                //cout << name() << " down acc " << m_accumulate_samples << endl;

                consume(0, m_accumulate_samples - m_corr_length);
                m_corr_direction = corr_none;
                m_accumulate_samples = 0;
                return DONE;
            }
            else if (m_corr_direction == corr_error)
            {
                m_accumulate_samples += n;
                //cout << name() << " error acc " << m_accumulate_samples << endl;
                consume(0, m_accumulate_samples);
                m_corr_direction = corr_none;
                m_accumulate_samples = 0;
            }
            else
            {
                consume(0, n);
            }
            
            return READY;
        }

        void carrier_sense::reset()
        {
            m_his_index      = 0;
            m_energy_sum     = 0;
            m_corr_sum       = 0;
            memset(m_his_energy, 0, sizeof(int) * m_corr_length);
            memset(m_his_sample, 0, sizeof(complex16) * m_corr_length);
            memset(m_his_corr, 0, sizeof(complex32) * m_corr_length);
        }

        //////////////////////////////////////////////////////////////////////////
        ofdm_symbol_boundary_check::ofdm_symbol_boundary_check()
            : dsp_block(
            "dsp_dot11a_symbol_boundary_check",
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(0))
        {
            reset();
        }

        ofdm_symbol_boundary_check::~ofdm_symbol_boundary_check()
        {

        }

        dsp_block::state ofdm_symbol_boundary_check::general_work()
        {
            dsp_block::state st = dsp_block::READY;

            complex16* ip = input<complex16>(0);
            
            if (ninput(0) < 192)
            {
                return BLKD_IN;
            }

            complex32 fc;
            complex32 corr;
            __int64 corr_sqr     = 0;
            __int64 max_corr_sqr = 0;
            int     max_pos      = 0;

            for (int i = 128; i < 192; i++)
            {
                fc.re = (int)ip[i].re;
                fc.im = (int)ip[i].im;

                corr.re = fc.re * ip[i - 128].re + fc.im * ip[i - 128].im;
                corr.im = fc.im * ip[i - 128].re - fc.re * ip[i - 128].im;
                
                corr >>= 5;
                m_corr_sum -= m_his_corr[m_his_index];
                m_corr_sum += corr;

                m_his_corr[m_his_index]   = corr;

                corr_sqr = ((__int64)m_corr_sum.re * (__int64)m_corr_sum.re + (__int64)m_corr_sum.im * (__int64)m_corr_sum.im);

                //cout << corr_sqr << "\t" << max_corr_sqr << endl;

                if (corr_sqr > max_corr_sqr)
                {
                    max_corr_sqr = corr_sqr;
                    max_pos = i;
                }
                
                m_his_index++;
                m_his_index %= m_corr_length;
            }

            int boundary_pos = max_pos - 128 - 8;
            boundary_pos &= ~0x3;

            log("%s: peak corr @ %d, boundary @ %d\n", name().c_str(), max_pos, boundary_pos);
            consume(0, boundary_pos);

            return DONE;
        }

        void ofdm_symbol_boundary_check::reset()
        {
            m_his_index = 0;
            m_corr_sum = 0;
            memset(m_his_corr, 0, sizeof(complex32) * m_corr_length);
        }


        //////////////////////////////////////////////////////////////////////////
        carrier_frequency_estimation::carrier_frequency_estimation()
            : dsp_block("carrier_frequency_estimation", 
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(0))
        {

        }

        carrier_frequency_estimation::~carrier_frequency_estimation()
        {

        }


        dsp_block::state carrier_frequency_estimation::general_work()
        {
            dsp_block::state st = dsp_block::READY;

            complex16* ip = input<complex16>(0);

            if (ninput(0) < 2 * m_estimation_length)
            {
                return BLKD_IN;
            }

            m_frequency_offset = m_frequency_estimator.estimate_i(ip, m_estimation_length, m_estimation_length);
            log("frequency offset = %d\n", m_frequency_offset);
            //consume(0, 2 * m_estimation_length);

            return DONE;
        }

        void carrier_frequency_estimation::reset()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        carrier_frequency_compensation::carrier_frequency_compensation()
            : dsp_block("carrier_frequency_compensation", 
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(1, sizeof(complex16))), m_compensate_length(64)
        {
        }

        carrier_frequency_compensation::~carrier_frequency_compensation()
        {

        }

        void carrier_frequency_compensation::set_frequency_offset(short frequency_offset)
        {
            m_cfo_theta_i   = 0;
            m_cfo_delta_i   = 0;
            m_cfo_step_i    = frequency_offset;
        }

        dsp_block::state carrier_frequency_compensation::general_work()
        {
            complex16* ip = input<complex16>(0);
            complex16* op = output<complex16>(0);

            if (ninput(0) < m_compensate_length)
            {
                return BLKD_IN;
            }

            if (noutput(0) < m_compensate_length)
            {
                return BLKD_OUT;
            }

            log("%s: delta=%d, step=%d, theta=%d\n", name().c_str(), m_cfo_delta_i, m_cfo_step_i, m_cfo_theta_i);

            m_frequency_compensator.make_coeff_i(m_cfo_coeff_i, m_compensate_length, m_cfo_delta_i, m_cfo_step_i, m_cfo_theta_i);
            m_frequency_compensator.compensate_i(ip, m_cfo_coeff_i, op, m_compensate_length);

            consume(0, m_compensate_length);
            produce(0, m_compensate_length);
            return READY;
        }

        void carrier_frequency_compensation::reset()
        {
            m_compensate_length = 64;
        }
        //////////////////////////////////////////////////////////////////////////
        channel_estimation::channel_estimation()
            : dsp_block("channel_estimation", 
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(0))
        {
            //m_draw = new dsp_draw_window("dot11a channel estimation", 0, 0, 400, 400);
            memset(m_channel_state, 0, sizeof(complex16) * 64);
        }

        channel_estimation::~channel_estimation()
        {
            //delete m_draw;
        }

        const char channel_estimation::m_preamble[64] =
        {
            0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1,
            1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
            1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1
        };

        void channel_estimation::channel_estimation_i(complex16* pcsin, complex16* pcschannel, int count)
        {
            __int32 sq;
            __int32 ire, iim;
            int i;
            for (i = 1; i <= 26; i++)
            {
                ire = (__int32)pcsin[i].re;
                iim = (__int32)pcsin[i].im;
                sq  = ire * ire + iim * iim;

                if (sq == 0)
                {
                    continue;
                }

                ire <<= 16;
                iim <<= 16;

                //sq >>= 16;

                if (m_preamble[i])
                {
                    ire = (ire + (sq >> 1)) / sq;
                    iim = (iim + (sq >> 1)) / sq;

                    //ire >>= 16;
                    //iim >>= 16;

                    pcschannel[i].re = (__int16)ire;
                    pcschannel[i].im = (__int16)-iim;
                }
                else
                {
                    ire = (ire + (sq >> 1)) / sq;
                    iim = (iim + (sq >> 1)) / sq;

                    //ire >>= 16;
                    //iim >>= 16;

                    pcschannel[i].re = (__int16)-ire;
                    pcschannel[i].im = (__int16)iim;
                }
            }
            for (i = 64 - 26; i < 64; i++)
            {
                ire = (__int32)pcsin[i].re;
                iim = (__int32)pcsin[i].im;
                sq  = ire * ire + iim * iim;

                if (sq == 0)
                {
                    continue;
                }
                ire <<= 16;
                iim <<= 16;

                //sq >>= 16;

                if (m_preamble[i])
                {
                    ire = (ire + (sq >> 1)) / sq;
                    iim = (iim + (sq >> 1)) / sq;

                    //ire >>= 16;
                    //iim >>= 16;

                    pcschannel[i].re = (__int16)ire;
                    pcschannel[i].im = (__int16)-iim;
                }
                else
                {
                    ire = (ire + (sq >> 1)) / sq;
                    iim = (iim + (sq >> 1)) / sq;

                    //ire >>= 16;
                    //iim >>= 16;

                    pcschannel[i].re = (__int16)-ire;
                    pcschannel[i].im = (__int16)iim;
                }
            }
        }

        dsp_block::state channel_estimation::general_work()
        {
            complex16* ip = input<complex16>(0);

            if (ninput(0) < 2 * m_estimation_length)
            {
                return BLKD_IN;
            }

            channel_estimation_i(ip, m_channel_state, 64);

            //m_draw->DrawSqrt(m_channel_state, 64);

            consume(0, 2 * m_estimation_length);

            return DONE;
        }

        void channel_estimation::reset()
        {
        }

        // TODO:::: how to make all blocks share some data.....without copying....
        //////////////////////////////////////////////////////////////////////////
        channel_compensation::channel_compensation()
            : dsp_block("channel_compensation", 
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(1, sizeof(complex16)))
        {
            //m_draw = new dsp_draw_window("dot11a channel_compensation", 0, 0, 400, 400);
        }

        channel_compensation::~channel_compensation()
        {
            //delete m_draw;
        }

        void channel_compensation::channel_compensation_i(complex16* pcsin, complex16* pcschannel, complex16* pcsout)
        {
            __int32 ire, iim;
            for (int i = 0; i < m_compensation_length; i++)
            {
                ire = (__int32)pcsin[i].re * (__int32)pcschannel[i].re - (__int32)pcsin[i].im * (__int32)pcschannel[i].im;
                iim = (__int32)pcsin[i].re * (__int32)pcschannel[i].im + (__int32)pcsin[i].im * (__int32)pcschannel[i].re;

                // finally, normalize it to 128
                pcsout[i].re = (__int16)(ire >> 9);
                pcsout[i].im = (__int16)(iim >> 9);
            }
        }

        dsp_block::state channel_compensation::general_work()
        {
            complex16* ip = input<complex16>(0);
            complex16* op = output<complex16>(0);

            if (ninput(0) < m_compensation_length)
            {
                return BLKD_IN;
            }
            if (noutput(0) < m_compensation_length)
            {
                return BLKD_OUT;
            }
            log("%s: input items = %d, output space = %d, produced = %d\n", 
                name().c_str(), ninput(0), noutput(0), m_compensation_length);
            channel_compensation_i(ip, m_channel_state, op);

            //m_draw->DrawSqrt(op, 64);
            //m_draw->DrawScatter(op, 64);

            consume(0, m_compensation_length);
            produce(0, m_compensation_length);
            return READY;
        }

        void channel_compensation::reset()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        remove_gi::remove_gi()
            : dsp_block("remove_gi",
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(0)), m_gi_length(16)
        {
        }
        remove_gi::~remove_gi()
        {
        }

        dsp_block::state remove_gi::general_work()
        {
            complex16* ip = input<complex16>(0);

            if (m_symbol_count == 0)
            {
                return DONE;
            }

            if (ninput(0) < 80)
            {
                return BLKD_IN;
            }

            log("%s: input items = %d, consume = %d\n", name().c_str(), ninput(0), m_gi_length);
            consume(0, m_gi_length);

            m_symbol_count--;
            return READY;
        }

        void remove_gi::reset()
        {
            m_gi_length = 16;
        }
        //////////////////////////////////////////////////////////////////////////
        fft::fft()
            : dsp_block("fft",
            dsp_io_signature(1, sizeof(complex16)),
            dsp_io_signature(1, sizeof(complex16)))
        {
        }
        fft::~fft()
        {
        }

        dsp_block::state fft::general_work()
        {
            complex16* ip = input<complex16>(0);
            complex16* op = output<complex16>(0);

            if (ninput(0) < 64)
            {
                return BLKD_IN;
            }
            
            log("%s: input items = %d, output space = %d, produced = %d\n", name().c_str(), ninput(0), noutput(0), 64);
            //cout << name() << " in: " << ninput(0) << " out: " << noutput(0) << endl;

            m_fft_executor((v_cs*)ip, (v_cs*)op);

            consume(0, 64);
            produce(0, 64);

            return READY;
        }

        void fft::reset()
        {
        }
        //////////////////////////////////////////////////////////////////////////
        demap::demap()
            : dsp_block("demap", dsp_io_signature(1, sizeof(complex16)), dsp_io_signature(1, sizeof(unsigned __int8))),
            m_rate(DOT11A_RATE_6M)
        {
        }
        demap::~demap()
        {
        }
        void demap::reset()
        {
            m_rate = DOT11A_RATE_6M;
        }
        dsp_block::state demap::general_work()
        {
            complex16* ip = input<complex16>(0);
            unsigned __int8* op = output<unsigned __int8>(0);

            if (ninput(0) < 64)
            {
                return BLKD_IN;
            }
            
            int noutput_required = 0;
            switch (m_rate)
            {
            case DOT11A_RATE_6M:
            case DOT11A_RATE_9M:
                noutput_required = 48;
                break;
            case DOT11A_RATE_12M:
            case DOT11A_RATE_18M:
                noutput_required = 48 * 2;
                break;
            case DOT11A_RATE_24M:
            case DOT11A_RATE_36M:
                noutput_required = 48 * 4;
                break;
            case DOT11A_RATE_48M:
            case DOT11A_RATE_54M:
                noutput_required = 48 * 6;
                break;
            }
            if (noutput(0) < noutput_required)
            {
                return BLKD_OUT;
            }

            log("%s: input items = %d, output space = %d, produced = %d\n", name().c_str(), ninput(0), noutput(0), noutput_required);

            if (m_rate == DOT11A_RATE_6M || m_rate == DOT11A_RATE_9M)
            {
                m_demapper.demap_limit_bpsk((v_cs*)ip, 16);
                _demap<-26, 26>(ip, op,
                    [&](complex16& cInput, unsigned __int8* &pOutput){
                        m_demapper.demap_bpsk_i(cInput, pOutput);
                        pOutput++;
                });
            }
            else if (m_rate == DOT11A_RATE_12M || m_rate == DOT11A_RATE_18M)
            {
                m_demapper.demap_limit_qpsk((v_cs*)ip, 16);
                _demap<-26, 26>(ip, op,
                    [&](complex16& cInput, unsigned __int8* &pOutput){
                        m_demapper.demap_qpsk(cInput, pOutput);
                        pOutput += 2;
                });
            }
            else if (m_rate == DOT11A_RATE_24M || m_rate == DOT11A_RATE_36M)
            {
                m_demapper.demap_limit_16qam((v_cs*)ip, 16);
                _demap<-26, 26>(ip, op,
                    [&](complex16& cInput, unsigned __int8* &pOutput){
                        m_demapper.demap_bpsk_i(cInput, pOutput);
                        pOutput += 4;
                });
            }
            else if (m_rate == DOT11A_RATE_48M || m_rate == DOT11A_RATE_54M)
            {
                m_demapper.demap_limit_64qam((v_cs*)ip, 16);
                _demap<-26, 26>(ip, op,
                    [&](complex16& cInput, unsigned __int8* &pOutput){
                        m_demapper.demap_bpsk_i(cInput, pOutput);
                        pOutput += 6;
                });
            }

#if 0
            printf("=============\n");
            for (int i = 0; i < noutput_required ; i++)
            {
                printf("%d ", op[i]);
            }
            printf("\n=============\n");
#endif

            consume(0, 64);
            produce(0, noutput_required);

            return READY;
        }

        //////////////////////////////////////////////////////////////////////////
        deinterleave::deinterleave()
            : dsp_block("deinterleave", dsp_io_signature(1, sizeof(unsigned __int8)), dsp_io_signature(1, sizeof(unsigned __int8))),
            m_rate(DOT11A_RATE_6M)
        {
        }
        deinterleave::~deinterleave()
        {
        }
        void deinterleave::reset()
        {
            m_rate = DOT11A_RATE_6M;
        }
        dsp_block::state deinterleave::general_work()
        {
            unsigned __int8* ip = input<unsigned __int8>(0);
            unsigned __int8* op = output<unsigned __int8>(0);
            
            int nrequired = 0;
            switch (m_rate)
            {
            case DOT11A_RATE_6M:
            case DOT11A_RATE_9M:
                nrequired = 48;
                break;
            case DOT11A_RATE_12M:
            case DOT11A_RATE_18M:
                nrequired = 48 * 2;
                break;
            case DOT11A_RATE_24M:
            case DOT11A_RATE_36M:
                nrequired = 48 * 4;
                break;
            case DOT11A_RATE_48M:
            case DOT11A_RATE_54M:
                nrequired = 48 * 6;
                break;
            default:
                throw::invalid_argument(name() + ": invalid parameter of deinterleve rate");
                break;
            }
            if (ninput(0) < nrequired)
            {
                return BLKD_IN;
            }
            if (noutput(0) < nrequired)
            {
                return BLKD_OUT;
            }

            log("%s: input items = %d, output space = %d, produced = %d\n", name().c_str(), ninput(0), noutput(0), nrequired);
            
            if (m_rate == DOT11A_RATE_6M || m_rate == DOT11A_RATE_9M)
            {
                m_deinterleaver._1bpsc(ip, op);
            }
            else if (m_rate == DOT11A_RATE_12M || m_rate == DOT11A_RATE_18M)
            {
                m_deinterleaver._2bpsc(ip, op);
            }
            else if (m_rate == DOT11A_RATE_24M || m_rate == DOT11A_RATE_36M)
            {
                m_deinterleaver._4bpsc(ip, op);
            }
            else if (m_rate == DOT11A_RATE_48M || m_rate == DOT11A_RATE_54M)
            {
                m_deinterleaver._6bpsc(ip, op);
            }
#if 0
            printf("=============\n");
            for (int i = 0; i < nrequired ; i++)
            {
            	printf("<%d -> %d> ", ip[i], op[i]);
            }
            printf("\n=============\n");
#endif
            consume(0, nrequired);
            produce(0, nrequired);

            return READY;
        }

        //////////////////////////////////////////////////////////////////////////
      
        viterbi_decoder::viterbi_decoder()
        	: dsp_block("viterbi_decoder", 
        		dsp_io_signature(1, sizeof(unsigned __int8)), 
        		dsp_io_signature(1, sizeof(unsigned __int8))),
                m_status(header)
        {
        
        }
        
        viterbi_decoder::~viterbi_decoder()
        {
        
        }

        void viterbi_decoder::setup_param(rate r, int totalbits)
        {
            m_viterbi_param.pTrellis     = (vub*)m_viterbi_trellis.vtrellis;
            m_viterbi.init_viterbi(m_viterbi_param);
            m_viterbi_param.nCodingRate  = r;
            m_viterbi_param.nTotalBits   = totalbits;
            
            switch(r)
            {
            case DOT11A_RATE_6M:
                m_viterbi_param.nTraceBackDepth      = 36;
                m_viterbi_param.nTraceBackOutputBits = 24;
                m_viterbi_param.nTraceBackNormMask   = 0x7;
                break;
            case DOT11A_RATE_9M:
                m_viterbi_param.nTraceBackDepth      = 72;
                m_viterbi_param.nTraceBackOutputBits = 72;
                m_viterbi_param.nTraceBackNormMask   = 0x3;
                break;
            case DOT11A_RATE_12M:
                m_viterbi_param.nTraceBackDepth      = 36;
                m_viterbi_param.nTraceBackOutputBits = 48;
                m_viterbi_param.nTraceBackNormMask   = 0x7;
                break;
            case DOT11A_RATE_18M:
                m_viterbi_param.nTraceBackDepth      = 72;
                m_viterbi_param.nTraceBackOutputBits = 72;
                m_viterbi_param.nTraceBackNormMask   = 0x7;
                break;
            case DOT11A_RATE_24M:
                m_viterbi_param.nTraceBackDepth      = 36;
                m_viterbi_param.nTraceBackOutputBits = 96;
                m_viterbi_param.nTraceBackNormMask   = 0x7;
                break;
            case DOT11A_RATE_36M:
                m_viterbi_param.nTraceBackDepth      = 72;
                m_viterbi_param.nTraceBackOutputBits = 144;
                m_viterbi_param.nTraceBackNormMask   = 0x3;
                break;
            case DOT11A_RATE_48M:
                m_viterbi_param.nTraceBackDepth      = 36;
                m_viterbi_param.nTraceBackOutputBits = 192;
                m_viterbi_param.nTraceBackNormMask   = 0x7;
                break;
            case DOT11A_RATE_54M:
                m_viterbi_param.nTraceBackDepth      = 36;
                m_viterbi_param.nTraceBackOutputBits = 216;
                m_viterbi_param.nTraceBackNormMask   = 0x7;
                break;
            }

            m_decoded_bytes = 0;
            m_status        = frame;
        }
        
        dsp_block::state viterbi_decoder::general_work()
        {
        	unsigned __int8 *ip = input<unsigned __int8>(0);
        	unsigned __int8 *op = output<unsigned __int8>(0);
            int ninput_required = 0;
            int noutput_required = 0;
            int nout = 0;
            int nin = ninput(0);
            
            if (m_status == header)
            {
                ninput_required  = 48;
                noutput_required = 3;

                if (ninput(0) < ninput_required)
                {
                    return BLKD_IN;
                }
                if (noutput(0) < noutput_required)
                {
                    return BLKD_OUT;
                }

                memcpy(m_buffer, ip, 48);
                memset(&m_buffer[48], 0, 48);
                m_viterbi_param.nTotalBits         = 24;
                m_viterbi_param.nTraceBackNormMask = 0x7;
                m_viterbi_param.nTraceBackDepth    = 12;

                m_viterbi.viterbi12_seq(m_viterbi_param, (vub*)m_viterbi_trellis.vtrellis, (char*)m_buffer, (char *)op);

                log("%s: header : %02x %02x %02x\n", name().c_str(), op[0], op[1], op[2]);

                consume(0, ninput_required);
                produce(0, noutput_required);
            }
            else if (m_status == frame)
            {
                if (nin <= 0)
                {
                    return BLKD_IN;
                }
                nout = m_viterbi.viterbi_blk(m_viterbi_param, ip, nin, op);
                int n = m_viterbi_param.nTotalBits - m_viterbi_param.nDecodedBits;
                // test if need flush
                if (n < m_viterbi_param.nTraceBackOutputBits)
                {
                    m_viterbi.viterbi_blk(m_viterbi_param, m_buffer, m_viterbi_param.nTraceBackOutputBits - n + m_viterbi_param.nTraceBackDepth, op);
                    nout += (n - 6) / 8;
                }
                m_decoded_bytes += nout;

                log("%s: input items = %d, output space = %d, decoded bytes = %d, total bits = %d\n",
                    name().c_str(), ninput(0), noutput(0), m_decoded_bytes, m_viterbi_param.nTotalBits);
                
                consume(0, nin);
                produce(0, nout);
            }
        
        	return READY;
        }
        void viterbi_decoder::reset()
        {
            m_status        = header;
            m_decoded_bytes = 0;            
        }
        
        //////////////////////////////////////////////////////////////////////////
        const unsigned int header_parser::DBPSLookUp[16] = 
        {
            /* R1-4 */
            /* 0000 */ 0,
            /* 1000 */ 0,
            /* 0100 */ 0,
            /* 1100 */ 0,
            /* 0010 */ 0,
            /* 1010 */ 0,
            /* 0110 */ 0,
            /* 1110 */ 0,

            /* 0001 */ 192,
            /* 1001 */ 96,
            /* 0101 */ 48,
            /* 1101 */ 24,
            /* 0011 */ 216,
            /* 1011 */ 144,
            /* 0111 */ 72,
            /* 1111 */ 36,
        };
        
        header_parser::header_parser()
        	: dsp_block("header_parser", 
        		dsp_io_signature(1, sizeof(unsigned __int8)), 
        		dsp_io_signature(0))
        {
        
        }
        
        header_parser::~header_parser()
        {
        
        }

        bool header_parser::parse()
        {
            unsigned int uiParity;

            m_signal &= 0xFFFFFF;
            if (m_signal & 0xFC0010) // all these bits should be always zero
            {
                log("%s: signal parse error... some bits should be zero\n", name().c_str());
                return FALSE;
            }

            uiParity = (m_signal >> 16) ^ (m_signal);
            uiParity = (uiParity >> 8) ^ (uiParity);
            uiParity = (uiParity >> 4) ^ (uiParity);
            uiParity = (uiParity >> 2) ^ (uiParity);
            uiParity = (uiParity >> 1) ^ (uiParity);
            if (uiParity & 0x1)
            {
                log("%s: signal parse error... parity check failed\n", name().c_str());
                return FALSE;
            }

            m_rate = m_signal & 0xF;
            if (!((m_rate) & 0x8))
            {
                log("%s: signal parse error... invalid rate\n", name().c_str());
                return FALSE;
            }

            m_length = (m_signal >> 5) & 0xFFF;
            m_dbps = DBPSLookUp[m_rate];

            log("%s: rate=%02x, length=%d, dbps=%d\n", name().c_str(), m_rate, m_length, m_dbps);

            m_symbol_count = (m_length * 8 + 16 + 6 + m_dbps) / m_dbps;

            return TRUE;
        }
        
        dsp_block::state header_parser::general_work()
        {
        	unsigned __int8 *ip = input<unsigned __int8>(0);
        
        	if (ninput(0) < 3)
        	{
        		return BLKD_IN;
        	}
            m_signal = *((unsigned int*)ip);
            m_ok = parse();

            consume(0, 3);
        
        	return DONE;
        }
        void header_parser::reset()
        {
        
        }

        //////////////////////////////////////////////////////////////////////////
        pilot_tracking::pilot_tracking()
        	: dsp_block("pilot_tracking", 
        		dsp_io_signature(1, sizeof(complex16)), 
        		dsp_io_signature(0)), m_theta(0)
        {        
        }
        
        pilot_tracking::~pilot_tracking()
        {        
        }

        short pilot_tracking::tracking(complex16* pc)
        {
            short th1 = dsp_math::atan(pc[64 - 21].re, pc[64 - 21].im);
            short th2 = dsp_math::atan(pc[64 - 07].re, pc[64 - 07].im);
            short th3 = dsp_math::atan(pc[07].re, pc[07].im);
            short th4 = dsp_math::atan(pc[21].re, pc[21].im);

            short ThisTheta = (th1 + th2 + th3 + th4) >> 2;

            return ThisTheta;
        }
        
        dsp_block::state pilot_tracking::general_work()
        {
        	complex16 *ip = input<complex16>(0);
            if (ninput(0) < 64)
            {
                return BLKD_IN;
            }
            m_theta = tracking(ip);
        
        	return READY;
        }
        void pilot_tracking::reset()
        {
            m_theta = 0;
        }
        //////////////////////////////////////////////////////////////////////////
        descramble::descramble()
        	: dsp_block("descramble", 
        		dsp_io_signature(1, sizeof(unsigned __int8)), 
        		dsp_io_signature(1, sizeof(unsigned __int8))), 
                m_counter(0), m_descrambler(0)
        {
        }
        
        descramble::~descramble()
        {
        }
        
        dsp_block::state descramble::general_work()
        {
        	unsigned __int8 *ip = input<unsigned __int8>(0);
        	unsigned __int8 *op = output<unsigned __int8>(0);
        
            int i = 0;
            int nin = ninput(0);
            int nout = 0;
        	
            if (ninput(0) < 2)
            {
                return BLKD_IN;
            }

            if (noutput(0) < nin)
        	{
        		return BLKD_OUT;
        	}

            if (m_counter < 2)
            {
                m_descrambler.reset(ip[1]);
                m_counter += 2;
                i = 2;
            }

            for (; i < nin ; nout++, i++)
            {
            	op[nout] = m_descrambler(ip[i]);
            }
        
            log("%s: input items = %d, output space = %d, produced = %d\n", name().c_str(), ninput(0), noutput(0), nout);

        	consume(0, nin);
            produce(0, nout);
        
        	return READY;
        }
        void descramble::reset()
        {
            m_counter = 0;
        }
        //////////////////////////////////////////////////////////////////////////
        crc_check::crc_check()
        	: dsp_block("crc_check", 
        		dsp_io_signature(1, sizeof(unsigned __int8)), 
        		dsp_io_signature(0)),
                m_counter(0), m_length(0), m_crc_check_passed(false)
        {
        }
        
        crc_check::~crc_check()
        {
        }
        
        dsp_block::state crc_check::general_work()
        {
        	unsigned __int8 *ip = input<unsigned __int8>(0);
            int nitems = ninput(0);

            if (nitems <= 0)
            {
                return BLKD_IN;
            }

            int i = 0;

            for (; i < nitems && m_counter < m_length - 4; i++)
            {
                m_crc(ip[i]);
                m_counter++;
            }

            log("%s: ninput=%d, length=%d, counter=%d\n", 
                name().c_str(), nitems, m_length, m_counter);

            if (m_counter == m_length - 4 && nitems - i >= 4)
            {
                m_crc_check_passed = (*((unsigned int*)&ip[i]) == m_crc.value());
                log("%s: crc passed? %d\n", name().c_str(), m_crc_check_passed);
                consume(0, nitems);
                return DONE;
            }

            consume(0, i);
        
        	return READY;
        }
        void crc_check::reset()
        {
            m_counter           = 0;
            m_length            = 0;
            m_crc_check_passed  = false;
            m_crc.reset();
        }





    }


    
}