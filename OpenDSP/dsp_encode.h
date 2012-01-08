#pragma once
#include "dsp_vector1.h"
#include "stdio.h"

namespace OpenDSP
{
    namespace ldpc
    {
        template<int codword_length, int a, int b>
        struct encoder
        {
            const static int codeword_length                   = codword_length;

            const static int paritybits_length                 = codeword_length * (b - a) / b;
            const static int paritybits_tailing                = (paritybits_length % 8);
            const static int paritybytes_padding               = paritybits_tailing > 0;
            const static int paritybytes_length                = (paritybits_length / 8) + paritybytes_padding;
            const static int vparitybytes_tailing              = (paritybytes_length % 16);
            const static int vparitybytes_padding              = vparitybytes_tailing > 0;
            const static int vparitybytes_length               = (paritybytes_length / 16) + vparitybytes_padding;

            const static int infobits_length                   = codeword_length * a / b;
            const static int infobytes_tailing                 = (infobits_length % 8);
            const static int infobytes_padding                 = infobytes_tailing > 0;
            const static int infobytes_length                  = infobits_length / 8 + infobytes_padding;    
            const static int vinfobytes_tailing                = (infobytes_length % 16);
            const static int vinfobytes_padding                = vinfobytes_tailing > 0;
            const static int vinfobytes_length                 = (infobytes_length / 16) + vinfobytes_padding;

            const static int paritymatrixbits_row              = paritybits_length;
            const static int paritymatrixbits_col              = codeword_length;
            const static int paritymatrixbits_col_tailing      = paritymatrixbits_col % 8;
            const static int paritymatrixbytes_col_padding     = paritymatrixbits_col_tailing > 0;
            const static int paritymatrixbytes_col             = paritymatrixbits_col / 8 + paritymatrixbytes_col_padding;    
            const static int vparitymatrix_row                 = paritymatrixbits_row;
            const static int vparitymatrixbytes_col_tailing    = paritymatrixbytes_col % 16;
            const static int vparitymatrixbytes_col_padding    = vparitymatrixbytes_col_tailing > 0;
            const static int vparitymatrixbytes_col            = paritymatrixbytes_col / 16 + vparitymatrixbytes_col_padding;
            const static int vparitymatrix_col                 = vparitymatrixbytes_col;

            const static int generatormatrixbits_row           = infobits_length;
            const static int generatormatrixbits_col           = paritybits_length;
            const static int generatormatrixbits_col_tailing   = generatormatrixbits_col % 8;
            const static int generatormatrixbytes_col_padding  = generatormatrixbits_col_tailing > 0;
            const static int generatormatrixbytes_col          = generatormatrixbits_col / 8 + generatormatrixbytes_col_padding;
            const static int vgeneratormatrix_row              = generatormatrixbits_row;
            const static int vgeneratormatrixbytes_col_tailing = generatormatrixbytes_col % 16;
            const static int vgeneratormatrixbytes_col_padding = vgeneratormatrixbytes_col_tailing > 0;
            const static int vgeneratormatrixbytes_col         = generatormatrixbytes_col / 16 + vgeneratormatrixbytes_col_padding;
            const static int vgeneratormatrix_col              = vgeneratormatrixbytes_col;

            const static int vgeneratormatrix_size = vgeneratormatrix_row * vgeneratormatrix_col * sizeof(v_ub);

            const static v_ub::type vgeneratormatrix[vgeneratormatrix_row][vgeneratormatrix_col];

            v_ub v_parity[vgeneratormatrix_col];

            v_ub v_infobytes[vinfobytes_length];

            __forceinline void encode(unsigned __int8 *pubInput, int nInput);
        };

        template<int codword_length, int a, int b>
        const v_ub::type ldpc::encoder<codword_length, a, b>::vgeneratormatrix[vgeneratormatrix_row][vgeneratormatrixbytes_col] = 
        {

        };

#include "_ldpc_generator_matrix.h"

        template<>
        __forceinline void ldpc::encoder<648, 1, 2>::encode(unsigned __int8 *pubInput, int nInput)
        {
            for (int i = 0; i < nInput; i++)
            {
#if 0
                for (int j = 0; j < 8; j++)
                {
                    if ( (pubInput[i] >> j) & 0x1 )
                    {
                        v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + j][0]);
                        v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + j][1]);
                        v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + j][2]);
                    }
                }
