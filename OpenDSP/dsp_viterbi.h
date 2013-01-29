#pragma once

#include "stdio.h"
#include "_vector128.h"
#include "dsp_fifo.h"
#include "dsp_vector1.h"
#include "dsp_tickcount.h"
#include "dsp_task.h"

namespace OpenDSP
{
    struct dsp_viterbi
    {
        // We use 3 bit SoftBit
#define SOFT_RANGE	    8

        // The path metric range
#define METRIC_SIZE     128
#define METRIC_OFFSET   7

#define NOR_MASK_VITAS  0x7
#define BYTE_MAX_VITAS  6
#define BIT_MAX_VITAS   (BYTE_MAX_VITAS * 8)

        // 6 leading bit
#define TB_PREFIX_VITAS  6          // must be 6. FJ: I guess this is by design trick. The real data start from the 7th
#define TB_DEPTH_VITAS   12         // must be multiple of 1
#define TB_OUTPUT_VITAS  (24)       // must be multiple of 8 and factor of 192
#define A16 __declspec(align(16))

#define SPACE_SIZE 64
#define INPUT_MAX_VITAS 96
#define BITS_PER_BYTE 8

        enum
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

        static const vub::data_type ALL_INVERSE_ONE;
        static const vub::data_type ALL_ONE;
        static const vub::data_type ALL_M128;
        static const vub::data_type VIT_MA[8 * SOFT_RANGE];
        static const vub::data_type VIT_MB[8 * SOFT_RANGE];

        static A16 const vub::data_type ALL_INIT;
        static A16 const vub::data_type ALL_INIT0;
        static A16 const vub::data_type INDEXES [4];

        static const int nFlushBufferSize = 128;
        unsigned char FlushBuffer[nFlushBufferSize];
        dsp_viterbi()
        {
            memset(FlushBuffer, 0, 128);
        }

        template<int nbits, int ntracebackdepth>
        struct Trellis 
        {
            __m128i vtrellis[ SPACE_SIZE / 16 * ( nbits + ntracebackdepth + TB_PREFIX_VITAS ) ];
        };
        
        typedef struct _input_param
        {
            int nTotalBits;
            int nTraceBackDepth;
            int nTraceBackNormMask;
            int nTraceBackOutputBits;
            int nDecodedBits;
            int nCodingRate;
            vub* pTrellis;
            int iTrellis;
        }input_param;

#if 0
        inline void v_print(vub &v)
        {
            for (int i = 0; i < 16; i++)
            {
                fprintf(stdout, "%u ", v[i]);
            }
            fprintf(stdout, "\n");
        }
#else
        inline void v_print(vub &v){}
#endif


