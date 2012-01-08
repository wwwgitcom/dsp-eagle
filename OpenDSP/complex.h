#pragma once

namespace OpenDSP
{
    template<typename T, typename R>
    struct _scomplex
    {
        typedef _scomplex<T, R> _Myt;

        static_assert(sizeof(R) == sizeof(T) * 2, "complex size unequal!");
        union
        {
            struct  
            {
                T re, im;
            };
            R _r;        
        };

        _Myt()
        {
            this->re = this->im = 0;
        }

        _Myt(_Myt &c)
        {
            this._r = c._r;
            //this->re = c.re;
            //this->im = c.im;
            //printf("%s\n", __FUNCTION__);
        }

        _Myt(T re, T im)
        {
            this->re = re;
            this->im = im;
        }

        _Myt(R r)
        {
            this->_r = r;
        }

        _Myt& operator=(_Myt &c)
        {
            this->re = c.re;
            this->im = c.im;
            return (*this);
        }
        // this is harmful, because it convert any other type to _Myt forcibly
        template<typename OT>
        _Myt& operator=(OT &c)
        {
            this->operator = ((_Myt&)c);
            //this->re = (*(_Myt*)&c).re;
            //this->im = (*(_Myt*)&c).im;
            //printf("template %s\n", __FUNCTION__);
            return (*this);
        }

        _Myt operator+(_Myt &a)
        {
            _Myt r;
            r.re = a.re + this->re;
            r.im = a.im + this->im;
            //printf("%s\n", __FUNCTION__);
            return  r;
        }

        _Myt& operator+=(_Myt &a)
        {
            this->re += a.re;
            this->im += a.im;
            return (*this);
        }

        _Myt operator-(_Myt &a)
        {
            _Myt r;
            r.re = a.re - re;
            r.im = a.im - im;
            return r;
        }

        _Myt& operator-=(_Myt &a)
        {
            this->re -= a.re;
            this->im -= a.im;
            return (*this);
        }

        void toStringI(_Myt& a)
        {
            printf("[%d,%d]\n", a.re, a.im);
        }
    };

    typedef _scomplex<signed __int8, __int16>               complex8;
    typedef _scomplex<signed __int16, __int32>              complex16;
    typedef _scomplex<signed __int32, __int64>              complex32;
    typedef _scomplex<signed __int64, __m128i>              complex64;
    typedef _scomplex<unsigned __int8, unsigned __int16>    complex8u;
    typedef _scomplex<unsigned __int16, unsigned __int32>   complex16u;
    typedef _scomplex<unsigned __int32, unsigned __int64>   complex32u;
    typedef _scomplex<unsigned __int64, __m128i>            complex64u;
    typedef _scomplex<float, double>                        complexf;
    typedef _scomplex<double, __m128>                       complexd;
}