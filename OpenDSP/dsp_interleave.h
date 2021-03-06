#pragma once
#include "dsp_vector1.h"

namespace OpenDSP
{
    namespace interleaver
    {
        template<int nbpsc>
        struct dot11a_interleaver
        {
            static_assert((nbpsc == 1 || nbpsc == 2 || nbpsc == 4 || nbpsc == 6), 
                "nbpsc only takes 1, 2, 4, 6");

            static const int total_sc      = 48;
            static const int bit_per_sc    = nbpsc;
            static const int total_bits    = total_sc * bit_per_sc;
            static const int padding_bits  = total_bits % 8;
            static const int padding_bound = padding_bits ? (1 << padding_bits) : 0;
            static const int padding_byte  = (padding_bits > 0);
            static const int total_bytes   = total_bits / 8 + padding_byte;

            static const int block_bits    = (nbpsc / 2) ? (nbpsc / 2) : 1;
            static const int outbuffer_size = 0 + ((total_bytes > 16) ? 16 : 0)
                + ((total_bytes > 32) ? 16 : 0) + ((total_bytes > 48) ? 16 : 0)
                + ((total_bytes & 0x7F) ? 16 : 0);
            static const int voutbuffer_size = outbuffer_size / v_ub::size;

            struct output_type
            {
                v_ub values[voutbuffer_size];
            };

            v_ub::type lookuptable[total_bytes][256][voutbuffer_size];

            void zero_lookuptable()
            {
                memset(lookuptable, 0, total_bytes * 256 * voutbuffer_size * v_ub::size);
            }

            void create_lookuptable()
            {
                for (int iB = 0; iB < (total_bytes - padding_byte); iB++)
                {
                    for (int v = 0; v < 256; v++)
                    {
                        for (int ib = 0; ib < 8; ib++)
                        {
                            int ibpos = iB * 8 + ib;
                            int obpos = 0;
                            obpos = interleave(ibpos);

                            int oB = obpos / 8;
                            int ob = obpos % 8;

                            unsigned char ibv = (((unsigned char)v & (1 << ib)) >> ib);
                            unsigned char obv = (ibv << ob);
                            lookuptable[iB][v][0][oB] |= obv;
                        }
                    }
                }
            }

            int interleave(int k)
            {
                int temp = round1(k);
                temp     = round2(temp);
                return temp;
            }

            int round1(int k)
            {
                return 3 * bit_per_sc * (k % 16) + k / 16;
            }

            int round2(int k)
            {
                return block_bits * (k / block_bits) + (k + total_bits - (16 * k) / total_bits) % block_bits;
            }
        };

        struct dot11a_interleaver_1bpsc : dot11a_interleaver<1>
        {
            // 48 bits, 6 bytes are valid in the output
            dot11a_interleaver_1bpsc()
            {
                zero_lookuptable();
                create_lookuptable();
            }

