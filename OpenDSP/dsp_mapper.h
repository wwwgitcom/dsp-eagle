#pragma once

#include "dsp_vector1.h"
#include "math.h"
#include "stdio.h"

namespace OpenDSP
{
    namespace mapper
    {
        template<typename complexT>
        struct dsp_mapper_bpsk
        {
            static const int lut_size = 256;

            struct output_type
            {
                complexT values[8];
            };
            const complexT one;

            dsp_mapper_bpsk(complexT _one) : one(_one)
            {
                create_lookuptable();
            }

            __forceinline output_type& operator[](unsigned __int8 input)
            {
                return lookuptable[input];
            }

        private:
            output_type lookuptable[lut_size];

            void create_lookuptable()
            {
                for (int i = 0; i < lut_size; i++)
                {
                    unsigned __int8 inputbyte = static_cast<unsigned __int8>(i);
                    for (int j = 0; j < 8; j++)
                    {
                        if ((inputbyte >> j) & 0x01)
                        {
                            lookuptable[i].values[j] = one;
                        }
                        else
                        {
                            lookuptable[i].values[j] = -one;
                        }
                    }
                }
            }

            dsp_mapper_bpsk(){}
            dsp_mapper_bpsk(dsp_mapper_bpsk &){}
        };

        template<typename complexT>
        struct dsp_mapper_qpsk
        {
            static const int lut_size = 256;

            struct output_type
            {
                complexT values[4];
            };
            const complexT one;

            dsp_mapper_qpsk(complexT _one) : one(_one)
            {
                create_lookuptable();
            }

            __forceinline output_type& operator[](unsigned __int8 input)
            {
                return lookuptable[input];
            }

        private:
            output_type lookuptable[lut_size];

            void create_lookuptable()
            {
                for (int i = 0; i < lut_size; i++)
                {
                    unsigned __int8 inputbyte = static_cast<unsigned __int8>(i);
                    for (int j = 0; j < 4; j++)
                    {
                        unsigned __int8 v = ((inputbyte >> (j * 2)) & 0x03);
                        switch (v)
                        {
                        case 0:
                            lookuptable[i].values[j] = -one;
                            break;
                        case 1:
                            lookuptable[i].values[j] = one.conj_im();
                            break;
                        case 2:
                            lookuptable[i].values[j] = one.conj_re();
                            break;
                        case 3:
                            lookuptable[i].values[j] = one;
                            break;
                        }
                    }
                }
            }

            dsp_mapper_qpsk(){}
            dsp_mapper_qpsk(dsp_mapper_qpsk &){}
        };

        template<typename complexT>
        struct dsp_mapper_16qam
        {
            static const int lut_size = 256;

            struct output_type
            {
                complexT values[2];
            };
            const complexT one;

            dsp_mapper_16qam(complexT _one) : one(_one)
            {
                create_lookuptable();
            }

            __forceinline output_type& operator[](unsigned __int8 input)
            {
                return lookuptable[input];
            }

        private:
            output_type lookuptable[lut_size];

