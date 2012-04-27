#pragma once

#include "windows.h"
#include "math.h"

#define dot11a_mode         0
#define use_fixed_point     1
#define use_sse             1
#define draw_enabled        0
#define perf_enabled        1
#define perf_unit_enabled   0


#if perf_enabled
#define _perf_begin() m_perf.perf_begin()
#else
#define _perf_begin(...)
#endif

#if perf_enabled
#define _perf_tick() m_perf.perf_tick()
#else
#define _perf_tick(...)
#endif

#if perf_enabled
#define _perf_end() m_perf.perf_end()
#else
#define _perf_end(...)
#endif

#if perf_enabled
#define _perf_report() m_perf.perf_report()
#else
#define _perf_report(...)
#endif




#include "_vector128.h"
#include "dsp_vector1.h"
#include "dsp_complex.h"
#include "dsp_tickcount.h"
#include "dsp_crc.h"
#include "dsp_fft.h"
#include "dsp_mapper.h"
#include "dsp_encode.h"
#include "dsp_interleave.h"
#include "dsp_ofdm.h"
#include "dsp_scramble.h"
#include "dsp_source.h"
#include "dsp_acorr.h"
#include "dsp_freqest.h"
#include "dsp_fifo.h"
#include "dsp_plot.h"
#include "dsp_viterbi.h"
#include "dsp_perf.h"
#include "dsp_math.h"
#include "dsp_draw.h"

using namespace OpenDSP;

#include "dot11n_plcp.h"

#define max_ht_data 1000
struct dot11n_tx_buffer
{
    v_align(64) int iPos;
    int iPadding[15];
    v_cs        vsamples[max_ht_data * 40];

    __forceinline void clear()
    {
        iPos = 0;
    }

    void toDumpFile(const char* filename)
    {
        for (int i = 0; i < 2; i++)
        {
            char namebuffer[1024];
            memset(namebuffer, 0, 1024);
            sprintf_s(namebuffer, 1024, "%s_%d.dmp", filename, i);
            FILE* hFile;
            fopen_s(&hFile, namebuffer, "wb");

            int total_count = sizeof(vsamples) / sizeof(complex16);
            unsigned char desc[16] = {0x01};

            complex16* pc = (complex16*)&vsamples[i];
            for (int j = 0; j < total_count; j += 56)
            {
                fwrite(desc, 16, 1, hFile);

                for (int k = 0; k < 7; k++)
                {
                    fwrite(pc, sizeof(complex16) * 4, 1, hFile);

                    pc += 8;
                }
            }

            fclose(hFile);
        }
    }

    void toRxDumpFile20M(const char* filename)
    {
        for (int i = 0; i < 2; i++)
        {
            char namebuffer[1024];
            memset(namebuffer, 0, 1024);
            sprintf_s(namebuffer, 1024, "%s_%d.dmp", filename, i);
            FILE* hFile;
            fopen_s(&hFile, namebuffer, "wb");

            int total_count = sizeof(vsamples) / sizeof(complex16);
            unsigned char desc[16] = {0x01};

            complex16* pc = (complex16*)&vsamples[i];
            for (int j = 0; j < total_count; j += 112)
            {
                fwrite(desc, 16, 1, hFile);

                for (int k = 0; k < 7; k++)
                {
                    fwrite(pc, sizeof(complex16) * 1, 1, hFile);
                    fwrite(pc + 2, sizeof(complex16) * 1, 1, hFile);
                    pc += 8;
                    fwrite(pc, sizeof(complex16) * 1, 1, hFile);
                    fwrite(pc + 2, sizeof(complex16) * 1, 1, hFile);
                    pc += 8;
                }
            }

            fclose(hFile);
        }
    }

    void toRxDumpFile(const char* filename)
    {
        for (int i = 0; i < 2; i++)
        {
            char namebuffer[1024];
            memset(namebuffer, 0, 1024);
            sprintf_s(namebuffer, 1024, "%s_%d.dmp", filename, i);
            FILE* hFile;
            fopen_s(&hFile, namebuffer, "wb");

            int total_count = sizeof(vsamples) / sizeof(v_cs);
            unsigned char desc[16] = {0x01};
            if (i == 0)
            {
                v_cs* pv = (v_cs*)&vsamples[0];
                for (int j = 0; j < total_count - 14; )
                {
                    fwrite(desc, 16, 1, hFile);
                    v_cs vout;

                    for (int k = 0; k < 7; k++)
                    {
                        vout[0].re = pv[j][0].re * 1 + pv[j + 1][0].re * 0;
                        vout[0].im = pv[j][0].im * 1 + pv[j + 1][0].im * 0;
                        vout[1].re = pv[j][2].re * 1 + pv[j + 1][2].re * 0;
                        vout[1].im = pv[j][2].im * 1 + pv[j + 1][2].im * 0;
                        vout[2].re = pv[j + 2][0].re * 1 + pv[j + 3][0].re * 0;
                        vout[2].im = pv[j + 2][0].im * 1 + pv[j + 3][0].im * 0;
                        vout[3].re = pv[j + 2][2].re * 1 + pv[j + 3][2].re * 0;
                        vout[3].im = pv[j + 2][2].im * 1 + pv[j + 3][2].im * 0;
                        fwrite(&vout, sizeof(vout), 1, hFile);
                        j += 4;
                    }
                }
            }
            else if (i == 1)
            {
                v_cs* pv = (v_cs*)&vsamples[0];
                for (int j = 0; j < total_count - 14; )
                {
                    fwrite(desc, 16, 1, hFile);
                    v_cs vout;

                    for (int k = 0; k < 7; k++)
                    {
                        vout[0].re = pv[j][0].re * 0 + pv[j + 1][0].re * 1;
                        vout[0].im = pv[j][0].im * 0 + pv[j + 1][0].im * 1;
                        vout[1].re = pv[j][2].re * 0 + pv[j + 1][2].re * 1;
                        vout[1].im = pv[j][2].im * 0 + pv[j + 1][2].im * 1;
                        vout[2].re = pv[j + 2][0].re * 0 + pv[j + 3][0].re * 1;
                        vout[2].im = pv[j + 2][0].im * 0 + pv[j + 3][0].im * 1;
                        vout[3].re = pv[j + 2][2].re * 0 + pv[j + 3][2].re * 1;
                        vout[3].im = pv[j + 2][2].im * 0 + pv[j + 3][2].im * 1;

                        fwrite(&vout, sizeof(vout), 1, hFile);
                        j += 4;
                    }
                }
            }

            fclose(hFile);
        }
    }

    void toTxtFile(const char* filename)
    {
        for (int i = 0; i < 2; i++)
        {
            char namebuffer[1024];
            memset(namebuffer, 0, 1024);
            sprintf_s(namebuffer, 1024, "%s_%d.txt", filename, i);
            FILE* hFile;
            fopen_s(&hFile, namebuffer, "w");

            int total_count = sizeof(vsamples) / sizeof(complex16);

            complex16* pc = (complex16*)&vsamples[i];
            for (int j = 0; j < total_count; j += 8)
            {
                fprintf(hFile, "%d\t%d\n", pc[0].re, pc[0].im);
                fprintf(hFile, "%d\t%d\n", pc[1].re, pc[1].im);
                fprintf(hFile, "%d\t%d\n", pc[2].re, pc[2].im);
                fprintf(hFile, "%d\t%d\n", pc[3].re, pc[3].im);

                pc += 8;
            }

            fclose(hFile);
        }
    }
};

struct dot11n_phy
{
    L_STF  m_lstf;
    L_LTF  m_lltf;
    L_SIG  m_lsig;
    HT_SIG m_htsig;
    HT_STF m_htstf;
    HT_LTF m_htltf;
    HT_SERVICE m_service;

    interleaver::dot11a_interleaver_1bpsc m_interleaver1bpsc_a;

    static const char sisoLongPrePositive[64];
    static const char mimoLongPrePositive[64];

    static const v_align(16) v_ci::type     m_vHTLTFMask_f[32];
    static const v_align(16) v_cs::type     m_vHTLTFMask[16];
    static const v_align(16) v_cs::type     m_vLLTFMask[16];

    static const v_align(16) complexf::type stdLSTF[16];

    unsigned __int8* m_psrc;
    unsigned int m_srcidx;
    unsigned int m_srcsize;
    int      m_txmcs;

    int m_symbolcount;
    unsigned int m_conv_outputcnt;

    v_align(16) complex16   m_cwindow1[4];
    v_align(16) complex16   m_cwindow2[4];

    L_STF::symbol    lstfsymbol[2];
    L_LTF::symbol    lltfsymbol[2];
    HT_STF::symbol   htstfsymbol[2];
    HT_LTF::symbol   htltfsymbol[2];
    HT_DATA::symbol  localsymbol[2];
    HT_DATA::symbol  tempsymbol[4];

    dot11n_ofdm_pilot m_pilot;
    __int16  m_pilotbuffer[4];

    mapper::dsp_mapper_bpsk<complex16> m_mapbpsk2;// for ht-sig, 90' rotated

    scrambler::dot11n_scrambler     m_scrambler;

    v_align(64) unsigned __int8 m_scramble_outputbuffer[64];

    convolution::encoder_1_2 m_conv12;
    convolution::encoder_2_3 m_conv23;
    convolution::encoder_3_4 m_conv34;

    v_align(64) unsigned __int8 m_streamsplitter_inputbuffer[128];

    stream_parser::stream_paser_bpsk_2ss  m_spbpsk;
    stream_parser::stream_paser_qpsk_2ss  m_spqpsk;
    stream_parser::stream_paser_16qam_2ss m_sp16qam;
    stream_parser::stream_paser_64qam_2ss m_sp64qam;

    stream_parser::stream_paser_64qam_2ss::output_type m_streamsplitter_outputbuffer;

    interleaver::dot11n_interleaver_1bpsc m_interleaver1bpsc_iss1;
    interleaver::dot11n_interleaver_1bpsc m_interleaver1bpsc_iss2;
    interleaver::dot11n_interleaver_2bpsc m_interleaver2bpsc_iss1;
    interleaver::dot11n_interleaver_2bpsc m_interleaver2bpsc_iss2;
    interleaver::dot11n_interleaver_4bpsc m_interleaver4bpsc_iss1;
    interleaver::dot11n_interleaver_4bpsc m_interleaver4bpsc_iss2;
    interleaver::dot11n_interleaver_6bpsc m_interleaver6bpsc_iss1;
    interleaver::dot11n_interleaver_6bpsc m_interleaver6bpsc_iss2;

    interleaver::dot11n_interleaver_6bpsc::output_type m_interleave_outputbuffers[2];

    mapper::dsp_mapper_bpsk<complex16>  m_mapbpsk;
    mapper::dsp_mapper_qpsk<complex16>  m_mapqpsk;
    mapper::dsp_mapper_16qam<complex16> m_map16qam;
    mapper::dsp_mapper_64qam<complex16> m_map64qam;

    mapper::dsp_mapper_bpsk<complex16>::output_type    m_map_outputmuffer;

    template<typename T>
    T& GetInterleaveBuffer(int i)
    {
        return reinterpret_cast<T&>(m_interleave_outputbuffers[i]);
    }

    template<typename T>
    T& GetMapBuffer()
    {
        return reinterpret_cast<T&>(m_map_outputmuffer);
    }

    template<typename T>
    __forceinline void FlushNT(T& symbol1, T& symbol2, dot11n_tx_buffer& txbuffer)
    {
        int i = 0, j = txbuffer.iPos;
        for (; i < symbol1.vntotal; i++, j += 2)
        {
            symbol1.vdata[i].v_storent(&txbuffer.vsamples[j]);
            symbol2.vdata[i].v_storent(&txbuffer.vsamples[j + 1]);
        }
        txbuffer.iPos = j;
    }

