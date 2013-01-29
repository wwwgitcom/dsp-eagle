#include "dot11n_def.h"
#include "dot11n_plcp.h"

const char dot11n_phy::sisoLongPrePositive[64] =
{
    0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1
};

const v_align(16) v_cs::type dot11n_phy::m_vLLTFMask[16] =
{
    //0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1,
    { 0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0x0000FFFF },
    { 0xFFFF0000, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000 },
    { 0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0x0000FFFF },
    { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0xFFFF0000 },

    //1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    { 0xFFFF0000, 0x0000FFFF, 0x0000FFFF, 0xFFFF0000 },
    { 0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000 },
    { 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0x0000FFFF },
    { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF },

    //0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
    { 0x0000FFFF, 0x0000FFFF, 0x0000FFFF, 0x0000FFFF },
    { 0x0000FFFF, 0x0000FFFF, 0xFFFF0000, 0xFFFF0000 },
    { 0x0000FFFF, 0x0000FFFF, 0xFFFF0000, 0xFFFF0000 },
    { 0x0000FFFF, 0xFFFF0000, 0x0000FFFF, 0xFFFF0000 },

    //1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1
    { 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000 },
    { 0xFFFF0000, 0x0000FFFF, 0x0000FFFF, 0xFFFF0000 },
    { 0xFFFF0000, 0x0000FFFF, 0xFFFF0000, 0x0000FFFF },
    { 0xFFFF0000, 0xFFFF0000, 0xFFFF0000, 0xFFFF0000 }
};



const char dot11n_phy::mimoLongPrePositive[64] =
{
    0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
    1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1
};

const v_align(16) v_cs::type dot11n_phy::m_vHTLTFMask[16] =
{
    //0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1,
    { 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
    { 0x00000000, 0x00000000, 0xFFFFFFFF, 0x00000000 },
    { 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF },
    { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },

    //1, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0,
    { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    { 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000 },
    { 0x00000000, 0x00000000, 0x00000000, 0xFFFFFFFF },
    { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },

    //0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1,
    { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF },
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000 },
    { 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF, 0x00000000 },

    //1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
    { 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF, 0x00000000 },
    { 0x00000000, 0xFFFFFFFF, 0x00000000, 0xFFFFFFFF },
    { 0x00000000, 0x00000000, 0x00000000, 0x00000000 }
};

const v_align(16) v_ci::type dot11n_phy::m_vHTLTFMask_f[32] =
{
    //0, 1, 0, 0 : 1, 1, 0, 1 : 0, 1, 0, 0 : 0, 0, 0, 1,
    { 0x8000000080000000, 0x0000000000000000}, { 0x8000000080000000, 0x8000000080000000},
    { 0x0000000000000000, 0x0000000000000000}, { 0x8000000080000000, 0x0000000000000000},
    { 0x8000000080000000, 0x0000000000000000}, { 0x8000000080000000, 0x8000000080000000},
    { 0x8000000080000000, 0x8000000080000000}, { 0x8000000080000000, 0x0000000000000000},

    //1, 0, 0, 1 : 0, 1, 0, 1 : 1, 1, 1, 0 : 0, 0, 0, 0,
    { 0x0000000000000000, 0x8000000080000000}, { 0x8000000080000000, 0x0000000000000000},
    { 0x8000000080000000, 0x0000000000000000}, { 0x8000000080000000, 0x0000000000000000},
    { 0x0000000000000000, 0x0000000000000000}, { 0x0000000000000000, 0x8000000080000000},
    { 0x8000000080000000, 0x8000000080000000}, { 0x8000000080000000, 0x8000000080000000},

    //0, 0, 0, 0 : 1, 1, 1, 1 : 0, 0, 1, 1 : 0, 1, 0, 1,
    { 0x8000000080000000, 0x8000000080000000}, { 0x8000000080000000, 0x8000000080000000},
    { 0x0000000000000000, 0x0000000000000000}, { 0x0000000000000000, 0x0000000000000000},
    { 0x8000000080000000, 0x8000000080000000}, { 0x0000000000000000, 0x0000000000000000},
    { 0x8000000080000000, 0x0000000000000000}, { 0x8000000080000000, 0x0000000000000000},
    
    //1, 1, 1, 1 : 1, 0, 0, 1 : 1, 0, 1, 0 : 1, 1, 1, 1
    { 0x0000000000000000, 0x0000000000000000}, { 0x0000000000000000, 0x0000000000000000},
    { 0x0000000000000000, 0x8000000080000000}, { 0x8000000080000000, 0x0000000000000000},
    { 0x0000000000000000, 0x8000000080000000}, { 0x0000000000000000, 0x8000000080000000},
    { 0x0000000000000000, 0x0000000000000000}, { 0x0000000000000000, 0x0000000000000000}
};

const v_align(16) complexf::type dot11n_phy::stdLSTF[16] =
{
    {   753,    -753     },
    {   -2162,  -32      },
    {   -212,   1294   },
    {   2342,   212    },
    {   1507,   0       },
    {   2342,   212    },
    {   -212,   1294   },
    {   -2162,  -32      },
    {   753,    -753     },
    {   32,     2162   },
    {   -1294,  212    },
    {   -212,   -2342    },
    {   0,      -1507    },
    {   -212,   -2342    },
    {   -1294,  212    },
    {   32,     2162   },
};

const dot11n_phy::dot11n_rate_param dot11n_phy::m_dot11n_rate_params[16] =
{
    /* MCS 0~7: for single spatial stream*/
    {52, 26},
    {104, 52},
    {104, 78},
    {208, 104},
    {208, 156},
    {312, 208},
    {312, 234},
    {312, 260},
    /* MCS 8~15: for two spatial streams*/
    {104, 52},
    {208, 104},
    {208, 156},
    {416, 208},
    {416, 312},
    {624, 416},
    {624, 468},
    {624, 520},
};

void dot11n_phy::make_plcp(dot11n_tx_buffer& tx_buffer)
{
    interleaver::dot11a_interleaver_1bpsc::output_type itlvbuffer1 =
        GetInterleaveBuffer<interleaver::dot11a_interleaver_1bpsc::output_type>(0);
    interleaver::dot11a_interleaver_1bpsc::output_type itlvbuffer2 =
        GetInterleaveBuffer<interleaver::dot11a_interleaver_1bpsc::output_type>(1);

    mapper::dsp_mapper_bpsk<complex16>::output_type mapbuffer =
        GetMapBuffer<mapper::dsp_mapper_bpsk<complex16>::output_type>();

    // clear everything
    clear_window(m_cwindow1, 8);
    clear_window(m_cwindow2, 8);
    tx_buffer.clear();

    m_lstf.get_stf_1( lstfsymbol[0] );
    m_lstf.get_stf_2( lstfsymbol[1] );

#if 0
    for (int i = 0; i < lstfsymbol[0].ntotal; i++)
    {
        printf("%d+1i*%d\t%d+1i*%d\n",
            lstfsymbol[0].data[i].re, lstfsymbol[0].data[i].im,
            lstfsymbol[1].data[i].re, lstfsymbol[1].data[i].im
            );
        if ( (i + 1) % 32 == 0 )
        {
            printf("\n");
        }
    }
#endif

    make_window(lstfsymbol[0], m_cwindow1);
    make_window(lstfsymbol[1], m_cwindow2);
    FlushNT(lstfsymbol[0], lstfsymbol[1], tx_buffer);

    m_lltf.get_ltf_1( lltfsymbol[0] );
    m_lltf.get_ltf_2( lltfsymbol[1] );
    make_window(lltfsymbol[0], m_cwindow1);
    make_window(lltfsymbol[1], m_cwindow2);
    FlushNT(lltfsymbol[0], lltfsymbol[1], tx_buffer);

    // l-sig
    m_lsig.clear();
    m_lsig.update(0xB, m_srcsize);

    m_conv12.reset(0);
    m_conv12(m_lsig.cdata[0], m_lsig.scodedbits[0]);
    m_conv12(m_lsig.cdata[1], m_lsig.scodedbits[1]);
    m_conv12(m_lsig.cdata[2], m_lsig.scodedbits[2]);

    m_interleaver1bpsc_a(m_lsig.codedbits, itlvbuffer1);

    unsigned int sc_idx = 128 - 26;
    for (int i = 0; i < 6; i++)
    {
        mapbuffer = m_mapbpsk[itlvbuffer1.values[0][i]];

        fill_symbol<8>(sc_idx, [&](int j)
        {
            m_lsig.localsymbol.subcarriers[sc_idx] = mapbuffer.values[j];
        });
    }

    m_lsig.localsymbol.subcarriers[7]         = bpsk_one;
    m_lsig.localsymbol.subcarriers[21]        = -bpsk_one;
    m_lsig.localsymbol.subcarriers[128 - 21]  = bpsk_one;
    m_lsig.localsymbol.subcarriers[128 - 7]   = bpsk_one;

#if 0
    for (int i = 0; i < m_lsig.localsymbol.ntotal; i++)
    {
        printf("%d+1i*%d\n",
            m_lsig.localsymbol.data[i].re,
            m_lsig.localsymbol.data[i].im
            );
    }
#endif

    m_ifft(m_lsig.localsymbol.vsubcarriers, m_lsig.tempsymbol.vsubcarriers);
    memset(m_lsig.localsymbol.subcarriers, 0, m_lsig.localsymbol.nsubcarrier * sizeof(complex16));

#if 0
    for (int i = 0; i < tx_buffer.buffer[0].lsig.ntotal; i++)
    {
        printf("%d+1i*%d\n",
            m_lsig.tempsymbol.data[i].re,
            m_lsig.tempsymbol.data[i].im
            );
    }
#endif

    m_lsig.tempsymbol.csd(m_lsig.localsymbol, 2);
    m_lsig.tempsymbol.copycp();
    make_window(m_lsig.tempsymbol, m_cwindow1);
    m_lsig.localsymbol.copycp();
    make_window(m_lsig.localsymbol, m_cwindow2);
    FlushNT(m_lsig.tempsymbol, m_lsig.localsymbol, tx_buffer);

    // ht-sig
    m_conv12.reset(0);
    m_htsig.update(m_txmcs, m_srcsize);
    m_conv12(m_htsig.cdata[0], m_htsig.scodedbits[0]);
    m_conv12(m_htsig.cdata[1], m_htsig.scodedbits[1]);
    m_conv12(m_htsig.cdata[2], m_htsig.scodedbits[2]);
    m_conv12(m_htsig.cdata[3], m_htsig.scodedbits[3]);
    m_conv12(m_htsig.cdata[4], m_htsig.scodedbits[4]);
    m_conv12(m_htsig.cdata[5], m_htsig.scodedbits[5]);

    m_interleaver1bpsc_a(&m_htsig.codedbits[0], itlvbuffer1);
    m_interleaver1bpsc_a(&m_htsig.codedbits[6], itlvbuffer2);

    sc_idx = 128 - 26;
    for (int i = 0; i < 6; i++)
    {
        mapbuffer = m_mapbpsk2[itlvbuffer1.values[0][i]];
        fill_symbol<8>(sc_idx, [&](int j)
        {
            m_htsig.localsymbol1.subcarriers[sc_idx] = mapbuffer.values[j];
        });
    }

    sc_idx = 128 - 26;
    for (int i = 0; i < 6; i++)
    {
        mapbuffer = m_mapbpsk2[itlvbuffer2.values[0][i]];
        fill_symbol<8>(sc_idx, [&](int j)
        {
            m_htsig.localsymbol2.subcarriers[sc_idx] = mapbuffer.values[j];
        });
    }

    m_htsig.localsymbol1.subcarriers[7]         = bpsk_one;
    m_htsig.localsymbol1.subcarriers[21]        = -bpsk_one;
    m_htsig.localsymbol1.subcarriers[128 - 21]  = bpsk_one;
    m_htsig.localsymbol1.subcarriers[128 - 7]   = bpsk_one;

    m_htsig.localsymbol2.subcarriers[7]         = bpsk_one;
    m_htsig.localsymbol2.subcarriers[21]        = -bpsk_one;
    m_htsig.localsymbol2.subcarriers[128 - 21]  = bpsk_one;
    m_htsig.localsymbol2.subcarriers[128 - 7]   = bpsk_one;

    m_ifft(m_htsig.localsymbol1.vsubcarriers, m_htsig.tempsymbol1.vsubcarriers);
    m_ifft(m_htsig.localsymbol2.vsubcarriers, m_htsig.tempsymbol2.vsubcarriers);

    memset(m_htsig.localsymbol1.subcarriers, 0, m_htsig.localsymbol1.nsubcarrier * sizeof(complex16));
    memset(m_htsig.localsymbol2.subcarriers, 0, m_htsig.localsymbol2.nsubcarrier * sizeof(complex16));

    m_htsig.tempsymbol1.csd(m_htsig.tempsymbol3, 2);
    m_htsig.tempsymbol1.copycp();
    make_window(m_htsig.tempsymbol1, m_cwindow1);
    m_htsig.tempsymbol3.copycp();
    make_window(m_htsig.tempsymbol3, m_cwindow2);
    FlushNT(m_htsig.tempsymbol1, m_htsig.tempsymbol3, tx_buffer);

    m_htsig.tempsymbol2.csd(m_htsig.tempsymbol3, 2);
    m_htsig.tempsymbol2.copycp();
    make_window(m_htsig.tempsymbol2, m_cwindow1);
    m_htsig.tempsymbol3.copycp();
    make_window(m_htsig.tempsymbol3, m_cwindow2);
    FlushNT(m_htsig.tempsymbol2, m_htsig.tempsymbol3, tx_buffer);

    // ht-stf
    m_htstf.get_stf_1( htstfsymbol[0] );
    m_htstf.get_stf_2( htstfsymbol[1] );
    make_window(htstfsymbol[0], m_cwindow1);
    make_window(htstfsymbol[1], m_cwindow2);
    FlushNT(htstfsymbol[0], htstfsymbol[1], tx_buffer);

    // ht-ltf
    m_htltf.get_ltf_1( htltfsymbol[0] );
    m_htltf.get_ltf_2( htltfsymbol[1] );
    make_window(htltfsymbol[0], m_cwindow1);
    make_window(htltfsymbol[1], m_cwindow2);
    FlushNT(htltfsymbol[0], htltfsymbol[1], tx_buffer);

#if 0
    printf("HT_LTF11\n");
    for (int i = 0; i < htltfsymbol[0].vnsubcarrier; i++)
    {
        printf("%d. [%d,%d] [%d,%d] [%d,%d] [%d,%d] \n",
            i,
            htltfsymbol[0].vsubcarriers[i][0].re, htltfsymbol[0].vsubcarriers[i][0].im,
            htltfsymbol[0].vsubcarriers[i][1].re, htltfsymbol[0].vsubcarriers[i][1].im,
            htltfsymbol[0].vsubcarriers[i][2].re, htltfsymbol[0].vsubcarriers[i][2].im,
            htltfsymbol[0].vsubcarriers[i][3].re, htltfsymbol[0].vsubcarriers[i][3].im
            );
    }
#endif
    m_htltf.get_ltf_11( htltfsymbol[0] );
    m_htltf.get_ltf_2( htltfsymbol[1] );
    make_window(htltfsymbol[0], m_cwindow1);
    make_window(htltfsymbol[1], m_cwindow2);
    FlushNT(htltfsymbol[0], htltfsymbol[1], tx_buffer);
#if 0
    printf("\n\nHT_LTF22\n");
    for (int i = 0; i < htltfsymbol[0].vnsubcarrier; i++)
    {
        printf("%d. [%d,%d] [%d,%d] [%d,%d] [%d,%d] \n",
            i,
            htltfsymbol[0].vsubcarriers[i][0].re, htltfsymbol[0].vsubcarriers[i][0].im,
            htltfsymbol[0].vsubcarriers[i][1].re, htltfsymbol[0].vsubcarriers[i][1].im,
            htltfsymbol[0].vsubcarriers[i][2].re, htltfsymbol[0].vsubcarriers[i][2].im,
            htltfsymbol[0].vsubcarriers[i][3].re, htltfsymbol[0].vsubcarriers[i][3].im
            );
    }
#endif
}

void dot11n_phy::make_service(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;

    m_service.clear();
    m_scrambler.reset(0xAB);// TODO: should be a random number here
    m_scrambler(m_service.bservice[0], m_scramble_outputbuffer[0]);
    m_scrambler(m_service.bservice[1], m_scramble_outputbuffer[1]);

    //printf("scrambled service: %02X %02X\n", m_scramble_outputbuffer[0], m_scramble_outputbuffer[1]);
}

void dot11n_phy::add_pilot(int iss, HT_DATA::symbol &htsymbol)
{
    m_pilot(iss, m_symbolcount, m_pilotbuffer);

    htsymbol.subcarriers[128 - 21]  = bpsk_one * m_pilotbuffer[0];
    htsymbol.subcarriers[128 - 7]   = bpsk_one * m_pilotbuffer[1];
    htsymbol.subcarriers[7]         = bpsk_one * m_pilotbuffer[2];
    htsymbol.subcarriers[21]        = bpsk_one * m_pilotbuffer[3];
}

// src_size must be a factor of 13
void dot11n_phy::mcs8_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;

    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 52;

    if (total_bits_count % 52 > 0)
    {
        total_symbol_count++;
    }

    make_plcp(tx_buffer);
    make_service(tx_buffer);

    m_symbolcount        = 0;
    m_conv_outputcnt     = 4;
    m_srcidx             = 0;

    m_conv12.reset(0);
    m_conv12(m_scramble_outputbuffer[0], (unsigned __int16&)stream_splitter_buffer[0]);
    m_conv12(m_scramble_outputbuffer[1], (unsigned __int16&)stream_splitter_buffer[2]);

    //printf("conv service: %02X %02X %02X %02X\n", stream_splitter_buffer[0], stream_splitter_buffer[1], stream_splitter_buffer[2], stream_splitter_buffer[3]);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 13)
        {
            m_scrambler(m_psrc[m_srcidx], m_scramble_outputbuffer[0]);
            if (m_srcidx == m_srcsize)
            {
                m_scramble_outputbuffer[0] &= 0XC0;
            }
            m_srcidx++;
            m_conv12(m_scramble_outputbuffer[0], (unsigned __int16&)stream_splitter_buffer[m_conv_outputcnt]);
#if 0
            printf("%d\t%02x==>%02x==>%02x, %02x\n", m_srcidx-1,
                m_psrc[m_srcidx - 1],
                m_scramble_outputbuffer[0],
                stream_splitter_buffer[m_conv_outputcnt],
                stream_splitter_buffer[m_conv_outputcnt + 1]);
#endif
            m_conv_outputcnt += 2;
        }

        // stream parser
        stream_parser::stream_paser_bpsk_2ss::output_type& spbuffer
            = (stream_parser::stream_paser_bpsk_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_spbpsk(stream_splitter_buffer, spbuffer);

#if 0
        printf("conv output\n");
        for (int i = 0; i < 13; i++)
        {
            printf("%02X ", stream_splitter_buffer[i]);
        }
        printf("\n");

        printf("sp1 output\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%02X ", spbuffer.cvalues[0][i]);
        }
        printf("\n");

        printf("sp1 output\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%02X ", spbuffer.cvalues[1][i]);
        }
        printf("\n");
