#pragma once
#include "dsp_vector1.h"
#include "dsp_math.h"
#include "math.h"
namespace OpenDSP
{
    struct dsp_freq
    {
        dsp_freq()
        {
        }

        inline void v_print(v_ci &v)
        {
            for (int i = 0; i < v_ci::elem_cnt; i++)
            {
                printf("[%d, %d] ", v[i].re, v[i].im);
            }
            printf("\n");
        }
        inline void v_print(v_cs &v)
        {
            for (int i = 0; i < v_cs::elem_cnt; i++)
            {
                printf("[%d, %d] ", v[i].re, v[i].im);
            }
            printf("\n");
        }

        short estimate_i(complex16* pc, int estcount, int estdist)
        {
            v_ci vsum;
            vsum.v_zero();
            unsigned long nshift = 0;
            _BitScanReverse(&nshift, estcount);

            const v_cs vmsk = VMASK::__0x80000001800000018000000180000001<v_cs>();

            for (int i = 0; i < estcount; i += v_cs::elem_cnt)
            {
                v_cs& va = (v_cs&)pc[i];
                v_cs& vb = (v_cs&)pc[i + estdist];
                v_ci vc;
                v_ci vd;

                v_conjmul2ci(va, vb, vmsk, vc, vd);
                vc = vc.v_shift_right_arithmetic(nshift);
                vd = vd.v_shift_right_arithmetic(nshift);
                vsum = v_add(vc, vsum);
                vsum = v_add(vd, vsum);
            }
            vsum = vsum.v_hsum();

            //printf(" vsum =[%d, %d]\n", vsum[0].re, vsum[0].im);

            short sdeltaf = dsp_math::atan(vsum[0].re, vsum[0].im);
            _BitScanReverse(&nshift, estdist);
            sdeltaf >>= nshift;

            return sdeltaf;
        }

        float estimate_f(complex16* pc, int estcount, int estdist)
        {
            complexf fsum(0.0f, 0.0f);
            float deltaf;

            for (int i = 0; i < estcount; i++)
            {
                complexf f1( pc[i].re, pc[i].im );
                complexf f2( pc[i + estdist].re, pc[i + estdist].im );

                complexf f3;
                f3.re = f1.re * f2.re + f1.im * f2.im;
                f3.im = f1.im * f2.re - f1.re * f2.im;
                fsum += f3;
            }
            //printf(" sum =[%f, %f]\n", fsum.re, fsum.im);

            //deltaf = -atan(fsum.im / fsum.re) * 1000000 / (2.0f * 3.1415926f * 3.2f);//in hz
            deltaf = atan(fsum.im / fsum.re) / (estdist);//in rad

            return deltaf;
        }

        __forceinline void v_make_coeff_f(v_cf* pvcf, int vcount, v_f& vdeltaf, v_f& vstep, v_f& vthetaf)
        {
            v_f t, vsin, vcos, _vdeltaf, _vstep, _vthetaf;
            int j      = 0;
            _vdeltaf   = vdeltaf;
            _vstep     = vstep;
            _vthetaf   = vthetaf;
            // vdeltaf = 3*deltaf, 2*deltaf, 1*deltaf, 0*deltaf
            // vstep   = 4*deltaf, 4*deltaf, 4*deltaf, 4*deltaf
            for (int i = 0; i < vcount; i++)
            {
                t = v_sub(_vdeltaf, _vthetaf);
                t.v_sincos(vsin, vcos);
                pvcf[j]     = v_unpacklo(vcos, vsin);
                pvcf[j + 1] = v_unpackhi(vcos, vsin);
                j          += 2;

                _vdeltaf = v_add(_vdeltaf, _vstep);
            }
            vdeltaf = _vdeltaf;
        }


        // vdeltaf = 7*deltaf, 6*deltaf, 5*deltaf, 4*deltaf 3*deltaf, 2*deltaf, 1*deltaf, 0*deltaf
        // vstep   = 8*deltaf, 8*deltaf, 8*deltaf, 8*deltaf, 8*deltaf, 8*deltaf, 8*deltaf, 8*deltaf
        void v_make_coeff_i(v_cs* pvcs, int vcount, v_s &vdeltaf, v_s &vstep, v_s &vthetaf)
        {
            v_s _vdeltaf = vdeltaf;
            v_s vsrad;
            v_cs vout;
            short v;
            int j = 0;
            for (int i = 0; i < vcount; i++)
            {
                vsrad       = v_sub(_vdeltaf, vthetaf);
                
                v           = vsrad.v_get_at<0>(); vout.v_set_at<0>( dsp_math::sincos( v ) );
                v           = vsrad.v_get_at<1>(); vout.v_set_at<1>( dsp_math::sincos( v ) );
                v           = vsrad.v_get_at<2>(); vout.v_set_at<2>( dsp_math::sincos( v ) );
                v           = vsrad.v_get_at<3>(); vout.v_set_at<3>( dsp_math::sincos( v ) );
                
                pvcs[j]     = vout;
                
                v           = vsrad.v_get_at<4>(); vout.v_set_at<0>( dsp_math::sincos( v ) );
                v           = vsrad.v_get_at<5>(); vout.v_set_at<1>( dsp_math::sincos( v ) );
                v           = vsrad.v_get_at<6>(); vout.v_set_at<2>( dsp_math::sincos( v ) );
                v           = vsrad.v_get_at<7>(); vout.v_set_at<3>( dsp_math::sincos( v ) );
                
                pvcs[j + 1] = vout;

                j          += 2;
                _vdeltaf    = v_add(_vdeltaf, vstep);
            }
            vdeltaf = _vdeltaf;
        }

