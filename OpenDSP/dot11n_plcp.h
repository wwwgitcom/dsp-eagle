#pragma once

#include "dsp_vector1.h"
#include "dsp_ofdm.h"
#include "dsp_crc.h"

using namespace OpenDSP;

struct L_STF
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 256, 64> symbol;
    static const int csd = 2;// 8 samples = 200ns, 40Mhz

    __forceinline void get_stf_1(symbol& output)
    {
        output = (symbol&)_stf;
    }

    __forceinline void get_stf_2(symbol& output)
    {
        symbol& input = (symbol&)_stf;
        int i = 0;
        for (; i < input.vntotal - csd; i++)
        {
            output.vdata[i + csd] = input.vdata[i];
        }
        for (int j = 0; i < input.vntotal; i++, j++)
        {
            output.vdata[j] = input.vdata[i];
        }
    }
    
private:
    static const symbol::type _stf;
};

struct L_LTF
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 256, 64> symbol;
    static const int csd = 2;// 8 samples = 200ns, 40Mhz

    __forceinline void get_ltf_1(symbol& output)
    {
        symbol& input = (symbol&)_ltf;
        output = input;
    }

    __forceinline void get_ltf_2(symbol& output)
    {
        symbol& input = (symbol&)_ltf;
        int i = 0;

        for (; i < input.vnsubcarrier; i++)
        {
            output.vsubcarriers[i] = input.vsubcarriers[i - csd];
        }
        for (i = 0; i < output.vncp; i++)
        {
            output.vcp[i] = output.vsubcarriers[output.vnsubcarrier - output.vncp + i];
        }
    }

private:
    static const symbol::type _ltf;
};


struct HT_STF
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 128, 32> symbol;
    static const int csd = 4;// 16samples = 400ns, 40Mhz

    __forceinline void get_stf_1(symbol& output)
    {
        output = (symbol&)_stf;
    }

    __forceinline void get_stf_2(symbol& output)
    {
        symbol& input = (symbol&)_stf;
        int i = 0;
        for (; i < output.vntotal - csd; i++)
        {
            output.vdata[i + csd] = input.vdata[i];
        }
        for (int j = 0; i < output.vntotal; i++, j++)
        {
            output.vdata[j] = input.vdata[i];
        }
    }

private:
    static const symbol::type _stf;
};

struct HT_LTF
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 128, 32> symbol;
    static const int csd = 4;// 16 samples = 400ns, 40Mhz

    __forceinline void get_ltf_1(symbol& output)
    {
        output = (symbol&)_ltf;
    }

    __forceinline void get_ltf_11(symbol& output)
    {
        symbol &input = ((symbol&)_ltf);
        symbol::velem_type msk = VMASK::__0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF<symbol::velem_type>();

        for (int i = 0; i < input.vntotal; i++)
        {
            output.vdata[i] = v_xor<symbol::velem_type>(msk, input.vdata[i]);
            output.vdata[i] = v_sub(output.vdata[i], msk);
        }
    }

    __forceinline void get_ltf_2(symbol& output)
    {
        symbol& input = (symbol&)_ltf;
        int i = 0;

        for (; i < input.vnsubcarrier; i++)
        {
            output.vsubcarriers[i] = input.vsubcarriers[i - csd];
        }
        for (i = 0; i < output.vncp; i++)
        {
            output.vcp[i] = output.vsubcarriers[output.vnsubcarrier - output.vncp + i];
        }
        //symbol& input = (symbol&)_ltf;
        //int i = 0;
        //for (; i < input.vnsubcarrier - csd; i++)
        //{
        //    output.vsubcarriers[i + csd] = input.vsubcarriers[i];
        //}
        //for (int j = 0; i < input.vnsubcarrier; i++, j++)
        //{
        //    output.vsubcarriers[j] = input.vsubcarriers[i];
        //}
        //for (i = 0; i < input.vncp; i++)
        //{
        //    output.vcp[i] = input.vsubcarriers[input.vnsubcarrier - input.vncp - csd + i];
        //}
    }