#endif
        // interleave
        interleaver::dot11n_interleaver_1bpsc::output_type& itlvbuffer1
            = reinterpret_cast<interleaver::dot11n_interleaver_1bpsc::output_type&>(m_interleave_outputbuffers[0]);

        interleaver::dot11n_interleaver_1bpsc::output_type& itlvbuffer2
            = reinterpret_cast<interleaver::dot11n_interleaver_1bpsc::output_type&>(m_interleave_outputbuffers[1]);

        m_interleaver1bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );

        m_interleaver1bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );
#if 0
        printf("\n");

        printf("interleaved1 output\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%02X ", itlvbuffer1.values[0][i]);
        }
        printf("\n");

        printf("interleaved2 output\n");
        for (int i = 0; i < 7; i++)
        {
            printf("%02X ", itlvbuffer2.values[0][i]);
        }
        printf("\n");
#endif
        // map
        mapper::dsp_mapper_bpsk<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_map_outputmuffer);
        {
            // first antenna
            unsigned int sc_idx = 100;
            for (int i = 0; i < 6; i++)
            {
                map_outputbuffer = m_mapbpsk[itlvbuffer1.values[0][i]];
                fill_symbol<8>(sc_idx, [&](int j)
                {
                    localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }

            map_outputbuffer = m_mapbpsk[itlvbuffer1.values[0][6]];
            fill_symbol<4>(sc_idx, [&](int j)
            {
                localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });

            // second antenna
            sc_idx = 100;
            for (int i = 0; i < 6; i++)
            {
                map_outputbuffer = m_mapbpsk[itlvbuffer2.values[0][i]];
                fill_symbol<8>(sc_idx, [&](int j)
                {
                    localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }

            map_outputbuffer = m_mapbpsk[itlvbuffer2.values[0][6]];
            fill_symbol<4>(sc_idx, [&](int j)
            {
                localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

#if 0
        printf("mapped\n");
        for (int isc = 128 - 28; isc < 128; isc++)
        {
            if (isc == (128 - 7) || isc == (128 - 21)) continue;
            printf("%d\t%d\n",
                localsymbol[0].subcarriers[isc].re > 0 ? 1 : -1,
                localsymbol[1].subcarriers[isc].re > 0 ? 1 : -1);
        }
        for (int isc = 1; isc <= 28; isc++)
        {
            if (isc == 7 || isc == 21) continue;
            printf("%d\t%d\n",
                localsymbol[0].subcarriers[isc].re > 0 ? 1 : -1,
                localsymbol[1].subcarriers[isc].re > 0 ? 1 : -1);
        }
#endif
        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

#if 0
        for (int i = 0, j = 0; i < 160; i += 2, j++)
        {
            m_rx_cfo_compensated_f[0][j].re = tempsymbol[0].data[i].re;
            m_rx_cfo_compensated_f[0][j].im = tempsymbol[0].data[i].im;
            m_rx_cfo_compensated_f[1][j].re = tempsymbol[1].data[i].re;
            m_rx_cfo_compensated_f[1][j].im = tempsymbol[1].data[i].im;
        }

        // fft
        m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
        m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

        m_plot.command("set yrange [-128:128]");
        m_plot.begin_plot("w p 7");
        int k = 0;
        for (int i = 0; i < 64; i++)
        {
            int v = (32 + i) % 64;
            if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
            {
                m_plot.data("%f\t%f", m_rx_fsamples_f[k][v].re, m_rx_fsamples_f[k][v].im);
                float fe = m_rx_fsamples_f[k][v].re * m_rx_fsamples_f[k][v].re
                    + m_rx_fsamples_f[k][v].im * m_rx_fsamples_f[k][v].im;
                //m_plot.data("%f", fe);
            }
        }
        m_plot.end_plot();

        printf("DeMap Ntx 1\n");
        for (int isc = 64 - 28; isc < 64; isc++)
        {
            if (isc == (64 - 7) || isc == (64 - 21)) continue;
            printf("%d ", m_rx_fsamples_f[0][isc].re > 0 ? 1 : -1);
        }
        printf("\n");
        for (int isc = 1; isc <= 28; isc++)
        {
            if (isc == 7 || isc == 21) continue;
            printf("%d ", m_rx_fsamples_f[0][isc].re > 0 ? 1 : -1);
        }
        printf("\n");

        printf("DeMap Ntx 2\n");
        for (int isc = 64 - 28; isc < 64; isc++)
        {
            if (isc == (64 - 7) || isc == (64 - 21)) continue;
            printf("%d ", m_rx_fsamples_f[1][isc].re > 0 ? 1 : -1);
        }
        printf("\n");
        for (int isc = 1; isc <= 28; isc++)
        {
            if (isc == 7 || isc == 21) continue;
            printf("%d ", m_rx_fsamples_f[1][isc].re > 0 ? 1 : -1);
        }
        printf("\n");
#endif
        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;

        if (m_conv_outputcnt == 14)
        {
            stream_splitter_buffer[0]      = stream_splitter_buffer[13];
            m_conv_outputcnt = 1;
        }
        else
        {
            m_conv_outputcnt = 0;
        }
    }
}

void dot11n_phy::mcs9_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;
    unsigned __int8  scramble_output;

    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 104;

    if (total_bits_count % 104 > 0)
    {
        total_symbol_count++;
    }

    make_plcp(tx_buffer);
    make_service(tx_buffer);

    m_symbolcount    = 0;
    m_conv_outputcnt = 4;
    m_srcidx         = 0;

    m_conv12.reset(0);
    m_conv12(m_scramble_outputbuffer[0], (unsigned __int16&)stream_splitter_buffer[0]);
    m_conv12(m_scramble_outputbuffer[1], (unsigned __int16&)stream_splitter_buffer[2]);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 26)
        {
            m_scrambler(m_psrc[m_srcidx], scramble_output);
            if (m_srcidx == m_srcsize)
            {
                scramble_output &= 0XC0;
            }
            m_srcidx++;

            m_conv12(scramble_output, (unsigned __int16&)stream_splitter_buffer[m_conv_outputcnt]);

            m_conv_outputcnt += 2;
        }
        stream_parser::stream_paser_qpsk_2ss::output_type& spbuffer = (stream_parser::stream_paser_qpsk_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_spqpsk(stream_splitter_buffer, spbuffer);

#if 0
        printf("conv output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", stream_splitter_buffer[i]);
        }
        printf("\n");

        printf("sp1 output\n");
        for (int i = 0; i < 13; i++)
        {
            printf("%02X ", spbuffer.cvalues[0][i]);
        }
        printf("\n");

        printf("sp2 output\n");
        for (int i = 0; i < 13; i++)
        {
            printf("%02X ", spbuffer.cvalues[1][i]);
        }
        printf("\n");
#endif

        interleaver::dot11n_interleaver_2bpsc::output_type& itlvbuffer1 = reinterpret_cast<interleaver::dot11n_interleaver_2bpsc::output_type&>(m_interleave_outputbuffers[0]);
        m_interleaver2bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );
        interleaver::dot11n_interleaver_2bpsc::output_type& itlvbuffer2 = reinterpret_cast<interleaver::dot11n_interleaver_2bpsc::output_type&>(m_interleave_outputbuffers[1]);
        m_interleaver2bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );

#if 0
        printf("\n");

        printf("interleaved1 output\n");
        for (int i = 0; i < 13; i++)
        {
            printf("%02X ", itlvbuffer1.values[0][i]);
        }
        printf("\n");

        printf("interleaved2 output\n");
        for (int i = 0; i < 13; i++)
        {
            printf("%02X ", itlvbuffer2.values[0][i]);
        }
        printf("\n");
#endif

        // map
        mapper::dsp_mapper_qpsk<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_qpsk<complex16>::output_type&>(m_map_outputmuffer);
        // first antenna
        unsigned int sc_idx = 100;
        for (int i = 0; i < 13; i++)
        {
            map_outputbuffer = m_mapqpsk[itlvbuffer1.values[0][i]];

            fill_symbol<4>(sc_idx, [&](int j)
            {
                localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // second antenna
        sc_idx = 100;
        for (int i = 0; i < 13; i++)
        {
            map_outputbuffer = m_mapqpsk[itlvbuffer2.values[0][i]];

            fill_symbol<4>(sc_idx, [&](int j)
            {
                localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

#if 0
        for (int isc = 128 - 28; isc < 128; isc++)
        {
            if (isc == (128 - 7) || isc == (128 - 21)) continue;
            printf("(%d,%d)\t(%d,%d)\n",
                localsymbol[0].subcarriers[isc].re > 0 ? 1 : -1,
                localsymbol[0].subcarriers[isc].im > 0 ? 1 : -1,
                localsymbol[1].subcarriers[isc].re > 0 ? 1 : -1,
                localsymbol[1].subcarriers[isc].im > 0 ? 1 : -1
                );
        }
        for (int isc = 1; isc <= 28; isc++)
        {
            if (isc == 7 || isc == 21) continue;
            printf("(%d,%d)\t(%d,%d)\n",
                localsymbol[0].subcarriers[isc].re > 0 ? 1 : -1,
                localsymbol[0].subcarriers[isc].im > 0 ? 1 : -1,
                localsymbol[1].subcarriers[isc].re > 0 ? 1 : -1,
                localsymbol[1].subcarriers[isc].im > 0 ? 1 : -1
                );
        }
#endif

        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;
        m_conv_outputcnt = 0;
    }
}

void dot11n_phy::mcs10_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;
    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 156;

    if (total_bits_count % 156 > 0)
    {
        total_symbol_count++;
    }

    make_plcp(tx_buffer);

    make_service(tx_buffer);

    m_symbolcount        = 0;
    m_conv_outputcnt       = 4;
    m_srcidx              = 0;

    m_conv34.reset(0);
    m_scrambler(m_psrc[m_srcidx++], m_scramble_outputbuffer[2]);
    m_conv34(m_scramble_outputbuffer, stream_splitter_buffer);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 26)
        {
            m_scrambler(m_psrc[m_srcidx], m_scramble_outputbuffer[0]);
            m_scrambler(m_psrc[m_srcidx + 1], m_scramble_outputbuffer[1]);
            m_scrambler(m_psrc[m_srcidx + 2], m_scramble_outputbuffer[2]);

            if (m_srcidx >= m_srcsize)
            {
                m_scramble_outputbuffer[m_srcidx - m_srcsize] &= 0xC0;
            }

            m_srcidx += 3;

            m_conv34(m_scramble_outputbuffer, &stream_splitter_buffer[m_conv_outputcnt]);

            m_conv_outputcnt += 4;
        }
        stream_parser::stream_paser_qpsk_2ss::output_type& spbuffer = (stream_parser::stream_paser_qpsk_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_spqpsk(stream_splitter_buffer, spbuffer);

        interleaver::dot11n_interleaver_2bpsc::output_type& itlvbuffer1 = reinterpret_cast<interleaver::dot11n_interleaver_2bpsc::output_type&>(m_interleave_outputbuffers[0]);
        m_interleaver2bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );
        interleaver::dot11n_interleaver_2bpsc::output_type& itlvbuffer2 = reinterpret_cast<interleaver::dot11n_interleaver_2bpsc::output_type&>(m_interleave_outputbuffers[1]);
        m_interleaver2bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );

        // map
        mapper::dsp_mapper_qpsk<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_qpsk<complex16>::output_type&>(m_map_outputmuffer);
        // first antenna
        unsigned int sc_idx = 100;
        for (int i = 0; i < 13; i++)
        {
            map_outputbuffer = m_mapqpsk[itlvbuffer1.values[0][i]];

            fill_symbol<4>(sc_idx, [&](int j)
            {
                localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // second antenna
        sc_idx = 100;
        for (int i = 0; i < 13; i++)
        {
            map_outputbuffer = m_mapqpsk[itlvbuffer2.values[0][i]];

            fill_symbol<4>(sc_idx, [&](int j)
            {
                localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

#if 0
        for (int i = 0, j = 0; i < 160; i += 2, j++)
        {
            m_rx_cfo_compensated_f[0][j].re = tempsymbol[0].data[i].re;
            m_rx_cfo_compensated_f[0][j].im = tempsymbol[0].data[i].im;
            m_rx_cfo_compensated_f[1][j].re = tempsymbol[1].data[i].re;
            m_rx_cfo_compensated_f[1][j].im = tempsymbol[1].data[i].im;
        }

        // fft
        m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
        m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

        //m_plot.command("reset");
        //m_plot.command("set terminal jpeg");
        //m_plot.command("set output 'y%02d_%05d.jpg'", k, nimage);

        m_plot.begin_plot();
        int k = 0;
        for (int i = 0; i < 64; i++)
        {
            int v = (32 + i) % 64;
            if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
            {
                m_plot.data("%f\t%f", m_rx_fsamples_f[k][v].re, m_rx_fsamples_f[k][v].im);
                float fe = m_rx_fsamples_f[k][v].re * m_rx_fsamples_f[k][v].re
                    + m_rx_fsamples_f[k][v].im * m_rx_fsamples_f[k][v].im;
                //m_plot.data("%f", fe);
            }
        }
        m_plot.end_plot();
        getchar();
#endif

        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;
        if (m_conv_outputcnt == 28)
        {
            stream_splitter_buffer[0]      = stream_splitter_buffer[26];
            stream_splitter_buffer[1]      = stream_splitter_buffer[27];
            m_conv_outputcnt = 2;
        }
        else
        {
            m_conv_outputcnt = 0;
        }
    }
    printf("tx symbol count = %d\n", m_symbolcount);
}

void dot11n_phy::mcs11_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;
    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 208;

    if (total_bits_count % 208 > 0)
    {
        total_symbol_count++;
    }

    make_plcp(tx_buffer);
    make_service(tx_buffer);

    m_symbolcount        = 0;
    m_conv_outputcnt     = 4;
    m_srcidx             = 0;

    m_conv12.reset(0);
    m_conv12(m_scramble_outputbuffer[0], (unsigned __int16&)stream_splitter_buffer[0]);
    m_conv12(m_scramble_outputbuffer[1], (unsigned __int16&)stream_splitter_buffer[2]);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 52)
        {
            m_scrambler(m_psrc[m_srcidx], m_scramble_outputbuffer[0]);

            if (m_srcidx == m_srcsize)
            {
                m_scramble_outputbuffer[0] &= 0xC0;
            }
            m_srcidx++;

            m_conv12(m_scramble_outputbuffer[0], (unsigned __int16&)stream_splitter_buffer[m_conv_outputcnt]);

            m_conv_outputcnt += 2;
        }

        stream_parser::stream_paser_16qam_2ss::output_type& spbuffer
            = (stream_parser::stream_paser_16qam_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_sp16qam(stream_splitter_buffer, spbuffer);

#if 0
        printf("conv output\n");
        for (int i = 0; i < 52; i++)
        {
            printf("%02X ", stream_splitter_buffer[i]);
        }
        printf("\n");

        printf("sp1 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", spbuffer.cvalues[0][i]);
        }
        printf("\n");

        printf("sp2 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", spbuffer.cvalues[1][i]);
        }
        printf("\n");
#endif

        interleaver::dot11n_interleaver_4bpsc::output_type& itlvbuffer1
            = reinterpret_cast<interleaver::dot11n_interleaver_4bpsc::output_type&>(m_interleave_outputbuffers[0]);

        interleaver::dot11n_interleaver_4bpsc::output_type& itlvbuffer2
            = reinterpret_cast<interleaver::dot11n_interleaver_4bpsc::output_type&>(m_interleave_outputbuffers[1]);

        m_interleaver4bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );

        m_interleaver4bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );

#if 0
        printf("\n");

        printf("interleaved1 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", itlvbuffer1.values[0][i]);
        }
        printf("\n");

        printf("interleaved2 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", itlvbuffer2.values[0][i]);
        }
        printf("\n");
#endif

        // map
        mapper::dsp_mapper_16qam<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_map_outputmuffer);
        {
            // first antenna
            unsigned int sc_idx = 100;
            for (int i = 0; i < 26; i++)
            {
                map_outputbuffer = m_map16qam[itlvbuffer1.values[0][i]];

                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }

            // second antenna
            sc_idx = 100;
            for (int i = 0; i < 26; i++)
            {
                map_outputbuffer = m_map16qam[itlvbuffer2.values[0][i]];
                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;
        m_conv_outputcnt = 0;
    }
}

void dot11n_phy::mcs12_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;
    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 312;

    if (total_bits_count % 312 > 0)
    {
        total_symbol_count++;
    }
    make_plcp(tx_buffer);
    make_service(tx_buffer);

    m_symbolcount        = 0;
    m_conv_outputcnt     = 4;
    m_srcidx             = 0;

    m_conv34.reset(0);
    m_scrambler(m_psrc[m_srcidx++], m_scramble_outputbuffer[2]);
    m_conv34(m_scramble_outputbuffer, stream_splitter_buffer);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 52)
        {
            m_scrambler(m_psrc[m_srcidx], m_scramble_outputbuffer[0]);
            m_scrambler(m_psrc[m_srcidx + 1], m_scramble_outputbuffer[1]);
            m_scrambler(m_psrc[m_srcidx + 2], m_scramble_outputbuffer[2]);

            if (m_srcidx >= m_srcsize)
            {
                m_scramble_outputbuffer[m_srcidx - m_srcsize] &= 0xC0;
            }

            m_srcidx += 3;

            m_conv34(m_scramble_outputbuffer, &stream_splitter_buffer[m_conv_outputcnt]);

            m_conv_outputcnt += 4;
        }
        stream_parser::stream_paser_16qam_2ss::output_type& spbuffer = (stream_parser::stream_paser_16qam_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_sp16qam(stream_splitter_buffer, spbuffer);

#if 1
        printf("conv output\n");
        for (int i = 0; i < 52; i++)
        {
            printf("%02X ", stream_splitter_buffer[i]);
        }
        printf("\n");

        printf("sp1 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", spbuffer.cvalues[0][i]);
        }
        printf("\n");

        printf("sp2 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", spbuffer.cvalues[1][i]);
        }
        printf("\n");