        void make_coeff_i(complex16* pcs, int count, short &deltaf, short &step, short &thetaf)
        {
            short _deltaf = deltaf;
            short srad;
            for (int i = 0; i < count; i++)
            {
                srad     = _deltaf - thetaf;
                pcs[i]   = dsp_math::sincos( srad );
                _deltaf += step;
            }
            deltaf = _deltaf;
        }

        void make_coeff_f(complexf* pcf, int count, float &deltaf, float &step, float &thetaf)
        {
            float _deltaf = deltaf;
            float frad;
            for (int i = 0; i < count; i++)
            {
                frad      = _deltaf - thetaf;
                //pcf[i].re = cos(2 * 3.1415926f * -deltaf * nt / 20000000.0f - avgf);
                //pcf[i].im = sin(2 * 3.1415926f * -deltaf * nt / 20000000.0f - avgf);
                pcf[i].re = cos( frad );
                pcf[i].im = sin( frad );
                _deltaf  += step;
                //printf("%f\t%f\n", pcf[i].re, pcf[i].im);
            }
            deltaf = _deltaf;
        }

        void compensate_i(complex16* pcsin, complex16* pcoeff, complex16* pcsout, int count)
        {
            __int32 ire, iim;
            for (int i = 0; i < count; i++)
            {
                ire = (__int32)pcsin[i].re * (__int32)pcoeff[i].re - (__int32)pcsin[i].im * (__int32)pcoeff[i].im;
                iim = (__int32)pcsin[i].re * (__int32)pcoeff[i].im + (__int32)pcsin[i].im * (__int32)pcoeff[i].re;

                //ire += (1 << 15);
                //iim += (1 << 15);

                pcsout[i].re = (ire >> 15);
                pcsout[i].im = (iim >> 15);
            }
        }

        void v_compensate_i(v_cs* pvcsin, v_cs* pvcscoeff, v_cs* pvcsout, int vcount)
        {
            v_ci vciout1, vciout2;
            const v_cs vmsk = VMASK::__0x80000001800000018000000180000001<v_cs>();

            for (int i = 0; i < vcount; i++)
            {
                v_cs &vin   = (v_cs &)pvcsin[i];
                v_cs &vcof  = (v_cs &)pvcscoeff[i];

                v_mul2ci(vin, vcof, vmsk, vciout1, vciout2);

                vciout1 = vciout1.v_shift_right_arithmetic(15);
                vciout2 = vciout2.v_shift_right_arithmetic(15);

                v_cs &vout  = (v_cs &)pvcsout[i];
                vout        = v_convert2cs(vciout1, vciout2);
            }
        }


        void v_compensate_f(v_cs* pvcsin, v_cf* pvcoeff, v_cf* pvcfout, int vcount)
        {
            for (int i = 0, j = 0; i < vcount; i++, j += 2)
            {
                v_ci vciinput1, vciinput2;
                v_cf vcfinput1, vcfinput2;
                v_cs& vcsinput = pvcsin[i];

                vcsinput.v_convert2ci(vciinput1, vciinput2);

                vcfinput1        = vciinput1.v_convert2cf();
                vcfinput2        = vciinput2.v_convert2cf();

                v_cf& vcfcoeff1  = pvcoeff[j];
                v_cf& vcfcoeff2  = pvcoeff[j + 1];
                v_cf& vcfoutput1 = pvcfout[j];
                v_cf& vcfoutput2 = pvcfout[j + 1];

                vcfoutput1       = v_mul(vcfinput1, vcfcoeff1);
                vcfoutput2       = v_mul(vcfinput2, vcfcoeff2);
            }
        }


        void compensate_f(complex16* pcsin, complexf* pcoeff, complexf* pcfout, int count)
        {
#if 0
            for (int i = 0; i < count; i += 4)
            {
                v_ci vciinput1, vciinput2;
                v_cf vcfinput1, vcfinput2;
                v_cs& vcsinput = (v_cs&)pcsin[i];
                vcsinput.v_convert2ci(vciinput1, vciinput2);
                vcfinput1        = vciinput1.v_convert2cf();
                vcfinput2        = vciinput2.v_convert2cf();
                v_cf& vcfcoeff1  = (v_cf&)pcoeff[i];
                v_cf& vcfcoeff2  = (v_cf&)pcoeff[i + 2];
                v_cf& vcfoutput1 = (v_cf&)pcfout[i];
                v_cf& vcfoutput2 = (v_cf&)pcfout[i + 2];
                vcfoutput1       = v_mul(vcfinput1, vcfcoeff1);
                vcfoutput2       = v_mul(vcfinput2, vcfcoeff2);
            }
#else
            for (int i = 0; i < count; i++)
            {
                complexf fc( pcsin[i].re, pcsin[i].im );
                pcfout[i] = fc * pcoeff[i];
            }
#endif
        }
    };
}