            void create_lookuptable()
            {
                for (int i = 0; i < lut_size; i++)
                {
                    unsigned __int8 inputbyte = static_cast<unsigned __int8>(i);
                    for (int j = 0; j < 2; j++)
                    {
                        unsigned __int8 v = ((inputbyte >> (j * 4)) & 0x0F);
                        switch (v)
                        {
                        case 0:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * -3);
                            break;
                        case 1:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * -3);
                            break;
                        case 2:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * -3);
                            break;
                        case 3:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * -3);
                            break;
                        case 4:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * 3);
                            break;
                        case 5:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * 3);
                            break;
                        case 6:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * 3);
                            break;
                        case 7:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * 3);
                            break;
                        case 8:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * -1);
                            break;
                        case 9:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * -1);
                            break;
                        case 10:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * -1);
                            break;
                        case 11:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * -1);
                            break;
                        case 12:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * 1);
                            break;
                        case 13:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * 1);
                            break;
                        case 14:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * 1);
                            break;
                        case 15:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * 1);
                            break;
                        }
                    }
                }
            }

            dsp_mapper_16qam(){}
            dsp_mapper_16qam(dsp_mapper_16qam &){}
        };

        template<typename complexT>
        struct dsp_mapper_64qam
        {
            static const int lut_size = 1024 * 4;

            struct output_type
            {
                complexT values[2];
            };
            const complexT one;

            dsp_mapper_64qam(complexT _one) : one(_one)
            {
                create_lookuptable();
            }

            __forceinline output_type& operator[](unsigned __int16 input)
            {
                return lookuptable[input];
            }

        private:
            output_type lookuptable[lut_size];

            void create_lookuptable()
            {
                for (int i = 0; i < lut_size; i++)
                {
                    unsigned __int16 inputvalue = static_cast<unsigned __int16>(i);
                    for (int j = 0; j < 2; j++)
                    {
                        unsigned __int16 v = ((inputvalue >> (j * 6)) & 0x3F);
                        switch (v)
                        {
                        case 0:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * -7);
                            break;
                        case 1:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * -7);
                            break;
                        case 2:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * -7);
                            break;
                        case 3:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * -7);
                            break;
                        case 4:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * -7);
                            break;
                        case 5:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * -7);
                            break;
                        case 6:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * -7);
                            break;
                        case 7:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * -7);
                            break;

                        case 8:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * 7);
                            break;
                        case 9:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * 7);
                            break;
                        case 10:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * 7);
                            break;
                        case 11:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * 7);
                            break;
                        case 12:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * 7);
                            break;
                        case 13:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * 7);
                            break;
                        case 14:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * 7);
                            break;
                        case 15:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * 7);
                            break;

                        case 16:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * -1);
                            break;
                        case 17:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * -1);
                            break;
                        case 18:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * -1);
                            break;
                        case 19:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * -1);
                            break;
                        case 20:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * -1);
                            break;
                        case 21:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * -1);
                            break;
                        case 22:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * -1);
                            break;
                        case 23:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * -1);
                            break;

                        case 24:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * 1);
                            break;
                        case 25:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * 1);
                            break;
                        case 26:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * 1);
                            break;
                        case 27:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * 1);
                            break;
                        case 28:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * 1);
                            break;
                        case 29:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * 1);
                            break;
                        case 30:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * 1);
                            break;
                        case 31:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * 1);
                            break;

                        case 32:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * -5);
                            break;
                        case 33:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * -5);
                            break;
                        case 34:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * -5);
                            break;
                        case 35:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * -5);
                            break;
                        case 36:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * -5);
                            break;
                        case 37:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * -5);
                            break;
                        case 38:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * -5);
                            break;
                        case 39:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * -5);
                            break;

                        case 40:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * 5);
                            break;
                        case 41:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * 5);
                            break;
                        case 42:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * 5);
                            break;
                        case 43:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * 5);
                            break;
                        case 44:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * 5);
                            break;
                        case 45:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * 5);
                            break;
                        case 46:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * 5);
                            break;
                        case 47:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * 5);
                            break;

                        case 48:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * -3);
                            break;
                        case 49:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * -3);
                            break;
                        case 50:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * -3);
                            break;
                        case 51:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * -3);
                            break;
                        case 52:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * -3);
                            break;
                        case 53:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * -3);
                            break;
                        case 54:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * -3);
                            break;
                        case 55:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * -3);
                            break;

                        case 56:
                            lookuptable[i].values[j] = complexT(one.re * -7, one.im * 3);
                            break;
                        case 57:
                            lookuptable[i].values[j] = complexT(one.re * 7, one.im * 3);
                            break;
                        case 58:
                            lookuptable[i].values[j] = complexT(one.re * -1, one.im * 3);
                            break;
                        case 59:
                            lookuptable[i].values[j] = complexT(one.re * 1, one.im * 3);
                            break;
                        case 60:
                            lookuptable[i].values[j] = complexT(one.re * -5, one.im * 3);
                            break;
                        case 61:
                            lookuptable[i].values[j] = complexT(one.re * 5, one.im * 3);
                            break;
                        case 62:
                            lookuptable[i].values[j] = complexT(one.re * -3, one.im * 3);
                            break;
                        case 63:
                            lookuptable[i].values[j] = complexT(one.re * 3, one.im * 3);
                            break;
                        }
                    }
                }
            }

            dsp_mapper_64qam(){}
            dsp_mapper_64qam(dsp_mapper_64qam &){}
        };
    }

    namespace demapper
    {
        struct dsp_demapper
        {
#define USE_SIGNED_RANGE 0
            /**
            * The constellation uses the gray code
            */
            static const int lut_bpsk_size  = 256;
            static const int lut_qpsk_size  = 256;
            static const int lut_16qam_size = 256;
            static const int lut_64qam_size = 288;

            static const v_cs::type DemapMin;
            static const v_cs::type DemapMax;

            static const v_cs::type Demap64qamMin;
            static const v_cs::type Demap64qamMax;

#if USE_SIGNED_RANGE
            static const __int8 lookup_table_bpsk[lut_bpsk_size];

            static const __int8 lookup_table_qpsk[lut_qpsk_size];

            static const __int8 lookup_table_16qam1[lut_16qam_size];
            static const __int8 lookup_table_16qam2[lut_16qam_size];

            static const __int8 lookup_table_64qam1[lut_64qam_size];
            static const __int8 lookup_table_64qam2[lut_64qam_size];
            static const __int8 lookup_table_64qam3[lut_64qam_size];

            static const __int8 *p_lookup_table_64qam1;
            static const __int8 *p_lookup_table_64qam2;
            static const __int8 *p_lookup_table_64qam3;
#else
            static const unsigned __int8 lookup_table_bpsk[lut_bpsk_size];

            static const unsigned __int8 lookup_table_qpsk[lut_qpsk_size];

            static const unsigned __int8 lookup_table_16qam1[lut_16qam_size];
            static const unsigned __int8 lookup_table_16qam2[lut_16qam_size];

            static const unsigned __int8 lookup_table_64qam1[lut_64qam_size];
            static const unsigned __int8 lookup_table_64qam2[lut_64qam_size];
            static const unsigned __int8 lookup_table_64qam3[lut_64qam_size];

            static const unsigned __int8 *p_lookup_table_64qam1;
            static const unsigned __int8 *p_lookup_table_64qam2;
            static const unsigned __int8 *p_lookup_table_64qam3;
#endif

            dsp_demapper(){}

            __forceinline void demap_limit_bpsk(v_cs* pvcs, int vcount)
            {
                for (int i = 0; i < vcount; i++)
                {
                    pvcs[i] = v_min(v_max((v_s&)pvcs[i], (v_s&)DemapMin), (v_s&)DemapMax);
                }
            }

            __forceinline void demap_limit_qpsk(v_cs* pvcs, int vcount)
            {
                for (int i = 0; i < vcount; i++)
                {
                    pvcs[i] = v_min(v_max((v_s&)pvcs[i], (v_s&)DemapMin), (v_s&)DemapMax);
                }
            }

            __forceinline void demap_limit_16qam(v_cs* pvcs, int vcount)
            {
                for (int i = 0; i < vcount; i++)
                {
                    pvcs[i] = v_min(v_max((v_s&)pvcs[i], (v_s&)DemapMin), (v_s&)DemapMax);
                }
            }

            __forceinline void demap_limit_64qam(v_cs* pvcs, int vcount)
            {
                for (int i = 0; i < vcount; i++)
                {
                    pvcs[i] = v_min(v_max((v_s&)pvcs[i], (v_s&)Demap64qamMin), (v_s&)Demap64qamMax);
                }
            }

            __forceinline void demap_bpsk_i(complex16 &cinput, unsigned __int8* pOutput)
            {
                *pOutput = lookup_table_bpsk[(unsigned __int8)cinput.re];
            }
            __forceinline void demap_bpsk_q(complex16 &cinput, unsigned __int8* pOutput)
            {
                *pOutput = lookup_table_bpsk[(unsigned __int8)cinput.im];
            }

            __forceinline void demap_qpsk(complex16 &cinput, unsigned __int8* pOutput)
            {
                pOutput[0] = lookup_table_qpsk[(unsigned __int8)cinput.re];
                pOutput[1] = lookup_table_qpsk[(unsigned __int8)cinput.im];
            }

            __forceinline void demap_16qam(complex16 &cinput, unsigned __int8* pOutput)
            {
                pOutput[0] = lookup_table_16qam1[(unsigned __int8)cinput.re];
                pOutput[1] = lookup_table_16qam2[(unsigned __int8)cinput.re];
                pOutput[2] = lookup_table_16qam1[(unsigned __int8)cinput.im];
                pOutput[3] = lookup_table_16qam2[(unsigned __int8)cinput.im];
            }

            __forceinline void demap_64qam(complex16 &cinput, unsigned __int8* pOutput)
            {
                pOutput[0] = p_lookup_table_64qam1[cinput.re];
                pOutput[1] = p_lookup_table_64qam2[cinput.re];
                pOutput[2] = p_lookup_table_64qam3[cinput.re];

                pOutput[3] = p_lookup_table_64qam1[cinput.im];
                pOutput[4] = p_lookup_table_64qam2[cinput.im];
                pOutput[5] = p_lookup_table_64qam3[cinput.im];
            }

            static void demodulate_soft_bits_bpsk(complexf *pInput, int nInput, float EbN0, float* pOutput)
            {
                float N0      = pow(10.0f, -0.1f * EbN0) * 0.5f;
                float factor  = 4 / N0;
                float max_llr = 1.0f * factor;

                for (int i = 0; i < nInput; i++)
                {
                    float fInput = pInput[i].re;

                    pOutput[i] = fInput * factor / max_llr;
                }
            }

            static void build_soft_bits_bpsk(float EsN0, unsigned __int8* pOutput)
            {
                int i, d;
                float es, sn_ratio, sigma;

                es = 1.0f;
                sn_ratio = (float) pow(10.0f, ( EsN0 / 10.0f ) );
                sigma    = (float) sqrt( es / ( 2.0f * sn_ratio ) );

                //d = (int) ( 8 * 0.6 * sigma );

                float D = 0.3f * sigma;

                d = (int)(D * 127.0f);
                                
                for (i = -128; i < ( -3 * d ); i++)
                    pOutput[i + 256] = 0;

                for (i = ( -3 * d ); i < ( -2 * d ); i++)
                    pOutput[i + 256] = 1;

                for (i = ( -2 * d ); i < ( -1 * d ); i++)
                    pOutput[i + 256] = 2;

                for (i = ( -1 * d ); i < 0; i++)
                    pOutput[i + 256] = 3;

                for (i = 0; i < ( 1 * d ); i++)
                    pOutput[i + 0] = 4;

                for (i = ( 1 * d ); i < ( 2 * d ); i++)
                    pOutput[i + 0] = 5;

                for (i = ( 2 * d ); i < ( 3 * d ); i++)
                    pOutput[i + 0] = 6;

                for (i = ( 3 * d ); i < 128; i++)
                    pOutput[i + 0] = 7;
            }

            static void demodulate_soft_bits_bpsk(complexf *pInput, int nInput, float EbN0, int* pOutput)
            {
                float N0     = pow(10.0f, -0.1f * EbN0) * 0.5f;
                float factor = 4 / N0;

                float max_llr = 1.0f * factor;
                float min_llr = -1.0f * factor;
                for (int i = 0; i < nInput; i++)
                {
                    float fInput = pInput[i].re;
                    float llr_f = fInput * factor / max_llr;
                    int   llr_i = (int)(llr_f * 4.0f);
                    llr_i += 4;
                    if (llr_i > 7)
                    {
                        llr_i = 7;
                    }
                    if (llr_i < 0)
                    {
                        llr_i = 0;
                    }
                    pOutput[i] = llr_i;
                }
            }
        };
    }
}