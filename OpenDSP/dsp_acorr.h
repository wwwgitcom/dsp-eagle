#pragma once

#include "dsp_vector1.h"

namespace OpenDSP
{
    struct dsp_acorr_vi
    {
        static const int m_vHisLength = 8;
        int m_vHisIdx;

        v_cs vMulMask;
        v_i  m_vInputEnergy;
        v_ci m_vInputCorr[2];

        v_i  m_vAverageEnergySum;
        v_ci m_vAverageCorrSum[2];

        v_cs m_vHisSample[8];
        v_ci m_vHisCorr[16];
        v_i  m_vHisEnergy[8];

        v_q  m_vCorrEnergy[2];
        v_q  m_vEnergy[2];


        dsp_acorr_vi()
        {
            m_vHisIdx  = 0;
            vMulMask   = VMASK::__0x80000001800000018000000180000001<v_cs>();
        }

        void reset()
        {
            m_vHisIdx  = 0;
        }

        void exec(v_cs& vInput)
        {
            m_vInputEnergy = vInput.v_sqr2i();
            m_vInputEnergy = m_vInputEnergy.v_shift_right_arithmetic(5);

            v_conjmul2ci(vInput, m_vHisSample[m_vHisIdx], vMulMask, m_vInputCorr[0], m_vInputCorr[1]);
            m_vInputCorr[0] = m_vInputCorr[0].v_shift_right_arithmetic(5);
            m_vInputCorr[1] = m_vInputCorr[1].v_shift_right_arithmetic(5);

            m_vHisSample[m_vHisIdx] = vInput;

            v_ci vDeltaCorr[3];

            vDeltaCorr[0] = v_sub(m_vInputCorr[0], m_vHisCorr[2 * m_vHisIdx]);
            vDeltaCorr[1] = v_sub(m_vInputCorr[1], m_vHisCorr[2 * m_vHisIdx + 1]);

            m_vHisCorr[2 * m_vHisIdx]     = m_vInputCorr[0];
            m_vHisCorr[2 * m_vHisIdx + 1] = m_vInputCorr[1];

            vDeltaCorr[2] = vDeltaCorr[0].v_shuffle_with<1, 0>(vDeltaCorr[1]);

            vDeltaCorr[0] = v_add(vDeltaCorr[0], (v_ci)vDeltaCorr[0].v_shift_bytes_left<8>());

            m_vAverageCorrSum[0] = v_add(m_vAverageCorrSum[0], vDeltaCorr[0]);
            m_vAverageCorrSum[1] = v_add(m_vAverageCorrSum[0], vDeltaCorr[1]);
            m_vAverageCorrSum[1] = v_add(m_vAverageCorrSum[1], vDeltaCorr[2]);

            // so far, we get 4 auto correlation results

            m_vCorrEnergy[0] = m_vAverageCorrSum[0].v_sqr2q();
            m_vCorrEnergy[1] = m_vAverageCorrSum[1].v_sqr2q();

            m_vAverageCorrSum[0] = m_vAverageCorrSum[1].v_shuffle<1, 1>();

            v_i vDeltaEnergy;
            vDeltaEnergy = v_sub(m_vInputEnergy, m_vHisEnergy[m_vHisIdx]);

            m_vHisEnergy[m_vHisIdx] = m_vInputEnergy;


            vDeltaEnergy = v_add(vDeltaEnergy, (v_i)vDeltaEnergy.v_shift_bytes_left<4>());
            vDeltaEnergy = v_add(vDeltaEnergy, (v_i)vDeltaEnergy.v_shift_bytes_left<8>());

            m_vAverageEnergySum  = v_add(m_vAverageEnergySum, vDeltaEnergy);

            m_vAverageEnergySum.v_sqr2q(m_vEnergy[0], m_vEnergy[1]);

            m_vAverageEnergySum = m_vAverageEnergySum.v_shuffle<3, 3, 3, 3>();

            m_vHisIdx++;
            m_vHisIdx %= m_vHisLength;
        }
    };

    struct dsp_acorr_f
    {
        static const int hislength = 32;
        int HisPt;