#else
                if ( (pubInput[i] ) & 0x1 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8][2]);
                }

                if ( (pubInput[i] ) & 0x2 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 1][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 1][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 1][2]);
                }

                if ( (pubInput[i] ) & 0x4 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 2][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 2][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 2][2]);
                }

                if ( (pubInput[i] ) & 0x8 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 3][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 3][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 3][2]);
                }

                if ( (pubInput[i] ) & 0x10 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 4][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 4][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 4][2]);
                }

                if ( (pubInput[i] ) & 0x20 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 5][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 5][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 5][2]);
                }

                if ( (pubInput[i] ) & 0x40 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 6][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 6][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 6][2]);
                }

                if ( (pubInput[i] ) & 0x80 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 7][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 7][1]);
                    v_parity[2]  = v_xor(v_parity[2], *(v_ub*)&vgeneratormatrix[i * 8 + 7][2]);
                }
#endif
            }
        }

        template<>
        __forceinline void ldpc::encoder<648, 2, 3>::encode(unsigned __int8 *pubInput, int nInput)
        {
            for (int i = 0; i < nInput; i++)
            {
#if 0
                for (int j = 0; j < 8; j++)
                {

                    if ( (pubInput[i] >> j) & 0x1 )
                    {
                        v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + j][0]);
                        v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + j][1]);
                    }

                }
#else 
                if ( (pubInput[i] ) & 0x1 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8][1]);
                }

                if ( (pubInput[i] ) & 0x2 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 1][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 1][1]);
                }

                if ( (pubInput[i] ) & 0x4 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 2][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 2][1]);
                }

                if ( (pubInput[i] ) & 0x8 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 3][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 3][1]);
                }

                if ( (pubInput[i] ) & 0x10 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 4][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 4][1]);
                }

                if ( (pubInput[i] ) & 0x20 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 5][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 5][1]);
                }

                if ( (pubInput[i] ) & 0x40 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 6][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 6][1]);
                }

                if ( (pubInput[i] ) & 0x80 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 7][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 7][1]);
                }
#endif
            }
        }

        template<>
        __forceinline void ldpc::encoder<648, 3, 4>::encode(unsigned __int8 *pubInput, int nInput)
        {
            for (int i = 0; i < nInput; i++)
            {
#if 0
                for (int j = 0; j < 8; j++)
                {

                    if ( (pubInput[i] >> j) & 0x1 )
                    {
                        v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + j][0]);
                        v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + j][1]);
                    }
                }
#else
                if ( (pubInput[i] ) & 0x1 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8][1]);
                }

                if ( (pubInput[i] ) & 0x2 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 1][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 1][1]);
                }

                if ( (pubInput[i] ) & 0x4 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 2][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 2][1]);
                }

                if ( (pubInput[i] ) & 0x8 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 3][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 3][1]);
                }

                if ( (pubInput[i] ) & 0x10 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 4][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 4][1]);
                }

                if ( (pubInput[i] ) & 0x20 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 5][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 5][1]);
                }

                if ( (pubInput[i] ) & 0x40 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 6][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 6][1]);
                }

                if ( (pubInput[i] ) & 0x80 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 7][0]);
                    v_parity[1]  = v_xor(v_parity[1], *(v_ub*)&vgeneratormatrix[i * 8 + 7][1]);
                }
#endif
            }
        }

        template<>
        __forceinline void ldpc::encoder<648, 5, 6>::encode(unsigned __int8 *pubInput, int nInput)
        {
            for (int i = 0; i < nInput; i++)
            {
#if 0
                for (int j = 0; j < 8; j++)
                {
                    if ( (pubInput[i] >> j) & 0x1 )
                    {
                        v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + j][0]);
                    }
                }
#else
                if ( (pubInput[i] ) & 0x1 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8][0]);
                }

                if ( (pubInput[i] ) & 0x2 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 1][0]);
                }

                if ( (pubInput[i] ) & 0x4 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 2][0]);
                }

                if ( (pubInput[i] ) & 0x8 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 3][0]);
                }

                if ( (pubInput[i] ) & 0x10 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 4][0]);
                }

                if ( (pubInput[i] ) & 0x20 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 5][0]);
                }

                if ( (pubInput[i] ) & 0x40 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 6][0]);
                }

                if ( (pubInput[i] ) & 0x80 )
                {
                    v_parity[0]  = v_xor(v_parity[0], *(v_ub*)&vgeneratormatrix[i * 8 + 7][0]);
                }