    fft_i<64>  m_fft;
    ifft_i<128> m_ifft;


    enum
    {
        draw_h11 = 0,
        draw_h12,
        draw_h21,
        draw_h22,
        draw_f1,
        draw_f2,
        draw_t1,
        draw_t2,
        draw_count
    };
    dsp_draw_window *m_draw[draw_count];

    const complex16 bpsk_one;

    dot11n_phy() :
        bpsk_one(30339, 0),
        m_scrambler(0xCC), m_rx_descrambler(0xCC),
        m_spbpsk(), m_spqpsk(), m_sp16qam(), m_sp64qam(),
        m_mapbpsk(complex16(30339, 0)), m_mapbpsk2(complex16(0, 30339)), m_mapqpsk(complex16(21453, 21453)),
        m_map16qam(complex16(9594, 9594)), m_map64qam(complex16(4681, 4681)),
        m_interleaver1bpsc_iss1(1), m_interleaver1bpsc_iss2(2), m_interleaver2bpsc_iss1(1), m_interleaver2bpsc_iss2(2),
        m_interleaver4bpsc_iss1(1), m_interleaver4bpsc_iss2(2), m_interleaver6bpsc_iss1(1), m_interleaver6bpsc_iss2(2),
        m_deinterleaver1bpsc_iss1(1), m_deinterleaver1bpsc_iss2(2), m_deinterleaver2bpsc_iss1(1), m_deinterleaver2bpsc_iss2(2),
        m_deinterleaver4bpsc_iss1(1), m_deinterleaver4bpsc_iss2(2), m_deinterleaver6bpsc_iss1(1), m_deinterleaver6bpsc_iss2(2),
        m_interleaver1bpsc_a(),
        m_conv12(0xCC), m_conv23(0xCC), m_conv34(0xCC),
        m_symbolcount(0),
        m_rx_current_sb_idx(0),
        m_currentvindex(0),
        m_asyncok(false),
        m_xsyncok(false),
        m_max_cca_sbcount(2048),
        m_rx_state(s_frame_detection),
        m_vsampleidx(0),
        m_fftw(64, fft_f::FFT),
        m_rx_decoded_frame(NULL),
        m_workindicator(1)
    {
        m_rx_decoded_frame = new unsigned char[2000];

        m_xcorr.SetLocalTemplate((v_cs*)&stdLSTF[0]);

#if draw_enabled
        char szBuf[128];
        int x = 0, y = 0;
        int width = 300, height = 300;
        for (int i = 0; i < draw_count; i++)
        {
            memset(szBuf, 0, 128);
            sprintf_s(szBuf, 128, "draw window %d", i);
            m_draw[i] = new dsp_draw_window(szBuf, x, y, width, height);
            x += width;

            if ( (i + 1) % 4 == 0)
            {
                x = 0;
                y += height;
            }
        }
#endif
    }

    ~dot11n_phy()
    {
        if (m_rx_decoded_frame) delete[] m_rx_decoded_frame;
#if draw_enabled
        for (int i = 0; i < draw_count; i++)
        {
            if (m_draw[i])
            {
                delete m_draw[i];
            }
        }
#endif
    }

    __forceinline void make_plcp(dot11n_tx_buffer& tx_buffer);
    __forceinline void make_service(dot11n_tx_buffer& tx_buffer);

    __forceinline void clear_window(complex16* pwindow, int n)
    {
        memset(pwindow, 0, n * sizeof(complex16));
    }

    template<typename T>
    void make_window(T &input, complex16* pwindow)
    {
        input.data[0] >>= 2;
        input.data[1] >>= 1;
        input.data[2].re -= (input.data[2].re >>= 1);
        input.data[2].im -= (input.data[2].im >>= 1);

        input.vdata[0] = v_add(input.vdata[0], *(v_cs*)pwindow);

        //input.data[0].re += pwindow[0].re;
        //input.data[0].im += pwindow[0].im;

        pwindow[0].re = input.data[32].re - (input.data[32].re >> 2);
        pwindow[0].im = input.data[32].im - (input.data[32].im >> 2);

        pwindow[1].re = input.data[33].re >> 1;
        pwindow[1].im = input.data[33].im >> 1;

        pwindow[2].re = input.data[34].re >> 2;
        pwindow[2].im = input.data[34].im >> 2;

        pwindow[3].re = 0;
        pwindow[3].im = 0;
    }

    template<int N, typename CallBack>
    __forceinline void fill_symbol(unsigned int &sc_idx, CallBack fn)
    {
        for(int j = 0; j < N; j++, sc_idx++)
        {
            if (sc_idx == 7 || sc_idx == 21)
            {
                sc_idx++;
            }
            else if (sc_idx == (128 - 7) || sc_idx == (128 - 21))
            {
                sc_idx++;
            }
            else if (sc_idx == 128)
            {
                sc_idx = 1;
            }
            fn(j);
        }
    }

    void flush_symbol(dot11n_tx_buffer& tx_buffer)
    {
        tempsymbol[0].copycp();
        make_window(tempsymbol[0], m_cwindow1);
        tempsymbol[1].csd(tempsymbol[2], 4);
        tempsymbol[2].copycp();
        make_window(tempsymbol[2], m_cwindow2);
        FlushNT(tempsymbol[0], tempsymbol[2], tx_buffer);
    }

    __forceinline void add_pilot(int iss, HT_DATA::symbol &htsymbol);

    // in bytes: 13
    void mcs8_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 13
    void mcs9_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 39
    void mcs10_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 26
    void mcs11_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 39
    void mcs12_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 52
    void mcs13_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 117
    void mcs14_entry(dot11n_tx_buffer& tx_buffer);
    // in bytes: 65
    void mcs15_entry(dot11n_tx_buffer& tx_buffer);

    //////////////////////////////////////////////////////////////////////////
    typedef dsp_stream<2, 131072> dot11n_rx_stream;
    dot11n_rx_stream      m_rxstream;
    dsp_acorr_vi          m_autocorr_vi;
    dsp_acorr_f           m_autocorr_f;
    dsp_xcorr             m_xcorr;
    dsp_freq              m_freq;
    int  m_rx_current_sb_idx;
    int  m_currentvindex;
    bool m_asyncok;
    bool m_xsyncok;

    dsp_perf<128> m_perf;

    int m_max_cca_sbcount;

    enum
    {
        s_frame_detection,
        s_sync_check,
        s_frame_boundary,
        s_l_ltf,
        s_l_sig,
        s_l_data,
        s_ht_sig1,
        s_ht_sig2,
        s_ht_stf,
        s_ht_ltf1,
        s_ht_ltf2,
        s_ht_data,
        s_done
    };

    dsp_plot m_plot;



    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 64, 16> dot11n_rx_symbol;
    typedef ofdm_symbol<OpenDSP::complexf, OpenDSP::v_cf, 64, 16> dot11n_rx_fsymbol;

    HT_SIG::sig_format m_rxhtsig;
    unsigned char m_chtsig[6];
    crc::crc8 m_crc8;

    bool parse_ht_sig()
    {
        unsigned char* p = (unsigned char*)m_chtsig;
        //unsigned char* p = (unsigned char*)&m_rxhtsig;

        unsigned char  crc8value;

        // test case
        //p[0] = 0x8F; p[1] = 0x64;
        //p[2] = 0x00; p[3] = 0x07;
        //p[4] = 0xFC;
        // crc8=0x15

        m_crc8.reset();
        m_crc8(p, 4);
        m_crc8(p[4] & 0x3, 2);
        crc8value = m_crc8.value();

        //printf("HTSIG: %x:%x:%x:%x:%x:%x\n", p[0], p[1], p[2], p[3], p[4], p[5]);

        //printf("CRC8=%x, %x\n", (p[4] >> 2) | (p[5] << 6), crc8value);

        if (crc8value != ((p[4] >> 2) | (p[5] << 6)))
        {
            m_rx_mcs = 0;
            m_rx_frame_length = 0;
            return false;
        }

        m_rx_mcs = (p[0] & 0x7F);
        m_rx_frame_length = *((unsigned short*)(p + 1));

        return true;
    }

    tick_count tstart, tend;

    dsp_math m_dspmath;

#define  pilot_tracking_length 4

    short m_rx_cfo_delta_i;
    short m_rx_cfo_step_i;
    short m_rx_cfo_theta_i;

    float m_rx_cfo_delta_f;
    float m_rx_cfo_step_f;
    float m_rx_cfo_theta_f;

    short m_pilotHis_i[pilot_tracking_length];
    short m_pilotSum_i;

    float m_pilotHis_f[pilot_tracking_length];
    float m_pilotSum_f;

    int   m_pilotPt_f;
    int   m_pilotPt_i;

    int  m_rx_cyclic_write_idx;
    v_cs m_rx_cyclic_buffer[2][32];

    complex16 m_rx_cfo_coeff_i[128];
    complex16 m_rx_cfo_compensated_i[2][128];
    complex16 m_rx_fsamples_i[2][128];

    v_s      m_rx_vcfo_step_i;
    v_s      m_rx_vcfo_delta_i;
    v_s      m_rx_vcfo_theta_i;
    v_cs     m_rx_vcfo_coeff_i[32];


    v_f      m_rx_vcfo_step_f;
    v_f      m_rx_vcfo_delta_f;
    v_f      m_rx_vcfo_theta_f;

    complexf m_rx_cfo_coeff_f[128];
    complexf m_rx_cfo_compensated_f[2][128];
    complexf m_rx_fsamples_f[2][128];

    complexf m_rx_noise[2][64];
    complexf m_rx_noise_mrc[64];

    complex16 m_rx_channel_i[2][128];
    complex16 m_rx_channelinv_i[2][128];
    complex16 m_rx_channel_compensated_i[2][128];

    complexf m_rx_channel_f[2][128];
    complexf m_rx_channelinv_f[2][128];
    complexf m_rx_channel_compensated_f[2][128];
    complex16 m_rx_channel_compensated_i2[2][128];// for comparison

    complexf m_rx_sic_f[2][64];

    unsigned __int8 m_rx_demapped[2][320]; // maximum is 312, but padding to 320 to align cache
    unsigned __int8 m_rx_deinterleaved[2][320];

    demapper::dsp_demapper m_demapper;

    deinterleaver::dot11n_deinterleaver<1> m_deinterleaver1bpsc_iss1;
    deinterleaver::dot11n_deinterleaver<1> m_deinterleaver1bpsc_iss2;
    deinterleaver::dot11n_deinterleaver<2> m_deinterleaver2bpsc_iss1;
    deinterleaver::dot11n_deinterleaver<2> m_deinterleaver2bpsc_iss2;
    deinterleaver::dot11n_deinterleaver<4> m_deinterleaver4bpsc_iss1;
    deinterleaver::dot11n_deinterleaver<4> m_deinterleaver4bpsc_iss2;
    deinterleaver::dot11n_deinterleaver<6> m_deinterleaver6bpsc_iss1;
    deinterleaver::dot11n_deinterleaver<6> m_deinterleaver6bpsc_iss2;

    deinterleaver::dot11a_deinterleaver m_siso_deinterleaver;

    void combine_i(complex16 * pa, complex16* pb, complex16* pc)
    {
        for (int i = 0; i < 64; i++)
        {
            pc[i] = pa[i] + pb[i];
            pc[i] >>= 1;
        }
    }

