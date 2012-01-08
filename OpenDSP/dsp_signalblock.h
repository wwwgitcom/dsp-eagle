#pragma once

#include "dsp_vector.h"
#include "dsp_vector1.h"


namespace OpenDSP
{
    struct v_signalblock
    {
        static const int v_datacount  = 7;
        static const int v_size       = 128;
        static const int v_datasize   = 112;
        static const int v_headersize = 16;

        unsigned char header[v_headersize];
        v_cs          v_data[v_datacount];
        v_cs&         operator[](int index) { return v_data[index]; }
        const v_cs&   operator[](int index) const { return v_data[index]; }
    };



}