#endif

        interleaver::dot11n_interleaver_4bpsc::output_type& itlvbuffer1 = reinterpret_cast<interleaver::dot11n_interleaver_4bpsc::output_type&>(m_interleave_outputbuffers[0]);
        m_interleaver4bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );
        interleaver::dot11n_interleaver_4bpsc::output_type& itlvbuffer2 = reinterpret_cast<interleaver::dot11n_interleaver_4bpsc::output_type&>(m_interleave_outputbuffers[1]);
        m_interleaver4bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );

#if 1
        printf("\n");

        printf("interleaved1 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", itlvbuffer1.values[0][i]);
        }
        printf("\n");

        printf("interleaved2 output\n");
        for (int i = 0; i < 26; i++)
        {
            printf("%02X ", itlvbuffer2.values[0][i]);
        }
        printf("\n");
#endif

        // map
        mapper::dsp_mapper_16qam<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_map_outputmuffer);
        {
            // first antenna
            unsigned int sc_idx = 100;
            for (int i = 0; i < 26; i++)
            {
                map_outputbuffer = m_map16qam[itlvbuffer1.values[0][i]];

                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }

            // second antenna
            sc_idx = 100;
            for (int i = 0; i < 26; i++)
            {
                map_outputbuffer = m_map16qam[itlvbuffer2.values[0][i]];

                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;
        m_conv_outputcnt = 0;
    }
}