        double Energy;
        double EnergySum;

        complexd Corr;
        complexd CorrSum;

        complexd HisSample[hislength];
        complexd HisCorr[hislength];
        double   HisEnergy[hislength];

        double   xcorrenergysqr[4];
        double   energysqr[4];

        dsp_acorr_f()
        {
            HisPt  = 0;
        }

        void reset()
        {
            HisPt  = 0;
        }

        void exec(v_cs& input)
        {
            for (int i = 0; i < input.elem_cnt; i++)
            {
                complexd fc;
                fc.re = (double)input[i].re;
                fc.im = (double)input[i].im;

                Corr.re = fc.re * HisSample[HisPt].re + fc.im * HisSample[HisPt].im;
                Corr.im = fc.im * HisSample[HisPt].re - fc.re * HisSample[HisPt].im;

                Energy     = fc.re * fc.re + fc.im * fc.im;
                EnergySum -= HisEnergy[HisPt];
                EnergySum += Energy;

                CorrSum -= HisCorr[HisPt];
                CorrSum += Corr;

                HisSample[HisPt] = fc;
                HisCorr[HisPt]   = Corr;
                HisEnergy[HisPt] = Energy;

                energysqr[i]      = EnergySum * EnergySum;

                xcorrenergysqr[i] = CorrSum.re * CorrSum.re + CorrSum.im * CorrSum.im;

                HisPt++;
                HisPt %= hislength;
            }
        }
    };
    struct dsp_acorr_i
    {
        static const int hislength = 32;
        int HisPt;

        int Energy;
        int EnergySum;

        complex32 Corr;
        complex32 CorrSum;

        complex16 HisSample[hislength];
        complex32 HisCorr[hislength];
        int       HisEnergy[hislength];

        dsp_acorr_i()
        {
            HisPt  = 0;
        }

        void reset()
        {
            HisPt  = 0;
        }

        void exec(v_cs& input, v_f& vxcorrenergysum, v_f& venergysum)
        {
            for (int i = 0; i < input.elem_cnt; i++)
            {
                complex32 fc;
                fc.re = (__int32)input[i].re;
                fc.im = (__int32)input[i].im;

                Corr.re = fc.re * HisSample[HisPt].re + fc.im * HisSample[HisPt].im;
                Corr.im = fc.im * HisSample[HisPt].re - fc.re * HisSample[HisPt].im;

                Energy     = fc.re * fc.re + fc.im * fc.im;
                EnergySum -= HisEnergy[HisPt];
                EnergySum += Energy;

                CorrSum -= HisCorr[HisPt];
                CorrSum += Corr;

                HisSample[HisPt] = input[i];
                HisCorr[HisPt]   = Corr;
                HisEnergy[HisPt] = Energy;

                venergysum[i]      = (float)((__int64)EnergySum * (__int64)EnergySum);
                vxcorrenergysum[i] = (float)((__int64)CorrSum.re * (__int64)CorrSum.re + (__int64)CorrSum.im * (__int64)CorrSum.im);

                HisPt++;
                HisPt %= hislength;
            }
        }
    };

    struct dsp_xcorr
    {
        int HisPt;
        int HisCnt;

        v_i  vEnergy;
        v_i  vHisEnergySum;

        v_ci vCorr[2];
        v_ci vHisCorrSum[16];

        v_q  vxcorrenergysum;
        v_q  venergysum;

        v_cs vCyclicTemplate[16][4];
        v_i  vLocalEnergy;

        v_i  vHisEnergy[4];