            __forceinline void operator()(unsigned char pInput[6], output_type &Output)
            {
                Output.values[0] = (v_ub&)lookuptable[0][pInput[0]][0];
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[1][pInput[1]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[2][pInput[2]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[3][pInput[3]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[4][pInput[4]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[5][pInput[5]][0]);
            }
        };


        //////////////////////////////////////////////////////////////////////////

        template<int nbpsc>
        struct dot11n_interleaver 
        {
            static_assert((nbpsc == 1 || nbpsc == 2 || nbpsc == 4 || nbpsc == 6), 
                "nbpsc only takes 1, 2, 4, 6");

            static const int total_sc      = 52;
            static const int bit_per_sc    = nbpsc;
            static const int total_bits    = total_sc * bit_per_sc;
            static const int padding_bits  = total_bits % 8;
            static const int padding_bound = padding_bits ? (1 << padding_bits) : 0;
            static const int padding_byte  = (padding_bits > 0);
            static const int total_bytes   = total_bits / 8 + padding_byte;

            static const int block_bits    = (nbpsc / 2) ? (nbpsc / 2) : 1;
            static const int outbuffer_size = 0 + ((total_bytes > 16) ? 16 : 0)
                + ((total_bytes > 32) ? 16 : 0) + ((total_bytes > 48) ? 16 : 0)
                + ((total_bytes & 0x7F) ? 16 : 0);
            static const int voutbuffer_size = outbuffer_size / v_ub::size;

            struct output_type
            {
                v_ub values[voutbuffer_size];
            };

            v_ub::type lookuptable[total_bytes][256][voutbuffer_size];

            void zero_lookuptable()
            {
                memset(lookuptable, 0, total_bytes * 256 * voutbuffer_size * v_ub::size);
            }

            void create_lookuptable(int iss)
            {
                for (int iB = 0; iB < (total_bytes - padding_byte); iB++)
                {
                    for (int v = 0; v < 256; v++)
                    {
                        for (int ib = 0; ib < 8; ib++)
                        {
                            int ibpos = iB * 8 + ib;
                            int obpos = 0;
                            obpos = interleave(iss, ibpos);

                            int oB = obpos / 8;
                            int ob = obpos % 8;

                            unsigned char ibv = (((unsigned char)v & (1 << ib)) >> ib);
                            unsigned char obv = (ibv << ob);
                            //printf("%d, %d, %d, %d\n", iB, v, oB, obv);
                            lookuptable[iB][v][0][oB] |= obv;
                            //printf("%d\n", lookuptable[iB][v][0][oB]);
                        }
                    }
                }
            }

            int interleave(int iss, int k)
            {
                int temp = round1(k);
                temp     = round2(temp);
                temp     = round3(iss, temp);
                return temp;
            }

            int round1(int k)
            {
                return 4 * bit_per_sc * (k % 13) + k / 13;
            }

            int round2(int k)
            {
                return block_bits * (k / block_bits) + (k + total_bits - (13 * k) / total_bits) % block_bits;
            }

            int round3(int iss, int k)
            {
                return (total_bits + k - ( ( (iss - 1) * 2 ) % 3 + 3 * ((iss - 1) / 3) ) * 11 * bit_per_sc) % total_bits;
            }
        };


        struct dot11n_interleaver_1bpsc : dot11n_interleaver<1>
        {
            // 52 bits, 6.5 bytes are valid in the output
            dot11n_interleaver_1bpsc(int iss)
            {
                zero_lookuptable();
                zero_lookuptable_padding();
                create_lookuptable(iss);
                create_lookuptable_padding(iss);
            }

            __forceinline void operator()(unsigned char pInput[7], output_type &Output)
            {
                Output.values[0] = (v_ub&)lookuptable[0][pInput[0]][0];
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[1][pInput[1]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[2][pInput[2]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[3][pInput[3]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[4][pInput[4]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[5][pInput[5]][0]);

                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable_padding[pInput[6] & 0x0F][0]);
            }

        protected:
            v_ub::type lookuptable_padding[padding_bound][voutbuffer_size];

            void zero_lookuptable_padding()
            {
                memset(lookuptable_padding, 0, padding_bound * voutbuffer_size * v_ub::size);
            }

            void create_lookuptable_padding(int iss)
            {
                for (int v = 0; v < padding_bound; v++)
                {
                    for (int ib = 0; ib < padding_bits; ib++)
                    {
                        int ibpos = (total_bytes - padding_byte) * 8 + ib;
                        int obpos = 0;
                        obpos = interleave(iss, ibpos);

                        int oB = obpos / 8;
                        int ob = obpos % 8;

                        unsigned char ibv = ((v & (1 << ib)) >> ib);
                        unsigned char obv = (ibv << ob);
                        lookuptable_padding[v][0][oB] |= obv;
                    }
                }

                //printf("interleave test\n");
                for (int i = 0; i < total_bits; i++)
                {
                    int k = interleave(iss, i);
                    //printf("[iss=%d, bpsc=%d]%d==>%d\n", iss, bit_per_sc, i, k);
                }
            }
        };

        struct dot11n_interleaver_2bpsc : dot11n_interleaver<2>
        {
            // 104 bits, 13 bytes are valid in the output
            dot11n_interleaver_2bpsc(int iss)
            {
                zero_lookuptable();
                create_lookuptable(iss);
            }

            __forceinline void operator()(unsigned char pInput[13], output_type &Output)
            {
                Output.values[0] = (v_ub&)lookuptable[0][pInput[0]][0];
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[1][pInput[1]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[2][pInput[2]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[3][pInput[3]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[4][pInput[4]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[5][pInput[5]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[6][pInput[6]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[7][pInput[7]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[8][pInput[8]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[9][pInput[9]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[10][pInput[10]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[11][pInput[11]][0]);
                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[12][pInput[12]][0]);
            }
        };

        struct dot11n_interleaver_4bpsc : dot11n_interleaver<4>
        {
            // 208 bits, 26 bytes are valid in the output
            dot11n_interleaver_4bpsc(int iss)
            {
                zero_lookuptable();
                create_lookuptable(iss);
            }

            __forceinline void operator()(unsigned char pInput[26], output_type &Output)
            {
                Output.values[0] = (v_ub&)lookuptable[0][pInput[0]][0];
                Output.values[1] = (v_ub&)lookuptable[0][pInput[0]][1];

                Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[1][pInput[1]][0]);
                Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[1][pInput[1]][1]);
                
#if 1
                for (int i = 2; i < 26; i += 4)
                {
                    Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[i][pInput[i]][0]);
                    Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[i][pInput[i]][1]);
                    Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[i + 1][pInput[i + 1]][0]);
                    Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[i + 1][pInput[i + 1]][1]);

                    Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[i + 2][pInput[i + 2]][0]);
                    Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[i + 2][pInput[i + 2]][1]);
                    Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[i + 3][pInput[i + 3]][0]);
                    Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[i + 3][pInput[i + 3]][1]);
                }
#endif
            }
        };

        struct dot11n_interleaver_6bpsc : dot11n_interleaver<6>
        {
            // 312 bits, 39 bytes are valid in the output
            dot11n_interleaver_6bpsc(int iss)
            {
                zero_lookuptable();
                create_lookuptable(iss);
            }

            __forceinline void operator()(unsigned char pInput[39], output_type &Output)
            {
                Output.values[0] = (v_ub&)lookuptable[0][pInput[0]][0];
                Output.values[1] = (v_ub&)lookuptable[0][pInput[0]][1];
                Output.values[2] = (v_ub&)lookuptable[0][pInput[0]][2];

                for (int i = 1; i < 39; i += 2)
                {
                    Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[i][pInput[i]][0]);
                    Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[i][pInput[i]][1]);
                    Output.values[2] = v_xor(Output.values[2], (v_ub&)lookuptable[i][pInput[i]][2]);
                    Output.values[0] = v_xor(Output.values[0], (v_ub&)lookuptable[i + 1][pInput[i + 1]][0]);
                    Output.values[1] = v_xor(Output.values[1], (v_ub&)lookuptable[i + 1][pInput[i + 1]][1]);
                    Output.values[2] = v_xor(Output.values[2], (v_ub&)lookuptable[i + 1][pInput[i + 1]][2]);
                }
            }
        };
    }