void dot11n_phy::mcs13_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;
    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 416;

    if (total_bits_count % 416 > 0)
    {
        total_symbol_count++;
    }
    make_plcp(tx_buffer);
    make_service(tx_buffer);

    m_symbolcount        = 0;
    m_conv_outputcnt       = 3;
    m_srcidx              = 0;
    m_conv23.reset(0);
    m_conv23(m_scramble_outputbuffer, stream_splitter_buffer);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 78)
        {
            m_scrambler(m_psrc[m_srcidx], m_scramble_outputbuffer[0]);
            m_scrambler(m_psrc[m_srcidx + 1], m_scramble_outputbuffer[1]);

            if (m_srcidx >= m_srcsize)
            {
                m_scramble_outputbuffer[m_srcidx - m_srcsize] &= 0xC0;
            }

            m_srcidx += 2;

            m_conv23(m_scramble_outputbuffer, &stream_splitter_buffer[m_conv_outputcnt]);

            m_conv_outputcnt += 3;
        }
        stream_parser::stream_paser_64qam_2ss::output_type& spbuffer
            = (stream_parser::stream_paser_64qam_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_sp64qam(stream_splitter_buffer, spbuffer);

#if 1
        printf("conv output\n");
        for (int i = 0; i < 78; i++)
        {
            printf("%02X ", stream_splitter_buffer[i]);
        }
        printf("\n");

        printf("sp1 output\n");
        for (int i = 0; i < 39; i++)
        {
            printf("%02X ", spbuffer.cvalues[0][i]);
        }
        printf("\n");

        printf("sp2 output\n");
        for (int i = 0; i < 39; i++)
        {
            printf("%02X ", spbuffer.cvalues[1][i]);
        }
        printf("\n");
#endif

        interleaver::dot11n_interleaver_6bpsc::output_type& itlvbuffer1
            = reinterpret_cast<interleaver::dot11n_interleaver_6bpsc::output_type&>(m_interleave_outputbuffers[0]);
        m_interleaver6bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );

        interleaver::dot11n_interleaver_6bpsc::output_type& itlvbuffer2
            = reinterpret_cast<interleaver::dot11n_interleaver_6bpsc::output_type&>(m_interleave_outputbuffers[1]);
        m_interleaver6bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );

#if 0
        printf("\n");

        printf("interleaved1 output\n");
        for (int i = 0; i < 39; i++)
        {
            printf("%02X ", itlvbuffer1.values[0][i]);
        }
        printf("\n");

        printf("interleaved2 output\n");
        for (int i = 0; i < 39; i++)
        {
            printf("%02X ", itlvbuffer2.values[0][i]);
        }
        printf("\n");
#endif

        // map
        mapper::dsp_mapper_64qam<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type&>(m_map_outputmuffer);
        {
            // first antenna
            unsigned int sc_idx = 100;
            unsigned __int32* pidx;
            for (int i = 0; i < 39; i += 3)
            {
                pidx = reinterpret_cast<unsigned __int32*>(&itlvbuffer1.values[0][i]);

                map_outputbuffer = m_map64qam[*pidx & 0x0FFF];
                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });

                map_outputbuffer = m_map64qam[(*pidx & 0x00FFF000) >> 12];
                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }

            // second antenna
            sc_idx = 100;
            for (int i = 0; i < 39; i += 3)
            {
                pidx = reinterpret_cast<unsigned __int32*>(&itlvbuffer2.values[0][i]);

                map_outputbuffer = m_map64qam[*pidx & 0x0FFF];
                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });

                map_outputbuffer = m_map64qam[(*pidx & 0x00FFF000) >> 12];
                fill_symbol<2>(sc_idx, [&](int j)
                {
                    localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
                });
            }
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

#if 0
        for (int i = 0, j = 0; i < 128; i++, j++)
        {
            if (i == 32)
            {
                i += 64;
            }
            m_tfreqcompensated_f[0][j].re = localsymbol[0].subcarriers[i].re;
            m_tfreqcompensated_f[0][j].im = localsymbol[0].subcarriers[i].im;
            m_tfreqcompensated_f[1][j].re = localsymbol[1].subcarriers[i].re;
            m_tfreqcompensated_f[1][j].im = localsymbol[1].subcarriers[i].im;
        }

        static int nimage = 0;
        for (int k = 0; k < 2; k++)
        {
            m_plot.command("reset");
            //m_plot.command("set terminal jpeg");
            m_plot.command("set output 'y%02d_%05d.jpg'", k, nimage);
            m_plot.command("set title 'y%05d[%d]'", nimage, k);
            //m_plot.command("set xrange [-2:2]");
            //m_plot.command("set yrange [-2:2]");
            m_plot.begin_plot("w lp 7");

            for (int i = 0; i < 64; i++)
            {
                //if (v == 64 - 21 || v == 64 - 7 || v == 7 || v == 21)
                {
                    //m_plot.data("%f\t%f", m_tfreqcompensated_f[k][i].re, m_tfreqcompensated_f[k][i].im);
                    float fe = m_tfreqcompensated_f[k][i].re * m_tfreqcompensated_f[k][i].re
                        + m_tfreqcompensated_f[k][i].im * m_tfreqcompensated_f[k][i].im;
                    m_plot.data("%f", fe);
                }
                //else
                //{
                //    m_plot.data("%f\t%f", 0, 0);
                //    //m_plot.data("%f", 0.0f);
                //}
            }
            m_plot.end_plot();
            getchar();
            //Sleep(300);
        }
        nimage++;
#endif

        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;
        m_conv_outputcnt = 0;
    }
}

void dot11n_phy::mcs14_entry(dot11n_tx_buffer& tx_buffer)
{
    unsigned __int8* stream_splitter_buffer = m_streamsplitter_inputbuffer;
    int total_bits_count   = (16 + m_srcsize * 8 + 6);
    int total_symbol_count = total_bits_count / 468;

    if (total_bits_count % 468 > 0)
    {
        total_symbol_count++;
    }
    make_plcp(tx_buffer);
    make_service(tx_buffer);

    m_symbolcount        = 0;
    m_conv_outputcnt     = 4;
    m_srcidx             = 0;

    m_conv34.reset(0);
    m_scrambler(m_psrc[m_srcidx++], m_scramble_outputbuffer[2]);
    m_conv34(m_scramble_outputbuffer, stream_splitter_buffer);

    //while (m_srcidx < m_srcsize)
    while (m_symbolcount < total_symbol_count)
    {
        while (m_conv_outputcnt < 78)
        {
            m_scrambler(m_psrc[m_srcidx], m_scramble_outputbuffer[0]);
            m_scrambler(m_psrc[m_srcidx + 1], m_scramble_outputbuffer[1]);
            m_scrambler(m_psrc[m_srcidx + 2], m_scramble_outputbuffer[2]);

            if (m_srcidx >= m_srcsize)
            {
                m_scramble_outputbuffer[m_srcidx - m_srcsize] &= 0xC0;
            }

            m_srcidx += 3;

            m_conv34(m_scramble_outputbuffer, &stream_splitter_buffer[m_conv_outputcnt]);

            m_conv_outputcnt += 4;
        }
        stream_parser::stream_paser_64qam_2ss::output_type& spbuffer
            = (stream_parser::stream_paser_64qam_2ss::output_type&)m_streamsplitter_outputbuffer;
        m_sp64qam(stream_splitter_buffer, spbuffer);

#if 0
        //printf("conv output\n");
        //for (int i = 0; i < 78; i++)
        //{
        //    printf("%02X ", stream_splitter_buffer[i]);
        //}
        //printf("\n");

        //printf("sp1 output\n");
        //for (int i = 0; i < 39; i++)
        //{
        //    printf("%02X ", spbuffer.cvalues[0][i]);
        //}
        //printf("\n");

        //printf("sp2 output\n");
        //for (int i = 0; i < 39; i++)
        //{
        //    printf("%02X ", spbuffer.cvalues[1][i]);
        //}
        //printf("\n");
#endif

        interleaver::dot11n_interleaver_6bpsc::output_type& itlvbuffer1
            = reinterpret_cast<interleaver::dot11n_interleaver_6bpsc::output_type&>(m_interleave_outputbuffers[0]);
        m_interleaver6bpsc_iss1(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[0][0]),
            itlvbuffer1
            );

        interleaver::dot11n_interleaver_6bpsc::output_type& itlvbuffer2
            = reinterpret_cast<interleaver::dot11n_interleaver_6bpsc::output_type&>(m_interleave_outputbuffers[1]);
        m_interleaver6bpsc_iss2(
            reinterpret_cast<unsigned __int8*>(&spbuffer.cvalues[1][0]),
            itlvbuffer2
            );

#if 0
        //printf("interleaved1 output\n");
        for (int i = 0; i < 39; i++)
        {
            printf("%02X ", itlvbuffer1.values[0][i]);
        }
        printf("\n");

        //printf("interleaved2 output\n");
        //for (int i = 0; i < 39; i++)
        //{
        //    printf("%02X ", itlvbuffer2.values[0][i]);
        //}
        //printf("\n");
#endif

        // map
        mapper::dsp_mapper_64qam<complex16>::output_type& map_outputbuffer=
            reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type&>(m_map_outputmuffer);
        // first antenna
        unsigned int sc_idx = 100;
        unsigned __int32* pidx;
        for (int i = 0; i < 39; i += 3)
        {
            pidx = reinterpret_cast<unsigned __int32*>(&itlvbuffer1.values[0][i]);

            map_outputbuffer = m_map64qam[*pidx & 0x0FFF];
            fill_symbol<2>(sc_idx, [&](int j)
            {
                localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });

            map_outputbuffer = m_map64qam[(*pidx & 0x00FFF000) >> 12];
            fill_symbol<2>(sc_idx, [&](int j)
            {
                localsymbol[0].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // second antenna
        sc_idx = 100;
        for (int i = 0; i < 39; i += 3)
        {
            pidx = reinterpret_cast<unsigned __int32*>(&itlvbuffer2.values[0][i]);

            map_outputbuffer = m_map64qam[*pidx & 0x0FFF];
            fill_symbol<2>(sc_idx, [&](int j)
            {
                localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });

            map_outputbuffer = m_map64qam[(*pidx & 0x00FFF000) >> 12];
            fill_symbol<2>(sc_idx, [&](int j)
            {
                localsymbol[1].subcarriers[sc_idx] = map_outputbuffer.values[j];
            });
        }

        // pilot
        add_pilot(0, localsymbol[0]);
        add_pilot(1, localsymbol[1]);

#if 0
        for (int i = 0, j = 0; i < 128; i++, j++)
        {
            if (i == 32)
            {
                i += 64;
            }
            m_tfreqcompensated_f[0][j].re = localsymbol[0].subcarriers[i].re;
            m_tfreqcompensated_f[0][j].im = localsymbol[0].subcarriers[i].im;
            m_tfreqcompensated_f[1][j].re = localsymbol[1].subcarriers[i].re;
            m_tfreqcompensated_f[1][j].im = localsymbol[1].subcarriers[i].im;
        }

        static int nimage = 0;
        for (int k = 0; k < 2; k++)
        {
            m_plot.command("reset");
            //m_plot.command("set terminal jpeg");
            m_plot.command("set output 'y%02d_%05d.jpg'", k, nimage);
            m_plot.command("set title 'y%05d[%d]'", nimage, k);
            //m_plot.command("set xrange [-2:2]");
            //m_plot.command("set yrange [-2:2]");
            m_plot.begin_plot("w p 7");

            for (int i = 0; i < 64; i++)
            {
                //if (v == 64 - 21 || v == 64 - 7 || v == 7 || v == 21)
                {
                    m_plot.data("%f\t%f", m_tfreqcompensated_f[k][i].re, m_tfreqcompensated_f[k][i].im);
                    float fe = m_tfreqcompensated_f[k][i].re * m_tfreqcompensated_f[k][i].re
                        + m_tfreqcompensated_f[k][i].im * m_tfreqcompensated_f[k][i].im;
                    //m_plot.data("%f", fe);
                }
                //else
                //{
                //    m_plot.data("%f\t%f", 0, 0);
                //    //m_plot.data("%f", 0.0f);
                //}
            }
            m_plot.end_plot();
            getchar();
            //Sleep(300);
        }
        nimage++;
#endif

        // ifft
        m_ifft(localsymbol[0].vsubcarriers, tempsymbol[0].vsubcarriers);
        m_ifft(localsymbol[1].vsubcarriers, tempsymbol[1].vsubcarriers);

        memset(localsymbol[0].subcarriers, 0, localsymbol[0].nsubcarrier * sizeof(complex16));
        memset(localsymbol[1].subcarriers, 0, localsymbol[1].nsubcarrier * sizeof(complex16));

        // csd, cp and window
        flush_symbol(tx_buffer);

        m_symbolcount++;
        if (m_conv_outputcnt == 80)
        {
            stream_splitter_buffer[0] = stream_splitter_buffer[78];
            stream_splitter_buffer[1] = stream_splitter_buffer[79];
            m_conv_outputcnt = 2;
        }
        else
        {
            m_conv_outputcnt = 0;
        }
    }
}

void dot11n_phy::rx_scheduler()
{
    start_viterbi();
#if perf_unit_enabled
    int beginsbidx, endsbidx;
    tick_count tduration;
#endif
    while (m_rx_state != s_done)
    {
        switch(m_rx_state)
        {
        case s_frame_detection:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_frame_detection(m_rxstream)) 
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][FD] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_l_ltf;
            }
            else
                break;
        case s_l_ltf:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_l_ltf(m_rxstream)) 
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;
                printf("[Perf][LTF] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_l_sig;
            }else break;
        case s_l_sig:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
#if dot11a_mode
            if (rx_on_l_sig(m_rxstream)) m_rx_state = s_l_data;
#else
            if (rx_on_l_sig(m_rxstream))
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][LSIG] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_ht_sig1;
            }else break;