        void SetLocalTemplate(v_cs *t)
        {
            // row
            for (int n = 0; n < 16; n++)
            {
                // col
                int k = n;
                for (int i = 0; i < 16; i++)
                {
                    complex16* psrc = (complex16*)&t[0];
                    complex16* pdst = (complex16*)&vCyclicTemplate[n][0];

                    pdst[k] = psrc[i];
                    k++;
                    k %= 16;
                }
            }

            vLocalEnergy.v_zero();
            for (int i = 0; i < 4; i++)
            {
                vEnergy = t[i].v_sqr2i();
                vLocalEnergy = v_add(vLocalEnergy, vEnergy);
            }
            vLocalEnergy = vLocalEnergy.v_hsum();
            //printf("LocalTemplateEnergy ");
            //v_print(stdout, vLocalEnergy);
#if 0
            for (int n = 0; n < 16; n++)
            {
                printf("row %d\n", n);
                for (int i = 0; i < 4; i++)
                {
                    v_print(stdout, vCyclicTemplate[n][i]);
                }
            }
#endif
        }

        dsp_xcorr()
        {
            HisCnt = 4;
            HisPt  = 0;
            for (int i = 0; i < 16; i++)
            {
                vHisCorrSum[i].v_zero();
            }
            vHisEnergySum.v_zero();
        }

        template<typename fncallback>
        void exec(v_cs& input, fncallback& fcb)
        {
            for (int i = 0; i < 16; i++)
            {
                v_conjmul2ci(input, vCyclicTemplate[i][HisPt], vCorr[0], vCorr[1]);
                vHisCorrSum[i]     = v_add(vHisCorrSum[i], vCorr[0]);
                vHisCorrSum[i]     = v_add(vHisCorrSum[i], vCorr[1]);

                //printf("==>\n");
                //printf("input "); v_print(stdout, input);
                //printf("vCyclicTemplate "); v_print(stdout, vCyclicTemplate[i][HisPt]);
                //printf("vCorr0 "); v_print(stdout, m_vInputCorr[0]);
                //printf("vCorr1 "); v_print(stdout, m_vInputCorr[1]);
                //printf("vHisCorrSum "); v_print(stdout, vHisCorrSum[i]);
                //printf("<==\n");
            }

            vEnergy = input.v_sqr2i();

            vHisEnergySum = v_add(vHisEnergySum, vEnergy);

            //printf("==>\n");
            //printf("energy "); v_print(stdout, m_vInputEnergy);
            //printf("energysum "); v_print(stdout, vHisEnergySum);
            //printf("<==\n");

            HisPt++;
            if (HisPt == HisCnt)
            {
                __int64 imax = 0;
                __int64 isum = 0;
                int     imaxpos = 0;
                vxcorrenergysum.v_zero();
                for (int i = 0; i < 16; i += 2)
                {
                    v_ci vxcorrsum = v_hsum(vHisCorrSum[i], vHisCorrSum[i + 1]);
#if 0
                    v_print(stdout, vHisCorrSum[i]);
                    v_print(stdout, vHisCorrSum[i + 1]);
                    v_print(stdout, vxcorrsum);
#endif
                    v_q vxcorrenergy = vxcorrsum.v_sqr2q();
                    vxcorrenergysum = v_add(vxcorrenergysum, vxcorrenergy);
                    if (vxcorrenergy[0] > imax)
                    {
                        imax = vxcorrenergy[0];
                        imaxpos = i;
                    }
                    if (vxcorrenergy[1] > imax)
                    {
                        imax = vxcorrenergy[1];
                        imaxpos = i + 1;
                    }
                    //printf("%I64d\n", vxcorrenergy[0]);
                    //printf("%I64d\n", vxcorrenergy[1]);
                    //v_print(stdout, vxcorrenergy);
                }

                vxcorrenergysum = vxcorrenergysum.v_hsum();

                //v_i vienergy = vHisEnergySum.v_hsum();
                //v_print(stdout, vienergy);

                fcb(vxcorrenergysum[0], imax, imaxpos);

                HisPt = 0;
                for (int i = 0; i < 16; i++)
                {
                    vHisCorrSum[i].v_zero();
                }
                vHisEnergySum.v_zero();
            }
        }
    };

    struct dsp_xcorr_f
    {
        void exec(v_cs& v)
        {
            v_cf vf1, vf2;
            v_ci vi1, vi2;

            v.v_convert2ci(vi1, vi2);
            vf1 = vi1.v_convert2cf();
            vf2 = vi2.v_convert2cf();
        }
    };
}