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

            complexf cgasdev(long *idum, float var)
            {
              float fac,rsq,v1,v2;
              complexf vc;

              //do 
              //{
              //  v1        = 2.0f * ran1(idum) - 1.0f;
              //  v2        = 2.0f * ran1(idum) - 1.0f;
              //  rsq       = v1 * v1 + v2 * v2;
              //} while (rsq >= 1.0f || rsq == 0.0f);


              v1 = ran1(idum);
              v2 = ran1(idum);
              vc.re = sqrt(-2.0f * log(v1)) * cos(2*3.14*v2) * var;
              vc.im = sqrt(-2.0f * log(v1)) * sin(2*3.14*v2) * var;

              fac = 1.0f;
              //fac           = sqrt(-2.0f * log(rsq) / rsq);

              //vc.re = fac * v2 * var;
              //vc.im = fac * v1 * var;
              return vc;
            }


            float gen_rand_uniform(long *idum) 
            {
                return ran1(idum);
            }


            void pass_channel(complex16* psignal, size_t signallength, float norm_one, float EsN0)
            {
              double sumes = 0.0;
                for (size_t i = 0; i < signallength; i++)
                {
                  double es = (double)psignal[i].re * (double)psignal[i].re + (double)psignal[i].im * (double)psignal[i].im;
                  sumes += es;
                }

                float avges = sumes / signallength;

                float ps = 10 * log10(avges);
                float pn = ps - EsN0;
                float en = pow(10, pn / 10);

                float sigma;
                sigma = sqrt(en / 2);
                
                srand(GetTickCount());
                for (size_t i = 0; i < signallength; i++)
                {
                  //float es = (float)psignal[i].re * (float)psignal[i].re + (float)psignal[i].im * (float)psignal[i].im;
                  //es = 127 * 127;


                  //sigma = sqrt(0.5f * es * pow(10.0f, -0.1f * EsN0));
                  complexf cnoise = cgasdev(&idum, sigma);
                    /*float noisere =  sigma * gasdev(&idum);
                    float noiseim =  sigma * gasdev(&idum);*/
                  /*int isigma = (int)sigma;
                  float noisere = (rand() % (isigma) ) * (rand() % 2 == 0 ? 1 : -1);
                  float noiseim = (rand() % (isigma) ) * (rand() % 2 == 0 ? 1 : -1);
                    float ne = noisere * noisere + noiseim * noiseim;*/
                    //printf("s: =%f, n= %f, s/n=%f\n", es, ne, 10 * log10(es/ ne));

                    psignal[i].re += (__int16)cnoise.re;
                    psignal[i].im += (__int16)cnoise.im;
                }
            }

            void pass_channel(complexf* psignal, size_t signallength, float EsN0)
            {
                //float sigma = pow(10.0f, -1.0f * EsN0 / 20.0f);
                //sigma *= sqrt(0.5f);

                float sigma = sqrt(0.5f * pow(10.0f, -0.1f * EsN0));

                for (size_t i = 0; i < signallength; i++)
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
