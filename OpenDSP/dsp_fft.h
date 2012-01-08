#pragma once

#include <fftw3.h>
#include "_fft_r4dif.h"
#include "_ifft_r4dif.h"
#include "dsp_vector1.h"

namespace OpenDSP
{
    struct fft_f
    {
        fft_f(int Size, int Type)
        {
            this->Size = Size;
            this->Sign = Type;
        }

        void operator()(fftwf_complex* pInput, fftwf_complex* pOutput)
        {
            p = fftwf_plan_dft_1d(Size, pInput, pOutput, Sign, FFTW_ESTIMATE);
            fftwf_execute(p);
            fftwf_destroy_plan(p);
        }

        enum
        {
            FFT  = FFTW_FORWARD,
            IFFT = FFTW_BACKWARD
        };

        int Size;
        int Sign;
        fftwf_plan p;
    };

    template<int N>
    struct fft_i
    {
        __forceinline void operator()(v_cs * pInput, v_cs * pOutput)
        {
            FFT<N>(reinterpret_cast<vcs *>(pInput), reinterpret_cast<vcs *>(pOutput));
        }
    };

    template<int N>
    struct ifft_i
    {
        __forceinline void operator()(v_cs * pInput, v_cs * pOutput)
        {
            IFFT<N>(reinterpret_cast<vcs *>(pInput), reinterpret_cast<vcs *>(pOutput));
        }
    };
}
