#include "dsp_mapper.h"

namespace OpenDSP
{
    namespace demapper
    {
        const v_cs::type dsp_demapper::DemapMin      = {complex16(-128, -128), complex16(-128, -128), complex16(-128, -128), complex16(-128, -128)};
        const v_cs::type dsp_demapper::DemapMax      = {complex16(127, 127), complex16(127, 127), complex16(127, 127), complex16(127, 127)};
        const v_cs::type dsp_demapper::Demap64qamMin = {complex16(-144, -144), complex16(-144, -144), complex16(-144, -144), complex16(-144, -144)};
        const v_cs::type dsp_demapper::Demap64qamMax = {complex16(143, 143), complex16(143, 143), complex16(143, 143), complex16(143, 143)};


#if USE_SIGNED_RANGE
        const __int8 *dsp_demapper::p_lookup_table_64qam1 = &lookup_table_64qam1[144];
        const __int8 *dsp_demapper::p_lookup_table_64qam2 = &lookup_table_64qam2[144];
        const __int8 *dsp_demapper::p_lookup_table_64qam3 = &lookup_table_64qam3[144];


        // soft range : -4 ~~ 3
        const __int8 dsp_demapper::lookup_table_bpsk[lut_bpsk_size] =
        {
            0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
            0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
            1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
            1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
            2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
            2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
            3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,
            3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,

            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,
            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,
            -3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,
            -3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,
            -2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,
            -2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,
            -1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
            -1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1
        };


        const __int8 dsp_demapper::lookup_table_qpsk[lut_qpsk_size] =
        {
            0,	0,  0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
            0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
            1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	2,	2,	2,
            2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
            2,	2,	2,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,
            3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,
            3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,
            3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,

            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,
            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,
            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,
            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-3,	-3,	-3,
            -3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,
            -3,	-3,	-3,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,
            -2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-1,	-1,	-1,	-1,	-1,	-1,
            -1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1
        };

        const __int8 dsp_demapper::lookup_table_16qam1[lut_16qam_size] =
        {
            0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
            0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
            0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,
            1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
            1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,
            1,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,	2,
            2,	2,	2,	2,	2,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,
            3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,	3,

            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,
            -4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-4,	-3,	-3,	-3,	-3,
            -3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,	-3,
            -2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,
            -2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,
            -2,	-2,	-2,	-2,	-2,	-2,	-2,	-2,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
            -1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,
            -1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1
        };


        const __int8 dsp_demapper::lookup_table_16qam2[lut_16qam_size] =
        {
            3, 3, 3,3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 2, 2,2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 1, 1,1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0,0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -2, -2, -2, -2, -2, -2,-2, -2, -2, -2, -2, -2, -2, -2, -2,
            -2, -3, -3, -3, -3, -3, -3,-3, -3, -3, -3, -3, -3, -3, -3, -3,
            -3, -4, -4, -4, -4, -4, -4,-4, -4, -4, -4, -4, -4, -4, -4, -4,

            -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,
            -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
            -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
            3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
        };

        const __int8 dsp_demapper::lookup_table_64qam1[lut_64qam_size] =
        {
            -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,
            -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,
            -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
            -3,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
            -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
            -2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,

            -1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
            1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,
            2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
            2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,
            3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
        };

        const __int8 dsp_demapper::lookup_table_64qam2[lut_64qam_size] =
        {
            -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,
            -4,-4,-4,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,
            -3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
            -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,
            1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,
            2,2,2,2,3,3,3,3,3,3,3,3,3,3,3,3,

            3,3,3,3,3,3,3,3,3,3,3,3,3,2,2,2,
            2,2,2,2,2,2,2,2,2,1,1,1,1,1,1,1,
            1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,
            0,0,0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
            -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-3,-3,-3,
            -3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-3,-4,-4,
            -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-4
        };

        const __int8 dsp_demapper::lookup_table_64qam3[lut_64qam_size] =
        {
            -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,
            0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,
            2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,
            3,4,3,3,3,3,3,3,3,3,3,2,2,2,2,2,
            2,2,2,2,2,1,1,1,1,1,1,1,1,1,1,0,
            0,0,0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-3,-3,-3,
            -3,-3,-3,-3,-3,-3,-3,-4,-4,-4,-4,-4,-4,-4,-4,-4,

            -4,-4,-4,-4,-4,-4,-4,-4,-4,-4,-3,-3,-3,-3,-3,-3,
            -3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,0,0,0,0,0,0,0,0,
            0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,
            2,2,2,2,2,2,3,3,3,3,3,3,3,3,3,4,
            3,3,3,3,3,3,3,3,3,2,2,2,2,2,2,2,
            2,2,2,1,1,1,1,1,1,1,1,1,1,0,0,0,
            0,0,0,0,0,0,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2

        };
#else // soft range : 0 ~~ 7
        const unsigned __int8 *dsp_demapper::p_lookup_table_64qam1 = &lookup_table_64qam1[144];
        const unsigned __int8 *dsp_demapper::p_lookup_table_64qam2 = &lookup_table_64qam2[144];
        const unsigned __int8 *dsp_demapper::p_lookup_table_64qam3 = &lookup_table_64qam3[144];
        