    void combine_f(complexf * pa, complexf* pb, complexf* pc)
    {
        for (int i = 0; i < 64; i++)
        {
            pc[i] = pa[i] + pb[i];
            pc[i] /= 2.0f;
        }
    }

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

#pragma region siso deinterleave
    __forceinline void siso_deinterleave1bpsc(unsigned __int8* pInput, unsigned __int8* pOutput)
    {
        m_siso_deinterleaver._1bpsc(pInput, pOutput);
    }

    __forceinline void siso_deinterleave2bpsc(unsigned __int8 * pInput, unsigned __int8 * pOutput)
    {
        m_siso_deinterleaver._2bpsc(pInput, pOutput);
    }

    __forceinline void siso_deinterleave4bpsc(unsigned __int8 * pInput, unsigned __int8 * pOutput)
    {
        m_siso_deinterleaver._4bpsc(pInput, pOutput);
    }

    __forceinline void siso_deinterleave6bpsc(unsigned __int8 * pInput, unsigned __int8 * pOutput)
    {
        m_siso_deinterleaver._6bpsc(pInput, pOutput);
    }
#pragma endregion siso deinterleave

    template<int nbpsc>
    __forceinline void mimo_deinterleave(unsigned __int8 demapped[2][320], unsigned __int8 deinterleaved[2][320]);

    template<> __forceinline void mimo_deinterleave<1>(unsigned __int8 demapped[2][320], unsigned __int8 deinterleaved[2][320])
    {
        m_deinterleaver1bpsc_iss1(demapped[0], deinterleaved[0]);
        m_deinterleaver1bpsc_iss2(demapped[1], deinterleaved[1]);
    }

    template<> __forceinline void mimo_deinterleave<2>(unsigned __int8 demapped[2][320], unsigned __int8 deinterleaved[2][320])
    {
        m_deinterleaver2bpsc_iss1(demapped[0], deinterleaved[0]);
        m_deinterleaver2bpsc_iss2(demapped[1], deinterleaved[1]);
    }

    template<> __forceinline void mimo_deinterleave<4>(unsigned __int8 demapped[2][320], unsigned __int8 deinterleaved[2][320])
    {
        m_deinterleaver4bpsc_iss1(demapped[0], deinterleaved[0]);
        m_deinterleaver4bpsc_iss2(demapped[1], deinterleaved[1]);
    }

    template<> __forceinline void mimo_deinterleave<6>(unsigned __int8 demapped[2][320], unsigned __int8 deinterleaved[2][320])
    {
        m_deinterleaver6bpsc_iss1(demapped[0], deinterleaved[0]);
        m_deinterleaver6bpsc_iss2(demapped[1], deinterleaved[1]);
    }

    void siso_demap_bpsk_i(complex16* pcInput, unsigned __int8* pOutput)
    {
        m_demapper.demap_limit_bpsk((v_cs*)pcInput, 16);
        _demap<-26, 26>(pcInput, pOutput,
            [&](complex16& cInput, unsigned __int8* &pOutput){
                m_demapper.demap_bpsk_i(cInput, pOutput);
                pOutput++;
        });
    }

    void siso_demap_bpsk_q(complex16* pcInput, unsigned __int8* pOutput)
    {
        m_demapper.demap_limit_bpsk((v_cs*)pcInput, 16);
        _demap<-26, 26>(pcInput, pOutput,
            [&](complex16& cInput, unsigned __int8* &pOutput){
                m_demapper.demap_bpsk_q(cInput, pOutput);
                pOutput++;
        });
    }

    void siso_demap_qpsk(complex16* pcInput, unsigned __int8* pOutput)
    {
        m_demapper.demap_limit_qpsk((v_cs*)pcInput, 16);
        _demap<-26, 26>(pcInput, pOutput,
            [&](complex16& cInput, unsigned __int8* &pOutput){
                m_demapper.demap_qpsk(cInput, pOutput);
                pOutput += 2;
        });
    }

    void siso_demap_16qam(complex16* pcInput, unsigned __int8* pOutput)
    {
        m_demapper.demap_limit_16qam((v_cs*)pcInput, 16);
        _demap<-26, 26>(pcInput, pOutput,
            [&](complex16& cInput, unsigned __int8* &pOutput){
                m_demapper.demap_16qam(cInput, pOutput);
                pOutput += 4;
        });
    }

    void siso_demap_64qam(complex16* pcInput, unsigned __int8* pOutput)
    {
        m_demapper.demap_limit_64qam((v_cs*)pcInput, 16);
        _demap<-26, 26>(pcInput, pOutput,
            [&](complex16& cInput, unsigned __int8* &pOutput){
                m_demapper.demap_64qam(cInput, pOutput);
                pOutput += 6;
        });
    }

    void mimo_demap_bpsk()
    {
        m_demapper.demap_limit_bpsk((v_cs*)&m_rx_channel_compensated_i[0], 16);
        m_demapper.demap_limit_bpsk((v_cs*)&m_rx_channel_compensated_i[1], 16);

        for (int i = 0; i < 2; i++)
        {
            _demap<-28, 28>((complex16*)&m_rx_channel_compensated_i[i], (unsigned __int8*)&m_rx_demapped[i],
                [&](complex16& cInput, unsigned __int8* &pOutput){
                    m_demapper.demap_bpsk_i(cInput, pOutput);
                    pOutput++;
            });
        }
    }

    void mimo_demap_qpsk()
    {
        m_demapper.demap_limit_qpsk((v_cs*)&m_rx_channel_compensated_i[0], 16);
        m_demapper.demap_limit_qpsk((v_cs*)&m_rx_channel_compensated_i[1], 16);
        for (int i = 0; i < 2; i++)
        {
            _demap<-28, 28>((complex16*)&m_rx_channel_compensated_i[i], (unsigned __int8*)&m_rx_demapped[i],
                [&](complex16& cInput, unsigned __int8* &pOutput){
                    m_demapper.demap_qpsk(cInput, pOutput);
                    pOutput += 2;
            });
        }
    }

    void mimo_demap_16qam()
    {
        m_demapper.demap_limit_16qam((v_cs*)&m_rx_channel_compensated_i[0], 16);
        m_demapper.demap_limit_16qam((v_cs*)&m_rx_channel_compensated_i[1], 16);
        for (int i = 0; i < 2; i++)
        {
            _demap<-28, 28>((complex16*)&m_rx_channel_compensated_i[i], (unsigned __int8*)&m_rx_demapped[i],
                [&](complex16& cInput, unsigned __int8* &pOutput){
                    m_demapper.demap_16qam(cInput, pOutput);
                    pOutput += 4;
            });
        }
    }

    void mimo_demap_64qam()
    {
        m_demapper.demap_limit_64qam((v_cs*)&m_rx_channel_compensated_i[0], 16);
        m_demapper.demap_limit_64qam((v_cs*)&m_rx_channel_compensated_i[1], 16);
        for (int i = 0; i < 2; i++)
        {
            _demap<-28, 28>((complex16*)&m_rx_channel_compensated_i[i], (unsigned __int8*)&m_rx_demapped[i],
                [&](complex16& cInput, unsigned __int8* &pOutput){
                    m_demapper.demap_64qam(cInput, pOutput);
                    pOutput += 6;
            });
        }
    }


    //! SISO channel estimation & compensation

