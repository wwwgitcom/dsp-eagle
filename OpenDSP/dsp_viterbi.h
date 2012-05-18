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
#if 0
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
#if 0
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

#pragma warning(disable: 4355)// disable warning: this used in init list
    template<int nTraceBackLength = 36, int nTraceBackOutput = 48>
    struct dsp_viterbi_64
    {
        static_assert(nTraceBackOutput % 8 == 0, "Trace back output must a factor of 8!");

        static const int nTracebackDataCount  = nTraceBackLength + nTraceBackOutput;
        static const int nTracebackOffset     = nTraceBackLength + nTraceBackOutput - 1;
        static const int nTraceBackOutputByte = nTraceBackOutput / 8;

        // ACS
        v_align(64) v_ub m_vBM[64][4]; // 64 states, each state has two 32 BM
        v_align(64) v_ub m_vBM0[8][4]; // 64 states, each state has two 32 BM
        v_align(64) v_ub m_vBM1[8][4]; // 64 states, each state has two 32 BM

        // shared
        v_align(64) v_ub m_vShuffleMask;
        v_ub m_vNormMask;
        unsigned __int8 *m_pSoftBits;
        unsigned __int8 *m_pDecodedBytes;
        __int32          m_nDecodedBytes;

        // shared
#define RQ_MASK (1024 - 1)
        typedef v_ub TBState[4];
        typedef unsigned __int16 SurviorPath[4];
        typedef RingQ<SurviorPath, unsigned __int16, 1024> VitRQ;
        
        v_align(64) VitRQ     m_TBQ;
        v_align(64) TBState   m_MinAddress[1024];

        // TB
        v_align(64) v_ub      m_vStateIndex[4];

        //////////////////////////////////////////////////////////////////////////

        dsp_viterbi_64() : m_TBWorker(*this), m_ACSWorker(*this), m_Worker(*this), m_ACSCPU(2), m_TBCPU(2)
        {
            unsigned char v = 0;
            int i;
            
            v = 0;
            for (i = 0; i < 8; i++)
            {
                m_vShuffleMask[i] = v;
                v += 2;
            }
            v = 1;
            for (; i < 16; i++)
            {
                m_vShuffleMask[i] = v;
                v += 2;
            }

            v = 0;
            for (i = 0; i < 64; i++)
            {
                m_vStateIndex[0][i] = v++;
            }

            m_vNormMask.v_setall(128);

            
            unsigned __int8 bAddress;
            unsigned __int8 m00, m01, m10, m11;
            unsigned __int8 m0, m1;

            v_ub v14;
            v14.v_setall(14);

            v_ub v7;
            v7.v_setall(7);

            for (unsigned __int8 b0 = 0; b0 < 8; b0++)
            {
                for (unsigned __int8 b1 = 0; b1 < 8; b1++)
                {
                    bAddress = ( (b0 & 0x7) | ((b1 & 0x7) << 3) );

                    //printf("b0= %d, b1 = %d, Address = %d \nSoftBits", b0, b1, bAddress);

                    m00 = b1 + b0;
                    m01 = b1 + 7 - b0;
                    m10 = 7 - b1 + b0;
                    m11 = 7 - b1 + 7 - b0;

                    //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\nSoftBits", m00, m01, m10, m11);

                    m_vBM[bAddress][0].v_set(m00, m01, m00, m01, m11, m10, m11, m10, m11, m10, m11, m10, m00, m01, m00, m01);
                    m_vBM[bAddress][1] = v_sub(v14, m_vBM[bAddress][0]);
                    m_vBM[bAddress][2].v_set(m10, m11, m10, m11, m01, m00, m01, m00, m01, m00, m01, m00, m10, m11, m10, m11);
                    m_vBM[bAddress][3] = v_sub(v14, m_vBM[bAddress][2]);
                }
            }

            for (unsigned __int8 b0 = 0; b0 < 8; b0++)
            {
                bAddress = (b0 & 0x7);

                //printf("b0= %d, b1 = %d, Address = %d \nSoftBits", b0, b1, bAddress);

                m0 = b0;
                m1 = 7 - b0;

                //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\nSoftBits", m00, m01, m10, m11);

                m_vBM0[bAddress][0].v_set(m0, m1, m0, m1, m1, m0, m1, m0, m1, m0, m1, m0, m0, m1, m0, m1);
                m_vBM0[bAddress][1] = v_sub(v7, m_vBM0[bAddress][0]);
                m_vBM0[bAddress][2].v_set(m0, m1, m0, m1, m1, m0, m1, m0, m1, m0, m1, m0, m0, m1, m0, m1);
                m_vBM0[bAddress][3] = v_sub(v7, m_vBM0[bAddress][2]);
            }

            for (unsigned __int8 b1 = 0; b1 < 8; b1++)
            {
                bAddress = (b1 & 0x7);

                //printf("b0= %d, b1 = %d, Address = %d \nSoftBits", b0, b1, bAddress);

                m0 = b1;
                m1 = 7 - b1;

                //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\nSoftBits", m00, m01, m10, m11);

                m_vBM1[bAddress][0].v_set(m0, m0, m0, m0, m1, m1, m1, m1, m1, m1, m1, m1, m0, m0, m0, m0);
                m_vBM1[bAddress][1] = v_sub(v7, m_vBM1[bAddress][0]);
                m_vBM1[bAddress][2].v_set(m1, m1, m1, m1, m0, m0, m0, m0, m0, m0, m0, m0, m1, m1, m1, m1);
                m_vBM1[bAddress][3] = v_sub(v7, m_vBM1[bAddress][2]);
            }
        }

        inline void v_print(v_ub &v)
        {
            for (int i = 0; i < v_ub::elem_cnt; i++)
            {
                printf("%4d ", v[i]);
            }
         
            printf("\n");
        }
        
        inline void v_print(v_ub &v1, v_ub &v2, v_ub &v3, v_ub &v4)
        {
            static int icount = 0;
            printf("%d: ", icount++);
            for (int i = 0; i < v_ub::elem_cnt; i++)
            {
                printf("%2d ", v1[i]);
            }

            printf("| ");

            for (int i = 0; i < v_ub::elem_cnt; i++)
            {
                printf("%2d ", v2[i]);
            }
            printf("| ");

            for (int i = 0; i < v_ub::elem_cnt; i++)
            {
                printf("%2d ", v3[i]);
            }
            printf("| ");
            for (int i = 0; i < v_ub::elem_cnt; i++)
            {
                printf("%2d ", v4[i]);
            }

            printf("\n");
        }

        inline void v_print(v_ub *v, int nv)
        {
            static int icount = 0;
            printf("%d: ", icount++);
            for (int j = 0; j < nv; j++)
            {
                for (int i = 0; i < v_ub::elem_cnt; i++)
                {
                    printf("%4d ", v[j][i]);
                }                
            }
            printf("\n");
        }


        __forceinline v_ub ACS_Branch(v_ub& va, v_ub& vb, v_ub& vBM1, v_ub& vBM2, unsigned __int16& iPH)
        {
            v_ub vc    = v_add(va, vBM1);// from 0
            v_ub vd    = v_add(vb, vBM2);// from 1
            v_b vdiff  = v_sub(vd, vc); // difference

            iPH        = (unsigned __int16)vdiff.v_signmask();
            
            v_ub vmin  = v_min(vc, vd);

            return vmin;
        }

        __forceinline void Shuffle(v_ub& va, v_ub& vb, v_ub& vmask, v_ub& veven, v_ub&vodd)
        {
            va = va.v_shuffle(vmask);
            vb = vb.v_shuffle(vmask);

            (v_q&)veven = ((v_q&)va).v_unpack_lo((v_q&)vb);
            (v_q&)vodd  = ((v_q&)va).v_unpack_hi((v_q&)vb);
        }

        __forceinline void ButterFly(v_ub& va, v_ub& vb, v_ub& vShuffleMask, v_ub& vBM1, v_ub& vBM2, unsigned __int16& iPH1, unsigned __int16& iPH2)
        {
            v_ub veven, vodd;
            Shuffle(va, vb, vShuffleMask, veven, vodd);

            va = ACS_Branch(veven, vodd, vBM1, vBM2, iPH1);//0-branch
            vb = ACS_Branch(veven, vodd, vBM2, vBM1, iPH2);//1-branch
        }

        __forceinline __int8 BMAddress(unsigned __int8 b0, unsigned __int8 b1)
        {
            return ( (b0 & 7) | ((b1 & 7) << 3) );
        }

        __forceinline unsigned __int8 BMAddress(unsigned __int8 b)
        {
            return b;
        }

        __forceinline __int8 PrevPHAddress(unsigned __int8* pvPath, unsigned __int8& Index)
        {
            unsigned __int8 AddressHi = Index >> 3;
            unsigned __int8 AddressLo = Index & 0x7;

            unsigned __int8 v = pvPath[AddressHi];
            return ( ((v >> AddressLo) & 1) | ((Index << 1) & 0x3F) );
        }

        __forceinline unsigned __int8 FindMinValueAddress(v_ub* pvTrellis)
        {
            v_ub vb0, vb1;
            v_us vs0, vs1, vs2, vs3;

            unsigned __int16 MinPos;

            vb0 = m_vStateIndex[0];
            vb1 = pvTrellis[0];

            vs1 = (v_us)v_unpack_lo( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi( vb0, vb1 );

            vs0 = v_min(vs1, vs2);

            vb0 = m_vStateIndex[1];
            vb1 = pvTrellis[1];
            vs1 = (v_us)v_unpack_lo ( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

            vs3 = v_min (vs1, vs2);
            vs0 = v_min (vs0, vs3);

            vb0 = m_vStateIndex[2];
            vb1 = pvTrellis[2];
            vs1 = (v_us)v_unpack_lo ( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

            vs3 = v_min (vs1, vs2);
            vs0 = v_min (vs0, vs3);

            vb0 = m_vStateIndex[3];
            vb1 = pvTrellis[3];
            vs1 = (v_us)v_unpack_lo ( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

            vs3 = v_min (vs1, vs2);
            vs0 = v_min (vs0, vs3);

            vs0 = vs0.v_hminpos();

            MinPos = vs0.v_get_at<0>();// the minimum index in 7:0
            //MinPos &= 0xFF;

            return (unsigned __int8)MinPos;
        }

        __forceinline void Normalize(v_ub* pvTrellis)
        {
            v_ub vb0, vb1, vb2, vb3, vb4, vb5;

            vb0 = pvTrellis[0];
            vb1 = pvTrellis[1];

            vb4 = v_min(vb0, vb1);

            vb2 = pvTrellis[2];
            vb3 = pvTrellis[3];

            vb5 = v_min(vb2, vb3);

            vb4 = v_min(vb4, vb5);

            vb5 = ((v_i&)vb4).v_shuffle<2, 3, 0, 1>();

            vb4 = v_min(vb4, vb5);

            // vb0 contains 8 minimum values
            // cast into 8 shorts
            vb5 = v_unpack_lo(vb4, vb4);

            vb4 = ((v_us&)vb5).v_hminpos();

            vb4 = ((v_s&)vb4).v_shuffle_lo<0, 0, 0, 0>();
            vb4 = v_unpack_lo(vb4, vb4);

            pvTrellis[0] = v_sub(vb0, vb4);
            pvTrellis[1] = v_sub(vb1, vb4);
            pvTrellis[2] = v_sub(vb2, vb4);
            pvTrellis[3] = v_sub(vb3, vb4);
        }

        __forceinline void Normalize(v_ub* pvTrellis, v_ub& vNormConst)
        {
            pvTrellis[0] = v_sub(pvTrellis[0], vNormConst);
            pvTrellis[1] = v_sub(pvTrellis[1], vNormConst);
            pvTrellis[2] = v_sub(pvTrellis[2], vNormConst);
            pvTrellis[3] = v_sub(pvTrellis[3], vNormConst);
        }

        __forceinline void TraceBack(unsigned __int16 HistoryIndex, unsigned __int8 MinIndex, int TraceBackLength, int TraceOutputLength, unsigned __int8* pDecodedBytes)
        {
            unsigned __int8* pPathHistory;

            unsigned __int8 PHAddress;

            PHAddress = MinIndex;

            unsigned __int8 Output = 0;
            unsigned __int8* p = pDecodedBytes;

            for (int i = 0; i < TraceBackLength; i++)
            {
                pPathHistory = (unsigned __int8*)&m_TBQ[HistoryIndex];

                PHAddress = PrevPHAddress(pPathHistory, PHAddress);

                HistoryIndex -= 1;
                HistoryIndex &= RQ_MASK;
            }

            for (int i = 0; i < TraceOutputLength >> 3; i++)
            {
                for ( int j = 0; j < 8; j++)
                {
                    Output <<= 1;
                    Output |= (PHAddress >> 5);// since we know only 6 bits are valid

                    pPathHistory = (unsigned __int8*)&m_TBQ[HistoryIndex];

                    PHAddress = PrevPHAddress(pPathHistory, PHAddress);

                    HistoryIndex -= 1;
                    HistoryIndex &= RQ_MASK;
                }
                *p = Output;
                p--;
            }
        }


        //////////////////////////////////////////////////////////////////////////


        struct viterbi_traceback_worker : dsp_task::task 
        {
            dsp_viterbi_64& m_viterbi;

            viterbi_traceback_worker(dsp_viterbi_64& vit) : m_viterbi(vit)
            {

            }

            dsp_task::task* execute()
            {
                VitRQ::iter      TBQrit            = 0;
                unsigned __int16 MinAddressIndex   = 0;

                unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
                __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                unsigned __int16 TraceBackPt       = 0;
                                
                tick_count tbegin, tend, tend2, tduration, tduration2;
                tbegin = tick_count::now();

                while (TotalDecodedBytes > 0)
                {
                    while( m_viterbi.m_TBQ.DataCount() < nTracebackDataCount )
                    {
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(&m_viterbi.m_MinAddress[MinAddressIndex++][0]);
                    MinAddressIndex &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit                   += nTraceBackOutput;
                    TBQrit                   &= RQ_MASK;
                    m_viterbi.m_TBQ.m_rcount += nTraceBackOutput;
                }

                tend = tick_count::now();
                tduration = tend - tbegin;
                printf("TBThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
                
                return this;
            }
        };

        struct viterbi_acs_worker : dsp_task::task 
        {
            dsp_viterbi_64& m_viterbi;
            
            viterbi_acs_worker(dsp_viterbi_64& vit) : m_viterbi(vit)
            {

            }


            void execute_1_2()
            {
                v_ub vStates[4];
                v_ub vBM0, vBM1;
                v_ub vShuffleMask;
                int  BMIndex = 4;
                unsigned __int16 iPH0, iPH1;
                unsigned __int16 MinAddressIndex = 0;

                vShuffleMask = m_viterbi.m_vShuffleMask;

                for (int i = 0; i < 4; i++)
                {
                    vStates[i].v_setall(128);
                }
                vStates[0].v_set_at<0>(0);

                VitRQ::iter TBQwit       = 0;
                unsigned int iTrellis    = 0;

                unsigned __int8 *pSoftBits = m_viterbi.m_pSoftBits;
                __int32 nTotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                __int32 nTotalSoftBits     = nTotalDecodedBytes * 8 * 2; // 1/2 coding

                m_viterbi.m_TBQ.m_wcount = nTraceBackLength;
                m_viterbi.m_TBWorker.RunA();

                tick_count tbegin, tend, tend2, tduration, tduration2;
                tbegin = tick_count::now();

                for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 4)
                {
                    // stage StageIndex
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

                    v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;
                    // stage StageIndex + 1
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2], pSoftBits[nSoftBits + 3]);

                    pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit &= RQ_MASK;
                    iTrellis += 2;

                    //m_viterbi.v_print(vStates, 4);

                    if (vStates[0].v_get_at<0>() > 208)
                    {
                        //m_viterbi.v_print(vStates, 1);
                        m_viterbi.Normalize(vStates);
                    }

                    if (iTrellis >= nTracebackDataCount)
                    {
                        //unsigned __int8 PHAddress = m_viterbi.FindMinValueAddress(vStates);
                        //m_viterbi.m_MinAddress[MinAddressIndex++] = PHAddress;
                        m_viterbi.m_MinAddress[MinAddressIndex][0] = vStates[0];
                        m_viterbi.m_MinAddress[MinAddressIndex][1] = vStates[1];
                        m_viterbi.m_MinAddress[MinAddressIndex][2] = vStates[2];
                        m_viterbi.m_MinAddress[MinAddressIndex][3] = vStates[3];

                        MinAddressIndex++;
                        MinAddressIndex &= RQ_MASK;
                        //printf("TB MinAddress=%d, Index=%d\nSoftBits", PHAddress, MinAddressIndex);

                        iTrellis                 -= nTraceBackOutput;
                        nTotalDecodedBytes       -= nTraceBackOutputByte;
                        m_viterbi.m_TBQ.m_wcount += nTraceBackOutput;

                        //printf("WTB: w=%u, r=%u\nSoftBits", m_viterbi.m_TBQ.m_wcount, m_viterbi.m_TBQ.m_rcount);

                        while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
                    }
                }

                v_ub vZero;
                vZero.v_zero();

                unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);
                if ( iTrellis >= nTraceBackLength )
                {
                    for (unsigned int i = 0; i < iTrellisPadding; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;

                    m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
                    m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
                    MinAddressIndex++;
                    MinAddressIndex &= RQ_MASK;

                    nTotalDecodedBytes                       -= nTraceBackOutputByte;
                    m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
                }

                while (nTotalDecodedBytes > 0)
                {
                    // pad traceback length zeros
                    while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
                    for (unsigned int i = 0; i < nTraceBackOutput; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;
                    m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
                    m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
                    MinAddressIndex++;
                    MinAddressIndex &= RQ_MASK;

                    nTotalDecodedBytes        -= nTraceBackOutputByte;
                    m_viterbi.m_TBQ.m_wcount  += nTraceBackOutput;
                }

                tend = tick_count::now();
                tduration = tend - tbegin;
                printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
            }

            // A0 A1 A2 A3
            // B0 *  B2 *
            // A0 B0 A1 A2 B2 A3
            void execute_2_3()
            {
                v_ub vStates[4];
                v_ub vBM0, vBM1;
                v_ub vShuffleMask;
                int  BMIndex = 4;
                unsigned __int16 iPH0, iPH1;
                unsigned __int16 MinAddressIndex = 0;

                vShuffleMask = m_viterbi.m_vShuffleMask;

                for (int i = 0; i < 4; i++)
                {
                    vStates[i].v_setall(128);
                }
                vStates[0].v_set_at<0>(0);

                VitRQ::iter TBQwit       = 0;
                unsigned int iTrellis    = 0;

                unsigned __int8 *pSoftBits = m_viterbi.m_pSoftBits;
                __int32 nTotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                __int32 nTotalSoftBits     = nTotalDecodedBytes * 8 * 3 / 2; // 2/3 coding

                m_viterbi.m_TBQ.m_wcount = nTraceBackLength;
                m_viterbi.m_TBWorker.RunA();

                tick_count tbegin, tend, tend2, tduration, tduration2;
                tbegin = tick_count::now();
                for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 3)
                {
                    // stage StageIndex
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

                    v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;
                    // stage StageIndex + 1
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

                    pvBM = &m_viterbi.m_vBM0[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit &= RQ_MASK;
                    iTrellis += 2;

                    //m_viterbi.v_print(vStates, 4);

                    if (vStates[0].v_get_at<0>() > 208)
                    {
                        //m_viterbi.v_print(vStates, 1);
                        m_viterbi.Normalize(vStates);
                    }

                    if (iTrellis >= nTracebackDataCount)
                    {
                        //unsigned __int8 PHAddress = m_viterbi.FindMinValueAddress(vStates);
                        //m_viterbi.m_MinAddress[MinAddressIndex++] = PHAddress;
                        m_viterbi.m_MinAddress[MinAddressIndex][0] = vStates[0];
                        m_viterbi.m_MinAddress[MinAddressIndex][1] = vStates[1];
                        m_viterbi.m_MinAddress[MinAddressIndex][2] = vStates[2];
                        m_viterbi.m_MinAddress[MinAddressIndex][3] = vStates[3];

                        MinAddressIndex++;
                        MinAddressIndex &= RQ_MASK;
                        //printf("TB MinAddress=%d, Index=%d\nSoftBits", PHAddress, MinAddressIndex);

                        iTrellis                 -= nTraceBackOutput;
                        nTotalDecodedBytes       -= nTraceBackOutputByte;
                        m_viterbi.m_TBQ.m_wcount += nTraceBackOutput;

                        //printf("WTB: w=%u, r=%u\nSoftBits", m_viterbi.m_TBQ.m_wcount, m_viterbi.m_TBQ.m_rcount);

                        while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
                    }
                }

                v_ub vZero;
                vZero.v_zero();

                unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);
                if ( iTrellis >= nTraceBackLength )
                {
                    for (unsigned int i = 0; i < iTrellisPadding; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;

                    m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
                    m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
                    MinAddressIndex++;
                    MinAddressIndex &= RQ_MASK;

                    nTotalDecodedBytes                       -= nTraceBackOutputByte;
                    m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
                }

                while (nTotalDecodedBytes > 0)
                {
                    // pad traceback length zeros
                    while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
                    for (unsigned int i = 0; i < nTraceBackOutput; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;
                    m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
                    m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
                    MinAddressIndex++;
                    MinAddressIndex &= RQ_MASK;

                    nTotalDecodedBytes                       -= nTraceBackOutputByte;
                    m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
                }

                tend = tick_count::now();
                tduration = tend - tbegin;
                //printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
            }

            // A0 A1 *  A3 A4 *
            // B0 *  B2 B3 *  B5
            // A0 B0 A1 B2 A3 B3 A4 B5
            void execute_3_4()
            {
                v_ub vStates[4];
                v_ub vBM0, vBM1;
                v_ub vShuffleMask;
                int  BMIndex = 4;
                unsigned __int16 iPH0, iPH1;
                unsigned __int16 MinAddressIndex = 0;

                vShuffleMask = m_viterbi.m_vShuffleMask;

                for (int i = 0; i < 4; i++)
                {
                    vStates[i].v_setall(128);
                }
                vStates[0].v_set_at<0>(0);

                VitRQ::iter TBQwit       = 0;
                unsigned int iTrellis    = 0;

                unsigned __int8 *pSoftBits = m_viterbi.m_pSoftBits;
                __int32 nTotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                __int32 nTotalSoftBits     = nTotalDecodedBytes * 8 * 4 / 3; // 3/4 coding

                m_viterbi.m_TBQ.m_wcount = nTraceBackLength;
                m_viterbi.m_TBWorker.RunA();

                tick_count tbegin, tend, tend2, tduration, tduration2;
                tbegin = tick_count::now();
                for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 8)
                {
                    // stage StageIndex
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

                    v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;

                    // stage StageIndex + 1
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

                    pvBM = &m_viterbi.m_vBM0[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;
                    // stage StageIndex + 2
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 3]);

                    pvBM = &m_viterbi.m_vBM1[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;

                    // stage StageIndex + 3
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 4], pSoftBits[nSoftBits + 5]);

                    pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;
                    // stage StageIndex + 4
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 6]);

                    pvBM = &m_viterbi.m_vBM0[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;
                    // stage StageIndex + 5
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 7]);

                    pvBM = &m_viterbi.m_vBM1[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;
                    iTrellis += 6;

                    //m_viterbi.v_print(vStates, 4);

                    if (vStates[0].v_get_at<0>() > 208)
                    {
                        //m_viterbi.v_print(vStates, 1);
                        m_viterbi.Normalize(vStates);
                    }

                    if (iTrellis >= nTracebackDataCount)
                    {
                        //unsigned __int8 PHAddress = m_viterbi.FindMinValueAddress(vStates);
                        //m_viterbi.m_MinAddress[MinAddressIndex++] = PHAddress;
                        m_viterbi.m_MinAddress[MinAddressIndex][0] = vStates[0];
                        m_viterbi.m_MinAddress[MinAddressIndex][1] = vStates[1];
                        m_viterbi.m_MinAddress[MinAddressIndex][2] = vStates[2];
                        m_viterbi.m_MinAddress[MinAddressIndex][3] = vStates[3];

                        MinAddressIndex++;
                        MinAddressIndex &= RQ_MASK;

                        //printf("TB MinAddress=%d, Index=%d\nSoftBits", PHAddress, MinAddressIndex);

                        iTrellis                 -= nTraceBackOutput;
                        nTotalDecodedBytes       -= nTraceBackOutputByte;
                        m_viterbi.m_TBQ.m_wcount += nTraceBackOutput;

                        //printf("WTB: w=%u, r=%u, bytes=%d\n", m_viterbi.m_TBQ.m_wcount, m_viterbi.m_TBQ.m_rcount, nTotalDecodedBytes);

                        while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
                    }
                }

                v_ub vZero;
                vZero.v_zero();

                unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);
                if ( iTrellis >= nTraceBackLength )
                {
                    for (unsigned int i = 0; i < iTrellisPadding; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
                    m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
                    MinAddressIndex++;
                    MinAddressIndex &= RQ_MASK;

                    nTotalDecodedBytes                       -= nTraceBackOutputByte;
                    m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
                }

                while (nTotalDecodedBytes > 0)
                {
                    // pad traceback length zeros
                    while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
                    for (unsigned int i = 0; i < nTraceBackOutput; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
                    m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
                    MinAddressIndex++;
                    MinAddressIndex &= RQ_MASK;

                    nTotalDecodedBytes                       -= nTraceBackOutputByte;
                    m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
                }

                tend = tick_count::now();
                tduration = tend - tbegin;
                printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
            }


            dsp_task::task* execute()
            {
                //execute_1_2();
                execute_2_3();
                //execute_3_4();

                return this;
            }
        };


        struct viterbi_worker : dsp_task::task 
        {
            dsp_viterbi_64& m_viterbi;

            viterbi_worker(dsp_viterbi_64& vit) : m_viterbi(vit)
            {

            }

            void execute_1_2()
            {
                v_ub vShuffleMask;
                v_ub vNormMask;
                v_ub vStates[4];
                v_ub vBM0, vBM1;

                int  BMIndex = 4;
                unsigned __int16 iPH0, iPH1;
                unsigned __int16 MinAddressIndex = 0;

                vShuffleMask = m_viterbi.m_vShuffleMask;
                vNormMask    = m_viterbi.m_vNormMask;

                for (int i = 0; i < 4; i++)
                {
                    vStates[i].v_setall(128);
                }
                vStates[0].v_set_at<0>(0);


                VitRQ::iter  TBQwit     = 0;
                VitRQ::iter  TBQrit     = 0;
                unsigned int iTrellis   = 0;

                unsigned __int8 *pSoftBits         = m_viterbi.m_pSoftBits;
                unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
                __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                unsigned __int16 TraceBackPt       = 0;

                tick_count tbegin, tend, tend2, tduration, tduration2;

                __int32 nTotalSoftBits = m_viterbi.m_nDecodedBytes * 8 * 2; // 1/2 coding

                tbegin = tick_count::now();

                for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 4)
                {
                    // stage StageIndex
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

                    v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    //m_viterbi.v_print(vStates, 4);

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;
                    // stage StageIndex + 1
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2], pSoftBits[nSoftBits + 3]);

                    pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;
                    iTrellis += 2;

                    //m_viterbi.v_print(vStates, 4);

                    if (vStates[0].v_get_at<0>() > 213)
                    {
                        //printf("===Normalization===\n");
                        //m_viterbi.v_print(vStates, 4);
                        //m_viterbi.Normalize(vStates);
                        m_viterbi.Normalize(vStates, vNormMask);
                        //m_viterbi.v_print(vStates, 4);
                    }

                    if (iTrellis >= nTracebackDataCount)
                    {
                        unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(vStates);
                        //unsigned __int8 MinAddress = 0;

                        TraceBackPt = TBQrit + nTracebackOffset;
                        TraceBackPt &= RQ_MASK;

                        m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                        pDecodedBytes     += nTraceBackOutputByte;
                        TotalDecodedBytes -= nTraceBackOutputByte;

                        TBQrit            += nTraceBackOutput;
                        TBQrit            &= RQ_MASK;
                        iTrellis          -= nTraceBackOutput;
                    }
                }


                //m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                if ( iTrellis >= nTraceBackLength )
                {
                    unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);

                    for (unsigned int i = 0; i < iTrellisPadding; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit            += nTraceBackOutput;
                    TBQrit            &= RQ_MASK;
                }

                while (TotalDecodedBytes > 0)
                {
                    // pad traceback length zeros
                    for (unsigned int i = 0; i < nTraceBackOutput; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);
                    
                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit            += nTraceBackOutput;
                    TBQrit            &= RQ_MASK;
                }


                tend = tick_count::now();
                tduration = tend - tbegin;
                printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
            }

            void execute_2_3()
            {
                v_ub vStates[4];
                v_ub vBM0, vBM1;
                v_ub vShuffleMask;
                int  BMIndex = 4;
                unsigned __int16 iPH0, iPH1;
                unsigned __int16 MinAddressIndex = 0;

                vShuffleMask = m_viterbi.m_vShuffleMask;

                for (int i = 0; i < 4; i++)
                {
                    vStates[i].v_setall(128);
                }
                vStates[0].v_set_at<0>(0);


                VitRQ::iter  TBQwit     = 0;
                VitRQ::iter  TBQrit     = 0;
                unsigned int iTrellis   = 0;

                unsigned __int8 *pSoftBits         = m_viterbi.m_pSoftBits;
                unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
                __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                unsigned __int16 TraceBackPt       = 0;

                tick_count tbegin, tend, tend2, tduration, tduration2;

                __int32 nTotalSoftBits = m_viterbi.m_nDecodedBytes * 8 * 3 / 2; // 2/3 coding
                
                tbegin = tick_count::now();

                for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 3)
                {
                    // stage StageIndex
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

                    v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;

                    // stage StageIndex + 1
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

                    pvBM = &m_viterbi.m_vBM0[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;
                    iTrellis += 2;

                    //m_viterbi.v_print(vStates, 4);

                    if (vStates[0].v_get_at<0>() > 192)
                    {
                        //m_viterbi.v_print(vStates, 4);
                        m_viterbi.Normalize(vStates);
                    }

                    if (iTrellis >= nTracebackDataCount)
                    {
                        unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(vStates);
                        //unsigned __int8 MinAddress = 0;

                        TraceBackPt = TBQrit + nTracebackOffset;
                        TraceBackPt &= RQ_MASK;

                        m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                        pDecodedBytes     += nTraceBackOutputByte;
                        TotalDecodedBytes -= nTraceBackOutputByte;

                        TBQrit            += nTraceBackOutput;
                        TBQrit            &= RQ_MASK;
                        iTrellis          -= nTraceBackOutput;
                    }
                }

                if ( iTrellis >= nTraceBackLength )
                {
                    unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);

                    for (unsigned int i = 0; i < iTrellisPadding; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit            += nTraceBackOutput;
                    TBQrit            &= RQ_MASK;
                }

                while (TotalDecodedBytes > 0)
                {
                    // pad traceback length zeros
                    for (unsigned int i = 0; i < nTraceBackOutput; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);
                    
                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit            += nTraceBackOutput;
                    TBQrit            &= RQ_MASK;
                }


                tend = tick_count::now();
                tduration = tend - tbegin;
                printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
            }

            void execute_3_4()
            {
                v_ub vStates[4];
                v_ub vBM0, vBM1;
                v_ub vShuffleMask;
                int  BMIndex = 4;
                unsigned __int16 iPH0, iPH1;
                unsigned __int16 MinAddressIndex = 0;

                vShuffleMask = m_viterbi.m_vShuffleMask;

                for (int i = 0; i < 4; i++)
                {
                    vStates[i].v_setall(128);
                }
                vStates[0].v_set_at<0>(0);


                VitRQ::iter  TBQwit     = 0;
                VitRQ::iter  TBQrit     = 0;
                unsigned int iTrellis   = 0;

                unsigned __int8 *pSoftBits         = m_viterbi.m_pSoftBits;
                unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
                __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
                unsigned __int16 TraceBackPt       = 0;

                tick_count tbegin, tend, tend2, tduration, tduration2;

                __int32 nTotalSoftBits = m_viterbi.m_nDecodedBytes * 8 * 4 / 3; // 3/4 coding

                tbegin = tick_count::now();
                
                for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 8)
                {
                    // stage StageIndex
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

                    v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    //m_viterbi.v_print(vStates, 4);

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;
                    // stage StageIndex + 1
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

                    pvBM = &m_viterbi.m_vBM0[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;

                    //m_viterbi.v_print(vStates, 4);

                    // stage StageIndex + 2
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 3]);

                    pvBM = &m_viterbi.m_vBM1[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;

                    //m_viterbi.v_print(vStates, 4);

                    // stage StageIndex + 3
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 4], pSoftBits[nSoftBits + 5]);

                    pvBM = &m_viterbi.m_vBM[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;

                    //m_viterbi.v_print(vStates, 4);

                    // stage StageIndex + 4
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 6]);

                    pvBM = &m_viterbi.m_vBM0[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][2] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][1] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit += 1;
                    TBQwit &= RQ_MASK;

                    //m_viterbi.v_print(vStates, 4);

                    // stage StageIndex + 5
                    BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 7]);

                    pvBM = &m_viterbi.m_vBM1[BMIndex][0];

                    vBM0 = pvBM[0];
                    vBM1 = pvBM[1];
                    m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][0] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][1] = iPH1;

                    vBM0 = pvBM[2];
                    vBM1 = pvBM[3];
                    m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                    m_viterbi.m_TBQ[TBQwit][2] = iPH0;
                    m_viterbi.m_TBQ[TBQwit][3] = iPH1;

                    TBQwit   += 1;
                    TBQwit   &= RQ_MASK;
                    iTrellis += 6;
                    
                    //m_viterbi.v_print(vStates, 4);

                    if (vStates[0].v_get_at<0>() > 208)
                    {
                        //m_viterbi.v_print(vStates, 4);
                        m_viterbi.Normalize(vStates);
                    }

                    if (iTrellis >= nTracebackDataCount)
                    {
                        unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(vStates);
                        //unsigned __int8 MinAddress = 0;

                        TraceBackPt = TBQrit + nTracebackOffset;
                        TraceBackPt &= RQ_MASK;

                        m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                        pDecodedBytes     += nTraceBackOutputByte;
                        TotalDecodedBytes -= nTraceBackOutputByte;

                        TBQrit            += nTraceBackOutput;
                        TBQrit            &= RQ_MASK;
                        iTrellis          -= nTraceBackOutput;
                    }
                }

                if ( iTrellis >= nTraceBackLength )
                {
                    unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);

                    for (unsigned int i = 0; i < iTrellisPadding; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit            += nTraceBackOutput;
                    TBQrit            &= RQ_MASK;
                }

                while (TotalDecodedBytes > 0)
                {
                    // pad traceback length zeros
                    for (unsigned int i = 0; i < nTraceBackOutput; i++)
                    {
                        m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
                        TBQwit += 1;
                        TBQwit &= RQ_MASK;
                    }

                    TraceBackPt = TBQrit + nTracebackOffset;
                    TraceBackPt &= RQ_MASK;

                    m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

                    pDecodedBytes     += nTraceBackOutputByte;
                    TotalDecodedBytes -= nTraceBackOutputByte;

                    TBQrit            += nTraceBackOutput;
                    TBQrit            &= RQ_MASK;
                }

                tend = tick_count::now();
                tduration = tend - tbegin;
                printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
            }



            dsp_task::task* execute()
            {
                //execute_1_2();
                execute_2_3();
                //execute_3_4();

                return this;
            }
        };

        
        viterbi_traceback_worker m_TBWorker;
        viterbi_acs_worker       m_ACSWorker;
        viterbi_worker           m_Worker;

        dsp_task::cpu_processor  m_TBCPU;
        dsp_task::cpu_processor  m_ACSCPU;


        void Run1()
        {
            m_Worker.execute();
        }


        void Run()
        {
            m_TBQ.Clear();

            m_TBCPU.Enqueue(&m_TBWorker);
            //m_ACSCPU.Enqueue(&m_ACSWorker);

            m_TBCPU.Create();
            //m_ACSCPU.Create();

            //m_ACSWorker.RunA();
            //m_TBWorker.RunA();

            m_ACSWorker.execute();
            //m_ACSWorker.Wait();
            m_TBWorker.Wait();

            m_TBCPU.Destroy();
            m_ACSCPU.Destroy();
        }


        void UnitTest2()
        {
            tick_count tbegin, tend, tduration;

            tbegin = tick_count::now();

            __asm
            {
                mov ecx, 100000000;
donext:
                psubb       xmm0,xmm2;
                pblendvb    xmm1,xmm2,xmm0;
                dec ecx;
                jnz donext;
            }

            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us\n", tduration.us());


            tbegin = tick_count::now();
            __asm
            {
                mov ecx, 100000000;
donext2:
                psubb       xmm0,xmm2;
                pminub      xmm1,xmm2;
                dec ecx;
                jnz donext2;
            }
            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us\n", tduration.us());

            tbegin = tick_count::now();
            __asm
            {
                mov ecx, 100000000;
donext3:
                pcmpgtb     xmm0,xmm2;
                pand        xmm1,xmm0;
                pandn       xmm0,xmm2;
                dec ecx;
                jnz donext3;
            }
            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us\n", tduration.us());
        }

    };
#pragma warning(default: 4355)
}