        __forceinline void viterbi12_seq(input_param& inputparm, vub *pvTrellis, char * pbInput, char * pbOutput)
        {
            int i, j;
            int ocnt = 0; // Counter for the output bits

            // Vector1 const used
            vub ALLONE    (ALL_ONE);
            vub ALLINVONE (ALL_INVERSE_ONE );
            vub ALLINE (ALL_M128);  

            A16 unsigned char outchar = 0;    // the output(decoded) char

            int i_trellis = 0;    // index of trellis

            // for trace back  
            A16 vub * pTraceBk;       // trace back pointer in trellis
            unsigned char i_minpos = 0;     // the minimal path position
            unsigned char i_tpos   = 0;

            char * psbit = pbInput;     // pointer to the s-bit stream;

            // temporal variables
            A16 vub rub0, rub1, rub2, rub3;
            A16 vus rus0, rus1, rus2, rus3;
            A16 vus rus4, rus5, rus6, rus7;

            unsigned int i_ma = 0; // index to the Branch Metric LUT table
            unsigned int i_mb = 0;

            pvTrellis[0] = ALL_INIT0; 
            pvTrellis[1] = ALL_INIT; 
            pvTrellis[2] = ALL_INIT; 
            pvTrellis[3] = ALL_INIT;

            while (ocnt < inputparm.nTotalBits) 
            {
                // Compute the branch metric
                i_ma = (unsigned int)(unsigned char)(* psbit    ) << 3;
                i_mb = (unsigned int)(unsigned char)(* (psbit+1)) << 3;

                // 1/2 coding, consume 2 soft bits one time
                psbit += 2;

                // Compute the new states
                rub0 = interleave_low (pvTrellis[0], pvTrellis[0]);
                rub1 = interleave_low (pvTrellis[2], pvTrellis[2]);

                // branch 0
                rub0 = add ( rub0, VIT_MA[i_ma] );
                rub0 = add ( rub0, VIT_MB[i_mb] );
                rub0 = and ( rub0, ALLINVONE); // mark the path

                // branch 1
                rub1 = add ( rub1, VIT_MA[i_ma+1] );
                rub1 = add ( rub1, VIT_MB[i_mb+1] );
                rub1 = or  ( rub1, ALLONE );

                // store the shortest path, state:[0-15]
                pvTrellis[4] = smin (rub0, rub1); 

                rub0 = interleave_high (pvTrellis[0], pvTrellis[0]);
                rub1 = interleave_high (pvTrellis[2], pvTrellis[2]);

                // branch 0
                rub0 = add ( rub0, VIT_MA[i_ma+2] );
                rub0 = add ( rub0, VIT_MB[i_mb+2] );  
                rub0 = and ( rub0, ALLINVONE); // mark the path

                // branch 1
                rub1 = add ( rub1, VIT_MA[i_ma+3] );
                rub1 = add ( rub1, VIT_MB[i_mb+3] );
                rub1 = or  ( rub1, ALLONE );

                // store the shortest path, state:[16-31] 
                pvTrellis[5] = smin (rub0, rub1);

                rub0 = interleave_low (pvTrellis[1], pvTrellis[1]);
                rub1 = interleave_low (pvTrellis[3], pvTrellis[3]);

                // branch 0
                rub0 = add ( rub0, VIT_MA[i_ma+4] );
                rub0 = add ( rub0, VIT_MB[i_mb+4] );
                rub0 = and ( rub0, ALLINVONE); // mark the path

                // branch 1
                rub1 = add ( rub1, VIT_MA[i_ma+5] );
                rub1 = add ( rub1, VIT_MB[i_mb+5] );
                rub1 = or  ( rub1, ALLONE );

                // store the shortest path, state:[32-47]    
                pvTrellis[6] = smin (rub0, rub1); 

                rub0 = interleave_high (pvTrellis[1], pvTrellis[1]);
                rub1 = interleave_high (pvTrellis[3], pvTrellis[3]);

                // branch 0
                rub0 = add ( rub0, VIT_MA[i_ma+6] );
                rub0 = add ( rub0, VIT_MB[i_mb+6] );  
                rub0 = and ( rub0, ALLINVONE); // mark the path

                // branch 1
                rub1 = add ( rub1, VIT_MA[i_ma+7] );
                rub1 = add ( rub1, VIT_MB[i_mb+7] );
                rub1 = or  ( rub1, ALLONE );

                // store the shortest path, state:[48-63]
                pvTrellis[7] = smin (rub0, rub1);

                // Move to next state
                pvTrellis += 4;
                i_trellis ++;

                // Normalize
                if ((i_trellis & 7) == 0 ) 
                {
                    // normalization
                    // find the smallest component and extract it from all states
                    rub0 = smin (pvTrellis[0], pvTrellis[1] );
                    rub1 = smin (pvTrellis[2], pvTrellis[3] );
                    rub2 = smin (rub0, rub1);
                    rub3 = hmin (rub2);

                    // make sure to clear the marker bit
                    rub3 = and  (rub3, ALLINVONE );

                    // normalize
                    pvTrellis[0] = sub ( pvTrellis[0], rub3);
                    pvTrellis[1] = sub ( pvTrellis[1], rub3);
                    pvTrellis[2] = sub ( pvTrellis[2], rub3);
                    pvTrellis[3] = sub ( pvTrellis[3], rub3);
                }

                // Traceback 
                // We should first skip TB_DEPTH_VITAS bits and 
                // TB_OUTPUT_VITAS is actual bits we can output
                //if ( i_trellis >= TB_DEPTH_VITAS + TB_OUTPUT_VITAS + TB_PREFIX_VITAS ) {
                if ( i_trellis >= inputparm.nTraceBackDepth + inputparm.nTotalBits + TB_PREFIX_VITAS ) 
                {
                    // track back
                    // we need to find the minimal state and index of the state
                    // do normalization first
                    rub0 = smin (pvTrellis[0], pvTrellis[1] );
                    rub1 = smin (pvTrellis[2], pvTrellis[3] );
                    rub2 = smin (rub0, rub1);

                    rub3 = hmin (rub2);
                    rub3 = and (rub3, ALLINVONE );

                    // normalize
                    pvTrellis[0] = sub ( pvTrellis[0], rub3);
                    pvTrellis[1] = sub ( pvTrellis[1], rub3);
                    pvTrellis[2] = sub ( pvTrellis[2], rub3);
                    pvTrellis[3] = sub ( pvTrellis[3], rub3);
                    // rub3 has the minimal value, we need to find the index
                    // the algorithm to find the right index is to embed the index at the least
                    // significant bits of state value, then we just find the minimal value

                    // ensure to use pminsw - not needed
                    rub0 = INDEXES[0];
                    rub1 = pvTrellis[0];

                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus4 = smin ( rus2, rus3);

                    rub0 = INDEXES[1];
                    rub1 = pvTrellis[1];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );

                    rus5 = smin (rus2, rus3);
                    rus4 = smin (rus4, rus5);

                    rub0 = INDEXES[2];
                    rub1 = pvTrellis[2];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );

                    rus6 = smin (rus2, rus3);
                    rus4 = smin (rus4, rus6);

                    rub0 = INDEXES[3];
                    rub1 = pvTrellis[3];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );

                    rus7 = smin (rus2, rus3);
                    rus4 = smin (rus4, rus7);

                    // now rus4 contains the minimal 8 
                    rus0 = hmin (rus4);

                    // now the first word contains the index and value
                    // index: bit [7:2]; 
                    // value: bit [15:8]
                    i_minpos = (unsigned char)extract<0>(rus0);

                    // now we can trace back ...
                    pTraceBk = pvTrellis;

                    // first part - trace back without output
                    i_minpos /= 2; // index 5:0
                    for ( i = 0; i < inputparm.nTraceBackDepth; i++)
                    {
                        pTraceBk -= 4;
                        //printf("TB MinPos=%d\n", i_minpos);

                        i_minpos = (i_minpos >> 1) & 0x3F;
                        i_tpos = ((char*) pTraceBk)[i_minpos];
                        i_minpos |= (i_tpos & 1) << 6;  // now i_minpos 6:0 is the new index
                    }

                    // second part - trace back with output
                    //pbOutput += (TB_OUTPUT_VITAS >> 3);
                    pbOutput += (inputparm.nTotalBits >> 3);

                    //for ( i = 0; i < TB_OUTPUT_VITAS >> 3; i++) {
                    for ( i = 0; i < inputparm.nTotalBits >> 3; i++) 
                    {
                        for ( j = 0; j < 8; j++ ) 
                        {
                            outchar = outchar << 1;
                            outchar |= (i_minpos >> 6) & 1;
                            ocnt ++;

                            // next bit
                            pTraceBk -= 4;
                            //printf("TB MinPos=%d\n", i_minpos);
                            i_minpos = (i_minpos >> 1) & 0x3F;
                            i_tpos = ((char*) pTraceBk)[i_minpos] ;
                            i_minpos |= (i_tpos & 1) << 6;  // now i_minpos 6:0 is the new index
                        }
                        // output the decoded bytes inversely
                        pbOutput --;
                        *pbOutput = outchar;
                        outchar = 0;
                    }
                }
            }
        }

        __forceinline void ViterbiAdvance(vub *&pTrellis, const vub pVITMA[], int i_ma, const vub pVITMB[], int i_mb)
        {
            const vub ALLONE    (ALL_ONE);
            const vub ALLINVONE (ALL_INVERSE_ONE);
            const vub ALLM128   (ALL_M128);

            // Index to the Branch Metric LUT table
            i_ma <<= 3;
            i_mb <<= 3;

            // temporal variables
            vub rub0, rub1, rub2, rub3;

            //for (int i = 0; i < 4; i++)
            //    v_print(pTrellis[i]);
            //printf("\n");

            // Compute the new states
            rub0 = interleave_low (pTrellis[0], pTrellis[0]);
            rub1 = interleave_low (pTrellis[2], pTrellis[2]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma] );
            rub0 = add ( rub0, pVITMB[i_mb] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+1] );
            rub1 = add ( rub1, pVITMB[i_mb+1] ); 
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[0-15]
            pTrellis[4] = smin (rub0, rub1); 
            v_print(shift_right(pTrellis[4], 1));

            rub0 = interleave_high (pTrellis[0], pTrellis[0]);
            rub1 = interleave_high (pTrellis[2], pTrellis[2]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma+2] );
            rub0 = add ( rub0, pVITMB[i_mb+2] );  
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+3] );
            rub1 = add ( rub1, pVITMB[i_mb+3] ); 
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[16-31]    
            pTrellis[5] = smin (rub0, rub1); 
            v_print(shift_right(pTrellis[5], 1));

            rub0 = interleave_low (pTrellis[1], pTrellis[1]);
            rub1 = interleave_low (pTrellis[3], pTrellis[3]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma+4] );
            rub0 = add ( rub0, pVITMB[i_mb+4] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+5] );
            rub1 = add ( rub1, pVITMB[i_mb+5] );
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[32-47]    
            pTrellis[6] = smin (rub0, rub1);
            v_print(shift_right(pTrellis[6], 1));

            rub0 = interleave_high (pTrellis[1], pTrellis[1]);
            rub1 = interleave_high (pTrellis[3], pTrellis[3]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma+6] );
            rub0 = add ( rub0, pVITMB[i_mb+6] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+7] );
            rub1 = add ( rub1, pVITMB[i_mb+7] );
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[48-63]        
            pTrellis[7] = smin (rub0, rub1); 
            v_print(shift_right(pTrellis[7], 1));

            pTrellis += 4;
        }

        __forceinline void ViterbiAdvance(vub *&pTrellis, const vub pVITMA[], int i_ma)
        {
            const vub ALLONE    (ALL_ONE);
            const vub ALLINVONE (ALL_INVERSE_ONE);
            const vub ALLM128   (ALL_M128);

            // Index to the Branch Metric LUT table
            i_ma <<= 3;

            //for (int i = 0; i < 4; i++)
            //    v_print(pTrellis[i]);
            //printf("\n");

            // temporal variables
            vub rub0, rub1, rub2, rub3;

            // Compute the new states
            rub0 = interleave_low (pTrellis[0], pTrellis[0]);
            rub1 = interleave_low (pTrellis[2], pTrellis[2]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+1] );
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[0-15]
            pTrellis[4] = smin (rub0, rub1); 
            v_print(shift_right(pTrellis[4], 1));

            rub0 = interleave_high (pTrellis[0], pTrellis[0]);
            rub1 = interleave_high (pTrellis[2], pTrellis[2]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma+2] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+3] );
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[16-31]    
            pTrellis[5] = smin (rub0, rub1); 
            v_print(shift_right(pTrellis[5], 1));

            rub0 = interleave_low (pTrellis[1], pTrellis[1]);
            rub1 = interleave_low (pTrellis[3], pTrellis[3]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma+4] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+5] );
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[32-47]    
            pTrellis[6] = smin (rub0, rub1); 
            v_print(shift_right(pTrellis[6], 1));

            rub0 = interleave_high (pTrellis[1], pTrellis[1]);
            rub1 = interleave_high (pTrellis[3], pTrellis[3]);

            // branch 0
            rub0 = add ( rub0, pVITMA[i_ma+6] );
            rub0 = and ( rub0, ALLINVONE); // mark the path

            // branch 1
            rub1 = add ( rub1, pVITMA[i_ma+7] );
            rub1 = or  ( rub1, ALLONE );

            // store the shortest path, state:[48-63] 
            pTrellis[7] = smin (rub0, rub1);
            v_print(shift_right(pTrellis[7], 1));

            pTrellis += 4;
        }

        template<int mbitrate, int NOR_MASK_, int TB_DEPTH_, int TB_OUTPUT_, size_t VB_DCSIZE, size_t VB_DCCOUNT, typename fncallback>
        inline void viterbi_blk(input_param& inputparam, vub* pTrellis, VBUFFER::VB<VB_DCSIZE, VB_DCCOUNT>& vb, unsigned char * pbOutput, volatile unsigned int& stop, fncallback& fcb)
        {
            const size_t BlockPadding = VBUFFER::VB<VB_DCSIZE, VB_DCCOUNT>::VB_DCBLOCK::BlockPadding;
            const size_t BlockOccupying = VBUFFER::VB<VB_DCSIZE, VB_DCCOUNT>::VB_DCBLOCK::BlockOccupying;

            const int TB_PREFIX        = 6;
            unsigned char * pbVitOutput = pbOutput;
            unsigned int uiBytesOutput = 0;
            char* const blkStart       = (char*)vb.BlocksBegin();
            char* const blkEnd         = (char*)vb.BlocksEnd();

            char* blkCurrent;
            char * blkInput = blkStart;

            vub* pTrellisBase = pTrellis;
            
            int i, j;

            int ocnt = 0; // Counter for the output bits

            int nTotalBits = inputparam.nTotalBits;

            // vector128 constants
            const vub * const pVITMA = (const vub*) VIT_MA; // Branch Metric A
            const vub * const pVITMB = (const vub*) VIT_MB; // Branch Metric B

            const vub ALLINVONE (ALL_INVERSE_ONE);

            unsigned char outchar = 0;    // the output(decoded) char
            unsigned char * pVTOutput;

            //vub * pTrellis;       // point to trellis
            int i_trellis = 0;    // index of trellis

            // for trace back  
            vub * pTraceBk;       // trace back pointer in trellis
            int i_minpos = 0;     // the minimal path position
            int i_tpos   = 0;

            // temporal variables
            vub rub0, rub1, rub2, rub3;
            vus rus0, rus1, rus2, rus3;
            vus rus4, rus5, rus6, rus7;

            //unsigned char i_crc;

            // Initialize Trellis
            //pTrellis = (vub*) g_Values;
            pTrellis[0] = vub(ALL_INIT0);
            pTrellis[1] = vub(ALL_INIT);
            pTrellis[2] = vub(ALL_INIT);
            pTrellis[3] = vub(ALL_INIT);

            while (*blkInput == 0 && stop == 0)
                _mm_pause();

            blkCurrent = blkStart;

            if (stop) return;
            blkInput += 4; // jump to data

            // viterbi decode and descramble here
            // continue to process all input bits
            //while (uiBitsTotal)
            while (nTotalBits > 0)
            {
                //if (mbitrate == 6 || mbitrate == 24 || mbitrate == 12)
                if (mbitrate == 12)
                {
                    ViterbiAdvance(pTrellis, pVITMA, blkInput[0], pVITMB, blkInput[1]);
                    blkInput += 2; // jump to data
                    i_trellis++;
                }
                //else if (mbitrate == 9 || mbitrate == 18 || mbitrate == 36 || mbitrate == 54)
                else if (mbitrate == 34)
                {
                    ViterbiAdvance(pTrellis, pVITMA, blkInput[0], pVITMB, blkInput[1]);
                    ViterbiAdvance(pTrellis, pVITMA, blkInput[2]);
                    ViterbiAdvance(pTrellis, pVITMB, blkInput[3]);
                    blkInput += 4;
                    i_trellis += 3;
                }
                //else if (mbitrate == 48)
                else if (mbitrate == 23)
                {
                    ViterbiAdvance(pTrellis, pVITMA, blkInput[0], pVITMB, blkInput[1]);
                    ViterbiAdvance(pTrellis, pVITMA, blkInput[2]);
                    blkInput  += 3;
                    i_trellis += 2;
                }

                // Dump trellis
                //DUMP_TRELLIS (pTrellis);
                // Normalize
#if 1
                if ((i_trellis & NOR_MASK_) == 0 )
                {
                    // normalization
                    // find the smallest component and extract it from all states
                    rub0 = smin (pTrellis[0], pTrellis[1] );
                    rub1 = smin (pTrellis[2], pTrellis[3] );
                    rub2 = smin (rub0, rub1);

                    rub3 = hmin (rub2);

                    // make sure to clear the marker bit
                    rub3 = and  (rub3, ALLINVONE );

                    // normalize
                    pTrellis[0] = sub ( pTrellis[0], rub3);
                    pTrellis[1] = sub ( pTrellis[1], rub3);
                    pTrellis[2] = sub ( pTrellis[2], rub3);
                    pTrellis[3] = sub ( pTrellis[3], rub3);
                }
#else
                if (pTrellis[0][0] > 192)
                {
                    vub vnorm;
                    for (int i = 0; i < 16; i++)
                    {
                        vnorm[i] = 128;
                    }
                    pTrellis[0] = sub ( pTrellis[0], vnorm);
                    pTrellis[1] = sub ( pTrellis[1], vnorm);
                    pTrellis[2] = sub ( pTrellis[2], vnorm);
                    pTrellis[3] = sub ( pTrellis[3], vnorm);
                }
#endif

                // here I need also check if we need to load a new descriptor
                //if (NeedNewDescriptor<mbitrate>(i_trellis))
                if ((size_t)(blkInput - blkCurrent) == BlockOccupying)
                {
                    // read a new descriptor
                    * (blkInput - BlockOccupying) = 0;
                    blkInput += BlockPadding; // hacking to be 64 aligned

                    if ( blkInput == blkEnd ) blkInput = blkStart;

                    blkCurrent = blkInput;

                    // wait ready
                    while ( * blkInput == 0 && stop == 0 )
                    {
                        //printf("Waiting new block %p\nSoftBits", blkInput);
                        _mm_pause();
                    }

                    static int blkcnt = 0;
                    //printf("vit blkcnt = %d,  bitsleft=%d\nSoftBits", ++blkcnt, inputparam.nTotalBits);

                    //printf("pop %p, %d\nSoftBits", blkInput, ++blkcnt);

                    if (stop) break;
                    blkInput += 4; // jump to data
                }


                // Traceback 
                // We should first skip TB_DEPTH_VITAS bits and 
                // TB_OUTPUT_VITAS is acutal bits we can output
                if ( i_trellis >= TB_DEPTH_ + TB_OUTPUT_ + TB_PREFIX )
                {
                    // track back
                    // we need to find the minimal state and index of the state

                    //printf("traceback...@%d\n", i_trellis);
                    // do normalization first
#if 1
                    rub0 = smin (pTrellis[0], pTrellis[1] );
                    rub1 = smin (pTrellis[2], pTrellis[3] );
                    rub2 = smin (rub0, rub1);

                    rub3 = hmin (rub2);
                    rub3 = and (rub3, ALLINVONE );

                    // normalize
                    pTrellis[0] = sub ( pTrellis[0], rub3);
                    pTrellis[1] = sub ( pTrellis[1], rub3);
                    pTrellis[2] = sub ( pTrellis[2], rub3);
                    pTrellis[3] = sub ( pTrellis[3], rub3);
#endif
                    // rub3 has the minimal value, we need to find the index
                    // the algorithm to find the right index is to embed the index at the least
                    // significant bits of state value, then we just find the minimal value

                    rub0 = vub(INDEXES[0]);
                    rub1 = pTrellis[0];

                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus4 = smin ( rus2, rus3);

                    rub0 = vub(INDEXES[1]);
                    rub1 = pTrellis[1];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus5 = smin (rus2, rus3);

                    rus4 = smin (rus4, rus5);

                    rub0 = vub(INDEXES[2]);
                    rub1 = pTrellis[2];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus6 = smin (rus2, rus3);

                    rus4 = smin (rus4, rus6);

                    rub0 = vub(INDEXES[3]);
                    rub1 = pTrellis[3];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus7 = smin (rus2, rus3);

                    rus4 = smin (rus4, rus7);

                    // now rus4 contains the minimal 8 
                    rus0 = hmin (rus4);

                    // now the first word contains the index and value
                    // index: bit [7:2]; 
                    // value: bit [15:8]
                    i_minpos = extract<0>(rus0); 

                    // now we can trace back ...
                    pTraceBk = pTrellis;

                    // first part - trace back without output
                    i_minpos = (i_minpos >> 2) & 0x3F; // index 6:0
                    for ( i = 0; i < TB_DEPTH_; i++)
                    {
                        pTraceBk -= 4;
                        i_minpos  = (i_minpos >> 1) & 0x3F;
                        i_tpos    = ((char*) pTraceBk)[i_minpos] ;
                        i_minpos |= (i_tpos & 1) << 6;  // now i_minpos 6:0 is the new index
                    }

                    // second part - trace back with output
                    pbVitOutput += (TB_OUTPUT_ >> 3);
                    pVTOutput    = (unsigned char*)pbVitOutput;

                    for ( i = 0; i < TB_OUTPUT_ >> 3; i++)
                    {
                        for ( j = 0; j < 8; j++ )
                        {
                            outchar = outchar << 1;
                            outchar |= (i_minpos >> 6) & 1;

                            // next bit
                            pTraceBk -= 4;
                            i_minpos = (i_minpos >> 1) & 0x3F;
                            i_tpos = ((char*) pTraceBk)[i_minpos] ;
                            i_minpos |= (i_tpos & 1) << 6;  // now i_minpos 6:0 is the new index
                        }
                        
                        pVTOutput --;
                        *pVTOutput = outchar;

                        outchar = 0;
                    }
#if 0
                    for (int i = 0; i < (TB_OUTPUT_ >> 3); i++)
                    {
                        printf("%02X ", pVTOutput[i]);
                    }
                    printf("\n");
#endif

                    // yield
                    fcb(pVTOutput, (TB_OUTPUT_ >> 3));

                    nTotalBits         -= TB_OUTPUT_;
                    i_trellis          -= TB_OUTPUT_;
                    //printf("nTotalBits=%d\nSoftBits", nTotalBits);
                }
            }
        }

        //////////////////////////////////////////////////////////////////////////

        inline void init_viterbi(input_param &param)
        {
            // Initialize Trellis
            param.pTrellis[0]  = vub(ALL_INIT0);
            param.pTrellis[1]  = vub(ALL_INIT);
            param.pTrellis[2]  = vub(ALL_INIT);
            param.pTrellis[3]  = vub(ALL_INIT);
            param.iTrellis     = 0;
            param.nCodingRate  = 6;// default 1/2
            param.nDecodedBits = 0;
            param.nTotalBits   = 0;
        }

        inline int viterbi_blk(input_param& inputparam, unsigned char* pbInput, int nInput, unsigned char * pbOutput)
        {
            const int TB_PREFIX        = 6;
            unsigned int uiBytesOutput = 0;
            unsigned char *pbVitOutput = pbOutput;
            unsigned char outchar      = 0;// the output(decoded) char            
            int ocnt                   = 0; // Counter for the output bits
            unsigned char * pVTOutput;

            int i, j;
            vub* pTrellis = inputparam.pTrellis;
            int i_trellis = inputparam.iTrellis;    // index of trellis
            
            // vector128 constants
            const vub * const pVITMA = (const vub*) VIT_MA; // Branch Metric A
            const vub * const pVITMB = (const vub*) VIT_MB; // Branch Metric B

            const vub ALLINVONE (ALL_INVERSE_ONE);
            
            // for trace back
            vub * pTraceBk;       // trace back pointer in trellis
            int i_minpos = 0;     // the minimal path position
            int i_tpos   = 0;

            // temporal variables
            vub rub0, rub1, rub2, rub3;
            vus rus0, rus1, rus2, rus3;
            vus rus4, rus5, rus6, rus7;

            while (nInput > 0)
            {
                if (inputparam.nCodingRate == DOT11A_RATE_6M
                    || inputparam.nCodingRate == DOT11A_RATE_24M
                    || inputparam.nCodingRate == DOT11A_RATE_12M)
                {
                    ViterbiAdvance(pTrellis, pVITMA, pbInput[0], pVITMB, pbInput[1]);
                    pbInput   += 2; // jump to data
                    i_trellis += 1;
                    nInput    -= 2;
                }
                else if (inputparam.nCodingRate == DOT11A_RATE_9M
                    || inputparam.nCodingRate == DOT11A_RATE_18M
                    || inputparam.nCodingRate == DOT11A_RATE_36M
                    || inputparam.nCodingRate == DOT11A_RATE_54M)                
                {
                    ViterbiAdvance(pTrellis, pVITMA, pbInput[0], pVITMB, pbInput[1]);
                    ViterbiAdvance(pTrellis, pVITMA, pbInput[2]);
                    ViterbiAdvance(pTrellis, pVITMB, pbInput[3]);
                    pbInput   += 4;
                    i_trellis += 3;
                    nInput    -= 4;
                }
                else if (inputparam.nCodingRate == DOT11A_RATE_48M)
                {
                    ViterbiAdvance(pTrellis, pVITMA, pbInput[0], pVITMB, pbInput[1]);
                    ViterbiAdvance(pTrellis, pVITMA, pbInput[2]);
                    pbInput   += 3;
                    i_trellis += 2;
                    nInput    -= 3;
                }
                // Normalize
                if ((i_trellis & inputparam.nTraceBackNormMask) == 0 )
                {
                    // normalization
                    // find the smallest component and extract it from all states
                    rub0 = smin (pTrellis[0], pTrellis[1] );
                    rub1 = smin (pTrellis[2], pTrellis[3] );
                    rub2 = smin (rub0, rub1);
                    rub3 = hmin (rub2);

                    // make sure to clear the marker bit
                    rub3 = and  (rub3, ALLINVONE );

                    // normalize
                    pTrellis[0] = sub ( pTrellis[0], rub3);
                    pTrellis[1] = sub ( pTrellis[1], rub3);
                    pTrellis[2] = sub ( pTrellis[2], rub3);
                    pTrellis[3] = sub ( pTrellis[3], rub3);
                }
                
                // Traceback 
                // We should first skip TB_DEPTH_VITAS bits and 
                // TB_OUTPUT_VITAS is actual bits we can output
                if ( i_trellis >= inputparam.nTraceBackDepth + inputparam.nTraceBackOutputBits + TB_PREFIX )
                {
                    // track back
                    // we need to find the minimal state and index of the state
                    // do normalization first
                    rub0 = smin (pTrellis[0], pTrellis[1] );
                    rub1 = smin (pTrellis[2], pTrellis[3] );
                    rub2 = smin (rub0, rub1);
                    rub3 = hmin (rub2);
                    rub3 = and (rub3, ALLINVONE );

                    // normalize
                    pTrellis[0] = sub ( pTrellis[0], rub3);
                    pTrellis[1] = sub ( pTrellis[1], rub3);
                    pTrellis[2] = sub ( pTrellis[2], rub3);
                    pTrellis[3] = sub ( pTrellis[3], rub3);

                    // rub3 has the minimal value, we need to find the index
                    // the algorithm to find the right index is to embed the index at the least
                    // significant bits of state value, then we just find the minimal value

                    rub0 = vub(INDEXES[0]);
                    rub1 = pTrellis[0];

                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus4 = smin ( rus2, rus3);

                    rub0 = vub(INDEXES[1]);
                    rub1 = pTrellis[1];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus5 = smin (rus2, rus3);

                    rus4 = smin (rus4, rus5);

                    rub0 = vub(INDEXES[2]);
                    rub1 = pTrellis[2];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus6 = smin (rus2, rus3);

                    rus4 = smin (rus4, rus6);

                    rub0 = vub(INDEXES[3]);
                    rub1 = pTrellis[3];
                    rus2 = (vus)interleave_low  ( rub0, rub1 );
                    rus3 = (vus)interleave_high ( rub0, rub1 );
                    rus7 = smin (rus2, rus3);

                    rus4 = smin (rus4, rus7);

                    // now rus4 contains the minimal 8 
                    rus0 = hmin (rus4);

                    // now the first word contains the index and value
                    // index: bit [7:2]; 
                    // value: bit [15:8]
                    i_minpos = extract<0>(rus0); 

                    // now we can trace back ...
                    pTraceBk = pTrellis;

                    // first part - trace back without output
                    i_minpos = (i_minpos >> 2) & 0x3F; // index 6:0
                    for ( i = 0; i < inputparam.nTraceBackDepth; i++)
                    {
                        pTraceBk -= 4;
                        i_minpos  = (i_minpos >> 1) & 0x3F;
                        i_tpos    = ((char*) pTraceBk)[i_minpos] ;
                        i_minpos |= (i_tpos & 1) << 6;  // now i_minpos 6:0 is the new index
                    }

                    // second part - trace back with output
                    pbVitOutput += (inputparam.nTraceBackOutputBits >> 3);
                    pVTOutput    = (unsigned char*)pbVitOutput;

                    for ( i = 0; i < inputparam.nTraceBackOutputBits >> 3; i++)
                    {
                        for ( j = 0; j < 8; j++ )
                        {
                            outchar = outchar << 1;
                            outchar |= (i_minpos >> 6) & 1;

                            // next bit
                            pTraceBk -= 4;
                            i_minpos = (i_minpos >> 1) & 0x3F;
                            i_tpos = ((char*) pTraceBk)[i_minpos] ;
                            i_minpos |= (i_tpos & 1) << 6;  // now i_minpos 6:0 is the new index
                        }

                        pVTOutput --;
                        *pVTOutput = outchar;

                        outchar = 0;
                    }

                    uiBytesOutput += (inputparam.nTraceBackOutputBits >> 3);

                    inputparam.nDecodedBits += inputparam.nTraceBackOutputBits;
                    i_trellis               -= inputparam.nTraceBackOutputBits;
                }
            }
            inputparam.pTrellis = pTrellis;
            inputparam.iTrellis = i_trellis;
            return uiBytesOutput;
        }
    


};

}