#endif
#if dot11a_mode
        case s_l_data:
            if (rx_on_l_data(m_rxstream)) m_rx_state = s_frame_detection;
            break;
#endif
        case s_ht_sig1:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_ht_sig1(m_rxstream))
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][HTSIG1] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif

                m_rx_state = s_ht_sig2;
            }
            //break;
        case s_ht_sig2:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_ht_sig2(m_rxstream))
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][HTSIG2] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_ht_stf;
            } else 
            {
                m_rx_state = s_frame_detection;
                break;
            }
        case s_ht_stf:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_ht_stf(m_rxstream))
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][HTSTF] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_ht_ltf1;
            }else break;
        case s_ht_ltf1:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_ht_ltf1(m_rxstream))
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][HTLTF1] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_ht_ltf2;
            }else break;
        case s_ht_ltf2:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            if (rx_on_ht_ltf2(m_rxstream))
            {
#if perf_unit_enabled
                tend      = tick_count::now();
                endsbidx  = m_rx_current_sb_idx;
                tduration = tend - tstart;

                printf("[Perf][HTLTF2] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                    beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
                m_rx_state = s_ht_data;
            }else break;
        case s_ht_data:
#if perf_unit_enabled
            beginsbidx = m_rx_current_sb_idx;
            tstart     = tick_count::now();
#endif
            while (!rx_on_ht_data(m_rxstream));
#if perf_unit_enabled
            tend      = tick_count::now();
            endsbidx  = m_rx_current_sb_idx;
            tduration = tend - tstart;

            printf("[Perf][HTDATA] %d -> %d, time = %.3f us, rate = %.3f MSPS\n",
                beginsbidx, endsbidx, tduration.us(), (endsbidx - beginsbidx) * 28 / tduration.us());
#endif
            _perf_report();


            printf("mcs=%d, length=%d, ht_symbol_count=%d\n", m_rx_mcs, m_rx_frame_length, m_rx_symbol_count);
            printf("CRC32 correct = %d!\n\n", m_rx_crc32_correct);

            m_rx_state = s_frame_detection;

            break;
        case s_done:
            break;
        }
    }
}

bool dot11n_phy::rx_on_frame_detection(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    //m_rxstream.sbidx = 4960; // debug
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
#if 1
    int positivecnt = 0;
    
    static int icount = 0;

    //m_plot.begin_plot("");
    for ( int n = 0; n < m_max_cca_sbcount; n++ )
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);

        for (int vidx = 0; vidx < sb1->v_datacount; vidx++)
        {
            icount++;
#if use_fixed_point
            m_autocorr_vi.exec(sb1->v_data[vidx]);
#else
            m_autocorr_f.exec(sb1->v_data[vidx]);
#endif
            for (int j = 0; j < 4; j++)
            {
#if use_fixed_point
#else
                double ratio = m_autocorr_f.xcorrenergysqr[j] / m_autocorr_f.energysqr[j];
#endif

                //if(m_asyncok)
                //m_plot.data("%f", ratio);
                
                if (!m_asyncok)                
                {
#if use_fixed_point
                    if ( m_autocorr_vi.m_vCorrEnergy[0][j]  > (m_autocorr_vi.m_vEnergy[0][j] >> 1) )
                    {
                        //printf("^ [fixed]energy = %I64d, xcorrenergy = %I64d\n", m_autocorr_vi.m_vEnergy[0][j], m_autocorr_vi.m_vCorrEnergy[0][j]);
                        
#else
                    if (ratio > 0.5 /*&& venergysum[j] > 1.5e14*/)
                    {
                        //printf("^ [float]energy = %.0f, xcorrenergy = %.0f, ratio=%f\n", m_autocorr_f.energysqr[j], m_autocorr_f.xcorrenergysqr[j], ratio);
                        //printf("^ sp=%d, sb=%d, ratio=%f\n", m_rx_current_sb_idx * 28 + vidx * 4 + j, m_rx_current_sb_idx, ratio);
#endif                  
                        m_asyncok = true;
                    }
                }
                else
                {
#if use_fixed_point
                    if ( m_autocorr_vi.m_vCorrEnergy[0][j]  < (m_autocorr_vi.m_vEnergy[0][j] >> 3) )
#else
                    if (ratio < 0.125)
#endif
                    {
                        if (positivecnt > 96 && positivecnt < 160)
                        {
#if use_fixed_point
                            m_autocorr_vi.reset();
                            //printf("v sp=%d, sb=%d, poscnt=%d\n",
                            //    m_rx_current_sb_idx * 28 + vidx * 4 + j, m_rx_current_sb_idx, positivecnt);
#else
                            m_autocorr_f.reset();
                            //printf("v sp=%d, sb=%d, ratio=%f, poscnt=%d\n",
                            //    m_rx_current_sb_idx * 28 + vidx * 4 + j, m_rx_current_sb_idx, ratio, positivecnt);
#endif
                            m_asyncok = false;
                            //m_plot.end_plot();
                            //getchar();

                            m_rx_cyclic_write_idx = 0;
                            vidx += 1; // skip current 4 samples if possible
                            for (; vidx < sb1->v_datacount; vidx++)
                            {
                                m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
                                m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
                                m_rx_cyclic_write_idx++;
                            }

                            printf("peak <- @ %d - %d\n", icount, j);

                            m_rx_current_sb_idx = ++m_rxstream;
                            return true;
                        }
                        else
                        {
                            m_asyncok = false;
                            positivecnt = 0;
                        }
                    }
                    else
                    {
                        positivecnt++;
                    }
                }
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }
#else
    int vidx = 0;
    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(0, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 48)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);
_work:

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;
    return true;
#endif

    return false;
}

bool dot11n_phy::rx_on_l_ltf(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;

    int vidx = 0;
    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 32)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

    _work:
    //remove_dc((complex16*)&m_rx_cyclic_buffer[0][0], 128);
    //remove_dc((complex16*)&m_rx_cyclic_buffer[1][0], 128);

#if use_fixed_point
    short cfo1_i       = m_freq.estimate_i( (complex16*)&m_rx_cyclic_buffer[0][0], 64, 64);
    short cfo2_i       = m_freq.estimate_i( (complex16*)&m_rx_cyclic_buffer[1][0], 64, 64);

    m_rx_cfo_theta_i   = 0;
    m_rx_cfo_delta_i   = 0;
    m_rx_cfo_step_i    = cfo1_i;
    //m_rx_cfo_step_i  = ((cfo1_i + cfo2_i) >> 1);

    //printf("cfo1_i=%d, cfo2_i=%d, avgcfo_i=%d\n", cfo1_i, cfo2_i, m_rx_cfo_step_i);

#if use_sse
    v_s vcfodelta;
    m_rx_vcfo_theta_i.v_zero();
    vcfodelta.v_setall(m_rx_cfo_step_i);

    m_rx_vcfo_step_i  = vcfodelta.v_shift_left(3);

    vcfodelta         = v_add(vcfodelta, (v_s&)vcfodelta.v_shift_bytes_left<2>());
    vcfodelta         = v_add(vcfodelta, (v_s&)vcfodelta.v_shift_bytes_left<4>());
    vcfodelta         = v_add(vcfodelta, (v_s&)vcfodelta.v_shift_bytes_left<8>());
    vcfodelta         = vcfodelta.v_shift_bytes_left<2>();
    m_rx_vcfo_delta_i = vcfodelta;

    // make 16 * 8 coefficients
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 16, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 32);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 32);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 128, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 128);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 128);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][64], (v_cs*)&m_rx_fsamples_i[0][0]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][64], (v_cs*)&m_rx_fsamples_i[1][0]);

#if use_sse
    v_siso_channel_estimation_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], 16);
    v_siso_channel_estimation_i((v_cs*)&m_rx_fsamples_i[1][0], (v_cs*)&m_rx_channel_i[1][0], 16);
#else

    //
    //v_siso_channel_estimation_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], 16);
    //siso_channel_estimation_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[1][0], 64);

    //for (int i = 0; i < 64; i++)
    //{
    //    printf("%d, %d -- %d, %d\n", m_rx_channel_i[0][i].re, m_rx_channel_i[0][i].im,
    //        m_rx_channel_i[1][i].re, m_rx_channel_i[1][i].im);
    //}
    //

    siso_channel_estimation_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[0][0], 64);
    siso_channel_estimation_i(&m_rx_fsamples_i[1][0], &m_rx_channel_i[1][0], 64);
#endif

    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////

    float cfo1_f = m_freq.estimate_f( (complex16*)&m_rx_cyclic_buffer[0][0], 64, 64);
    float cfo2_f = m_freq.estimate_f( (complex16*)&m_rx_cyclic_buffer[1][0], 64, 64);
    m_rx_cfo_step_f = cfo1_f;
    //m_rx_cfo_step_f  = (cfo1_f + cfo2_f) / 2.0f;
    m_rx_cfo_delta_f = 0.0f;
    m_rx_cfo_theta_f = 0.0f;

    //printf("cfo1_f=%f, cfo2_f=%f, avgcfo_f=%f\n", cfo1_f, cfo2_f, m_rx_cfo_step_f);

#if use_sse
    // set up parameters
    m_rx_vcfo_theta_f.v_zero();

    v_f vfdelta1, vfdelta2;
    v_f vfstep;

    vfdelta1.v_setall(m_rx_cfo_step_f);

    // vstep = 4*vdelta
    // add 4 times. in fixed point version, just shift left 2 bits.
    vfstep           = v_add(vfdelta1, vfdelta1);
    m_rx_vcfo_step_f = v_add(vfstep, vfstep);
    // vcfo_delta    = (0, 1, 2, 3)*vdelta
    vfdelta1         = vfdelta1.v_shift_bytes_left<4>();
    vfdelta2         = vfdelta1.v_shift_bytes_left<4>();
    vfdelta1         = v_add(vfdelta1, vfdelta2);
    vfdelta2         = vfdelta2.v_shift_bytes_left<4>();

    m_rx_vcfo_delta_f = v_add(vfdelta1, vfdelta2);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f, 32, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 32);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 32);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 128, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 128);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 128);
#endif

    // fft, use the second LTF to estimate channel
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][64], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][64], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

    siso_channel_estimation_f(&m_rx_fsamples_f[0][0], &m_rx_channel_f[0][0], 64);
    siso_channel_estimation_f(&m_rx_fsamples_f[1][0], &m_rx_channel_f[1][0], 64);
#endif

#if 0
    m_plot.command("set title 'siso channel'");
    //m_plot.command("set xrange [-2:2]");
    //m_plot.command("set yrange [-2:2]");
    m_plot.begin_plot("w lp 7");
    for (int i = 0; i < 64; i++)
    {
        //m_plot.data("%f\t%f", m_rx_channel_f[0][i].re, m_rx_channel_f[0][i].im);
        float fe = m_rx_fsamples_f[0][i].re * m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i].im * m_rx_fsamples_f[0][i].im;
        m_plot.data("%f", fe);
    }
    m_plot.end_plot();
    getchar();
#endif

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

    m_pilotPt_i = 0;
    m_pilotPt_f = 0;

    return true;
}

bool dot11n_phy::rx_on_l_sig(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

_work:

#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][0]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][0]);

#if use_sse
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], (v_cs*)&m_rx_channel_compensated_i[0][0], 16);
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[1][0], (v_cs*)&m_rx_channel_i[1][0], (v_cs*)&m_rx_channel_compensated_i[1][0], 16);
#else
    siso_channel_compensate_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[0][0], &m_rx_channel_compensated_i[0][0], 64);
    siso_channel_compensate_i(&m_rx_fsamples_i[1][0], &m_rx_channel_i[1][0], &m_rx_channel_compensated_i[1][0], 64);
#endif

    //pilot_tracking_i(&m_rx_channel_compensated_i[0][0], 64);
    
    // MRC combining
    //combine_i(&m_rx_channel_compensated_i[0][0], &m_rx_channel_compensated_i[1][0], &m_rx_channel_compensated_i[0][0]);

    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////
#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f, 20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

#if use_sse
    v_siso_channel_compensate_f((v_cf*)&m_rx_fsamples_f[0][0], (v_cf*)&m_rx_channel_f[0][0], (v_cf*)&m_rx_channel_compensated_f[0][0], 32);
    v_siso_channel_compensate_f((v_cf*)&m_rx_fsamples_f[1][0], (v_cf*)&m_rx_channel_f[1][0], (v_cf*)&m_rx_channel_compensated_f[1][0], 32);
#else
    siso_channel_compensate_f(&m_rx_fsamples_f[0][0], &m_rx_channel_f[0][0], &m_rx_channel_compensated_f[0][0], 64);
    siso_channel_compensate_f(&m_rx_fsamples_f[1][0], &m_rx_channel_f[1][0], &m_rx_channel_compensated_f[1][0], 64);
#endif

    pilot_tracking_f(&m_rx_channel_compensated_f[0][0], 64);

    // MRC combining
    combine_f(&m_rx_channel_compensated_f[0][0], &m_rx_channel_compensated_f[1][0], &m_rx_channel_compensated_f[0][0]);

    // calculate EVM to estimate SNR per subcarrier
