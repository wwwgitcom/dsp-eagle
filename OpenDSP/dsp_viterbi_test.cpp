#include "stdafx.h"


int soft_demap(complexf *in_ary, unsigned char *soft_ary, int len)
{
    int i;
    char softvalue;
    //	float temp;
    for (i=0; i<len; i++)
    {
        if (in_ary[i].re > 0.0f)
            softvalue = (int)(in_ary[i].re*2.0f+0.5f);
        else
            softvalue = (int)(in_ary[i].re*2.0f-0.5f);
        // saturation
        if (softvalue < -4)
            softvalue = -4;
        else if (softvalue > 3)
            softvalue = 3;

        softvalue += 3;

        soft_ary[i] = (unsigned char)softvalue;
    }
    return 0;
}




/*
1）如果你的软比特从【0，7】（3bits），你添0的话显然有问题。
如果你的软比特是从【－3，3】（3bits），添0没有问题。

2）注意你的SNR。不要用同样的SNR来比较1/2 , 2/3 , 3/4 , 5/6 的性能。
Guass channel   QPSK   BER = 2*10^-4
1/2      3.1dB
2/3      4.9dB
3/4      5.9dB
5/6      6.9dB
7/8      7.7dB
according to DVB-T EN300744 pages 40.
*/

struct ViterbiTest 
{
    static const int m_nSource  = 300; // max 536870910
    //static const int m_nEncoded = m_nSource * 2;// 1/2 coding
    static const int m_nEncoded = m_nSource * 3 / 2;// 2/3 coding
    //static const int m_nEncoded = m_nSource * 4 / 3;// 3/4 coding

    //static const int m_nMapped   = m_nEncoded * 8;//bpsk
    //static const int m_nDemapped = m_nMapped;//bpsk
    //static const int m_nMapped  = m_nEncoded * 4;//qpsk
    //static const int m_nMapped    = m_nEncoded * 2;//16qam
    //static const int m_nDemapped  = m_nMapped * 4;//16qam

    static const int m_nMapped    = m_nEncoded * 8 / 6;//64qam
    static const int m_nDemapped  = m_nMapped * 6;//64qam

    convolution::encoder_1_2 m_conv12;
    convolution::encoder_2_3 m_conv23;
    convolution::encoder_3_4 m_conv34;
    scrambler::dot11n_scrambler m_scrambler;
    mapper::dsp_mapper_bpsk<complex16>  m_mapbpsk;
    mapper::dsp_mapper_qpsk<complex16>  m_mapqpsk;
    mapper::dsp_mapper_16qam<complex16>  m_map16qam;
    mapper::dsp_mapper_64qam<complex16>  m_map64qam;

    demapper::dsp_demapper m_demapper;

    unsigned __int8 m_Source[m_nSource];
    unsigned __int8 m_ScrambledSource[m_nSource];
    unsigned __int8 m_EncodedSource[m_nEncoded];

    unsigned __int8 m_Decoded[m_nSource + 64];
    unsigned __int8 m_Descrambled[m_nSource];

    v_align(16) complexf  m_Mapped_f[m_nMapped];
    v_align(16) complex16 m_Mapped_i[m_nMapped];
    v_align(16) unsigned __int8 m_SoftBits[m_nMapped];
    v_align(16) unsigned __int8 m_Demapped[m_nDemapped];

    dsp_channel::AWGN m_AWGNChannel;

    dsp_plot m_plot;

    unsigned __int8 m_Demap_BPSK_LUT[256];


    ViterbiTest() : m_conv12(0x00), m_conv23(0x00), m_conv34(0x00), m_scrambler(0xCC), 
        m_mapbpsk(complex16(127.0f, 0.0f)),
        m_mapqpsk(complex16(127.0f / sqrt(2.0f), 127.0f / sqrt(2.0f))),
        m_map16qam(complex16(127.0f / sqrt(10.0f), 127.0f / sqrt(10.0f))),
        m_map64qam(complex16(127.0f / sqrt(42.0f), 127.0f / sqrt(42.0f)))
    {
    }

    void RandomSource()
    {
        srand(GetTickCount());
        for (int i = 0; i < m_nSource; i++)
        {
            m_Source[i] = 3;//rand();
        }
        m_Source[m_nSource - 1] = 0;
    }

    void DemapBPSK(complex16* pcInput, unsigned __int8* pOutput, int nInput)
    {
        m_demapper.demap_limit_bpsk((v_cs*)pcInput, nInput / 4);

        for (int i = 0; i < nInput; i++)
        {
            m_demapper.demap_bpsk_i(pcInput[i], &pOutput[i]);
            //pOutput[i] = m_Demap_BPSK_LUT[pcInput[i].re];
        }
    }

