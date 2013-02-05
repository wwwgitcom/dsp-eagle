#pragma once

namespace OpenDSP
{
    struct dsp_ber 
    {
        static const unsigned __int8 BitsSetTable256[256];

        static double ber(unsigned __int8 *pA, unsigned __int8 *pB, unsigned __int32 N)
        {
            unsigned __int8 v;
            unsigned __int64 TotalCount = 0;
            for (unsigned __int32 i = 0; i < N; i++)
            {
                v = pA[i] ^ pB[i];
                TotalCount += BitsSetTable256[v];
                //printf("a=%02X, b=%02X, c=%02X, n=%d\n", pA[i], pB[i], v, TotalCount);
            }
            //printf("err: %I64d, total=%d\n", TotalCount, N << 3);
            return (double)TotalCount / ((double)N * 8.0f);
        }
    };
}