        // How to generate LLR?
        // Ref: http://home.netcom.com/~chip.f/viterbi/ccode/sdvd.html
        // This LUT is constructed at Eb/N0 about 4dB
        const unsigned __int8 dsp_demapper::lookup_table_bpsk[lut_bpsk_size] =
        {
#if 1
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 5,  5, 5, 5, 5,
            5, 5, 5, 5,  5, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 2,  2, 2, 2, 2,
            2, 2, 2, 2,  2, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,
#else
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
            5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
            6, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 6,
            6, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 6,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
            1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
            2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,
            2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,
            3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,
            3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3
#endif
        };

        const unsigned __int8 dsp_demapper::lookup_table_qpsk[lut_qpsk_size] =
        {
#if 0
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 5,  5, 5, 5, 5,
            5, 5, 5, 5,  5, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 2,  2, 2, 2, 2,
            2, 2, 2, 2,  2, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,
#else
            4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
            4, 4, 4, 4,  4, 4, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,
            5, 5, 5, 5,  5, 5, 5, 5,  5, 5, 5, 5,  5, 6, 6, 6,
            6, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 6,  6, 6, 6, 6,
            6, 6, 6, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 1, 1,
            1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,  1, 1, 1, 1,
            1, 1, 1, 2,  2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 2, 2,
            2, 2, 2, 2,  2, 2, 2, 2,  2, 2, 3, 3,  3, 3, 3, 3,
            3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3,  3, 3, 3, 3
#endif
        };

        const unsigned __int8 dsp_demapper::lookup_table_16qam1[lut_16qam_size] =
        {
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            5, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3
        };

        const unsigned __int8 dsp_demapper::lookup_table_16qam2[lut_16qam_size] =
        {
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,

            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
        };
#if 1
        const unsigned __int8 dsp_demapper::lookup_table_64qam1[lut_64qam_size] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,

            3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
        };

        const unsigned __int8 dsp_demapper::lookup_table_64qam2[lut_64qam_size] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,

            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5,
            5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
        };
        const unsigned __int8 dsp_demapper::lookup_table_64qam3[lut_64qam_size] =
        {
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4,
            4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6,
            6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7,
            7, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3,
            3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1,
            1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,

            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
            1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
            3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
            4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6,
            6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 8,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6,
            6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4,
            4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
        };
#else

        //added by v-dohe
        const unsigned __int8 dsp_demapper::lookup_table_64qam1[lut_64qam_size] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 
            4, 4, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,   
        };

        //updated by v-dohe
        const unsigned __int8 dsp_demapper::lookup_table_64qam2[lut_64qam_size] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 
            2, 3, 3, 4, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 4, 3, 3, 
            2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        };

        //updated by v-dohe
        const unsigned __int8 dsp_demapper::lookup_table_64qam3[lut_64qam_size] =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 
            2, 2, 3, 3, 4, 4, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 
            4, 4, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 4, 
            4, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
            7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 4, 4, 3, 3, 2, 
            2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
        };
#endif
#endif



#if 0 // old demapper lookup table
        const unsigned __int8 dsp_demapper::lookup_table_bpsk[lut_bpsk_size] =
        {
            // positive
            4, 5, 6, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            // negative
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 1, 2, 3,
        };

        const unsigned __int8 dsp_demapper::lookup_table_16qam2[lut_16qam_size] =
        {
            // positive
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  6, 5, 4, 3,

            2, 1, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

            // negative
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 1, 2,

            3, 4, 5, 6,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
        };

        const unsigned __int8 dsp_demapper::lookup_table_64qam2[lut_64qam_size] =
        {
            // positive
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 6,  5, 4, 3, 2,

            1, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

            // negative
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 1,

            2, 3, 4, 5,  6, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
        };

        const unsigned __int8 dsp_demapper::lookup_table_64qam3[lut_64qam_size] =
        {
            // positive
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  1, 2, 3, 4,
            5, 6, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 6, 5, 4,  3, 2, 1, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,

            // negative
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 1, 2, 3,  4, 5, 6, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,

            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,
            7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 7, 7,  7, 7, 6, 5,
            4, 3, 2, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
            0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
        };
#endif
    }
}