    namespace deinterleaver
    {
        template<int nbpsc>
        struct dot11n_deinterleaver 
        {
            static_assert((nbpsc == 1 || nbpsc == 2 || nbpsc == 4 || nbpsc == 6), 
                "nbpsc only takes 1, 2, 4, 6");

            static const int total_sc      = 52;
            static const int bit_per_sc    = nbpsc;
            static const int total_bits    = total_sc * bit_per_sc;
            static const int block_bits    = (nbpsc / 2) ? (nbpsc / 2) : 1;
            
            // make sure the number of total_bits can be fit in the range of __int16
            __int16 lookuptable[total_bits];

            dot11n_deinterleaver(int iss)
            {
                zero_lookuptable();
                create_lookuptable(iss);
            }

            __forceinline void operator()(unsigned __int8 *pInput, unsigned __int8* pOutput)
            {
                for (int k = 0; k < total_bits; k++)
                {
                    //printf("%d->%d\n", k, lookuptable[k]);
                    pOutput[ lookuptable[k] ] = pInput[k];
                }
            }

        protected:
            void zero_lookuptable()
            {
                memset(lookuptable, 0, total_bits * sizeof(int));
            }

            void create_lookuptable(int iss)
            {
                for (int i = 0; i < total_bits; i++)
                {
                    lookuptable[i] = (__int16)interleave(iss, i);
                }
            }

            int interleave(int iss, int k)
            {
                int temp = round1(iss, k);
                temp     = round2(temp);
                temp     = round3(temp);
                return temp;
            }

            int round1(int iss, int k)
            {
                return (k + ( ( (iss - 1) * 2 ) % 3 + 3 * ((iss - 1) / 3) ) * 11 * bit_per_sc) % total_bits;
            }

            int round2(int k)
            {
                return block_bits * (k / block_bits) + (k + (13 * k) / total_bits) % block_bits;
            }

            int round3(int k)
            {
                return (k * 13) - (total_bits - 1) * (k / (4 * bit_per_sc));
            }
        };

        struct dot11a_deinterleaver 
        {
            __forceinline void _1bpsc(unsigned __int8 *pInput, unsigned __int8* pOutput)
            {
                pOutput[0]  = pInput[0];
                pOutput[1]  = pInput[3];
                pOutput[2]  = pInput[6];
                pOutput[3]  = pInput[9];
                pOutput[4]  = pInput[12];
                pOutput[5]  = pInput[15];
                pOutput[6]  = pInput[18];
                pOutput[7]  = pInput[21];
                pOutput[8]  = pInput[24];
                pOutput[9]  = pInput[27];
                pOutput[10] = pInput[30];
                pOutput[11] = pInput[33];
                pOutput[12] = pInput[36];
                pOutput[13] = pInput[39];
                pOutput[14] = pInput[42];
                pOutput[15] = pInput[45];
                pOutput[16] = pInput[1];
                pOutput[17] = pInput[4];
                pOutput[18] = pInput[7];
                pOutput[19] = pInput[10];
                pOutput[20] = pInput[13];
                pOutput[21] = pInput[16];
                pOutput[22] = pInput[19];
                pOutput[23] = pInput[22];
                pOutput[24] = pInput[25];
                pOutput[25] = pInput[28];
                pOutput[26] = pInput[31];
                pOutput[27] = pInput[34];
                pOutput[28] = pInput[37];
                pOutput[29] = pInput[40];
                pOutput[30] = pInput[43];
                pOutput[31] = pInput[46];
                pOutput[32] = pInput[2];
                pOutput[33] = pInput[5];
                pOutput[34] = pInput[8];
                pOutput[35] = pInput[11];
                pOutput[36] = pInput[14];
                pOutput[37] = pInput[17];
                pOutput[38] = pInput[20];
                pOutput[39] = pInput[23];
                pOutput[40] = pInput[26];
                pOutput[41] = pInput[29];
                pOutput[42] = pInput[32];
                pOutput[43] = pInput[35];
                pOutput[44] = pInput[38];
                pOutput[45] = pInput[41];
                pOutput[46] = pInput[44];
                pOutput[47] = pInput[47];
            }

