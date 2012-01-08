#pragma once

#include "dsp_vector1.h"
#include "math.h"

namespace OpenDSP
{
    namespace dsp_channel
    {
        struct AWGN
        {

#define IA 16807
#define IM 2147483647
#define AM (1.0f/IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define EPS 1.2e-7f
#define RNMX (1.0f-EPS)
#define PI 3.141592653589793f


            long  iy;
            long  iv[NTAB];
            int   iset;
            float gset;
            long  idum;

            AWGN()
            {
                iy   = 0;
                iset = 0;
                idum = 0;
            }

            float ran1(long *idum)
            {
                int  j;
                long k;

                float temp;

                if (*idum <= 0 || !iy)
                {
                    if (-(*idum) < 1)
                        *idum=1;
                    else
                        *idum = -(*idum);

                    for (j=NTAB+7;j>=0;j--)
                    {
                        k     = (*idum) / IQ;
                        *idum = IA * (*idum - k * IQ) - IR * k;
                        if (*idum < 0)
                            *idum += IM;
                        if (j < NTAB)
                            iv[j] = *idum;
                    }
                    iy=iv[0];
                }

                k     = (*idum) / IQ;
                *idum = IA * (*idum - k * IQ) - IR * k;
                
                if (*idum < 0)
                    *idum += IM;

                j          = iy / NDIV;
                iy         = iv[j];
                iv[j]      = *idum;

                if ((temp  = AM * iy) > RNMX)
                    return RNMX;
                else
                    return temp;
            }

            /* 
            Returns a normally distributed deviate with zero mean and unit variance, using ran1(idum)
            as the source of uniform deviates. 
            */
            float gasdev(long *idum)
            {
                float fac,rsq,v1,v2;

                if (*idum < 0) 
                    iset=0;
                if (iset == 0) 
                {
                    do 
                    {
                        v1        = 2.0f * ran1(idum) - 1.0f;
                        v2        = 2.0f * ran1(idum) - 1.0f;
                        rsq       = v1 * v1 + v2 * v2;
                    } while (rsq >= 1.0f || rsq == 0.0f);

                    fac           = sqrt(-2.0f * log(rsq) / rsq);
                    gset          = v1 * fac;
                    iset          = 1;
                    return (v2 * fac);
                }
                else
                {
                    iset = 0;
                    return gset;
                }
            }


            float gen_rand_uniform(long *idum) 
            {
                return ran1(idum);
            }


            void pass_channel(complex16* psignal, int signallength, float norm_one, float EsN0)
            {
                float sigma;
                sigma = sqrt(0.5f * pow(10.0f, -0.1f * EsN0));

                for (int i = 0; i < signallength; i++)
                {
                    float noisere = norm_one * sigma * gasdev(&idum);
                    float noiseim = norm_one * sigma * gasdev(&idum);

                    psignal[i].re += (__int16)noisere;
                    psignal[i].im += (__int16)noiseim;
                }
            }

            void pass_channel(complexf* psignal, int signallength, float EsN0)
            {
                //float sigma = pow(10.0f, -1.0f * EsN0 / 20.0f);
                //sigma *= sqrt(0.5f);

                float sigma = sqrt(0.5f * pow(10.0f, -0.1f * EsN0));

                for (int i = 0; i < signallength; i++)
                {
                    float noisere = sigma * gasdev(&idum);
                    float noiseim = sigma * gasdev(&idum);

                    psignal[i].re += noisere;
                    psignal[i].im += noiseim;
                }
            }
        };
    }
}