#if 0
    complexf noise;
    for (int k = 0; k < 2; k++)
    {
        for (int i = 0; i < 64; i++)
        {
            if ( (i >= 1 && i <= 26 && i != 7 && i != 21)
                || (i >= 64 - 26 && i < 64 && i != 64 - 7 && i != 64 - 21)
                )
            {
                if ( m_rx_channel_compensated_f[0][i].re > 0 )
                {
                    noise.re = m_rx_channel_compensated_f[k][i].re - 1.0f;
                    noise.im = m_rx_channel_compensated_f[k][i].im;
                }
                else
                {
                    noise.re = m_rx_channel_compensated_f[k][i].re + 1.0f;
                    noise.im = m_rx_channel_compensated_f[k][i].im;
                }
                float noise_energy = noise.re * noise.re + noise.im * noise.im;
                float snr = 10 * log10f(1 / noise_energy);
                printf("snr_%d@%d: %f dB\n", k, i, snr);
            }
        }
    }
#endif

    cvt2cs(m_rx_channel_compensated_f[0], m_rx_channel_compensated_i[0], 64, 128.0f);

#endif// float point ends

    // plot L_SIG
#if 0
    m_plot.command("set title 'L_SIG'");
    //m_plot.command("set xrange [-2:2]");
    //m_plot.command("set yrange [-2:2]");
    m_plot.begin_plot("w p 7");
    for (int i = 0; i < 64; i++)
    {
        //m_plot.data("%f\t%f", m_rx_channel_compensated_f[0][i].re, m_rx_channel_compensated_f[0][i].im);
        m_plot.data("%d\t%d", m_rx_channel_compensated_i[0][i].re, m_rx_channel_compensated_i[0][i].im);
        //float fe = m_rx_fsamples_f[0][i].re * m_rx_fsamples_f[0][i].re
        //    + m_rx_fsamples_f[0][i].im * m_rx_fsamples_f[0][i].im;
        //m_plot.data("%f", fe);
    }
    m_plot.end_plot();
    getchar();
#endif

    siso_demap_bpsk_i(m_rx_channel_compensated_i[0], m_rx_demapped[0]);
    siso_deinterleave1bpsc(m_rx_demapped[0], m_rx_deinterleaved[0]);

    m_rx_viterbi_param.nTotalBits         = 24;
    m_rx_viterbi_param.nTraceBackNormMask = 0x7;
    m_rx_viterbi_param.nTraceBackDepth    = 12;

    memset(&m_rx_deinterleaved[0][48], 0, 18);
    m_rxviterbi.viterbi12_seq(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis, (char*)m_rx_deinterleaved[0], (char *)(&m_rx_l_signal));

    bool bret = Parse_L_SIG(m_rx_l_signal, &m_rx_l_length, &m_rx_l_rate, &m_rx_l_dbps);

    //printf("L_SIG: correct=%d, signal=%X, length=%d, rate=%X, dbps=%d\n",
    //    bret, m_rx_l_signal, m_rx_l_length, m_rx_l_rate, m_rx_l_dbps);

    //static int icorrect = 0;
    //icorrect += bret;
    //printf("=====L_SIG Correct: %d========\n", icorrect);

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

#if dot11a_mode
    if (!bret)
    {
        return false;
    }

    m_rx_symbol_count = (m_rx_l_length * 8 + 16 + 6 + m_rx_l_dbps) / m_rx_l_dbps;
    m_rx_viterbi_symbol_count = m_rx_symbol_count;

    switch (m_rx_l_rate)
    {
    case DOT11A_RATE_54M:
        m_rx_viterbi_fifo.lvb4.Clear();
        break;
    }

    m_viterbi_status = job_running;

    _mm_mfence();
#endif

    return true;
}

bool dot11n_phy::rx_on_ht_sig1(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

_work:

#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);

    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][0]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][0]);

#if use_sse
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], (v_cs*)&m_rx_channel_compensated_i[0][0], 16);
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[1][0], (v_cs*)&m_rx_channel_i[1][0], (v_cs*)&m_rx_channel_compensated_i[1][0], 16);
#else
    siso_channel_compensate_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[0][0], &m_rx_channel_compensated_i[0][0], 64);
    siso_channel_compensate_i(&m_rx_fsamples_i[1][0], &m_rx_channel_i[1][0], &m_rx_channel_compensated_i[1][0], 64);
#endif    
    //pilot_tracking_i(&m_rx_channel_compensated_i[0][0], 64);

    combine_i(&m_rx_channel_compensated_i[0][0], &m_rx_channel_compensated_i[1][0], &m_rx_channel_compensated_i[0][0]);
    
    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////
#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

    siso_channel_compensate_f(&m_rx_fsamples_f[0][0], &m_rx_channel_f[0][0], &m_rx_channel_compensated_f[0][0], 64);
    siso_channel_compensate_f(&m_rx_fsamples_f[1][0], &m_rx_channel_f[1][0], &m_rx_channel_compensated_f[1][0], 64);

    pilot_tracking_f(&m_rx_channel_compensated_f[0][0], 64);

    combine_f(&m_rx_channel_compensated_f[0][0], &m_rx_channel_compensated_f[1][0], &m_rx_channel_compensated_f[0][0]);

    cvt2cs(m_rx_channel_compensated_f[0], m_rx_channel_compensated_i[0], 64, 128.0f);
#endif    

    siso_demap_bpsk_q(m_rx_channel_compensated_i[0], &m_rx_demapped[0][0]);
    siso_deinterleave1bpsc(m_rx_demapped[0], m_rx_deinterleaved[0]);

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

    return true;
}

bool dot11n_phy::rx_on_ht_sig2(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

_work:

#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);

    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][0]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][0]);
    
#if use_sse
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], (v_cs*)&m_rx_channel_compensated_i[0][64], 16);
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[1][0], (v_cs*)&m_rx_channel_i[1][0], (v_cs*)&m_rx_channel_compensated_i[1][64], 16);
#else
    //// check
    //v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], (v_cs*)&m_rx_channel_compensated_i[0][0], 16);
    //siso_channel_compensate_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[0][0], &m_rx_channel_compensated_i[0][64], 64);
    //for (int i = 0; i < 64; i++)
    //{
    //    printf("(%d,%d)\n", m_rx_channel_compensated_i[0][i].re - m_rx_channel_compensated_i[0][i+64].re, 
    //        m_rx_channel_compensated_i[0][i].im - m_rx_channel_compensated_i[0][i+64].im);
    //}
    //// --<

    siso_channel_compensate_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[0][0], &m_rx_channel_compensated_i[0][64], 64);
    siso_channel_compensate_i(&m_rx_fsamples_i[1][0], &m_rx_channel_i[1][0], &m_rx_channel_compensated_i[1][64], 64);
#endif

    //pilot_tracking_i(&m_rx_channel_compensated_i[0][64], 64);

    combine_i(&m_rx_channel_compensated_i[0][64], &m_rx_channel_compensated_i[1][64], &m_rx_channel_compensated_i[0][64]);

    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////

#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

    siso_channel_compensate_f(&m_rx_fsamples_f[0][0], &m_rx_channel_f[0][0], &m_rx_channel_compensated_f[0][64], 64);
    siso_channel_compensate_f(&m_rx_fsamples_f[1][0], &m_rx_channel_f[1][0], &m_rx_channel_compensated_f[1][64], 64);

    pilot_tracking_f(&m_rx_channel_compensated_f[0][64], 64);

    combine_f(&m_rx_channel_compensated_f[0][64], &m_rx_channel_compensated_f[1][64], &m_rx_channel_compensated_f[0][64]);

    cvt2cs(&m_rx_channel_compensated_f[0][64], &m_rx_channel_compensated_i[0][64], 64, 128.0f);
#endif

    siso_demap_bpsk_q(&m_rx_channel_compensated_i[0][64], &m_rx_demapped[0][48]);
    siso_deinterleave1bpsc(&m_rx_demapped[0][48], &m_rx_deinterleaved[0][48]);

    m_rx_viterbi_param.nTotalBits         = 48;
    m_rx_viterbi_param.nTraceBackNormMask = 0x7;
    m_rx_viterbi_param.nTraceBackDepth    = 12;

    memset(&m_rx_deinterleaved[0][96], 0, 18);
    m_rxviterbi.viterbi12_seq(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis, (char*)m_rx_deinterleaved[0], (char *)(m_chtsig));

    bool bret = parse_ht_sig();

    if (!bret)
    {
        m_rx_current_sb_idx = ++m_rxstream;
        //printf("HT signal error!\n");
        return false;
    }

    m_rx_symbol_count = calc_rx_symbol_count(m_rx_mcs, m_rx_frame_length);

    //printf("mcs=%d, length=%d, ht_symbol_count=%d\n", m_rx_mcs, m_rx_frame_length, m_rx_symbol_count);

    m_rx_viterbi_symbol_count = m_rx_symbol_count;

    switch (m_rx_mcs)
    {
    case 8:
        m_rx_viterbi_fifo.htvb1.Clear();
        break;
    case 9:
    case 10:
        m_rx_viterbi_fifo.htvb2.Clear();
        break;
    case 11:
    case 12:
        m_rx_viterbi_fifo.htvb3.Clear();
        break;
    case 13:
    case 14:
        //case 15:
        m_rx_viterbi_fifo.htvb4.Clear();
        break;
    }

    m_viterbi_status = job_running;
    _mm_mfence();

#if 0
    m_plot.command("set title 'HT_SIG2'");
    m_plot.begin_plot("w p 7");
    for (int i = 0; i < 64; i++)
    {
        //m_plot.data("%f\t%f", m_rx_channel_compensated_f[1][i].re, m_rx_channel_compensated_f[1][i].im);
        m_plot.data("%d\t%d", m_rx_channel_compensated_i[1][i].re, m_rx_channel_compensated_i[1][i].im);
        //float fe = m_rx_fsamples_f[0][i].re * m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i].im * m_rx_fsamples_f[0][i].im;
        //m_plot.data("%f", fe);
    }
    m_plot.end_plot();
    getchar();
#endif

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

    return true;
}

bool dot11n_phy::rx_on_ht_stf(dot11n_rx_stream& rxstream)
{
    signal_block* sb;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb = m_rxstream.get(0, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

_work:

#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);    
#endif
    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////
#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);    
#endif

#endif

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

    return true;
}

bool dot11n_phy::rx_on_ht_ltf1(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

_work:
#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);

    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][0]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][0]);

    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////
#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);
#endif

#if 0
    m_plot.command("set title 'HT_LTF1'");
    m_plot.begin_plot("w lp 7");
    for (int i = 0; i < 64; i++)
    {
        //m_plot.data("%f\t%f", m_rx_fsamples_f[0][i].re, m_rx_fsamples_f[0][i].im);
        float fe = m_rx_fsamples_f[0][i].re * m_rx_fsamples_f[0][i].re + m_rx_fsamples_f[0][i].im * m_rx_fsamples_f[0][i].im;
        m_plot.data("%f", fe);
    }
    m_plot.end_plot();
    getchar();
#endif

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

    return true;
}

bool dot11n_phy::rx_on_ht_ltf2(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);

_work:

#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);

    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][64]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][64]);

    
#if use_sse
    v_mimo_channel_estimation_i();
#else
    mimo_channel_estimation_i();
#endif

    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////

#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][64]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][64]);

#if use_sse
    v_mimo_channel_estimation_f();
#else
    mimo_channel_estimation_f();
#endif

#endif

#if draw_enabled
    m_draw[draw_h11]->DrawSqrt(&m_rx_channel_i[0][0], 64);
    m_draw[draw_h12]->DrawSqrt(&m_rx_channel_i[0][64], 64);
    m_draw[draw_h21]->DrawSqrt(&m_rx_channel_i[1][0], 64);
    m_draw[draw_h22]->DrawSqrt(&m_rx_channel_i[1][64], 64);
#endif

#if 0
    m_plot.command("reset");
    m_plot.command("set multiplot layout 2, 2");
    for (int h = 0; h < 2; h++)
    {
        for (int k = 0; k < 2; k++)
        {
            //m_plot.command("reset");
            m_plot.command("set title 'H[%d][%d]'", h, k);
            //m_plot.command("set terminal jpeg");
            //m_plot.command("set output 'h%d%d.jpg'", h, k);
            m_plot.begin_plot("w lp 7");
            for (int i = 0; i < 64; i++)
            {
                int v = (32 + i) % 64;
#if use_fixed_point
                int fe = (int)m_rx_channel_i[h][k * 64 + v].re * (int)m_rx_channel_i[h][k * 64 + v].re
                    + (int)m_rx_channel_i[h][k * 64 + v].im * (int)m_rx_channel_i[h][k * 64 + v].im;;
                m_plot.data("%d", fe);
#else
                //m_plot.data("%f\t%f", m_rx_channel_f[0][i].re, m_rx_channel_f[0][i].im);
                float fe = m_rx_channel_f[h][k * 64 + v].re * m_rx_channel_f[h][k * 64 + v].re
                    + m_rx_channel_f[h][k * 64 + v].im * m_rx_channel_f[h][k * 64 + v].im;
                m_plot.data("%f", fe);
#endif
            }
            m_plot.end_plot();
        }
    }
    m_plot.command("unset multiplot");
    getchar();
    //Sleep(500);