            __forceinline void _2bpsc(unsigned __int8 *pInput, unsigned __int8* pOutput)
            {
                pOutput[0] = pInput[0];
                pOutput[1] = pInput[6];
                pOutput[2] = pInput[12];
                pOutput[3] = pInput[18];
                pOutput[4] = pInput[24];
                pOutput[5] = pInput[30];
                pOutput[6] = pInput[36];
                pOutput[7] = pInput[42];
                pOutput[8] = pInput[48];
                pOutput[9] = pInput[54];
                pOutput[10] = pInput[60];
                pOutput[11] = pInput[66];
                pOutput[12] = pInput[72];
                pOutput[13] = pInput[78];
                pOutput[14] = pInput[84];
                pOutput[15] = pInput[90];
                pOutput[16] = pInput[1];
                pOutput[17] = pInput[7];
                pOutput[18] = pInput[13];
                pOutput[19] = pInput[19];
                pOutput[20] = pInput[25];
                pOutput[21] = pInput[31];
                pOutput[22] = pInput[37];
                pOutput[23] = pInput[43];
                pOutput[24] = pInput[49];
                pOutput[25] = pInput[55];
                pOutput[26] = pInput[61];
                pOutput[27] = pInput[67];
                pOutput[28] = pInput[73];
                pOutput[29] = pInput[79];
                pOutput[30] = pInput[85];
                pOutput[31] = pInput[91];
                pOutput[32] = pInput[2];
                pOutput[33] = pInput[8];
                pOutput[34] = pInput[14];
                pOutput[35] = pInput[20];
                pOutput[36] = pInput[26];
                pOutput[37] = pInput[32];
                pOutput[38] = pInput[38];
                pOutput[39] = pInput[44];
                pOutput[40] = pInput[50];
                pOutput[41] = pInput[56];
                pOutput[42] = pInput[62];
                pOutput[43] = pInput[68];
                pOutput[44] = pInput[74];
                pOutput[45] = pInput[80];
                pOutput[46] = pInput[86];
                pOutput[47] = pInput[92];
                pOutput[48] = pInput[3];
                pOutput[49] = pInput[9];
                pOutput[50] = pInput[15];
                pOutput[51] = pInput[21];
                pOutput[52] = pInput[27];
                pOutput[53] = pInput[33];
                pOutput[54] = pInput[39];
                pOutput[55] = pInput[45];
                pOutput[56] = pInput[51];
                pOutput[57] = pInput[57];
                pOutput[58] = pInput[63];
                pOutput[59] = pInput[69];
                pOutput[60] = pInput[75];
                pOutput[61] = pInput[81];
                pOutput[62] = pInput[87];
                pOutput[63] = pInput[93];
                pOutput[64] = pInput[4];
                pOutput[65] = pInput[10];
                pOutput[66] = pInput[16];
                pOutput[67] = pInput[22];
                pOutput[68] = pInput[28];
                pOutput[69] = pInput[34];
                pOutput[70] = pInput[40];
                pOutput[71] = pInput[46];
                pOutput[72] = pInput[52];
                pOutput[73] = pInput[58];
                pOutput[74] = pInput[64];
                pOutput[75] = pInput[70];
                pOutput[76] = pInput[76];
                pOutput[77] = pInput[82];
                pOutput[78] = pInput[88];
                pOutput[79] = pInput[94];
                pOutput[80] = pInput[5];
                pOutput[81] = pInput[11];
                pOutput[82] = pInput[17];
                pOutput[83] = pInput[23];
                pOutput[84] = pInput[29];
                pOutput[85] = pInput[35];
                pOutput[86] = pInput[41];
                pOutput[87] = pInput[47];
                pOutput[88] = pInput[53];
                pOutput[89] = pInput[59];
                pOutput[90] = pInput[65];
                pOutput[91] = pInput[71];
                pOutput[92] = pInput[77];
                pOutput[93] = pInput[83];
                pOutput[94] = pInput[89];
                pOutput[95] = pInput[95];
            }