    void siso_channel_estimation_i(complex16* pcsin, complex16* pcschannel, int count)
    {
        __int32 sq;
        __int32 ire, iim;
        int i;
        for (i = 1; i <= 26; i++)
        {
            ire = (__int32)pcsin[i].re;
            iim = (__int32)pcsin[i].im;
            sq  = ire * ire + iim * iim;

            ire <<= 16;
            iim <<= 16;

            //sq >>= 16;

            if (sisoLongPrePositive[i])
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

            ire <<= 16;
            iim <<= 16;

            //sq >>= 16;

            if (sisoLongPrePositive[i])
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

    // issue: not match with C code
    void v_siso_channel_estimation_i(v_cs* pcsin, v_cs* pcschannel, int vcount)
    {
        v_i  vsqr, vsqrtemp;
        v_i  vciinput1, vciinput2;
        v_cs vout;
        v_cs *pvmask = (v_cs*)&m_vLLTFMask[0];
        int  v;
        int  i;

        for (i = 0; i < 7; i++)
        {
            v_cs& vin = (v_cs&)pcsin[i];
            int sqr;
            vsqr = vin.v_sqr2i();

            vin.v_convert2ci((v_ci&)vciinput1, (v_ci&)vciinput2);

            vciinput1 = vciinput1.v_shift_left(16);
            vciinput2 = vciinput2.v_shift_left(16);

            vsqrtemp = vsqr.v_shift_right_arithmetic(1);

            vciinput1 = v_add(vciinput1, vsqrtemp);
            vciinput2 = v_add(vciinput2, vsqrtemp);

            sqr = vsqr.v_get_at<0>();
            if (sqr == 0) sqr = 1;
            v = vciinput1.v_get_at<0>() / sqr;
            vciinput1.v_set_at<0>( v );

            v = vciinput1.v_get_at<1>() / sqr;
            vciinput1.v_set_at<1>( v );

            sqr = vsqr.v_get_at<1>();
            if (sqr == 0) sqr = 1;
            v = vciinput1.v_get_at<2>() / sqr;
            vciinput1.v_set_at<2>( v );

            v = vciinput1.v_get_at<3>() / sqr;    
            vciinput1.v_set_at<3>( v );

            sqr = vsqr.v_get_at<2>();
            if (sqr == 0) sqr = 1;
            v = vciinput2.v_get_at<0>() / sqr;
            vciinput2.v_set_at<0>( v );

            v = vciinput2.v_get_at<1>() / sqr;
            vciinput2.v_set_at<1>( v );

            sqr = vsqr.v_get_at<3>();
            if (sqr == 0) sqr = 1;
            v = vciinput2.v_get_at<2>() / sqr;
            vciinput2.v_set_at<2>( v );

            v = vciinput2.v_get_at<3>() / sqr;
            vciinput2.v_set_at<3>( v );

            vout = v_convert2cs((v_ci&)vciinput1, (v_ci&)vciinput2);

            vout = v_xor(vout, pvmask[i]);
            vout = v_sub(vout, pvmask[i]);

            pcschannel[i] = vout;
        }

        for (i = 9; i < 16; i++)
        {
            v_cs& vin = (v_cs&)pcsin[i];
            int sqr;
            vsqr = vin.v_sqr2i();

            vin.v_convert2ci((v_ci&)vciinput1, (v_ci&)vciinput2);

            vciinput1 = vciinput1.v_shift_left(16);
            vciinput2 = vciinput2.v_shift_left(16);

            vsqrtemp = vsqr.v_shift_right_arithmetic(1);

            vciinput1 = v_add(vciinput1, vsqrtemp);
            vciinput2 = v_add(vciinput2, vsqrtemp);

            sqr = vsqr.v_get_at<0>();
            if (sqr == 0) sqr = 1;
            v = vciinput1.v_get_at<0>() / sqr;
            vciinput1.v_set_at<0>( v );

            v = vciinput1.v_get_at<1>() / sqr;
            vciinput1.v_set_at<1>( v );

            sqr = vsqr.v_get_at<1>();
            if (sqr == 0) sqr = 1;
            v = vciinput1.v_get_at<2>() / sqr;
            vciinput1.v_set_at<2>( v );

            v = vciinput1.v_get_at<3>() / sqr;
            vciinput1.v_set_at<3>( v );

            sqr = vsqr.v_get_at<2>();
            if (sqr == 0) sqr = 1;
            v = vciinput2.v_get_at<0>() / sqr;
            vciinput2.v_set_at<0>( v );

            v = vciinput2.v_get_at<1>() / sqr;
            vciinput2.v_set_at<1>( v );

            sqr = vsqr.v_get_at<3>();
            if (sqr == 0) sqr = 1;
            v = vciinput2.v_get_at<2>() / sqr;
            vciinput2.v_set_at<2>( v );

            v = vciinput2.v_get_at<3>() / sqr;
            vciinput2.v_set_at<3>( v );

            vout = v_convert2cs((v_ci&)vciinput1, (v_ci&)vciinput2);

            vout = v_xor(vout, pvmask[i]);
            vout = v_sub(vout, pvmask[i]);

            pcschannel[i] = vout;
        }
    }

    void siso_channel_estimation_f(complexf* pcfin, complexf* pcfchannel, int count)
    {
        int i;

        for (i = 1; i <= 26; i++)
        {
            float sq = pcfin[i].re * pcfin[i].re + pcfin[i].im * pcfin[i].im;
            if (sisoLongPrePositive[i])
            {
                pcfchannel[i].re =  pcfin[i].re / sq;
                pcfchannel[i].im = -pcfin[i].im / sq;
            }
            else
            {
                pcfchannel[i].re = -pcfin[i].re / sq;
                pcfchannel[i].im =  pcfin[i].im / sq;
            }
        }

        for (i = 64 - 26; i < 64; i++)
        {
            float sq = pcfin[i].re * pcfin[i].re + pcfin[i].im * pcfin[i].im;
            if (sisoLongPrePositive[i])
            {
                pcfchannel[i].re =  pcfin[i].re / sq;
                pcfchannel[i].im = -pcfin[i].im / sq;
            }
            else
            {
                pcfchannel[i].re = -pcfin[i].re / sq;
                pcfchannel[i].im =  pcfin[i].im / sq;
            }
        }
    }

    // pcschannel: range is limitted in -32768~~32767
    // can be held in complex16, but here in complex32 to avoid 16->32 conversion when doing the multiplication
    // in vector1 implementation, this can be optimized to use vcs. because in vector1 there is a nature instruction mul_add
    void siso_channel_compensate_i(complex16* pcsin, complex16* pcschannel, complex16* pcschannelcompensated, int count)
    {
        __int32 ire, iim;
        for (int i = 0; i < count; i++)
        {
            ire = (__int32)pcsin[i].re * (__int32)pcschannel[i].re - (__int32)pcsin[i].im * (__int32)pcschannel[i].im;
            iim = (__int32)pcsin[i].re * (__int32)pcschannel[i].im + (__int32)pcsin[i].im * (__int32)pcschannel[i].re;

            // finally, normalize it to 128
            pcschannelcompensated[i].re = (__int16)(ire >> 9);
            pcschannelcompensated[i].im = (__int16)(iim >> 9);
        }
    }

    void v_siso_channel_compensate_i(v_cs* pvcsin, v_cs* pvcschannel, v_cs* pvcschannelcompensated, int vcount)
    {
        v_ci vciout1, vciout2;
        const v_cs vmsk = VMASK::__0x80000001800000018000000180000001<v_cs>();

        for (int i = 0; i < vcount; i++)
        {
            v_cs &vin   = (v_cs &)pvcsin[i];
            v_cs &vcof  = (v_cs &)pvcschannel[i];
            v_cs &vout  = (v_cs &)pvcschannelcompensated[i];

            v_mul2ci(vin, vcof, vmsk, vciout1, vciout2);

            vciout1 = vciout1.v_shift_right_arithmetic(9);
            vciout2 = vciout2.v_shift_right_arithmetic(9);

            vout        = v_convert2cs(vciout1, vciout2);
        }
    }


    void v_siso_channel_compensate_f(v_cf* pvcfin, v_cf* pvcfchannel, v_cf* pvcfchannelcompensated, int vcount)
    {
        for (int i = 0; i < vcount; i++)
        {
            v_cf &h = pvcfchannel[i];
            v_cf &y = pvcfin[i];
            v_cf &x = pvcfchannelcompensated[i];
            x = v_mul(y, h);
        }
    }

    void siso_channel_compensate_f(complexf* pcfin, complexf* pcfchannel, complexf* pcfchannelcompensated, int count)
    {
        for (int i = 0; i < count; i++)
        {
            pcfchannelcompensated[i] = pcfin[i] * pcfchannel[i];
        }
    }

    //! MIMO channel estimation & compensation (2x2)
    void mimo_channel_estimation_i()
    {
        int i;
        for (i = 1; i <= 28; i++)
        {
            if (mimoLongPrePositive[i])
            {
                m_rx_channel_i[0][i].re      = ( (m_rx_fsamples_i[0][i].re - m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i].im      = ( (m_rx_fsamples_i[0][i].im - m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[0][i + 64].re = ( (m_rx_fsamples_i[0][i].re + m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i + 64].im = ( (m_rx_fsamples_i[0][i].im + m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[1][i].re      = ( (m_rx_fsamples_i[1][i].re - m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i].im      = ( (m_rx_fsamples_i[1][i].im - m_rx_fsamples_i[1][i + 64].im) >> 1 );
                m_rx_channel_i[1][i + 64].re = ( (m_rx_fsamples_i[1][i].re + m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i + 64].im = ( (m_rx_fsamples_i[1][i].im + m_rx_fsamples_i[1][i + 64].im) >> 1 );
            }
            else
            {
                m_rx_channel_i[0][i].re      = -( (m_rx_fsamples_i[0][i].re - m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i].im      = -( (m_rx_fsamples_i[0][i].im - m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[0][i + 64].re = -( (m_rx_fsamples_i[0][i].re + m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i + 64].im = -( (m_rx_fsamples_i[0][i].im + m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[1][i].re      = -( (m_rx_fsamples_i[1][i].re - m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i].im      = -( (m_rx_fsamples_i[1][i].im - m_rx_fsamples_i[1][i + 64].im) >> 1 );
                m_rx_channel_i[1][i + 64].re = -( (m_rx_fsamples_i[1][i].re + m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i + 64].im = -( (m_rx_fsamples_i[1][i].im + m_rx_fsamples_i[1][i + 64].im) >> 1 );
            }
        }
        for (i = 64 - 28; i < 64; i++)
        {
            if (mimoLongPrePositive[i])
            {
                m_rx_channel_i[0][i].re      = ( (m_rx_fsamples_i[0][i].re - m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i].im      = ( (m_rx_fsamples_i[0][i].im - m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[0][i + 64].re = ( (m_rx_fsamples_i[0][i].re + m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i + 64].im = ( (m_rx_fsamples_i[0][i].im + m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[1][i].re      = ( (m_rx_fsamples_i[1][i].re - m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i].im      = ( (m_rx_fsamples_i[1][i].im - m_rx_fsamples_i[1][i + 64].im) >> 1 );
                m_rx_channel_i[1][i + 64].re = ( (m_rx_fsamples_i[1][i].re + m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i + 64].im = ( (m_rx_fsamples_i[1][i].im + m_rx_fsamples_i[1][i + 64].im) >> 1 );
            }
            else
            {
                m_rx_channel_i[0][i].re      = -( (m_rx_fsamples_i[0][i].re - m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i].im      = -( (m_rx_fsamples_i[0][i].im - m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[0][i + 64].re = -( (m_rx_fsamples_i[0][i].re + m_rx_fsamples_i[0][i + 64].re) >> 1 );
                m_rx_channel_i[0][i + 64].im = -( (m_rx_fsamples_i[0][i].im + m_rx_fsamples_i[0][i + 64].im) >> 1 );
                m_rx_channel_i[1][i].re      = -( (m_rx_fsamples_i[1][i].re - m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i].im      = -( (m_rx_fsamples_i[1][i].im - m_rx_fsamples_i[1][i + 64].im) >> 1 );
                m_rx_channel_i[1][i + 64].re = -( (m_rx_fsamples_i[1][i].re + m_rx_fsamples_i[1][i + 64].re) >> 1 );
                m_rx_channel_i[1][i + 64].im = -( (m_rx_fsamples_i[1][i].im + m_rx_fsamples_i[1][i + 64].im) >> 1 );
            }
        }
        //
        complex32 star;
        __int64   starsqr;
        __int64   temp64;
        for (i = 1; i <= 28; i++)
        {
            // m_rx_channel_i range: 16bits

            star.re = ( (__int32)m_rx_channel_i[0][i].re * (__int32)m_rx_channel_i[1][i + 64].re - (__int32)m_rx_channel_i[0][i].im * (__int32)m_rx_channel_i[1][i + 64].im)
                - ((__int32)m_rx_channel_i[0][i + 64].re * (__int32)m_rx_channel_i[1][i].re - (__int32)m_rx_channel_i[0][i + 64].im * (__int32)m_rx_channel_i[1][i].im);

            star.im = ((__int32)m_rx_channel_i[0][i].re * (__int32)m_rx_channel_i[1][i + 64].im + (__int32)m_rx_channel_i[0][i].im * (__int32)m_rx_channel_i[1][i + 64].re)
                - ((__int32)m_rx_channel_i[0][i + 64].re * (__int32)m_rx_channel_i[1][i].im + (__int32)m_rx_channel_i[0][i + 64].im * (__int32)m_rx_channel_i[1][i].re);

            //star    = m_rx_channel_i[0][i] * m_rx_channel_i[1][i + 64] - m_rx_channel_i[0][i + 64] * m_rx_channel_i[1][i];// star range: 32bits
            star.im = -star.im;

            starsqr = ((__int64)star.re * (__int64)star.re + (__int64)star.im * (__int64)star.im);

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //! note: due to the size of demap lookup table is 256, so we have to normalize the I & Q to 128
            //! TODO: or, we can make a larger lookup table to hold 64QAM constellation points (the demap lookup table has been rebuilt)
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //
            temp64 = (__int64)m_rx_channel_i[1][i + 64].re * (__int64)star.re - (__int64)m_rx_channel_i[1][i + 64].im * (__int64)star.im;// 48b
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i].re      =  (__int16)(temp64);//16b, after channel compensation, the results should be 16b, then shift the result right 9b to normalize to 8b

            temp64 = (__int64)m_rx_channel_i[1][i + 64].re * (__int64)star.im + (__int64)m_rx_channel_i[1][i + 64].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i].im      =  (__int16)(temp64);//16b

            //
            temp64 = (__int64)m_rx_channel_i[0][i + 64].re * (__int64)star.re - (__int64)m_rx_channel_i[0][i + 64].im * (__int64)star.im;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i + 64].re      =  (__int16)(-temp64);//16b

            temp64 = (__int64)m_rx_channel_i[0][i + 64].re * (__int64)star.im + (__int64)m_rx_channel_i[0][i + 64].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i + 64].im      =  (__int16)(-temp64);//16b

            //
            temp64 = (__int64)m_rx_channel_i[1][i].re * (__int64)star.re - (__int64)m_rx_channel_i[1][i].im * (__int64)star.im;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i].re      =  (__int16)(-temp64);//16b

            temp64 = (__int64)m_rx_channel_i[1][i].re * (__int64)star.im + (__int64)m_rx_channel_i[1][i].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i].im      =  (__int16)(-temp64);//16b

            //
            temp64 = (__int64)m_rx_channel_i[0][i].re * (__int64)star.re - (__int64)m_rx_channel_i[0][i].im * (__int64)star.im;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i + 64].re      =  (__int16)(temp64);//16b

            temp64 = (__int64)m_rx_channel_i[0][i].re * (__int64)star.im + (__int64)m_rx_channel_i[0][i].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i + 64].im      =  (__int16)(temp64);//16b
        }

        for (i = 64 - 28; i < 64; i++)
        {
            // m_rx_channel_i range: 16bits
            star.re = ( (__int32)m_rx_channel_i[0][i].re * (__int32)m_rx_channel_i[1][i + 64].re - (__int32)m_rx_channel_i[0][i].im * (__int32)m_rx_channel_i[1][i + 64].im)
                - ((__int32)m_rx_channel_i[0][i + 64].re * (__int32)m_rx_channel_i[1][i].re - (__int32)m_rx_channel_i[0][i + 64].im * (__int32)m_rx_channel_i[1][i].im);

            star.im = ((__int32)m_rx_channel_i[0][i].re * (__int32)m_rx_channel_i[1][i + 64].im + (__int32)m_rx_channel_i[0][i].im * (__int32)m_rx_channel_i[1][i + 64].re)
                - ((__int32)m_rx_channel_i[0][i + 64].re * (__int32)m_rx_channel_i[1][i].im + (__int32)m_rx_channel_i[0][i + 64].im * (__int32)m_rx_channel_i[1][i].re);

            //star    = m_rx_channel_i[0][i] * m_rx_channel_i[1][i + 64] - m_rx_channel_i[0][i + 64] * m_rx_channel_i[1][i];// star range: 32bits
            star.im = -star.im;

            starsqr = ((__int64)star.re * (__int64)star.re + (__int64)star.im * (__int64)star.im);

            //////////////////////////////////////////////////////////////////////////

            //
            temp64 = (__int64)m_rx_channel_i[1][i + 64].re * (__int64)star.re - (__int64)m_rx_channel_i[1][i + 64].im * (__int64)star.im;// 48b
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i].re      =  (__int16)(temp64);//16b, after channel compensation, the results should be 16b, then shift the result right 9b to normalize to 8b

            temp64 = (__int64)m_rx_channel_i[1][i + 64].re * (__int64)star.im + (__int64)m_rx_channel_i[1][i + 64].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i].im      =  (__int16)(temp64);//16b

            //
            temp64 = (__int64)m_rx_channel_i[0][i + 64].re * (__int64)star.re - (__int64)m_rx_channel_i[0][i + 64].im * (__int64)star.im;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i + 64].re      =  (__int16)(-temp64);//16b

            temp64 = (__int64)m_rx_channel_i[0][i + 64].re * (__int64)star.im + (__int64)m_rx_channel_i[0][i + 64].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[0][i + 64].im      =  (__int16)(-temp64);//16b

            //
            temp64 = (__int64)m_rx_channel_i[1][i].re * (__int64)star.re - (__int64)m_rx_channel_i[1][i].im * (__int64)star.im;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i].re      =  (__int16)(-temp64);//16b

            temp64 = (__int64)m_rx_channel_i[1][i].re * (__int64)star.im + (__int64)m_rx_channel_i[1][i].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i].im      =  (__int16)(-temp64);//16b

            //
            temp64 = (__int64)m_rx_channel_i[0][i].re * (__int64)star.re - (__int64)m_rx_channel_i[0][i].im * (__int64)star.im;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i + 64].re      =  (__int16)(temp64);//16b

            temp64 = (__int64)m_rx_channel_i[0][i].re * (__int64)star.im + (__int64)m_rx_channel_i[0][i].im * (__int64)star.re;
            temp64 <<= 16;// 64b
            temp64 = (temp64 + (starsqr >> 1)) / starsqr;// range 16b
            m_rx_channelinv_i[1][i + 64].im      =  (__int16)(temp64);//16b
        }
    }

    void v_mimo_channel_estimation_i()
    {
        v_ci vtemp[4];
        
        v_ci vstar[2];
        v_q  vstarsqr[2];

        v_ci vcih1, vcih2;
        
        const v_cs vMulMask = VMASK::__0x80000001800000018000000180000001<v_cs>();
        const v_cs vNegMask = VMASK::__0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF<v_cs>();

        const complex16* pHTLTFMask = &m_vHTLTFMask[0][0];

        int i;

        for (i = 0; i < 64; i += 4)
        {
            v_cs  vhtmsk = (v_cs&)pHTLTFMask[i];

            v_cs& v11    = (v_cs&)m_rx_fsamples_i[0][i];
            v_cs& v12    = (v_cs&)m_rx_fsamples_i[0][i + 64];
            v_cs& v21    = (v_cs&)m_rx_fsamples_i[1][i];
            v_cs& v22    = (v_cs&)m_rx_fsamples_i[1][i + 64];

            v_cs& vh11   = (v_cs&)m_rx_channel_i[0][i];
            v_cs& vh12   = (v_cs&)m_rx_channel_i[0][i + 64];
            v_cs& vh21   = (v_cs&)m_rx_channel_i[1][i];
            v_cs& vh22   = (v_cs&)m_rx_channel_i[1][i + 64];

            v_cs& vtemph = (v_cs&)vtemp[0];

            vtemph = v_subs(v11, v12);
            vtemph = vtemph.v_shift_right_arithmetic(1);
            vtemph = v_xor(vtemph, vhtmsk);
            vh11   = v_sub(vtemph, vhtmsk);

            vtemph = v_adds(v11, v12); 
            vtemph = vtemph.v_shift_right_arithmetic(1);
            vtemph = v_xor(vtemph, vhtmsk);
            vh12   = v_sub(vtemph, vhtmsk);

            vtemph = v_subs(v21, v22);
            vtemph = vtemph.v_shift_right_arithmetic(1);
            vtemph = v_xor(vtemph, vhtmsk);
            vh21   = v_sub(vtemph, vhtmsk);

            vtemph = v_adds(v21, v22); 
            vtemph = vtemph.v_shift_right_arithmetic(1);
            vtemph = v_xor(vtemph, vhtmsk);
            vh22   = v_sub(vtemph, vhtmsk);
        }

        for (i = 0; i < 64; i += 4)
        {
            v_cs& vh11   = (v_cs&)m_rx_channel_i[0][i];
            v_cs& vh12   = (v_cs&)m_rx_channel_i[0][i + 64];
            v_cs& vh21   = (v_cs&)m_rx_channel_i[1][i];
            v_cs& vh22   = (v_cs&)m_rx_channel_i[1][i + 64];

            v_mul2ci(vh11, vh22, vMulMask, vtemp[0], vtemp[1]);
            v_mul2ci(vh12, vh21, vMulMask, vtemp[2], vtemp[3]);

            vstar[0] = v_sub(vtemp[0], vtemp[2]);
            vstar[1] = v_sub(vtemp[1], vtemp[3]);

            vstarsqr[0] = vstar[0].v_sqr2q();
            vstarsqr[1] = vstar[1].v_sqr2q();

            // Add 1 to avoid divided by 0 exception
            vstarsqr[0] = v_sub(vstarsqr[0], (v_q&)vNegMask);
            vstarsqr[1] = v_sub(vstarsqr[1], (v_q&)vNegMask);
            //
            v_cs &vinvh11 = (v_cs&)m_rx_channelinv_i[0][i];
            v_cs &vinvh12 = (v_cs&)m_rx_channelinv_i[0][i + 64];
            v_cs &vinvh21 = (v_cs&)m_rx_channelinv_i[1][i];
            v_cs &vinvh22 = (v_cs&)m_rx_channelinv_i[1][i + 64];

            v_cq &vres1 = (v_cq&)vtemp[0];
            v_cq &vres2 = (v_cq&)vtemp[1];
            v_cq &vres3 = (v_cq&)vtemp[2];
            v_cq &vres4 = (v_cq&)vtemp[3];

            // - invh11
            vh22.v_convert2ci(vcih1, vcih2);

            v_conjmul2cq(vcih1, vstar[0], vres1, vres2);
            v_conjmul2cq(vcih2, vstar[1], vres3, vres4);

            vres1 = vres1.v_shift_left(16);
            vres2 = vres2.v_shift_left(16);
            vres3 = vres3.v_shift_left(16);
            vres4 = vres4.v_shift_left(16);

            vres1[0].re /= vstarsqr[0][0]; vres1[0].im /= vstarsqr[0][0];
            vres2[0].re /= vstarsqr[0][1]; vres2[0].im /= vstarsqr[0][1];
            vres3[0].re /= vstarsqr[1][0]; vres3[0].im /= vstarsqr[1][0];
            vres4[0].re /= vstarsqr[1][1]; vres4[0].im /= vstarsqr[1][1];

            ((v_ci&)vres1) = v_convert2ci_lo(vres1, vres2);
            ((v_ci&)vres3) = v_convert2ci_lo(vres3, vres4);

            v_cs &vout11 = (v_cs&)vres2;
            vout11       = v_convert2cs((v_ci&)vres1, (v_ci&)vres3);
            vinvh11      = vout11;

            // - invh12
            vh12.v_convert2ci(vcih1, vcih2);

            v_conjmul2cq(vcih1, vstar[0], vres1, vres2);
            v_conjmul2cq(vcih2, vstar[1], vres3, vres4);

            vres1 = vres1.v_shift_left(16);
            vres2 = vres2.v_shift_left(16);
            vres3 = vres3.v_shift_left(16);
            vres4 = vres4.v_shift_left(16);

            vres1[0].re /= vstarsqr[0][0]; vres1[0].im /= vstarsqr[0][0];
            vres2[0].re /= vstarsqr[0][1]; vres2[0].im /= vstarsqr[0][1];
            vres3[0].re /= vstarsqr[1][0]; vres3[0].im /= vstarsqr[1][0];
            vres4[0].re /= vstarsqr[1][1]; vres4[0].im /= vstarsqr[1][1];

            ((v_ci&)vres1) = v_convert2ci_lo(vres1, vres2);
            ((v_ci&)vres3) = v_convert2ci_lo(vres3, vres4);

            v_cs &vout12 = (v_cs&)vres2;
            vout12       = v_convert2cs((v_ci&)vres1, (v_ci&)vres3);
            vout12       = v_xor(vout12, vNegMask);
            vout12       = v_sub((v_cs&)vout12, (v_cs&)vNegMask);
            vinvh12      = vout12;

            // - invh21
            vh21.v_convert2ci(vcih1, vcih2);

            v_conjmul2cq(vcih1, vstar[0], vres1, vres2);
            v_conjmul2cq(vcih2, vstar[1], vres3, vres4);

            vres1 = vres1.v_shift_left(16);
            vres2 = vres2.v_shift_left(16);
            vres3 = vres3.v_shift_left(16);
            vres4 = vres4.v_shift_left(16);

            vres1[0].re /= vstarsqr[0][0]; vres1[0].im /= vstarsqr[0][0];
            vres2[0].re /= vstarsqr[0][1]; vres2[0].im /= vstarsqr[0][1];
            vres3[0].re /= vstarsqr[1][0]; vres3[0].im /= vstarsqr[1][0];
            vres4[0].re /= vstarsqr[1][1]; vres4[0].im /= vstarsqr[1][1];

            ((v_ci&)vres1) = v_convert2ci_lo(vres1, vres2);
            ((v_ci&)vres3) = v_convert2ci_lo(vres3, vres4);

            v_cs &vout21 = (v_cs&)vres2;
            vout21       = v_convert2cs((v_ci&)vres1, (v_ci&)vres3);
            vout21       = v_xor(vout21, vNegMask);
            vout21       = v_sub((v_cs&)vout21, (v_cs&)vNegMask);
            vinvh21      = vout21;

            // - invh22
            vh11.v_convert2ci(vcih1, vcih2);

            v_conjmul2cq(vcih1, vstar[0], vres1, vres2);
            v_conjmul2cq(vcih2, vstar[1], vres3, vres4);

            vres1 = vres1.v_shift_left(16);
            vres2 = vres2.v_shift_left(16);
            vres3 = vres3.v_shift_left(16);
            vres4 = vres4.v_shift_left(16);

            vres1[0].re /= vstarsqr[0][0]; vres1[0].im /= vstarsqr[0][0];
            vres2[0].re /= vstarsqr[0][1]; vres2[0].im /= vstarsqr[0][1];
            vres3[0].re /= vstarsqr[1][0]; vres3[0].im /= vstarsqr[1][0];
            vres4[0].re /= vstarsqr[1][1]; vres4[0].im /= vstarsqr[1][1];

            ((v_ci&)vres1) = v_convert2ci_lo(vres1, vres2);
            ((v_ci&)vres3) = v_convert2ci_lo(vres3, vres4);

            v_cs &vout22 = (v_cs&)vres2;
            vout22       = v_convert2cs((v_ci&)vres1, (v_ci&)vres3);
            vinvh22      = vout22;
        }
    }

    void v_mimo_channel_estimation_f()
    {
        v_f vtwo;
        v_cf vhtemp;        

        complex32* pHTLTFMask = (complex32*)&m_vHTLTFMask_f[0];
        vtwo.v_setall(2.0f);
        
        int i;

        for (i = 0; i < 64; i += 2)
        {
            v_cf& v11 = (v_cf&)m_rx_fsamples_f[0][i];
            v_cf& v12 = (v_cf&)m_rx_fsamples_f[0][i + 64];
            v_cf& v21 = (v_cf&)m_rx_fsamples_f[1][i];
            v_cf& v22 = (v_cf&)m_rx_fsamples_f[1][i + 64];

            v_cf& vh11 = (v_cf&)m_rx_channel_f[0][i];
            v_cf& vh12 = (v_cf&)m_rx_channel_f[0][i + 64];
            v_cf& vh21 = (v_cf&)m_rx_channel_f[1][i];
            v_cf& vh22 = (v_cf&)m_rx_channel_f[1][i + 64];

            v_ci vMask = (v_ci&)pHTLTFMask[i];

            vhtemp = v_sub(v11, v12);
            vhtemp = v_div((v_f&)vhtemp, vtwo);
            vh11   = v_add((v_ci&)vhtemp, vMask);

            vhtemp = v_add(v11, v12);
            vhtemp = v_div((v_f&)vhtemp, vtwo);
            vh12   = v_add((v_ci&)vhtemp, vMask);

            vhtemp = v_sub(v21, v22);
            vhtemp = v_div((v_f&)vhtemp, vtwo);
            vh21   = v_add((v_ci&)vhtemp, vMask);

            vhtemp = v_add(v21, v22);
            vhtemp = v_div((v_f&)vhtemp, vtwo);
            vh22   = v_add((v_ci&)vhtemp, vMask);
        }

        // sse opt.
        v_cf vstar;
        v_f  vstarsq;
        v_ci vconjmask = VMASK::__0x80000000000000008000000000000000<v_ci>();
        v_ci vnegmask  = VMASK::__0x80000000800000008000000080000000<v_ci>();

        for (i = 0; i < 64; i += 2)
        {
            v_cf& vh11 = (v_cf&)m_rx_channel_f[0][i];
            v_cf& vh12 = (v_cf&)m_rx_channel_f[0][i + 64];
            v_cf& vh21 = (v_cf&)m_rx_channel_f[1][i];
            v_cf& vh22 = (v_cf&)m_rx_channel_f[1][i + 64];

            vstar = v_sub( v_mul(vh11, vh22), v_mul(vh12, vh21) );
            vstar = (v_cf&)v_add((v_ci&)vstar, vconjmask);

            vstarsq = v_mul((v_f&)vstar, (v_f&)vstar);
            vstarsq = v_add(vstarsq, vstarsq.v_shuffle<1, 0, 3, 2>());

            vstar   = (v_cf&)v_div((v_f&)vstar, vstarsq);

            v_cf& invh11 = (v_cf&)m_rx_channelinv_f[0][i];
            v_cf& invh12 = (v_cf&)m_rx_channelinv_f[0][i + 64];
            v_cf& invh21 = (v_cf&)m_rx_channelinv_f[1][i];
            v_cf& invh22 = (v_cf&)m_rx_channelinv_f[1][i + 64];

            invh11 = v_mul(vh22, vstar);
            invh12 = v_mul(vh12, vstar);
            invh12 = v_add((v_ci&)invh12, vnegmask);
            invh21 = v_mul(vh21, vstar);
            invh21 = v_add((v_ci&)invh21, vnegmask);
            invh22 = v_mul(vh11, vstar);
        }
    }

    void mimo_channel_estimation_f()
    {
        int sidx = 0;

        int i;
        for (i = 1; i <= 28; i++)
        {
            if (mimoLongPrePositive[i])
            {
                m_rx_channel_f[0][i].re      = (m_rx_fsamples_f[0][i].re - m_rx_fsamples_f[0][i + 64].re) / 2.0f;
                m_rx_channel_f[0][i].im      = (m_rx_fsamples_f[0][i].im - m_rx_fsamples_f[0][i + 64].im) / 2.0f;
                m_rx_channel_f[0][i + 64].re = (m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i + 64].re) / 2.0f;
                m_rx_channel_f[0][i + 64].im = (m_rx_fsamples_f[0][i].im + m_rx_fsamples_f[0][i + 64].im) / 2.0f;

                m_rx_channel_f[1][i].re      = (m_rx_fsamples_f[1][i].re - m_rx_fsamples_f[1][i + 64].re) / 2.0f;
                m_rx_channel_f[1][i].im      = (m_rx_fsamples_f[1][i].im - m_rx_fsamples_f[1][i + 64].im) / 2.0f;
                m_rx_channel_f[1][i + 64].re = (m_rx_fsamples_f[1][i].re + m_rx_fsamples_f[1][i + 64].re) / 2.0f;
                m_rx_channel_f[1][i + 64].im = (m_rx_fsamples_f[1][i].im + m_rx_fsamples_f[1][i + 64].im) / 2.0f;
            }
            else
            {
                m_rx_channel_f[0][i].re      = (m_rx_fsamples_f[0][i].re - m_rx_fsamples_f[0][i + 64].re) / -2.0f;
                m_rx_channel_f[0][i].im      = (m_rx_fsamples_f[0][i].im - m_rx_fsamples_f[0][i + 64].im) / -2.0f;
                m_rx_channel_f[0][i + 64].re = (m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i + 64].re) / -2.0f;
                m_rx_channel_f[0][i + 64].im = (m_rx_fsamples_f[0][i].im + m_rx_fsamples_f[0][i + 64].im) / -2.0f;

                m_rx_channel_f[1][i].re      = (m_rx_fsamples_f[1][i].re - m_rx_fsamples_f[1][i + 64].re) / -2.0f;
                m_rx_channel_f[1][i].im      = (m_rx_fsamples_f[1][i].im - m_rx_fsamples_f[1][i + 64].im) / -2.0f;
                m_rx_channel_f[1][i + 64].re = (m_rx_fsamples_f[1][i].re + m_rx_fsamples_f[1][i + 64].re) / -2.0f;
                m_rx_channel_f[1][i + 64].im = (m_rx_fsamples_f[1][i].im + m_rx_fsamples_f[1][i + 64].im) / -2.0f;
            }
        }

        for (i = 64 - 28; i < 64; i++)
        {
            if (mimoLongPrePositive[i])
            {
                m_rx_channel_f[0][i].re      = (m_rx_fsamples_f[0][i].re - m_rx_fsamples_f[0][i + 64].re) / 2.0f;
                m_rx_channel_f[0][i].im      = (m_rx_fsamples_f[0][i].im - m_rx_fsamples_f[0][i + 64].im) / 2.0f;
                m_rx_channel_f[0][i + 64].re = (m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i + 64].re) / 2.0f;
                m_rx_channel_f[0][i + 64].im = (m_rx_fsamples_f[0][i].im + m_rx_fsamples_f[0][i + 64].im) / 2.0f;

                m_rx_channel_f[1][i].re      = (m_rx_fsamples_f[1][i].re - m_rx_fsamples_f[1][i + 64].re) / 2.0f;
                m_rx_channel_f[1][i].im      = (m_rx_fsamples_f[1][i].im - m_rx_fsamples_f[1][i + 64].im) / 2.0f;
                m_rx_channel_f[1][i + 64].re = (m_rx_fsamples_f[1][i].re + m_rx_fsamples_f[1][i + 64].re) / 2.0f;
                m_rx_channel_f[1][i + 64].im = (m_rx_fsamples_f[1][i].im + m_rx_fsamples_f[1][i + 64].im) / 2.0f;
            }
            else
            {
                m_rx_channel_f[0][i].re      = (m_rx_fsamples_f[0][i].re - m_rx_fsamples_f[0][i + 64].re) / -2.0f;
                m_rx_channel_f[0][i].im      = (m_rx_fsamples_f[0][i].im - m_rx_fsamples_f[0][i + 64].im) / -2.0f;
                m_rx_channel_f[0][i + 64].re = (m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i + 64].re) / -2.0f;
                m_rx_channel_f[0][i + 64].im = (m_rx_fsamples_f[0][i].im + m_rx_fsamples_f[0][i + 64].im) / -2.0f;

                m_rx_channel_f[1][i].re      = (m_rx_fsamples_f[1][i].re - m_rx_fsamples_f[1][i + 64].re) / -2.0f;
                m_rx_channel_f[1][i].im      = (m_rx_fsamples_f[1][i].im - m_rx_fsamples_f[1][i + 64].im) / -2.0f;
                m_rx_channel_f[1][i + 64].re = (m_rx_fsamples_f[1][i].re + m_rx_fsamples_f[1][i + 64].re) / -2.0f;
                m_rx_channel_f[1][i + 64].im = (m_rx_fsamples_f[1][i].im + m_rx_fsamples_f[1][i + 64].im) / -2.0f;
            }
        }
        //////////////////////////////////////////////////////////////////////////

        complexf star;
        float    starsqr;
        for (i = 1; i <= 28; i++)
        {
            star    = m_rx_channel_f[0][i] * m_rx_channel_f[1][i + 64] - m_rx_channel_f[0][i + 64] * m_rx_channel_f[1][i];
            star.im = -star.im;

            starsqr = star.re * star.re + star.im * star.im;
            star.re /= starsqr;
            star.im /= starsqr;

            m_rx_channelinv_f[0][i]      =  m_rx_channel_f[1][i + 64] * star;
            m_rx_channelinv_f[0][i + 64] = -m_rx_channel_f[0][i + 64] * star;
            m_rx_channelinv_f[1][i]      = -m_rx_channel_f[1][i] * star;
            m_rx_channelinv_f[1][i + 64] =  m_rx_channel_f[0][i] * star;
        }

        for (i = 64 - 28; i < 64; i++)
        {
            star    = m_rx_channel_f[0][i] * m_rx_channel_f[1][i + 64] - m_rx_channel_f[0][i + 64] * m_rx_channel_f[1][i];
            star.im = -star.im;

            starsqr = star.re * star.re + star.im * star.im;
            star.re /= starsqr;
            star.im /= starsqr;

            m_rx_channelinv_f[0][i]      =  m_rx_channel_f[1][i + 64] * star;
            m_rx_channelinv_f[0][i + 64] = -m_rx_channel_f[0][i + 64] * star;
            m_rx_channelinv_f[1][i]      = -m_rx_channel_f[1][i] * star;
            m_rx_channelinv_f[1][i + 64] =  m_rx_channel_f[0][i] * star;
        }
    }
    
    void v_mimo_channel_compensate_i()
    {
        const v_cs vMulMask = VMASK::__0x80000001800000018000000180000001<v_cs>();
        v_ci vcomp1[2], vcomp2[2];

        for (int i = 0; i < 64; i += 4)
        {
            v_cs &vinvh11 = (v_cs&)m_rx_channelinv_i[0][i];
            v_cs &vinvh12 = (v_cs&)m_rx_channelinv_i[0][i + 64];
            v_cs &vinvh21 = (v_cs&)m_rx_channelinv_i[1][i];
            v_cs &vinvh22 = (v_cs&)m_rx_channelinv_i[1][i + 64];
            v_cs &y1      = (v_cs&)m_rx_fsamples_i[0][i];
            v_cs &y2      = (v_cs&)m_rx_fsamples_i[1][i];
            v_cs& x1      = (v_cs&)m_rx_channel_compensated_i[0][i];
            v_cs& x2      = (v_cs&)m_rx_channel_compensated_i[1][i];

            v_mul2ci(vinvh11, y1, vMulMask, vcomp1[0], vcomp1[1]);
            v_mul2ci(vinvh12, y2, vMulMask, vcomp2[0], vcomp2[1]);

            vcomp1[0] = v_add(vcomp1[0], vcomp2[0]);
            vcomp1[1] = v_add(vcomp1[1], vcomp2[1]);

            vcomp1[0] = vcomp1[0].v_shift_right_arithmetic(9);
            vcomp1[1] = vcomp1[1].v_shift_right_arithmetic(9);

            x1 = v_convert2cs(vcomp1[0], vcomp1[1]);

            v_mul2ci(vinvh21, y1, vMulMask, vcomp1[0], vcomp1[1]);
            v_mul2ci(vinvh22, y2, vMulMask, vcomp2[0], vcomp2[1]);

            vcomp1[0] = v_add(vcomp1[0], vcomp2[0]);
            vcomp1[1] = v_add(vcomp1[1], vcomp2[1]);

            vcomp1[0] = vcomp1[0].v_shift_right_arithmetic(9);
            vcomp1[1] = vcomp1[1].v_shift_right_arithmetic(9);

            x2 = v_convert2cs(vcomp1[0], vcomp1[1]);
        }
    }


    void mimo_channel_compensate_i()
    {
        int i;
        __int32 ire, iim;
        // x^hat = Hy
        for (i = 1; i <= 28; i++)
        {
            ire = ( (__int32)m_rx_channelinv_i[0][i].re * (__int32)m_rx_fsamples_i[0][i].re - (__int32)m_rx_channelinv_i[0][i].im * (__int32)m_rx_fsamples_i[0][i].im)
                  + ((__int32)m_rx_channelinv_i[0][i + 64].re * (__int32)m_rx_fsamples_i[1][i].re - (__int32)m_rx_channelinv_i[0][i + 64].im * (__int32)m_rx_fsamples_i[1][i].im);

            iim = ((__int32)m_rx_channelinv_i[0][i].re * (__int32)m_rx_fsamples_i[0][i].im + (__int32)m_rx_channelinv_i[0][i].im * (__int32)m_rx_fsamples_i[0][i].re)
                + ((__int32)m_rx_channelinv_i[0][i + 64].re * (__int32)m_rx_fsamples_i[1][i].im + (__int32)m_rx_channelinv_i[0][i + 64].im * (__int32)m_rx_fsamples_i[1][i].re);

            m_rx_channel_compensated_i[0][i].re = (__int16)(ire >> 9);
            m_rx_channel_compensated_i[0][i].im = (__int16)(iim >> 9);

            //! ----------------------------------------------------------------------------------------------------------------------------------------
            ire = ((__int32)m_rx_channelinv_i[1][i].re * (__int32)m_rx_fsamples_i[0][i].re - (__int32)m_rx_channelinv_i[1][i].im * (__int32)m_rx_fsamples_i[0][i].im)
                + ((__int32)m_rx_channelinv_i[1][i + 64].re * (__int32)m_rx_fsamples_i[1][i].re - (__int32)m_rx_channelinv_i[1][i + 64].im * (__int32)m_rx_fsamples_i[1][i].im);

            iim = ((__int32)m_rx_channelinv_i[1][i].re * (__int32)m_rx_fsamples_i[0][i].im + (__int32)m_rx_channelinv_i[1][i].im * (__int32)m_rx_fsamples_i[0][i].re)
                + ((__int32)m_rx_channelinv_i[1][i + 64].re * (__int32)m_rx_fsamples_i[1][i].im + (__int32)m_rx_channelinv_i[1][i + 64].im * (__int32)m_rx_fsamples_i[1][i].re);

            m_rx_channel_compensated_i[1][i].re = (__int16)(ire >> 9);
            m_rx_channel_compensated_i[1][i].im = (__int16)(iim >> 9);
        }

        for (i = 64 - 28; i < 64; i++)
        {
            ire = ( (__int32)m_rx_channelinv_i[0][i].re * (__int32)m_rx_fsamples_i[0][i].re - (__int32)m_rx_channelinv_i[0][i].im * (__int32)m_rx_fsamples_i[0][i].im)
                + ((__int32)m_rx_channelinv_i[0][i + 64].re * (__int32)m_rx_fsamples_i[1][i].re - (__int32)m_rx_channelinv_i[0][i + 64].im * (__int32)m_rx_fsamples_i[1][i].im);

            iim = ((__int32)m_rx_channelinv_i[0][i].re * (__int32)m_rx_fsamples_i[0][i].im + (__int32)m_rx_channelinv_i[0][i].im * (__int32)m_rx_fsamples_i[0][i].re)
                + ((__int32)m_rx_channelinv_i[0][i + 64].re * (__int32)m_rx_fsamples_i[1][i].im + (__int32)m_rx_channelinv_i[0][i + 64].im * (__int32)m_rx_fsamples_i[1][i].re);

            m_rx_channel_compensated_i[0][i].re = (__int16)(ire >> 9);
            m_rx_channel_compensated_i[0][i].im = (__int16)(iim >> 9);

            //! ----------------------------------------------------------------------------------------------------------------------------------------
            ire = ((__int32)m_rx_channelinv_i[1][i].re * (__int32)m_rx_fsamples_i[0][i].re - (__int32)m_rx_channelinv_i[1][i].im * (__int32)m_rx_fsamples_i[0][i].im)
                + ((__int32)m_rx_channelinv_i[1][i + 64].re * (__int32)m_rx_fsamples_i[1][i].re - (__int32)m_rx_channelinv_i[1][i + 64].im * (__int32)m_rx_fsamples_i[1][i].im);

            iim = ((__int32)m_rx_channelinv_i[1][i].re * (__int32)m_rx_fsamples_i[0][i].im + (__int32)m_rx_channelinv_i[1][i].im * (__int32)m_rx_fsamples_i[0][i].re)
                + ((__int32)m_rx_channelinv_i[1][i + 64].re * (__int32)m_rx_fsamples_i[1][i].im + (__int32)m_rx_channelinv_i[1][i + 64].im * (__int32)m_rx_fsamples_i[1][i].re);

            m_rx_channel_compensated_i[1][i].re = (__int16)(ire >> 9);
            m_rx_channel_compensated_i[1][i].im = (__int16)(iim >> 9);
        }
    }


    void v_mimo_channel_compensate_f()
    {
        int i;
        for (i = 0; i < 30; i += 2)
        {
            v_cf& invh11 = (v_cf&)m_rx_channelinv_f[0][i];
            v_cf& invh12 = (v_cf&)m_rx_channelinv_f[0][i + 64];
            v_cf& invh21 = (v_cf&)m_rx_channelinv_f[1][i];
            v_cf& invh22 = (v_cf&)m_rx_channelinv_f[1][i + 64];
            v_cf& y1     = (v_cf&)m_rx_fsamples_f[0][i];
            v_cf& y2     = (v_cf&)m_rx_fsamples_f[1][i];
            v_cf& x1     = (v_cf&)m_rx_channel_compensated_f[0][i];
            v_cf& x2     = (v_cf&)m_rx_channel_compensated_f[1][i];

            x1 = v_add( v_mul(invh11, y1), v_mul(invh12, y2) );
            x2 = v_add( v_mul(invh21, y1), v_mul(invh22, y2) );
        }

        for (i = 64 - 28; i < 64; i += 2)
        {
            v_cf& invh11 = (v_cf&)m_rx_channelinv_f[0][i];
            v_cf& invh12 = (v_cf&)m_rx_channelinv_f[0][i + 64];
            v_cf& invh21 = (v_cf&)m_rx_channelinv_f[1][i];
            v_cf& invh22 = (v_cf&)m_rx_channelinv_f[1][i + 64];
            v_cf& y1     = (v_cf&)m_rx_fsamples_f[0][i];
            v_cf& y2     = (v_cf&)m_rx_fsamples_f[1][i];
            v_cf& x1     = (v_cf&)m_rx_channel_compensated_f[0][i];
            v_cf& x2     = (v_cf&)m_rx_channel_compensated_f[1][i];

            x1 = v_add( v_mul(invh11, y1), v_mul(invh12, y2) );
            x2 = v_add( v_mul(invh21, y1), v_mul(invh22, y2) );
        }
    }


    void mimo_channel_compensate_f()
    {
        int i;

        // x^hat = Hy
        for (i = 1; i <= 28; i++)
        {
            m_rx_channel_compensated_f[0][i] = m_rx_channelinv_f[0][i] * m_rx_fsamples_f[0][i]
            + m_rx_channelinv_f[0][i + 64] * m_rx_fsamples_f[1][i];

            m_rx_channel_compensated_f[1][i] = m_rx_channelinv_f[1][i] * m_rx_fsamples_f[0][i]
            + m_rx_channelinv_f[1][i + 64] * m_rx_fsamples_f[1][i];
        }

        for (i = 64 - 28; i < 64; i++)
        {
            m_rx_channel_compensated_f[0][i] = m_rx_channelinv_f[0][i] * m_rx_fsamples_f[0][i]
            + m_rx_channelinv_f[0][i + 64] * m_rx_fsamples_f[1][i];

            m_rx_channel_compensated_f[1][i] = m_rx_channelinv_f[1][i] * m_rx_fsamples_f[0][i]
            + m_rx_channelinv_f[1][i + 64] * m_rx_fsamples_f[1][i];
        }
    }

    void remove_dc(complex16* pc, int count)
    {
#if 1
        complex32 isum(0, 0);

        for (int i = 0; i < count; i++)
        {
            isum.re += (int)pc[i].re;
            isum.im += (int)pc[i].im;
        }

        complex16 iavg;
        iavg.re = (short)(isum.re / count);
        iavg.im = (short)(isum.im / count);

        for (int i = 0; i < count; i++)
        {
            pc[i].re -= iavg.re;
            pc[i].im -= iavg.im;
        }
#else
        complexf fsum(0.0f, 0.0f);

        for (int i = 0; i < count; i++)
        {
            fsum.re += (float)pc[i].re;
            fsum.im += (float)pc[i].im;
        }

        fsum.re /= (float)count;
        fsum.im /= (float)count;

        for (int i = 0; i < count; i++)
        {
            pc[i].re -= (short)fsum.re;
            pc[i].im -= (short)fsum.im;
        }
#endif
    }

    void cvt2cf(complex16* pc16, complexf* pcf, int count)
    {
#if 1
        for (int i = 0; i < count; i += v_cs::elem_cnt)
        {
            v_cs& vsinput   = (v_cs&)pc16[i];
            v_cf& vfoutput1 = (v_cf&)pcf[i];
            v_cf& vfoutput2 = (v_cf&)pcf[i + 2];
            v_ci vci1, vci2;
            vsinput.v_convert2ci(vci1, vci2);
            vfoutput1 = vci1.v_convert2cf();
            vfoutput2 = vci2.v_convert2cf();
        }
#else
        for (int i = 0; i < count; i++)
        {
            pcf[i].re = (float)pc16[i].re;
            pcf[i].im = (float)pc16[i].im;
        }
#endif
    }

    void cvt2cs(complexf* pcf, complex16* pc16, int count, float scale)
    {
#if 1
        v_f vfscale = _mm_set1_ps(scale);
        for (int i = 0; i < count; i += 4)
        {
            v_f& vfinput1 = (v_f&)pcf[i];
            v_f& vfinput2 = (v_f&)pcf[i + 2];
            v_s& vsoutput = (v_s&)pc16[i];
            v_f vfoutput1, vfoutput2;
            v_i vi1, vi2;
            vfoutput1 = v_mul(vfinput1, vfscale);
            vfoutput2 = v_mul(vfinput2, vfscale);
            vi1 = vfoutput1.v_truncate2i();
            vi2 = vfoutput2.v_truncate2i();
            vsoutput = v_convert2s(vi1, vi2);
        }
#else
        for (int i = 0; i < count; i++)
        {
            pc16[i].re = (__int16)(pcf[i].re * scale);
            pc16[i].im = (__int16)(pcf[i].im * scale);
        }
#endif
    }

    void pilot_tracking_i(complex16* pcs, int count)
    {
        short th1 = dsp_math::atan(pcs[64 - 21].re, pcs[64 - 21].im);
        short th2 = dsp_math::atan(pcs[64 - 07].re, pcs[64 - 07].im);
        short th3 = dsp_math::atan(pcs[07].re, pcs[07].im);
        short th4 = dsp_math::atan(pcs[21].re, pcs[21].im);

        short ThisTheta = (th1 + th2 + th3 + th4) >> 2;

        m_rx_cfo_theta_i += ThisTheta;
    }

    void pilot_tracking_f(complexf* pcf, int count)
    {
        float th1 = atan(pcf[64 - 21].im / pcf[64 - 21].re);
        float th2 = atan(pcf[64 - 07].im / pcf[64 - 07].re);
        float th3 = atan(pcf[07].im / pcf[07].re);
        float th4 = atan(pcf[21].im / pcf[21].re);

        //printf("pilot1=%f, %f\n", pcf[64 - 21].re, pcf[64 - 21].im);
        //printf("pilot2=%f, %f\n", pcf[64 - 7].re, pcf[64 - 7].im);
        //printf("pilot3=%f, %f\n", pcf[7].re, pcf[7].im);
        //printf("pilot4=%f, %f\n", pcf[21].re, pcf[21].im);
        //printf("%f\t%f\t%f\t%f\n", th1, th2, th3, th4);

        float avgtheta = (th1 + th2 + th3 + th4) / 4.0f;

        m_pilotSum_f             -= m_pilotHis_f[m_pilotPt_f];
        m_pilotSum_f             += avgtheta;
        m_pilotHis_f[m_pilotPt_f] = avgtheta;
        //m_rx_cfo_delta         += m_pilotSum_f * 2 / (pilot_tracking_length + 1);// tracking
        m_rx_cfo_theta_f         += avgtheta;

        //printf("m_rx_cfo=%f, m_rx_cfo_delta=%f\n", m_rx_cfo, m_rx_cfo_delta);

        m_pilotPt_f++;
        if (m_pilotPt_f == pilot_tracking_length)
        {
            m_pilotPt_f = 0;
        }
    }

    fft_f m_fftw;

    dot11n_rx_symbol m_rxfsymbol[2];

    dot11n_rx_symbol m_rxsymbol[2];
    static const int m_vsamplecnt = 20; // 20 v_cs, 80 complex16
    int m_vsampleidx;

    int m_rx_state;

    int m_rx_sample_idx;
    int m_rx_symbol_count;

    void rx_scheduler();

    bool rx_on_frame_detection(dot11n_rx_stream& rxstream);
    bool rx_on_l_ltf(dot11n_rx_stream& rxstream);
    bool rx_on_l_sig(dot11n_rx_stream& rxstream);
    bool rx_on_ht_sig1(dot11n_rx_stream& rxstream);
    bool rx_on_ht_sig2(dot11n_rx_stream& rxstream);
    bool rx_on_ht_stf(dot11n_rx_stream& rxstream);
    bool rx_on_ht_ltf1(dot11n_rx_stream& rxstream);
    bool rx_on_ht_ltf2(dot11n_rx_stream& rxstream);
    bool rx_on_ht_data(dot11n_rx_stream& rxstream);
    bool rx_on_l_data(dot11n_rx_stream& rxstream);

    //////////////////////////////////////////////////////////////////////////

    unsigned int m_rx_l_signal;
    unsigned int m_rx_l_length;
    unsigned int m_rx_l_rate;
    unsigned int m_rx_l_dbps;

    int m_rx_mcs;
    int m_rx_frame_length;

    struct dot11n_rate_param
    {
        int cdbps;
        int ndbps;
    };

    static const dot11n_rate_param m_dot11n_rate_params[16];

    __forceinline int calc_rx_symbol_count(int mcs, int length_bytes)
    {
        int ntotalbits   = length_bytes * 8 + 16 + 6;
        int nsymbolcount = (ntotalbits + m_dot11n_rate_params[mcs].ndbps) / m_dot11n_rate_params[mcs].ndbps;

        // pad one symbol so that it wont produce half byte
        // Q: after all, we will pad some symbols, why do we have to pad this symbol here?
        //nsymbolcount += (nsymbolcount & 0x1);

        return nsymbolcount;
    }

    __forceinline
        bool Parse_L_SIG(
        unsigned int uiSignal,
        unsigned int * puiLength,
        unsigned int * puiRate,
        unsigned int * puiDBPS)
    {
        unsigned int uiParity;
        // signal rate look up table
        static const unsigned int g11a_rguiDBPSLookUp[16] = {
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

        uiSignal &= 0xFFFFFF;
        if (uiSignal & 0xFC0010) // all these bits should be always zero
            return FALSE;

        uiParity = (uiSignal >> 16) ^ (uiSignal);
        uiParity = (uiParity >> 8) ^ (uiParity);
        uiParity = (uiParity >> 4) ^ (uiParity);
        uiParity = (uiParity >> 2) ^ (uiParity);
        uiParity = (uiParity >> 1) ^ (uiParity);
        if (uiParity & 0x1)
            return FALSE;

        (*puiRate) = uiSignal & 0xF;
        if (!((*puiRate) & 0x8))
            return FALSE;

        (*puiLength) = (uiSignal >> 5) & 0xFFF;
        (*puiDBPS) = g11a_rguiDBPSLookUp[*puiRate];

        return TRUE;
    }

    template<int ns> __forceinline void stream_joiner(unsigned __int8 pInput[][320], int count, unsigned __int8* pOutput);
    template<> __forceinline void stream_joiner<1>(unsigned __int8 pInput[][320], int count, unsigned __int8* pOutput)
    {
        int iOut = 0;

// has issues...., produces 128 bytes instead of 104 bytes.
// overrides the next buffer
#if 0
        v_b vinput1, vinput2;
        v_b voutput1, voutput2;
        for (int iIn = 0; iIn < count; iIn += 16, iOut += 32)
        {
            vinput1.v_load(&pInput[0][iIn]);
            vinput2.v_load(&pInput[1][iIn]);
            voutput1 = v_unpack_lo(vinput1, vinput2);
            voutput1.v_storeu(&pOutput[iOut]);
            voutput2 = v_unpack_hi(vinput1, vinput2);
            voutput2.v_storeu(&pOutput[iOut + 16]);

            for (int i = 0; i < 16; i++)
            {
                printf("%d ", vinput1[i]);
            }
            printf("\n");
            for (int i = 0; i < 16; i++)
            {
                printf("%d ", vinput2[i]);
            }
            printf("\n");
            for (int i = 0; i < 32; i++)
            {
                printf("%d ", pOutput[iOut + i]);
            }
            printf("\n\n");
        }
#else
        for (int iIn = 0; iIn < count; iIn++, iOut += 2)
        {
            pOutput[iOut]     = pInput[0][iIn];
            pOutput[iOut + 1] = pInput[1][iIn];
        }
#endif
    }

    template<> __forceinline void stream_joiner<2>(unsigned __int8 pInput[][320], int count,  unsigned __int8* pOutput)
    {
        int iOut = 0;
#if 1
        v_s vinput1, vinput2;
        for (int iIn = 0; iIn < count; iIn += 16, iOut += 32)
        {
            vinput1.v_load(&pInput[0][iIn]);
            vinput2.v_load(&pInput[1][iIn]);
            v_unpack_lo(vinput1, vinput2).v_storeu(&pOutput[iOut]);
            v_unpack_hi(vinput1, vinput2).v_storeu(&pOutput[iOut + 16]);
        }
#else
        for (int iIn = 0; iIn < count; iIn += 2, iOut += 4)
        {
            pOutput[iOut]     = pInput[0][iIn + 0];
            pOutput[iOut + 1] = pInput[0][iIn + 1];
            pOutput[iOut + 2] = pInput[1][iIn + 0];
            pOutput[iOut + 3] = pInput[1][iIn + 1];
        }
#endif
    }

    template<> __forceinline void stream_joiner<3>(unsigned __int8 pInput[][320], int count, unsigned __int8* pOutput)
    {
        int iOut = 0;
#if 0
        v_b::type vmask1 = {0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
#else
        for (int iIn = 0; iIn < count; iIn += 3, iOut += 6)
        {
            pOutput[iOut]     = pInput[0][iIn + 0];
            pOutput[iOut + 1] = pInput[0][iIn + 1];
            pOutput[iOut + 2] = pInput[0][iIn + 2];
            pOutput[iOut + 3] = pInput[1][iIn + 0];
            pOutput[iOut + 4] = pInput[1][iIn + 1];
            pOutput[iOut + 5] = pInput[1][iIn + 2];
        }
#endif
    }

    //////////////////////////////////////////////////////////////////////////
#define DOT11A_RATE_6M  0xB // 1-011
#define DOT11A_RATE_9M  0XF // 1-111
#define DOT11A_RATE_12M 0xA // 1-010
#define DOT11A_RATE_18M 0xE // 1-110
#define DOT11A_RATE_24M 0x9 // 1-001
#define DOT11A_RATE_36M 0xD // 1-101
#define DOT11A_RATE_48M 0x8 // 1-000
#define DOT11A_RATE_54M 0xC // 1-100

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

    volatile unsigned int m_workindicator;

    viterbi_fifo m_rx_viterbi_fifo;

    crc::crc32 m_rx_crc32;
    descrambler::dot11n_descrambler m_rx_descrambler;
    int m_rx_viterbi_symbol_count;
    int m_rx_crc32_correct;
    unsigned char* m_rx_decoded_frame;
    //////////////////////////////////////////////////////////////////////////

    dsp_viterbi m_rxviterbi;
    dsp_viterbi::input_param m_rx_viterbi_param;
    dsp_viterbi::Trellis<5000 * BITS_PER_BYTE, 12>   m_rx_viterbi_trellis;

    //////////////////////////////////////////////////////////////////////////
    enum
    {
        job_waiting,
        job_running,
        job_done
    };
    
    volatile unsigned int m_viterbi_status;
    volatile unsigned int m_viterbi_stop;
    volatile unsigned int m_viterbi_affinity;

    void start_viterbi();
    void stop_viterbi();

    void viterbi_worker();

    static DWORD WINAPI viterbi_thread(LPVOID lpThreadParam);
};