#endif

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;

#if perf_enabled
    m_perf.perf_reset();
#endif

    return true;
}

bool dot11n_phy::rx_on_l_data(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);
_work:
#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    //m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);

    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    //m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][64]);
    //m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][64]);

#if use_sse
    v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[0][0], (v_cs*)&m_rx_channel_i[0][0], (v_cs*)&m_rx_channel_compensated_i[0][64], 16);
    //v_siso_channel_compensate_i((v_cs*)&m_rx_fsamples_i[1][0], (v_cs*)&m_rx_channel_i[1][0], (v_cs*)&m_rx_channel_compensated_i[1][64], 16);
#else
    siso_channel_compensate_i(&m_rx_fsamples_i[0][0], &m_rx_channel_i[0][0], &m_rx_channel_compensated_i[0][64], 64);
    //siso_channel_compensate_i(&m_rx_fsamples_i[1][0], &m_rx_channel_i[1][0], &m_rx_channel_compensated_i[1][64], 64);
#endif
    //pilot_tracking_i(&m_rx_channel_compensated_i[0][64], 64);

    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////

#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    //m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    //m_freq.compensate((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    //m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

    siso_channel_compensate_f(&m_rx_fsamples_f[0][0], &m_rx_channel_f[0][0], &m_rx_channel_compensated_f[0][0], 64);

    pilot_tracking_f(&m_rx_channel_compensated_f[0][0], 64);
#endif

    unsigned char *stream_joiner_buffer;
    static int pushcnt = 0;
    static int nimage = 0;
    static int inputsymbolcnt = 0;

    switch (m_rx_l_rate)
    {
    case DOT11A_RATE_54M:
#if use_fixed_point
#else
        cvt2cs(&m_rx_channel_compensated_f[0][0], &m_rx_channel_compensated_i[0][0], 64, 128.0f / (7.0f / sqrt(42.0f)) );
#endif
        siso_demap_64qam(m_rx_channel_compensated_i[0], m_rx_demapped[0]);
        m_rx_viterbi_fifo.lvb4.SpaceWait(1, &m_workindicator);
        stream_joiner_buffer = m_rx_viterbi_fifo.lvb4.Push();
        siso_deinterleave6bpsc(m_rx_demapped[0], stream_joiner_buffer);
        m_rx_viterbi_fifo.lvb4.Flush();
        break;
    }

    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;
    m_rx_symbol_count--;

    // zero-padding for viterbi traceback
    if (m_rx_symbol_count == 0)
    {
        switch (m_rx_l_rate)
        {
        case DOT11A_RATE_54M:
            m_rx_viterbi_fifo.lvb4.SpaceWait(1, &m_workindicator);
            stream_joiner_buffer = m_rx_viterbi_fifo.lvb4.Push();
            memset(stream_joiner_buffer, 0, 288);
            m_rx_viterbi_fifo.lvb4.Flush();
            break;
        }

        while (m_viterbi_status != job_done)
        {
            if (!(m_workindicator))
            {
                break;
            }
            _mm_pause();
        }

#if 0
        for (unsigned int i = 0; i < uiLength; i++)
        {
            if (i % 16 == 0)
            {
                printf("\n");
            }
            printf("%02X ", m_rx_decoded_frame[i]);
        }
        printf("\n");
#endif
        return true;
    }
    else
    {
        return false;
    }
}

bool dot11n_phy::rx_on_ht_data(dot11n_rx_stream& rxstream)
{
    signal_block* sb1;
    signal_block* sb2;
    m_rx_current_sb_idx = m_rxstream.currentsbidx();
    int vidx = 0;

    _perf_begin();
    _perf_tick();
    do
    {
        sb1 = m_rxstream.get(0, m_rx_current_sb_idx);
        sb2 = m_rxstream.get(1, m_rx_current_sb_idx);
        vidx = 0;
        for (; vidx < sb1->v_datacount; vidx++)
        {
            m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
            m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
            m_rx_cyclic_write_idx++;
            if (m_rx_cyclic_write_idx == 20)
            {
                vidx++;
                goto _work;
            }
        }
        m_rx_current_sb_idx = ++m_rxstream;
    }while(true);
_work:
    
    _perf_tick();

#if use_fixed_point

#if use_sse
    m_rx_vcfo_theta_i.v_setall(m_rx_cfo_theta_i);
    m_freq.v_make_coeff_i((v_cs*)m_rx_cfo_coeff_i, 10, m_rx_vcfo_delta_i, m_rx_vcfo_step_i, m_rx_vcfo_theta_i);

    _perf_tick();

    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[0][0], 20);
    m_freq.v_compensate_i((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cs*)m_rx_cfo_coeff_i, (v_cs*)&m_rx_cfo_compensated_i[1][0], 20);
#else
    m_freq.make_coeff_i(m_rx_cfo_coeff_i, 80, m_rx_cfo_delta_i, m_rx_cfo_step_i, m_rx_cfo_theta_i);

    _perf_tick();

    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[0][0], 80);
    m_freq.compensate_i((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_i, &m_rx_cfo_compensated_i[1][0], 80);
#endif
    
    _perf_tick();

    m_fft((v_cs*)&m_rx_cfo_compensated_i[0][16], (v_cs*)&m_rx_fsamples_i[0][0]);
    m_fft((v_cs*)&m_rx_cfo_compensated_i[1][16], (v_cs*)&m_rx_fsamples_i[1][0]);

    _perf_tick();

#if use_sse
    v_mimo_channel_compensate_i();
#else
    mimo_channel_compensate_i();
#endif

    _perf_tick();

    pilot_tracking_i(&m_rx_channel_compensated_i[0][0], 64);


    //////////////////////////////////////////////////////////////////////////
#else// float point version
    //////////////////////////////////////////////////////////////////////////
#if use_sse
    m_rx_vcfo_theta_f.v_setall(m_rx_cfo_theta_f);
    m_freq.v_make_coeff_f((v_cf*)m_rx_cfo_coeff_f,  20, m_rx_vcfo_delta_f, m_rx_vcfo_step_f, m_rx_vcfo_theta_f);

    _perf_tick();

    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[0][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[0][0], 20);
    m_freq.v_compensate_f((v_cs*)&m_rx_cyclic_buffer[1][0], (v_cf*)m_rx_cfo_coeff_f, (v_cf*)&m_rx_cfo_compensated_f[1][0], 20);
#else
    m_freq.make_coeff_f(m_rx_cfo_coeff_f, 80, m_rx_cfo_delta_f, m_rx_cfo_step_f, m_rx_cfo_theta_f);

    _perf_tick();

    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[0][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[0][0], 80);
    m_freq.compensate_f((complex16*)&m_rx_cyclic_buffer[1][0], m_rx_cfo_coeff_f, &m_rx_cfo_compensated_f[1][0], 80);
#endif

    _perf_tick();

    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[0][16], (fftwf_complex*)&m_rx_fsamples_f[0][0]);
    m_fftw((fftwf_complex*)&m_rx_cfo_compensated_f[1][16], (fftwf_complex*)&m_rx_fsamples_f[1][0]);

    _perf_tick();

#if use_sse
    v_mimo_channel_compensate_f();
#else
    mimo_channel_compensate_f();
#endif

    _perf_tick();

    pilot_tracking_f(&m_rx_channel_compensated_f[0][0], 64);

    _perf_tick();
    // Note: 1 will be normalized to 128 for all cases, because demap lookup table has been rebuilt to hold 64QAM symbols
    cvt2cs(&m_rx_channel_compensated_f[0][0], &m_rx_channel_compensated_i[0][0], 64, 128.0f);
    cvt2cs(&m_rx_channel_compensated_f[1][0], &m_rx_channel_compensated_i[1][0], 64, 128.0f);
#endif

    _perf_tick();

    unsigned char *stream_joiner_buffer;
    static int pushcnt        = 0;
    static int nimage         = 0;
    static int inputsymbolcnt = 0;
    unsigned char outchar1    = 0;
    unsigned char outchar2    = 0;

    int outcnt = 0;
    switch (m_rx_mcs)
    {
    case 8:

#if 0
        //m_plot.command("set terminal gif");
        //m_plot.command("set output '%d.gif'", nimage);
        //m_plot.command("set multiplot layout 2, 1");

        if (nimage == 0)
        {
            //m_plot.command("set title 'y%05d[%d]'", nimage, k);
            m_plot.command("set xrange [-200:200]");
            m_plot.command("set yrange [-200:200]");
            m_plot.command("set xtics 16");
            m_plot.command("set ytics 16");
            m_plot.command("set grid");
            m_plot.begin_plot("w p 7");
        }

        for (int k = 0; k < 1; k++)
        {
            for (int i = 0; i < 64; i++)
            {
                int v = (32 + i) % 64;
                if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
                {
                    m_plot.data("%d\t%d", m_rx_sic_f[k][v].re, m_rx_sic_f[k][v].im);
                    //m_plot.data("%d\t%d", m_rx_channel_compensated_i[k][v].re, m_rx_channel_compensated_i[k][v].im);
                    float fe = m_rx_channel_compensated_f[k][v].re * m_rx_channel_compensated_f[k][v].re
                        + m_rx_channel_compensated_f[k][v].im * m_rx_channel_compensated_f[k][v].im;
                    //m_plot.data("%f", fe);
                }
            }
            //m_plot.end_plot();
            //getchar();
        }

        nimage++;
        //m_plot.command("unset multiplot");

        if (m_rx_symbol_count == 1)
        {
            m_plot.end_plot();
            nimage = 0;
            getchar();
            //Sleep(1000);
        }
#endif
        mimo_demap_bpsk();        
        _perf_tick();

        mimo_deinterleave<1>(m_rx_demapped, m_rx_deinterleaved);
        _perf_tick();

        m_rx_viterbi_fifo.htvb1.SpaceWait(1, &m_workindicator);
        stream_joiner_buffer = m_rx_viterbi_fifo.htvb1.Push();

        _perf_tick();

        stream_joiner<1>(m_rx_deinterleaved, 52, stream_joiner_buffer);

#if 0
        for (int i = 0; i < 104; i++)
        {
            printf("%u ", stream_joiner_buffer[i]);
        }
        printf("\n\n");
        getchar();
#endif

        _perf_tick();

        m_rx_viterbi_fifo.htvb1.Flush();
        break;
    case 9:
    case 10:
#if 0
        //m_plot.command("set terminal gif");
        //m_plot.command("set output '%d.gif'", nimage);
        //m_plot.command("set multiplot layout 2, 1");

        if (nimage == 0)
        {
            //m_plot.command("set title 'y%05d[%d]'", nimage, k);
            //m_plot.command("set xrange [-200:200]");
            //m_plot.command("set yrange [-200:200]");
            m_plot.command("set xtics 16");
            m_plot.command("set ytics 16");
            m_plot.command("set grid");
            m_plot.begin_plot("w p 7");
        }

        for (int k = 0; k < 1; k++)
        {
            //m_plot.command("reset");
            //m_plot.command("set terminal jpeg");
            //m_plot.command("set output 'y%02d_%05d.jpg'", k, nimage);
            for (int i = 0; i < 64; i++)
            {
                int v = (32 + i) % 64;
                if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
                {
                    m_plot.data("%d\t%d", m_rx_channel_compensated_i[k][v].re, m_rx_channel_compensated_i[k][v].im);
                    float fe = m_rx_channel_compensated_f[k][v].re * m_rx_channel_compensated_f[k][v].re
                        + m_rx_channel_compensated_f[k][v].im * m_rx_channel_compensated_f[k][v].im;
                    //m_plot.data("%f", fe);
                }
            }
            //m_plot.end_plot();
            //getchar();
        }

        nimage++;
        //if (nimage % 20 == 0)
        //{
        //    m_plot.end_plot();
        //}

        //m_plot.command("unset multiplot");

        if (m_rx_symbol_count == 1)
        {
            m_plot.end_plot();
            nimage = 0;
            getchar();
            //Sleep(1000);
        }
#endif
        mimo_demap_qpsk();
        _perf_tick();
        mimo_deinterleave<2>(m_rx_demapped, m_rx_deinterleaved);
        _perf_tick();

        m_rx_viterbi_fifo.htvb2.SpaceWait(1, &m_workindicator);
        stream_joiner_buffer = m_rx_viterbi_fifo.htvb2.Push();
        _perf_tick();
        stream_joiner<1>(m_rx_deinterleaved, 104, stream_joiner_buffer);
        _perf_tick();
        m_rx_viterbi_fifo.htvb2.Flush();
        break;
    case 11:
    case 12:
#if 0
        //m_plot.command("set terminal gif");
        //m_plot.command("set output '%d.gif'", nimage);
        //m_plot.command("set multiplot layout 2, 1");

        if (nimage == 0)
        {
            //m_plot.command("set title 'y%05d[%d]'", nimage, k);
            m_plot.command("set xrange [-200:200]");
            m_plot.command("set yrange [-200:200]");
            m_plot.command("set xtics 16");
            m_plot.command("set ytics 16");
            m_plot.command("set grid");
            m_plot.begin_plot("w p 7");
        }

        for (int k = 0; k < 1; k++)
        {
            //m_plot.command("reset");
            //m_plot.command("set terminal jpeg");
            //m_plot.command("set output 'y%02d_%05d.jpg'", k, nimage);

            for (int i = 0; i < 64; i++)
            {
                int v = (32 + i) % 64;
                if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
                {
                    m_plot.data("%d\t%d", m_rx_channel_compensated_i[k][v].re, m_rx_channel_compensated_i[k][v].im);
                    float fe = m_rx_channel_compensated_f[k][v].re * m_rx_channel_compensated_f[k][v].re
                        + m_rx_channel_compensated_f[k][v].im * m_rx_channel_compensated_f[k][v].im;
                    //m_plot.data("%f", fe);
                }
            }
            //m_plot.end_plot();
            //getchar();
        }

        nimage++;
        //if (nimage % 20 == 0)
        //{
        //    m_plot.end_plot();
        //}

        //m_plot.command("unset multiplot");

        if (m_rx_symbol_count == 1)
        {
            m_plot.end_plot();
            nimage = 0;
            getchar();
            //Sleep(1000);
        }
#endif
        mimo_demap_16qam();
        _perf_tick();
        mimo_deinterleave<4>(m_rx_demapped, m_rx_deinterleaved);
        _perf_tick();

        m_rx_viterbi_fifo.htvb3.SpaceWait(1, &m_workindicator);
        stream_joiner_buffer = m_rx_viterbi_fifo.htvb3.Push();
        _perf_tick();
        stream_joiner<2>(m_rx_deinterleaved, 208, stream_joiner_buffer);
        _perf_tick();
        m_rx_viterbi_fifo.htvb3.Flush();
        
        break;
    case 13:
    case 14:
    //case 15:  
#if 0
        if (nimage == 0)
        {
            //m_plot.command("set title 'y%05d[%d]'", nimage, k);
            m_plot.command("set xrange [-200:200]");
            m_plot.command("set yrange [-200:200]");
            m_plot.command("set xtics 16");
            m_plot.command("set ytics 16");
            m_plot.command("set grid");
            m_plot.begin_plot("w p 7");
        }
        for (int k = 0; k < 1; k++)
        {
            for (int i = 0; i < 64; i++)
            {
                int v = (32 + i) % 64;
                if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
                //if ( v == 7 || v == 21 || v == 64 - 21 || v == 64 -7)
                {
                    m_plot.data("%d\t%d", m_rx_channel_compensated_i[k][v].re, m_rx_channel_compensated_i[k][v].im);
                    float fe = m_rx_channel_compensated_f[k][v].re * m_rx_channel_compensated_f[k][v].re
                        + m_rx_channel_compensated_f[k][v].im * m_rx_channel_compensated_f[k][v].im;
                    //m_plot.data("%f", fe);
                }
            }
        }
        //m_plot.command("unset multiplot");
        nimage++;
        if (m_rx_symbol_count == 1)
        {
            m_plot.end_plot();
            nimage = 0;
            getchar();
            //Sleep(1000);
        }
#endif
        mimo_demap_64qam();
        _perf_tick();
        mimo_deinterleave<6>(m_rx_demapped, m_rx_deinterleaved);
        _perf_tick();

        m_rx_viterbi_fifo.htvb4.SpaceWait(1, &m_workindicator);
        stream_joiner_buffer = m_rx_viterbi_fifo.htvb4.Push();
        _perf_tick();
        stream_joiner<3>(m_rx_deinterleaved, 312, stream_joiner_buffer);

#if 0
        for (int i = 0; i < 624; i++)
        {
            //printf("%u ", stream_joiner_buffer[i]);
            stream_joiner_buffer[i] = i % 8;
        }
        //printf("\n\n");
        //getchar();
#endif

        _perf_tick();
        m_rx_viterbi_fifo.htvb4.Flush();
        break;
    }

