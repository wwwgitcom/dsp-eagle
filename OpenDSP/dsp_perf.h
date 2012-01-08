#pragma once

#include "stdlib.h"
#include "stdio.h"
#include "dsp_tickcount.h"

namespace OpenDSP
{
    template<int tickcnt>
    struct dsp_perf 
    {
        int m_tickidx;
        int m_tickround;
        tick_count m_ticks[tickcnt];
        tick_count m_ticks_avg[tickcnt];

        dsp_perf()
        {
            perf_reset();
        }

        void perf_tick()
        {
            if ((m_tickidx + 1) == tickcnt)
            {
                printf("perf stack is full!\n");
            }
            else
            {
                m_ticks[m_tickidx] = tick_count::now();
                m_tickidx++;
            }
        }

        void perf_begin()
        {
            m_tickidx   = 0;
            m_tickround++;
        }

        void perf_end()
        {
            for (int i = 1; i < m_tickidx; i++)
            {
                m_ticks_avg[i] += m_ticks[i] - m_ticks[i - 1];
            }
        }

        void perf_report()
        {
            printf("===Perf Report ===\n");
            for (int i = 1; i < m_tickidx; i++)
            {
                printf("perf_%d/%d: %f us\n", i, m_tickround, m_ticks_avg[i].us() / m_tickround);
            }
        }

        void perf_reset()
        {
            m_tickidx   = 0;
            m_tickround = 0;
            memset(m_ticks_avg, 0, tickcnt * sizeof(tick_count));
        }
    };
}
