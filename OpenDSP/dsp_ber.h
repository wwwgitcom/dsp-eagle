#pragma once

namespace OpenDSP
{
    struct dsp_ber 
    {
        static const unsigned __int8 BitsSetTable256[256];

        static double ber(unsigned __int8 *pA, unsigned __int8 *pB, unsigned __int32 N)
        {
            unsigned __int8 v;
            unsigned __int32 TotalCount = 0;
            for (unsigned __int32 i = 0; i < N; i++)
            {
                v = pA[i] ^ pB[i];
                TotalCount += BitsSetTable256[v];
            }

            return (double)TotalCount / ((double)N * 8.0f);
        }
    };
}