#include "dsp_scope_sink.h"
#include "dsp_vector1.h"
#include "dsp_tickcount.h"
namespace OpenDSP
{
    dsp_scope_sink::dsp_scope_sink()
        : dsp_sync_block("dsp_scope_sink", dsp_io_signature(1, sizeof(complex16)), dsp_io_signature(0), 64, 0)
    {
        m_pdraw_window1 = new dsp_draw_window("scatter view", 0, 0, 400, 400);
        m_pdraw_window2 = new dsp_draw_window("IQ view", 400, 0, 400, 400);
        m_pdraw_window3 = new dsp_draw_window("energy view", 0, 400, 400, 400);
        m_pdraw_window4 = new dsp_draw_window("FFT view", 400, 400, 400, 400);
    }
    dsp_scope_sink::~dsp_scope_sink()
    {
        if (m_pdraw_window1) delete m_pdraw_window1;
        if (m_pdraw_window2) delete m_pdraw_window2;
        if (m_pdraw_window3) delete m_pdraw_window3;
        if (m_pdraw_window4) delete m_pdraw_window4;
    }
    void dsp_scope_sink::reset()
    {

    }
    dsp_block::state dsp_scope_sink::work()
    {
        dsp_block::state st = dsp_block::READY;
        complex16* ip = input<complex16>(0);
        tick_count t1, t2, intval;

        do 
        {
            if (ninput(0) < 64)
            {
                st = BLKD_IN;
                break;
            }

            std::cout << "scope... ";
                       
            t1 = tick_count::now();
            m_fft((v_cs*)ip, (v_cs*)m_freq);
            m_freq[0].re = m_freq[0].im = 0;

            m_pdraw_window1->DrawScatter(m_freq, 64);
            m_pdraw_window2->DrawComplex(m_freq, 64);
            m_pdraw_window3->DrawSqrt(m_freq, 64);
            m_pdraw_window4->DrawSqrt(m_freq, 64);

            t2 = tick_count::now();
            intval = t2 - t1;
            std::cout << intval.ms() << " ms passed!" << endl;
        } while (false);

        return st;
    }
}