#if 0
    static int nimage = 0;
    //m_plot.command("set terminal gif");
    //m_plot.command("set output '%d.gif'", nimage);
    m_plot.command("set multiplot layout 2, 1");
    for (int k = 0; k < 2; k++)
    {
        //m_plot.command("reset");
        //m_plot.command("set terminal jpeg");
        //m_plot.command("set output 'y%02d_%05d.jpg'", k, nimage);
        m_plot.command("set title 'y%05d[%d]'", nimage, k);
        //m_plot.command("set xrange [-2:2]");
        //m_plot.command("set yrange [-2:2]");
        m_plot.begin_plot("w p 7");

        for (int i = 0; i < 64; i++)
        {
            int v = (32 + i) % 64;
            if ( (v >= 1 && v <= 28) || (v >= 64 - 28 && v <= 63))
            {
                m_plot.data("%d\t%d", m_rx_channel_compensated_i[k][v].re, m_rx_channel_compensated_i[k][v].im);
                float fe = m_rx_channel_compensated_f[k][v].re * m_rx_channel_compensated_f[k][v].re
                    + m_rx_channel_compensated_f[k][v].im * m_rx_channel_compensated_f[k][v].im;
                //m_plot.data("%f", fe);
            }
        }
        m_plot.end_plot();
        //getchar();
    }
    m_plot.command("unset multiplot");
    nimage++;
    if (m_rx_symbol_count == 1)
    {
        nimage = 0;
    }
#endif

    _perf_tick();
    
    m_rx_cyclic_write_idx = 0;
    for (; vidx < sb1->v_datacount; vidx++)
    {
        m_rx_cyclic_buffer[0][m_rx_cyclic_write_idx] = sb1->v_data[vidx];
        m_rx_cyclic_buffer[1][m_rx_cyclic_write_idx] = sb2->v_data[vidx];
        m_rx_cyclic_write_idx++;
    }
    m_rx_current_sb_idx = ++m_rxstream;
    m_rx_symbol_count--;
    //printf("totalsymbol=%d\n", m_rx_symbol_count);

    _perf_tick();
    _perf_end();

#if draw_enabled
    m_draw[draw_f1]->DrawSqrt(&m_rx_channel_compensated_i[0][0], 64);
    m_draw[draw_t1]->DrawScatter(&m_rx_channel_compensated_i[0][0], 64);
    m_draw[draw_f2]->DrawSqrt(&m_rx_channel_compensated_i[1][0], 64);
    m_draw[draw_t2]->DrawScatter(&m_rx_channel_compensated_i[1][0], 64);
#endif

    // zero-padding for viterbi traceback
    if (m_rx_symbol_count == 0)
    {
        switch (m_rx_mcs)
        {
        case 8:
            m_rx_viterbi_fifo.htvb1.SpaceWait(1, &m_workindicator);
            stream_joiner_buffer = m_rx_viterbi_fifo.htvb1.Push();
            memset(stream_joiner_buffer, 0, 104);
            m_rx_viterbi_fifo.htvb1.Flush();
            break;
        case 9:
        case 10:
            m_rx_viterbi_fifo.htvb2.SpaceWait(1, &m_workindicator);
            stream_joiner_buffer = m_rx_viterbi_fifo.htvb2.Push();
            memset(stream_joiner_buffer, 0, 208);
            m_rx_viterbi_fifo.htvb2.Flush();
            break;
        case 11:
        case 12:
            m_rx_viterbi_fifo.htvb3.SpaceWait(1, &m_workindicator);
            stream_joiner_buffer = m_rx_viterbi_fifo.htvb3.Push();
            memset(stream_joiner_buffer, 0, 416);
            m_rx_viterbi_fifo.htvb3.Flush();
            break;
        case 13:
        case 14:
        //case 15:
            m_rx_viterbi_fifo.htvb4.SpaceWait(1, &m_workindicator);
            stream_joiner_buffer = m_rx_viterbi_fifo.htvb4.Push();
            memset(stream_joiner_buffer, 0, 624);
            m_rx_viterbi_fifo.htvb4.Flush();
            break;
        }

        while (m_viterbi_status != job_done)
        {
            if (!(m_workindicator))
            {
                break;
            }
            _mm_pause();
        }
#if 0
        for (int i = 0; i < m_rx_frame_length; i++)
        {
            if (i % 16 == 0)
            {
                printf("\n");
            }
            printf("%02X ", m_rx_decoded_frame[i]);
        }
        printf("\n");
#endif
        return true;
    }
    else
    {
        return false;
    }
}

void dot11n_phy::start_viterbi()
{
    m_viterbi_stop     = false;
    m_viterbi_status   = job_waiting;
    m_viterbi_affinity = 0x4;

    HANDLE hViterbi = CreateThread(NULL, 0, dot11n_phy::viterbi_thread, this, 0, NULL);
    SetThreadPriority(hViterbi, THREAD_PRIORITY_TIME_CRITICAL);
    CloseHandle(hViterbi);
}

void dot11n_phy::stop_viterbi()
{
    m_viterbi_stop = true;
}

void dot11n_phy::viterbi_worker()
{
    int nOutputBytes;
    int nTotalBytes;
    int nCRC32Pos;
    unsigned int crc32_local;
    int nOutIdx;

    auto vit_call_back = [&](unsigned char* pviterbi_output, int count)
    {
        for (int i = 0; i < count; i++)
        {
            if (nOutputBytes < 2)
            {
                m_rx_descrambler.reset(*(pviterbi_output + 1));
                nOutputBytes += 2;
                i            += 2;
            }

            m_rx_decoded_frame[nOutIdx] = m_rx_descrambler(pviterbi_output[i]);

            m_rx_crc32(m_rx_decoded_frame[nOutIdx]);

            //printf("%d: %x -- %x\n", nOutputBytes, pbOutput[vidx], m_rx_decoded_frame[nOutIdx]);

            nOutputBytes++;

            if (nOutputBytes == nCRC32Pos)
            {
                crc32_local = m_rx_crc32.value();
            }

            nOutIdx++;
        }
    };

    tick_count tbegin, tend;

    while (!m_viterbi_stop)
    {
        while (m_viterbi_status != job_running && !m_viterbi_stop)
        {
            _mm_pause();
        }
        
        if (m_viterbi_stop) break;

        
        m_rx_crc32.reset();
#if dot11a_mode
        nOutputBytes                  = 0;
        nCRC32Pos                     = m_rx_l_length + 2 - 4;
        nTotalBytes                   = m_rx_l_length + 2;
        nOutIdx                       = 0;
        m_rx_viterbi_param.nTotalBits = m_rx_l_length * 8 + 16 + 6;

        // 64-QAM
        if (m_rx_l_rate == DOT11A_RATE_54M)
        {
            m_rxviterbi.viterbi_blk<34, 7, 36, 216, 288, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.lvb4, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
#else // dot11n mode
        nOutputBytes                  = 0;
        nCRC32Pos                     = m_rx_frame_length + 2 - 4;
        nTotalBytes                   = m_rx_frame_length + 2;
        nOutIdx                       = 0;
        m_rx_viterbi_param.nTotalBits = m_rx_frame_length * 8 + 16 + 6;

        // BPSK
        if (m_rx_mcs == 8)
        {
            m_rxviterbi.viterbi_blk<12, 7, 36, 48, 104, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb1, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
        // QPSK
        else if (m_rx_mcs == 9)
        {
            m_rxviterbi.viterbi_blk<12, 7, 36, 48, 208, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb2, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
        else if (m_rx_mcs == 10)
        {
            m_rxviterbi.viterbi_blk<34, 3, 72, 72, 208, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb2, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
        // 16-QAM
        else if (m_rx_mcs == 11)
        {
            m_rxviterbi.viterbi_blk<12, 7, 36, 96, 416, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb3, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
        else if (m_rx_mcs == 12)
        {
            m_rxviterbi.viterbi_blk<34, 3, 72, 192, 416, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb3, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
        // 64-QAM
        else if (m_rx_mcs == 13)
        {
            m_rxviterbi.viterbi_blk<23, 7, 96, 256, 624, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb4, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
        else if (m_rx_mcs == 14)
        {
            m_rxviterbi.viterbi_blk<34, 3, 128, 256, 624, 128>(m_rx_viterbi_param, (vub*)m_rx_viterbi_trellis.vtrellis,
                m_rx_viterbi_fifo.htvb4, m_rx_decoded_frame, m_viterbi_stop, vit_call_back);
        }
#endif

#if 0
        m_rx_crc32_correct = (crc32_local ^ *((unsigned int *)&m_rx_decoded_frame[m_rx_frame_length - 4]));
#else
        if (crc32_local == *((unsigned int *)&m_rx_decoded_frame[m_rx_frame_length - 4]))
        {
            //printf("\nCRC32 correct! %p::%p\n", crc32_local, *((unsigned int *)&m_rx_decoded_frame[m_rx_frame_length - 4]));
            m_rx_crc32_correct = true;
        }
        else
        {
            //printf("\nCRC32 incorrect! %p::%p\n", crc32_local, *((unsigned int *)&m_rx_decoded_frame[m_rx_frame_length - 4]));
            m_rx_crc32_correct = false;
        }
#endif
        m_viterbi_status = job_done;
    }
}

DWORD WINAPI dot11n_phy::viterbi_thread(LPVOID lpdwThreadParam)
{
    dot11n_phy* me = (dot11n_phy*)lpdwThreadParam;
    SetThreadAffinityMask(GetCurrentThread(), me->m_viterbi_affinity);
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);

    me->viterbi_worker();

    return 0;
}