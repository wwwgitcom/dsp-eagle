#pragma once
#include "dsp_block.h"
#include "dsp_draw.h"
#include "dsp_fft.h"
namespace OpenDSP
{
    class dsp_scope_sink : public dsp_sync_block
    {
    private:
        dsp_draw_window *m_pdraw_window1;
        dsp_draw_window *m_pdraw_window2;
        dsp_draw_window *m_pdraw_window3;
        dsp_draw_window *m_pdraw_window4;
        fft_i<64>        m_fft;
        v_align(64) complex16 m_freq[64];
        
    public:
        dsp_scope_sink();
        ~dsp_scope_sink();

        dsp_block::state work();
        void reset();
    };
}