    void DemapQPSK(complex16* pcInput, unsigned __int8* pOutput, int nInput)
    {
        m_demapper.demap_limit_qpsk((v_cs*)pcInput, nInput / 4);

        int j = 0;
        for (int i = 0; i < nInput; i++)
        {
            m_demapper.demap_qpsk(pcInput[i], &pOutput[j]);
            j += 2;
        }
    }

    void Demap16QAM(complex16* pcInput, unsigned __int8* pOutput, int nInput)
    {
        m_demapper.demap_limit_16qam((v_cs*)pcInput, nInput / 4);

        int j = 0;
        for (int i = 0; i < nInput; i++)
        {
            m_demapper.demap_16qam(pcInput[i], &pOutput[j]);
            j += 4;
        }
    }

    void Demap64QAM(complex16* pcInput, unsigned __int8* pOutput, int nInput)
    {
        m_demapper.demap_limit_64qam((v_cs*)pcInput, nInput / 4);

        int j = 0;
        for (int i = 0; i < nInput; i++)
        {
            m_demapper.demap_64qam(pcInput[i], &pOutput[j]);
            j += 6;
        }
    }


    void Run_Test0()
    {
        m_Source[0] = 0x0a;
        m_Source[1] = 0xdc;
        m_Source[2] = 0x05;
        m_Source[3] = 0x03;
        m_Source[4] = 0x10;
        m_Source[5] = 0x00;
        unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
        // conv encode
        m_conv12.reset(0);
        for (int i = 0; i < m_nSource; i++)
        {
            m_conv12(m_Source[i], pEncoded[i]);
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
            Mapped = m_mapbpsk[m_EncodedSource[i]];
        }

        DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // bpsk 1/2
    void Run_Test1(float EbN0)
    {
        RandomSource();

        unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
        // conv encode
        m_conv12.reset(0);
        for (int i = 0; i < m_nSource; i++)
        {
            m_conv12(m_Source[i], pEncoded[i]);
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
            Mapped = m_mapbpsk[m_EncodedSource[i]];
        }

        float CodingRate = 1.0f / 2.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate);
        m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
        //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

        //for (int i = 0; i < m_nMapped; i++)
        //{
        //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
        //}
        //demap
        DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // bpsk 2/3
    void Run_Test2(float EbN0)
    {
        RandomSource();

        unsigned __int8* pEncoded = m_EncodedSource;
        // conv encode
        m_conv23.reset(0);
        for (int i = 0; i < m_nSource; i += 2)
        {
            m_conv23(&m_Source[i], pEncoded);
            pEncoded += 3;
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
            Mapped = m_mapbpsk[m_EncodedSource[i]];
        }

        float CodingRate = 2.0f / 3.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate);
        //m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
        //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

        //for (int i = 0; i < m_nMapped; i++)
        //{
        //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
        //}
        //demap
        DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // bpsk 3/4
    void Run_Test3(float EbN0)
    {
        RandomSource();

        unsigned __int8* pEncoded = m_EncodedSource;
        // conv encode
        m_conv34.reset(0);
        for (int i = 0; i < m_nSource; i += 3)
        {
            m_conv34(&m_Source[i], pEncoded);
            pEncoded += 4;
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
            Mapped = m_mapbpsk[m_EncodedSource[i]];
        }

        float CodingRate = 3.0f / 4.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate);
        //m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
        //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

        //for (int i = 0; i < m_nMapped; i++)
        //{
        //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
        //}
        //demap
        DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
    }

    void Run(float EbN0)
    {
        // source
        //memset(m_Source, 0xF5, m_nSource);
        //m_Source[m_nSource - 1] = 0;
        RandomSource();
        // scramble
#if 0
        for (int i = 0; i < m_nSource; i++)
        {
            m_scrambler(m_Source[i], m_ScrambledSource[i]);
        }
#endif

#define _CC23

#ifdef _CC12
        unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
        // conv encode
        m_conv12.reset(0);
        for (int i = 0; i < m_nSource; i++)
        {
            m_conv12(m_Source[i], pEncoded[i]);
            //m_conv12(m_ScrambledSource[i], pEncoded[i]);
        }
#endif
#ifdef _CC23
        unsigned __int8* pEncoded = m_EncodedSource;
        // conv encode
        m_conv23.reset(0);
        for (int i = 0; i < m_nSource; i += 2)
        {
            m_conv23(&m_Source[i], pEncoded);
            pEncoded += 3;
        }
#endif
#ifdef _CC34
        unsigned __int8* pEncoded = m_EncodedSource;
        // conv encode
        m_conv34.reset(0);
        for (int i = 0; i < m_nSource; i += 3)
        {
            m_conv34(&m_Source[i], pEncoded);
            pEncoded += 4;
        }
#endif


        //for (unsigned int i = 0; i < m_nEncoded; i++)
        //{
        //    if (i % 16 == 0)
        //    {
        //        printf("%8d: ", i);
        //    }
        //    printf("%02X ", m_EncodedSource[i]);

        //    if (((i + 1) % 16) == 0)
        //    {
        //        printf("\n");
        //    }
        //}
        //printf("\n");

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_f[i * 8]);
            Mapped = m_mapbpsk[m_EncodedSource[i]];
        }