            __forceinline void _4bpsc(unsigned __int8 *pInput, unsigned __int8* pOutput)
            {
                pOutput[0] = pInput[0];
                pOutput[1] = pInput[13];
                pOutput[2] = pInput[24];
                pOutput[3] = pInput[37];
                pOutput[4] = pInput[48];
                pOutput[5] = pInput[61];
                pOutput[6] = pInput[72];
                pOutput[7] = pInput[85];
                pOutput[8] = pInput[96];
                pOutput[9] = pInput[109];
                pOutput[10] = pInput[120];
                pOutput[11] = pInput[133];
                pOutput[12] = pInput[144];
                pOutput[13] = pInput[157];
                pOutput[14] = pInput[168];
                pOutput[15] = pInput[181];
                pOutput[16] = pInput[1];
                pOutput[17] = pInput[12];
                pOutput[18] = pInput[25];
                pOutput[19] = pInput[36];
                pOutput[20] = pInput[49];
                pOutput[21] = pInput[60];
                pOutput[22] = pInput[73];
                pOutput[23] = pInput[84];
                pOutput[24] = pInput[97];
                pOutput[25] = pInput[108];
                pOutput[26] = pInput[121];
                pOutput[27] = pInput[132];
                pOutput[28] = pInput[145];
                pOutput[29] = pInput[156];
                pOutput[30] = pInput[169];
                pOutput[31] = pInput[180];
                pOutput[32] = pInput[2];
                pOutput[33] = pInput[15];
                pOutput[34] = pInput[26];
                pOutput[35] = pInput[39];
                pOutput[36] = pInput[50];
                pOutput[37] = pInput[63];
                pOutput[38] = pInput[74];
                pOutput[39] = pInput[87];
                pOutput[40] = pInput[98];
                pOutput[41] = pInput[111];
                pOutput[42] = pInput[122];
                pOutput[43] = pInput[135];
                pOutput[44] = pInput[146];
                pOutput[45] = pInput[159];
                pOutput[46] = pInput[170];
                pOutput[47] = pInput[183];
                pOutput[48] = pInput[3];
                pOutput[49] = pInput[14];
                pOutput[50] = pInput[27];
                pOutput[51] = pInput[38];
                pOutput[52] = pInput[51];
                pOutput[53] = pInput[62];
                pOutput[54] = pInput[75];
                pOutput[55] = pInput[86];
                pOutput[56] = pInput[99];
                pOutput[57] = pInput[110];
                pOutput[58] = pInput[123];
                pOutput[59] = pInput[134];
                pOutput[60] = pInput[147];
                pOutput[61] = pInput[158];
                pOutput[62] = pInput[171];
                pOutput[63] = pInput[182];
                pOutput[64] = pInput[4];
                pOutput[65] = pInput[17];
                pOutput[66] = pInput[28];
                pOutput[67] = pInput[41];
                pOutput[68] = pInput[52];
                pOutput[69] = pInput[65];
                pOutput[70] = pInput[76];
                pOutput[71] = pInput[89];
                pOutput[72] = pInput[100];
                pOutput[73] = pInput[113];
                pOutput[74] = pInput[124];
                pOutput[75] = pInput[137];
                pOutput[76] = pInput[148];
                pOutput[77] = pInput[161];
                pOutput[78] = pInput[172];
                pOutput[79] = pInput[185];
                pOutput[80] = pInput[5];
                pOutput[81] = pInput[16];
                pOutput[82] = pInput[29];
                pOutput[83] = pInput[40];
                pOutput[84] = pInput[53];
                pOutput[85] = pInput[64];
                pOutput[86] = pInput[77];
                pOutput[87] = pInput[88];
                pOutput[88] = pInput[101];
                pOutput[89] = pInput[112];
                pOutput[90] = pInput[125];
                pOutput[91] = pInput[136];
                pOutput[92] = pInput[149];
                pOutput[93] = pInput[160];
                pOutput[94] = pInput[173];
                pOutput[95] = pInput[184];
                pOutput[96] = pInput[6];
                pOutput[97] = pInput[19];
                pOutput[98] = pInput[30];
                pOutput[99] = pInput[43];
                pOutput[100] = pInput[54];
                pOutput[101] = pInput[67];
                pOutput[102] = pInput[78];
                pOutput[103] = pInput[91];
                pOutput[104] = pInput[102];
                pOutput[105] = pInput[115];
                pOutput[106] = pInput[126];
                pOutput[107] = pInput[139];
                pOutput[108] = pInput[150];
                pOutput[109] = pInput[163];
                pOutput[110] = pInput[174];
                pOutput[111] = pInput[187];
                pOutput[112] = pInput[7];
                pOutput[113] = pInput[18];
                pOutput[114] = pInput[31];
                pOutput[115] = pInput[42];
                pOutput[116] = pInput[55];
                pOutput[117] = pInput[66];
                pOutput[118] = pInput[79];
                pOutput[119] = pInput[90];
                pOutput[120] = pInput[103];
                pOutput[121] = pInput[114];
                pOutput[122] = pInput[127];
                pOutput[123] = pInput[138];
                pOutput[124] = pInput[151];
                pOutput[125] = pInput[162];
                pOutput[126] = pInput[175];
                pOutput[127] = pInput[186];
                pOutput[128] = pInput[8];
                pOutput[129] = pInput[21];
                pOutput[130] = pInput[32];
                pOutput[131] = pInput[45];
                pOutput[132] = pInput[56];
                pOutput[133] = pInput[69];
                pOutput[134] = pInput[80];
                pOutput[135] = pInput[93];
                pOutput[136] = pInput[104];
                pOutput[137] = pInput[117];
                pOutput[138] = pInput[128];
                pOutput[139] = pInput[141];
                pOutput[140] = pInput[152];
                pOutput[141] = pInput[165];
                pOutput[142] = pInput[176];
                pOutput[143] = pInput[189];
                pOutput[144] = pInput[9];
                pOutput[145] = pInput[20];
                pOutput[146] = pInput[33];
                pOutput[147] = pInput[44];
                pOutput[148] = pInput[57];
                pOutput[149] = pInput[68];
                pOutput[150] = pInput[81];
                pOutput[151] = pInput[92];
                pOutput[152] = pInput[105];
                pOutput[153] = pInput[116];
                pOutput[154] = pInput[129];
                pOutput[155] = pInput[140];
                pOutput[156] = pInput[153];
                pOutput[157] = pInput[164];
                pOutput[158] = pInput[177];
                pOutput[159] = pInput[188];
                pOutput[160] = pInput[10];
                pOutput[161] = pInput[23];
                pOutput[162] = pInput[34];
                pOutput[163] = pInput[47];
                pOutput[164] = pInput[58];
                pOutput[165] = pInput[71];
                pOutput[166] = pInput[82];
                pOutput[167] = pInput[95];
                pOutput[168] = pInput[106];
                pOutput[169] = pInput[119];
                pOutput[170] = pInput[130];
                pOutput[171] = pInput[143];
                pOutput[172] = pInput[154];
                pOutput[173] = pInput[167];
                pOutput[174] = pInput[178];
                pOutput[175] = pInput[191];
                pOutput[176] = pInput[11];
                pOutput[177] = pInput[22];
                pOutput[178] = pInput[35];
                pOutput[179] = pInput[46];
                pOutput[180] = pInput[59];
                pOutput[181] = pInput[70];
                pOutput[182] = pInput[83];
                pOutput[183] = pInput[94];
                pOutput[184] = pInput[107];
                pOutput[185] = pInput[118];
                pOutput[186] = pInput[131];
                pOutput[187] = pInput[142];
                pOutput[188] = pInput[155];
                pOutput[189] = pInput[166];
                pOutput[190] = pInput[179];
                pOutput[191] = pInput[190];
            }