private:
    static const symbol::type _ltf;
};

struct HT_DATA
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 128, 32> symbol;
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 64, 16> rxsymbol;
};


///////////////////////////////////////////////////////////////////////////////////////
struct L_SIG 
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 128, 32> symbol;
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 64, 16>  rxsymbol;
    // Rate  Reserved  Length  Parity  Tail
    // 4     1         12      1       6
    static const int size = 3;

    union
    {
        unsigned __int8  cdata[4];
        unsigned __int32 idata;
    };
    symbol localsymbol;
    symbol tempsymbol;
    
    union
    {
        unsigned __int8  codedbits[6];
        unsigned __int16  scodedbits[3];
    };

    __forceinline void clear()
    {
        idata = 0;
    }

    __forceinline void zerotail()
    {
        idata &= 0x0003FFFF; // zero tailing bits
    }

    __forceinline void update(unsigned __int8 bRateCode, unsigned __int16 usLength)
    {
        unsigned __int32 uiParity;

        idata = bRateCode;
        idata |= ((unsigned __int32)(usLength)) << 5;

        uiParity = idata ^ (idata >> 16);
        uiParity ^= uiParity >> 8;
        uiParity ^= uiParity >> 4;
        uiParity ^= uiParity >> 2;
        uiParity ^= uiParity >> 1;
        uiParity &= 0x1;

        idata |= uiParity << 17;
    }
};

struct HT_SIG 
{
    typedef ofdm_symbol<OpenDSP::complex16, OpenDSP::v_cs, 128, 32> symbol;
    static const int size = 6;

    struct sig_format
    {
        unsigned __int8 mcs : 7;
        unsigned __int8 cbw : 1;

        unsigned __int16 length;

        unsigned __int8 smoothing : 1;
        unsigned __int8 notsounding : 1;
        unsigned __int8 reserved : 1; // value is 1
        unsigned __int8 aggregation : 1;
        unsigned __int8 stbc : 2;
        unsigned __int8 feccoding : 1;
        unsigned __int8 shortgi : 1;

        unsigned __int16 nes : 2; // number of extension spatial streams
        unsigned __int16 crc8 : 8;
        unsigned __int16 tail : 6;
    };

    union
    {
        sig_format  fmt;
        unsigned __int8 cdata[6];
        struct
        {
            unsigned __int32 lo4;
            unsigned __int16 hi2;
        };
    };
    union
    {
        unsigned __int8 codedbits[12];
        unsigned __int16 scodedbits[6];
    };
    

    symbol localsymbol1;
    symbol localsymbol2;
    symbol tempsymbol1;
    symbol tempsymbol2;
    symbol tempsymbol3;

    crc::crc8 _crc8;

    __forceinline void clear()
    {
        lo4 = 0; hi2 = 0;
    }

    __forceinline void zerotail()
    {
        hi2 &= 0x03FF; // zero tailing bits
    }

    __forceinline void update(unsigned __int8 msc, unsigned __int16 usLength)
    {
        clear();

        //fmt.mcs       = msc;
        //fmt.reserved  = 1;
        //fmt.feccoding = 0;
        //fmt.length    = usLength;

        cdata[0] = msc;
        *(unsigned __int16 *)&cdata[1] = usLength;
        cdata[3] = 3;
        
        // calc crc8
        _crc8.reset();
        _crc8(cdata, 4);
        _crc8(cdata[4], 2);

        cdata[4] |= (_crc8.value() << 2);
        cdata[5] |= (_crc8.value() >> 6);

        //fmt.crc8 = _crc8.value();
    }
};

struct HT_SERVICE
{
    union
    {
        struct  
        {
            unsigned __int16 Seed : 7 ;
            unsigned __int16 Tail : 9 ;
        };
        struct  
        {
            unsigned __int8 bservice[2];
        };
        unsigned __int16 service;
    };
    unsigned __int8 bscrambled[2];

    __forceinline void clear()
    {
        service = 0;
    }
};