#if 0
        m_plot.command("set title 'ideal mapped symbols'");
        m_plot.begin_plot("w p 7");
        for (int i = 0; i < m_nMapped; i++)
        {
            m_plot.data("%d\t%d", m_Mapped[i].re, m_Mapped[i].im);
        }
        m_plot.end_plot();
#endif
        // noise
        //% Define number of bits per symbol (k).
        //M = 4;  % or 2
        //k = log2(M);
        //codeRate = 1/2;
        //Adjust SNR for coded bits and multi-bit symbols.
        //adjSNR = EbNo - 10*log10(1/codeRate) + 10*log10(k);

        float CodingRate = 3.0f / 4.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate);
        m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
        //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

        //printf("EbN0=%f, EsN0=%f\n", EbN0, EsN0);

        //soft_demap(m_Mapped_f, m_Demapped, m_nMapped);

        demapper::dsp_demapper::build_soft_bits_bpsk(EsN0, m_Demap_BPSK_LUT);

        //demapper::dsp_demapper::demodulate_soft_bits_bpsk(m_Mapped, m_nMapped, EbN0, m_SoftBits_i);
#if 0
        m_plot.command("set title 'AWGN mapped symbols'");
        m_plot.begin_plot("w p 7");
        for (int i = 0; i < m_nMapped; i++)
        {
            //m_plot.data("%f\t%f", m_Mapped[i].re, m_Mapped[i].im);
            m_plot.data("%d", m_SoftBits_i[i]);
        }
        m_plot.end_plot();
        getchar();
#endif

        //for (int i = 0; i < m_nMapped; i++)
        //{
        //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
        //}
        //demap
        DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // qpsk 1/2
    void Run_Test4(float EbN0)
    {
        RandomSource();

        unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
        // conv encode
        m_conv12.reset(0);
        for (int i = 0; i < m_nSource; i++)
        {
            m_conv12(m_Source[i], pEncoded[i]);
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_qpsk<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_qpsk<complex16>::output_type&>(m_Mapped_i[i * 4]);
            Mapped = m_mapqpsk[m_EncodedSource[i]];
        }

        float CodingRate = 1.0f / 2.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate) + 10.0f * log10(2.0f);
        m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
        //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

        //for (int i = 0; i < m_nMapped; i++)
        //{
        //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127;
        //    m_Mapped_i[i].im = m_Mapped_f[i].im * 127;
        //}
        //demap
        DemapQPSK(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // 16QAM 1/2
    void Run_Test5(float EbN0)
    {
        RandomSource();

        unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
        // conv encode
        m_conv12.reset(0);
        for (int i = 0; i < m_nSource; i++)
        {
            m_conv12(m_Source[i], pEncoded[i]);
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_16qam<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_Mapped_i[i * 2]);
            Mapped = m_map16qam[m_EncodedSource[i]];
        }

        float CodingRate = 1.0f / 2.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(4.0f);
        m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

        //demap
        Demap16QAM(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // 16QAM 3/4
    void Run_Test6(float EbN0)
    {
        RandomSource();

        unsigned __int8* pEncoded = m_EncodedSource;
        // conv encode
        m_conv34.reset(0);        
        for (int i = 0; i < m_nSource; i += 3)
        {
            m_conv34(&m_Source[i], pEncoded);
            pEncoded += 4;
        }

        // map
        for (int i = 0; i < m_nEncoded; i++)
        {
            mapper::dsp_mapper_16qam<complex16>::output_type &Mapped = 
                reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_Mapped_i[i * 2]);
            Mapped = m_map16qam[m_EncodedSource[i]];
        }

        float CodingRate = 3.0f / 4.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(4.0f);
        //m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

        //demap
        Demap16QAM(m_Mapped_i, m_Demapped, m_nMapped);
    }

    // 64QAM 2/3
    void Run_Test7(float EbN0)
    {
        RandomSource();

        m_Source[0] = m_Source[1] = 0;
        m_scrambler.reset(0xAB);
        for (int i = 0; i < m_nSource; i++)
        {
            m_scrambler(m_Source[i], m_ScrambledSource[i]);
        }

        unsigned __int8* pEncoded = m_EncodedSource;
        // conv encode
        m_conv23.reset(0);        
        for (int i = 0; i < m_nSource; i += 2)
        {
            m_conv23(&m_ScrambledSource[i], pEncoded);
            pEncoded += 3;
        }

        // map
        int j = 0;
        for (int i = 0; i < m_nEncoded; i += 3)
        {
            mapper::dsp_mapper_64qam<complex16>::output_type *Mapped = 
                reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

            unsigned __int32 * p = reinterpret_cast<unsigned __int32*>(&m_EncodedSource[i]);

            *Mapped = m_map64qam[*p & 0x0FFF];

            j += 2;

            Mapped = reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

            *Mapped = m_map64qam[(*p & 0x00FFF000) >> 12];

            j += 2;
        }

        float CodingRate = 2.0f / 3.0f;
        float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(6.0f);
        //m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

        //demap
        Demap64QAM(m_Mapped_i, m_Demapped, m_nMapped);
    }
};