            __forceinline void _6bpsc(unsigned __int8 *pInput, unsigned __int8* pOutput)
            {
                pOutput[0] = pInput[0];
                pOutput[1] = pInput[20];
                pOutput[2] = pInput[37];
                pOutput[3] = pInput[54];
                pOutput[4] = pInput[74];
                pOutput[5] = pInput[91];
                pOutput[6] = pInput[108];
                pOutput[7] = pInput[128];
                pOutput[8] = pInput[145];
                pOutput[9] = pInput[162];
                pOutput[10] = pInput[182];
                pOutput[11] = pInput[199];
                pOutput[12] = pInput[216];
                pOutput[13] = pInput[236];
                pOutput[14] = pInput[253];
                pOutput[15] = pInput[270];
                pOutput[16] = pInput[1];
                pOutput[17] = pInput[18];
                pOutput[18] = pInput[38];
                pOutput[19] = pInput[55];
                pOutput[20] = pInput[72];
                pOutput[21] = pInput[92];
                pOutput[22] = pInput[109];
                pOutput[23] = pInput[126];
                pOutput[24] = pInput[146];
                pOutput[25] = pInput[163];
                pOutput[26] = pInput[180];
                pOutput[27] = pInput[200];
                pOutput[28] = pInput[217];
                pOutput[29] = pInput[234];
                pOutput[30] = pInput[254];
                pOutput[31] = pInput[271];
                pOutput[32] = pInput[2];
                pOutput[33] = pInput[19];
                pOutput[34] = pInput[36];
                pOutput[35] = pInput[56];
                pOutput[36] = pInput[73];
                pOutput[37] = pInput[90];
                pOutput[38] = pInput[110];
                pOutput[39] = pInput[127];
                pOutput[40] = pInput[144];
                pOutput[41] = pInput[164];
                pOutput[42] = pInput[181];
                pOutput[43] = pInput[198];
                pOutput[44] = pInput[218];
                pOutput[45] = pInput[235];
                pOutput[46] = pInput[252];
                pOutput[47] = pInput[272];
                pOutput[48] = pInput[3];
                pOutput[49] = pInput[23];
                pOutput[50] = pInput[40];
                pOutput[51] = pInput[57];
                pOutput[52] = pInput[77];
                pOutput[53] = pInput[94];
                pOutput[54] = pInput[111];
                pOutput[55] = pInput[131];
                pOutput[56] = pInput[148];
                pOutput[57] = pInput[165];
                pOutput[58] = pInput[185];
                pOutput[59] = pInput[202];
                pOutput[60] = pInput[219];
                pOutput[61] = pInput[239];
                pOutput[62] = pInput[256];
                pOutput[63] = pInput[273];
                pOutput[64] = pInput[4];
                pOutput[65] = pInput[21];
                pOutput[66] = pInput[41];
                pOutput[67] = pInput[58];
                pOutput[68] = pInput[75];
                pOutput[69] = pInput[95];
                pOutput[70] = pInput[112];
                pOutput[71] = pInput[129];
                pOutput[72] = pInput[149];
                pOutput[73] = pInput[166];
                pOutput[74] = pInput[183];
                pOutput[75] = pInput[203];
                pOutput[76] = pInput[220];
                pOutput[77] = pInput[237];
                pOutput[78] = pInput[257];
                pOutput[79] = pInput[274];
                pOutput[80] = pInput[5];
                pOutput[81] = pInput[22];
                pOutput[82] = pInput[39];
                pOutput[83] = pInput[59];
                pOutput[84] = pInput[76];
                pOutput[85] = pInput[93];
                pOutput[86] = pInput[113];
                pOutput[87] = pInput[130];
                pOutput[88] = pInput[147];
                pOutput[89] = pInput[167];
                pOutput[90] = pInput[184];
                pOutput[91] = pInput[201];
                pOutput[92] = pInput[221];
                pOutput[93] = pInput[238];
                pOutput[94] = pInput[255];
                pOutput[95] = pInput[275];
                pOutput[96] = pInput[6];
                pOutput[97] = pInput[26];
                pOutput[98] = pInput[43];
                pOutput[99] = pInput[60];
                pOutput[100] = pInput[80];
                pOutput[101] = pInput[97];
                pOutput[102] = pInput[114];
                pOutput[103] = pInput[134];
                pOutput[104] = pInput[151];
                pOutput[105] = pInput[168];
                pOutput[106] = pInput[188];
                pOutput[107] = pInput[205];
                pOutput[108] = pInput[222];
                pOutput[109] = pInput[242];
                pOutput[110] = pInput[259];
                pOutput[111] = pInput[276];
                pOutput[112] = pInput[7];
                pOutput[113] = pInput[24];
                pOutput[114] = pInput[44];
                pOutput[115] = pInput[61];
                pOutput[116] = pInput[78];
                pOutput[117] = pInput[98];
                pOutput[118] = pInput[115];
                pOutput[119] = pInput[132];
                pOutput[120] = pInput[152];
                pOutput[121] = pInput[169];
                pOutput[122] = pInput[186];
                pOutput[123] = pInput[206];
                pOutput[124] = pInput[223];
                pOutput[125] = pInput[240];
                pOutput[126] = pInput[260];
                pOutput[127] = pInput[277];
                pOutput[128] = pInput[8];
                pOutput[129] = pInput[25];
                pOutput[130] = pInput[42];
                pOutput[131] = pInput[62];
                pOutput[132] = pInput[79];
                pOutput[133] = pInput[96];
                pOutput[134] = pInput[116];
                pOutput[135] = pInput[133];
                pOutput[136] = pInput[150];
                pOutput[137] = pInput[170];
                pOutput[138] = pInput[187];
                pOutput[139] = pInput[204];
                pOutput[140] = pInput[224];
                pOutput[141] = pInput[241];
                pOutput[142] = pInput[258];
                pOutput[143] = pInput[278];
                pOutput[144] = pInput[9];
                pOutput[145] = pInput[29];
                pOutput[146] = pInput[46];
                pOutput[147] = pInput[63];
                pOutput[148] = pInput[83];
                pOutput[149] = pInput[100];
                pOutput[150] = pInput[117];
                pOutput[151] = pInput[137];
                pOutput[152] = pInput[154];
                pOutput[153] = pInput[171];
                pOutput[154] = pInput[191];
                pOutput[155] = pInput[208];
                pOutput[156] = pInput[225];
                pOutput[157] = pInput[245];
                pOutput[158] = pInput[262];
                pOutput[159] = pInput[279];
                pOutput[160] = pInput[10];
                pOutput[161] = pInput[27];
                pOutput[162] = pInput[47];
                pOutput[163] = pInput[64];
                pOutput[164] = pInput[81];
                pOutput[165] = pInput[101];
                pOutput[166] = pInput[118];
                pOutput[167] = pInput[135];
                pOutput[168] = pInput[155];
                pOutput[169] = pInput[172];
                pOutput[170] = pInput[189];
                pOutput[171] = pInput[209];
                pOutput[172] = pInput[226];
                pOutput[173] = pInput[243];
                pOutput[174] = pInput[263];
                pOutput[175] = pInput[280];
                pOutput[176] = pInput[11];
                pOutput[177] = pInput[28];
                pOutput[178] = pInput[45];
                pOutput[179] = pInput[65];
                pOutput[180] = pInput[82];
                pOutput[181] = pInput[99];
                pOutput[182] = pInput[119];
                pOutput[183] = pInput[136];
                pOutput[184] = pInput[153];
                pOutput[185] = pInput[173];
                pOutput[186] = pInput[190];
                pOutput[187] = pInput[207];
                pOutput[188] = pInput[227];
                pOutput[189] = pInput[244];
                pOutput[190] = pInput[261];
                pOutput[191] = pInput[281];
                pOutput[192] = pInput[12];
                pOutput[193] = pInput[32];
                pOutput[194] = pInput[49];
                pOutput[195] = pInput[66];
                pOutput[196] = pInput[86];
                pOutput[197] = pInput[103];
                pOutput[198] = pInput[120];
                pOutput[199] = pInput[140];
                pOutput[200] = pInput[157];
                pOutput[201] = pInput[174];
                pOutput[202] = pInput[194];
                pOutput[203] = pInput[211];
                pOutput[204] = pInput[228];
                pOutput[205] = pInput[248];
                pOutput[206] = pInput[265];
                pOutput[207] = pInput[282];
                pOutput[208] = pInput[13];
                pOutput[209] = pInput[30];
                pOutput[210] = pInput[50];
                pOutput[211] = pInput[67];
                pOutput[212] = pInput[84];
                pOutput[213] = pInput[104];
                pOutput[214] = pInput[121];
                pOutput[215] = pInput[138];
                pOutput[216] = pInput[158];
                pOutput[217] = pInput[175];
                pOutput[218] = pInput[192];
                pOutput[219] = pInput[212];
                pOutput[220] = pInput[229];
                pOutput[221] = pInput[246];
                pOutput[222] = pInput[266];
                pOutput[223] = pInput[283];
                pOutput[224] = pInput[14];
                pOutput[225] = pInput[31];
                pOutput[226] = pInput[48];
                pOutput[227] = pInput[68];
                pOutput[228] = pInput[85];
                pOutput[229] = pInput[102];
                pOutput[230] = pInput[122];
                pOutput[231] = pInput[139];
                pOutput[232] = pInput[156];
                pOutput[233] = pInput[176];
                pOutput[234] = pInput[193];
                pOutput[235] = pInput[210];
                pOutput[236] = pInput[230];
                pOutput[237] = pInput[247];
                pOutput[238] = pInput[264];
                pOutput[239] = pInput[284];
                pOutput[240] = pInput[15];
                pOutput[241] = pInput[35];
                pOutput[242] = pInput[52];
                pOutput[243] = pInput[69];
                pOutput[244] = pInput[89];
                pOutput[245] = pInput[106];
                pOutput[246] = pInput[123];
                pOutput[247] = pInput[143];
                pOutput[248] = pInput[160];
                pOutput[249] = pInput[177];
                pOutput[250] = pInput[197];
                pOutput[251] = pInput[214];
                pOutput[252] = pInput[231];
                pOutput[253] = pInput[251];
                pOutput[254] = pInput[268];
                pOutput[255] = pInput[285];
                pOutput[256] = pInput[16];
                pOutput[257] = pInput[33];
                pOutput[258] = pInput[53];
                pOutput[259] = pInput[70];
                pOutput[260] = pInput[87];
                pOutput[261] = pInput[107];
                pOutput[262] = pInput[124];
                pOutput[263] = pInput[141];
                pOutput[264] = pInput[161];
                pOutput[265] = pInput[178];
                pOutput[266] = pInput[195];
                pOutput[267] = pInput[215];
                pOutput[268] = pInput[232];
                pOutput[269] = pInput[249];
                pOutput[270] = pInput[269];
                pOutput[271] = pInput[286];
                pOutput[272] = pInput[17];
                pOutput[273] = pInput[34];
                pOutput[274] = pInput[51];
                pOutput[275] = pInput[71];
                pOutput[276] = pInput[88];
                pOutput[277] = pInput[105];
                pOutput[278] = pInput[125];
                pOutput[279] = pInput[142];
                pOutput[280] = pInput[159];
                pOutput[281] = pInput[179];
                pOutput[282] = pInput[196];
                pOutput[283] = pInput[213];
                pOutput[284] = pInput[233];
                pOutput[285] = pInput[250];
                pOutput[286] = pInput[267];
                pOutput[287] = pInput[287];
            }
        };
    }
}