#endif
            }
        }
    }


    namespace stream_parser
    {
        struct stream_paser_bpsk_2ss
        {
            stream_paser_bpsk_2ss()
            {
                create_lookuptable();
            }

            struct output_type
            {
                // output for two streams
                union 
                {
                    v_ub::type values[2][1];
                    unsigned __int8 cvalues[2][v_ub::size];
                };
            };

            __forceinline void operator()(unsigned __int8 pInput[13], output_type &Output)
            {
                int i = 0, j = 0;

                for (; i < 12; i += 2)
                {
                    unsigned __int8 b1 = pInput[i];
                    unsigned __int8 b2 = pInput[i + 1];
                    Output.cvalues[0][j] = lookuptable[b1][b2][0];
                    Output.cvalues[1][j] = lookuptable[b1][b2][1];
                    ++j;
                }
                Output.cvalues[0][j] = lookuptable2[pInput[i]][0];
                Output.cvalues[1][j] = lookuptable2[pInput[i]][1];
            }

        private:
            static const int lut_size_x = 256;
            static const int lut_size_y = 256;
            unsigned __int8 lookuptable[lut_size_x][lut_size_y][2];
            unsigned __int8 lookuptable2[lut_size_x][2];// lookup table for half byte

            void create_lookuptable()
            {
                auto split = [](unsigned __int8 &b, unsigned __int8 &lb, unsigned __int8 &hb)
                {
                    lb  = b & 0x01;
                    lb |= ((b & 0x4) >> 1);
                    lb |= ((b & 0x10) >> 2);
                    lb |= ((b & 0x40) >> 3);

                    hb  = ((b & 0x02) >> 1);
                    hb |= ((b & 0x8) >> 2);
                    hb |= ((b & 0x20) >> 3);
                    hb |= ((b & 0x80) >> 4);

                    //printf("%02X : h = %02X, l = %02X\n", b, hb, lb);
                };
                // generate the lookuptable for BPSK, 2 spatial stream
                for ( unsigned __int16 i = 0; i <= 255; i++)
                {
                    for ( unsigned __int16 j = 0; j <= 255; j++)
                    {
                        unsigned __int8 b1 = static_cast<unsigned __int8>(i & 0xFF);
                        unsigned __int8 b2 = static_cast<unsigned __int8>(j & 0xFF);
                        unsigned __int8 lb1 = 0, lb2 = 0, hb1 = 0, hb2 = 0;

                        split(b1, lb1, hb1);
                        split(b2, lb2, hb2);

                        lookuptable2[b1][0] = lb1;
                        lookuptable2[b1][1] = hb1;

                        unsigned __int8 vb1 = ((lb2 << 4) | lb1);
                        unsigned __int8 vb2 = ((hb2 << 4) | hb1);

                        lookuptable[b1][b2][0] = vb1;
                        lookuptable[b1][b2][1] = vb2;
                    }
                }
            }
        };

        struct stream_paser_qpsk_2ss
        {
            stream_paser_qpsk_2ss()
            {
                create_lookuptable();
            }

            struct output_type
            {
                // output for two streams
                union 
                {
                    v_ub::type values[2][1];
                    unsigned __int8 cvalues[2][v_ub::size];
                };
            };

            __forceinline void operator()(unsigned __int8 pInput[26], output_type &Output)
            {
                int i = 0, j = 0;

                for (; i < 26; i += 2)
                {
                    Output.cvalues[0][j] = lookuptable[pInput[i]][pInput[i + 1]][0];
                    Output.cvalues[1][j] = lookuptable[pInput[i]][pInput[i + 1]][1];
                    ++j;
                }
            }

        private:
            static const int lut_size_x = 256;
            static const int lut_size_y = 256;
            unsigned __int8 lookuptable[lut_size_x][lut_size_y][2];

            void create_lookuptable()
            {
                auto split = [](unsigned __int8 &b, unsigned __int8 &lb, unsigned __int8 &hb)
                {
                    lb = b & 0x01;
                    lb |= ((b & 0x4) >> 1);
                    lb |= ((b & 0x10) >> 2);
                    lb |= ((b & 0x40) >> 3);

                    hb = ((b & 0x02) >> 1);
                    hb |= ((b & 0x8) >> 2);
                    hb |= ((b & 0x20) >> 3);
                    hb |= ((b & 0x80) >> 4);
                };
                // generate the lookuptable for QPSK, 2 spatial stream
                for ( unsigned __int16 i = 0; i <= 255; i++)
                {
                    for ( unsigned __int16 j = 0; j <= 255; j++)
                    {
                        unsigned __int8 b1 = static_cast<unsigned __int8>(i & 0xFF);
                        unsigned __int8 b2 = static_cast<unsigned __int8>(j & 0xFF);
                        unsigned __int8 lb1 = 0, lb2 = 0, hb1 = 0, hb2 = 0;

                        split(b1, lb1, hb1);
                        split(b2, lb2, hb2);

                        unsigned __int8 vb1 = ((lb2 << 4) | lb1);
                        unsigned __int8 vb2 = ((hb2 << 4) | hb1);

                        lookuptable[b1][b2][0] = vb1;
                        lookuptable[b1][b2][1] = vb2;
                    }
                }
            }
        };

        struct stream_paser_16qam_2ss
        {
            stream_paser_16qam_2ss()
            {
                create_lookuptable();
            }

            struct output_type
            {
                // output for two streams, 
                union 
                {
                    v_ub::type values[2][2];
                    unsigned __int8 cvalues[2][v_ub::size * 2];
                };
            };

            __forceinline void operator()(unsigned __int8 pInput[52], output_type &Output)
            {
                int i = 0, j = 0;

                for (; i < 52; i += 2)
                {
                    Output.cvalues[0][j] = lookuptable[pInput[i]][pInput[i + 1]][0];
                    Output.cvalues[1][j] = lookuptable[pInput[i]][pInput[i + 1]][1];
                    ++j;
                }
            }

        private:
            static const int lut_size_x = 256;
            static const int lut_size_y = 256;
            unsigned __int8 lookuptable[lut_size_x][lut_size_y][2];

            void create_lookuptable()
            {
                auto split = [](unsigned __int8 &b, unsigned __int8 &lb, unsigned __int8 &hb)
                {
                    lb  = b & 0x03;
                    lb |= ((b & 0x30) >> 2);

                    hb  = ((b & 0x0C) >> 2);
                    hb |= ((b & 0xC0) >> 4);
                };
                // generate the lookuptable for 16-qam, 2 spatial stream
                for ( unsigned __int16 i = 0; i <= 255; i++)
                {
                    for ( unsigned __int16 j = 0; j <= 255; j++)
                    {
                        unsigned __int8 b1  = static_cast<unsigned __int8>(i & 0xFF);
                        unsigned __int8 b2  = static_cast<unsigned __int8>(j & 0xFF);
                        unsigned __int8 lb1 = 0, lb2 = 0, hb1 = 0, hb2 = 0;

                        split(b1, lb1, hb1);
                        split(b2, lb2, hb2);

                        unsigned __int8 vb1 = ((lb2 << 4) | lb1);
                        unsigned __int8 vb2 = ((hb2 << 4) | hb1);

                        lookuptable[b1][b2][0] = vb1;
                        lookuptable[b1][b2][1] = vb2;
                    }
                }
            }
        };

        struct stream_paser_64qam_2ss
        {
            stream_paser_64qam_2ss()
            {
                create_lookuptable();
            }

            struct output_type
            {
                // output for two streams, 
                union 
                {
                    v_ub::type values[2][3];
                    unsigned __int8 cvalues[2][v_ub::size * 3];
                };
            };

            __forceinline void operator()(unsigned __int8 pInput[78], output_type &Output)
            {
                unsigned __int16 *psInput      = reinterpret_cast<unsigned __int16*>(&pInput[0]);
                unsigned __int16 sInput        = 0;
                unsigned __int32 iOutput       = 0;

                for (int i = 0, j = 0; i < 39; i += 3, j += 3)
                {
                    unsigned __int32 *piOutput1 = (unsigned __int32 *)&Output.cvalues[0][j];
                    unsigned __int32 *piOutput2 = (unsigned __int32 *)&Output.cvalues[1][j];

                    sInput        = psInput[i] & 0x0FFF;
                    *piOutput1    = lookuptable[sInput][0];
                    *piOutput2    = lookuptable[sInput][1];

                    sInput        = (psInput[i] >> 12) | ((psInput[i + 1] & 0x00FF) << 4);
                    iOutput       = lookuptable[sInput][0];
                    *piOutput1   |= (iOutput << 6);
                    iOutput       = lookuptable[sInput][1];
                    *piOutput2   |= (iOutput << 6);

                    sInput        = (psInput[i + 1] >> 8) | ((psInput[i + 2] & 0x000F) << 8);
                    iOutput       = lookuptable[sInput][0];
                    *piOutput1   |= (iOutput << 12);
                    iOutput       = lookuptable[sInput][1];
                    *piOutput2   |= (iOutput << 12);

                    sInput        = (psInput[i + 2] >> 4);
                    iOutput       = lookuptable[sInput][0];
                    *piOutput1   |= (iOutput << 18);
                    iOutput       = lookuptable[sInput][1];
                    *piOutput2   |= (iOutput << 18);
                }
            }

        private:
            static const int lut_size = 1024 * 4;
            unsigned __int8 lookuptable[lut_size][2];

            void create_lookuptable()
            {
                auto split = [](unsigned __int16 &b, unsigned __int8 &lb, unsigned __int8 &hb)
                {
                    lb  = b & 0x0007;
                    lb |= ((b & 0x01C0) >> 3);

                    hb  = ((b & 0x0038) >> 3);
                    hb |= ((b & 0x0E00) >> 6);
                };
                // generate the lookuptable for 64-qam, 2 spatial stream
                for ( unsigned __int16 i = 0; i < lut_size; i++)
                {
                    unsigned __int8 lb1 = 0, hb1 = 0;

                    split(i, lb1, hb1);

                    lookuptable[i][0] = lb1;
                    lookuptable[i][1] = hb1;
                }
            }
        };        
    }


    namespace convolution
    {
        struct encoder_1_2
        {
            static const int input_size = (0x1 << 14);
            static const int lut_size   = input_size;

            encoder_1_2(unsigned __int16 conv_register) : _conv_register(conv_register){}

            __forceinline unsigned __int16 operator()(unsigned __int8 &ubInput)
            {
                unsigned __int16 usOutput;

                usOutput       = lookup_table[((_conv_register << 8) | ubInput)];
                _conv_register = (ubInput >> 2);

                return usOutput;
            }

            __forceinline void operator()(unsigned __int8 &ubInput, unsigned __int16 &usOutput)
            {
                usOutput       = lookup_table[((_conv_register << 8) | ubInput)];
                _conv_register = (ubInput >> 2);
            }

            void reset(unsigned __int16 conv_register)
            {
                _conv_register = conv_register;
            }

        private:
            unsigned __int16 _conv_register;
            static const unsigned __int16 lookup_table[lut_size];
        };

        struct encoder_2_3
        {
            static const int input_size = (0x1 << 14);
            static const int lut_size   = input_size;

            encoder_2_3(unsigned __int16 conv_register) : _conv_register(conv_register){}

            __forceinline void operator()(unsigned __int8 pubInput[2], unsigned __int8 pubOutput[3])
            {
                unsigned __int16 b1, b2;
                unsigned __int16 l1, l2;

                b1 = (pubInput[0] << 6) | (_conv_register);
                b2 = (pubInput[1] << 6) | (pubInput[0] >> 2);

                l1 = lookup_table[b1];
                l2 = lookup_table[b2];

                pubOutput[0] = l1 & 0xff;
                pubOutput[1] = (l1 >> 8) | (l2 << 4);
                pubOutput[2] = (l2 >> 4);

                _conv_register = (pubInput[1] >> 2);
            }
            
            void reset(unsigned __int16 conv_register)
            {
                _conv_register = conv_register;
            }

        private:
            unsigned __int16 _conv_register;
            static const unsigned __int16 lookup_table[lut_size];
        };

        struct encoder_3_4
        {
            static const int input_size = (0x1 << 12);
            static const int lut_size   = input_size;

            encoder_3_4(unsigned __int16 conv_register) : _conv_register(conv_register){}

            __forceinline void operator()(unsigned __int8 pubInput[3], unsigned __int8 pubOutput[4])
            {
                unsigned char c1, c2, c3;
                unsigned short b1, b2, b3, b4;

                c1 = (unsigned char)(pubInput[0]);
                c2 = (unsigned char)(pubInput[1]);
                c3 = (unsigned char)(pubInput[2]);

                b1 = ((c1 & 0x3F) << 6) | _conv_register;
                b2 = ((c2 & 0xF) << 8) | c1;
                b3 = ((c3 & 0x3) << 10) | (c2 << 2) | (c1 >> 6);
                b4 = (c3 << 4) | (c2 >> 4);

                _conv_register = (c3 >> 2);

                pubOutput[0] = lookup_table[b1];
                pubOutput[1] = lookup_table[b2];
                pubOutput[2] = lookup_table[b3];
                pubOutput[3] = lookup_table[b4];
            }

            void reset(unsigned __int16 conv_register)
            {
                _conv_register = conv_register;
            }

        private:
            unsigned __int16 _conv_register;
            static const unsigned __int8 lookup_table[lut_size];
        };
    }

}