// Ref from www.dsplog.com
//    |BER for BPSK in AWGN with hard/soft Viterbi decoder
//    |----------------------------------------------
//    |Eb/N0 | BER (sim) | BER (sim) | BER (theory) |
//    | (dB) | (hard)    | (soft)    | (uncoded)    |
//    |----------------------------------------------
//    |  0   | 0.198044  | 0.096864  | 0.078650     |
//    |  1   | 0.128787  | 0.045707  | 0.056282     |
//    |  2   | 0.072306  | 0.017853  | 0.037506     |
//    |  3   | 0.032038  | 0.005107  | 0.022878     |
//    |  4   | 0.011104  | 0.001269  | 0.012501     |
//    |  5   | 0.002886  | 0.000277  | 0.005954     |
//    |  6   | 0.000696  | 0.000054  | 0.002388     |
//    |  7   | 0.000100  | 0.000005  | 0.000773     |
//    |  8   | 0.000014  | 0.000000  | 0.000191     |
//    |  9   | 0.000000  | 0.000000  | 0.000034     |


/*
Data rate(Mbits/s) Modulation Coding rate(R) Coded bits per subcarrier(NBPSC) Coded bits per OFDM symbol(NCBPS) Data bits per OFDM symbol(NDBPS)
6 BPSK 1/2 1 48 24
9 BPSK 3/4 1 48 36
12 QPSK 1/2 2 96 48
18 QPSK 3/4 2 96 72
24 16-QAM 1/2 4 192 96
36 16-QAM 3/4 4 192 144
48 64-QAM 2/3 6 288 192
54 64-QAM 3/4 6 288 216
*/

ViterbiTest vt;

dsp_viterbi_64<36, 48> dv;

int viterbi_check()
{
    dv.m_pSoftBits     = vt.m_Demapped;
    dv.m_pDecodedBytes = vt.m_Decoded;
    dv.m_nDecodedBytes = vt.m_nSource;

    vt.Run_Test0();
    dv.Run1();

    return 1;
}


int viterbi_test(float EbN0)
{

    dv.m_pSoftBits     = vt.m_Demapped;
    dv.m_pDecodedBytes = vt.m_Decoded;
    dv.m_nDecodedBytes = vt.m_nSource;

    double SumBer = 0.0;
    double MinBer = 100.0f;

    //dv.UnitTest2();
    //return 1;

#if 1
    //vt.Run(EbN0);
    vt.Run_Test7(EbN0);
    dv.Run1();

    vt.m_scrambler.reset(vt.m_Decoded[1]);
    for (int i = 2; i < vt.m_nSource; i++)
    {
        vt.m_Descrambled[i] = vt.m_scrambler(vt.m_Decoded[i]);
    }
    

    double fber = dsp_ber::ber(vt.m_Source, vt.m_Descrambled, vt.m_nSource - 16);

    SumBer += fber;
    printf("BER: %f : %f\n", EbN0, SumBer);
#else
    printf("EbN0(dB)  :  AvgBER  :  MinBER\n");
    for (int i = 0; i < 40; i++)
    {
        SumBer = 0.0;
        MinBer = 100.0f;
        for (int i = 0; i < 10; i++)
        {
            vt.Run_Test1(EbN0);
            //vt.Run_Test2(EbN0);
            //vt.Run(EbN0);
            dv.Run1();
            //dv.UnitTest();

            double fber = dsp_ber::ber(vt.m_Source, vt.m_Decoded, vt.m_nSource - 16);

            if (fber < MinBer)
            {
                MinBer = fber;
            }

            SumBer += fber;
        }
        SumBer /= 10.0;
        printf("%.2f : %.10f : %.10f\n", EbN0, SumBer, MinBer);
        EbN0 += .5f;
    }
#endif


#if 0
    for (unsigned int i = 0; i < vt.m_nSource - 1; i++)
    {
        if (i % 16 == 0)
        {
            printf("%8d: ", i);
        }
        printf("%02X ", vt.m_Decoded[i] ^ vt.m_Source[i]);

        if (((i + 1) % 16) == 0)
        {
            printf("\n");
        }
    }
    printf("\n");
#endif

    return 1;
}
