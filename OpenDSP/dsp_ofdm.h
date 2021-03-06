#pragma once
#include "stdio.h"
namespace OpenDSP
{
    template<typename T, typename VT, int _nsubcarrier, int _ncp>
    struct __declspec(align(16)) ofdm_symbol
    {
        static const int nsubcarrier  = _nsubcarrier;
        static const int ncp          = _ncp;
        static const int vnsubcarrier = _nsubcarrier / VT::elem_cnt;
        static const int vncp         = _ncp / VT::elem_cnt;

        static const int vntotal      = vncp + vnsubcarrier;
        static const int ntotal       = ncp + nsubcarrier;

        typedef ofdm_symbol<T, VT, _nsubcarrier, _ncp>* iterator;
        typedef ofdm_symbol<T, VT, _nsubcarrier, _ncp>  _Myt;

        typedef T  elem_type;
        typedef VT velem_type;

        typedef __declspec(align(16)) typename T::type type[ncp + nsubcarrier];
        typedef __declspec(align(16)) typename VT      vtype[vncp + vnsubcarrier];

        union
        {
            struct
            {
                __declspec(align(16)) T cp[ncp];
                __declspec(align(16))T subcarriers[nsubcarrier];
            };

            struct
            {
                __declspec(align(16)) T data[ntotal];
            };

            struct
            {
                __declspec(align(16)) VT vcp[vncp];
                __declspec(align(16))VT vsubcarriers[vnsubcarrier];
            };

            struct  
            {
                __declspec(align(16)) VT  vdata[vntotal];
            };
        };

        __forceinline _Myt& operator=(const _Myt& other)
        {
            for (int i = 0; i < vntotal; i++)
            {
                vdata[i].m128i = other.vdata[i].m128i;
            }
            return (*this);
        }

        __forceinline void shiftleft(int n)
        {
            for (int i = 0; i < vntotal; i++)
            {
                vdata[i].m128i = vdata[i].v_shift_left(n).m128i;
            }
        }

        __forceinline _Myt operator^(VT &msk)
        {
            _Myt other;
            for (int i = 0; i < vnsubcarrier; i++)
            {
                other.vsubcarriers[i] = v_xor<VT>(msk, vsubcarriers[i]);
            }
            return other;
        }

        __forceinline _Myt operator>>(int vncs) const
        {
            _Myt output;
            int i = 0;
            for (; i < vnsubcarrier - vncs; i++)
            {
                output.vsubcarriers[i + vncs].m128i = vsubcarriers[i].m128i;
            }
            for (int j = 0; i < vnsubcarrier; i++, j++)
            {
                output.vsubcarriers[j].m128i = vsubcarriers[i].m128i;
            }
            return output;
        }

        __forceinline void csd(_Myt& output, int vncs) const
        {
            int i = 0;
            for (; i < vnsubcarrier - vncs; i++)
            {
                output.vsubcarriers[i + vncs].m128i = vsubcarriers[i].m128i;
            }
            for (int j = 0; i < vnsubcarrier; i++, j++)
            {
                output.vsubcarriers[j].m128i = vsubcarriers[i].m128i;
            }
        }

        __forceinline void assign_with_cp(_Myt& output)
        {
            for (int i = 0; i < vncp; i++)
            {
                output.vcp[i].m128i = vsubcarriers[vnsubcarrier - vncp + i].m128i;
            }
            for (int i = 0; i < vnsubcarrier; i++)
            {
                output.vdata[i].vsubcarriers.m128i = vsubcarriers[i].m128i;
            }
        }

        __forceinline void assign(_Myt& output)
        {
            for (int i = 0; i < vntotal; i++)
            {
                output.vdata[i].m128i = vdata[i].m128i;
            }
        }


        __forceinline void copycp()
        {
            for (int i = 0; i < vncp; i++)
            {
                vcp[i].m128i = vsubcarriers[vnsubcarrier - vncp + i].m128i;
            }
        }

        __forceinline void window()
        {

        }
    };

    struct dot11n_ofdm_pilot
    {
        static const int pilot_size = 127;

        __forceinline void operator()(int iss, int n, __int16 pilots[4])
        {
            int pilot_sign_idx = (n + 3) % 127;
            int pilot_idx = n & 0x3;

            pilots[0] = (_pilot_sign[pilot_sign_idx] * _pilot[pilot_idx][iss][0]);
            pilots[1] = (_pilot_sign[pilot_sign_idx] * _pilot[pilot_idx][iss][1]);
            pilots[2] = (_pilot_sign[pilot_sign_idx] * _pilot[pilot_idx][iss][2]);
            pilots[3] = (_pilot_sign[pilot_sign_idx] * _pilot[pilot_idx][iss][3]);
        }

        __forceinline __int16 operator()(int n)
        {
            return _pilot_sign[n];
        }
        
    private:
        static __int16 _pilot_sign[pilot_size];
        static __int16 _pilot[4][2][4];
    };

    __declspec(selectany) __int16 dot11n_ofdm_pilot::_pilot[4][2][4]=
    {
        { {1, 1, -1, -1}, {1, -1, -1, 1} },
        { {1, -1, -1, 1}, {-1, -1, 1, 1} },
        { {-1, -1, 1, 1}, {-1, 1, 1, -1} },
        { {-1, 1, 1, -1}, {1, 1, -1, -1} },
    };

    __declspec(selectany) __int16 dot11n_ofdm_pilot::_pilot_sign[pilot_size] =
    {
        1,  1,  1, -1, -1, -1,  1, -1,         -1, -1, -1,  1,  1, -1,  1, -1,         -1,  1,  1, -1,  1,  1, -1,  1,         1,  1,  1,  1,  1, -1,  1,  1, 
        1, -1,  1,  1, -1, -1,  1,  1,         1, -1,  1, -1, -1, -1,  1, -1,         1, -1, -1,  1, -1, -1,  1,  1,         1,  1,  1, -1, -1,  1,  1, -1, 
        -1,  1, -1,  1, -1,  1,  1, -1,         -1, -1,  1,  1, -1, -1, -1, -1,         1, -1, -1,  1, -1,  1,  1,  1,         1, -1,  1, -1,  1, -1,  1, -1, 
        -1, -1, -1, -1,  1, -1,  1,  1,         -1,  1, -1,  1,  1,  1, -1, -1,         1, -1, -1, -1,  1,  1,  1, -1,         -1, -1, -1, -1, -1, -1,  1, 
    };
}