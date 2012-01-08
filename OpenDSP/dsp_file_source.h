#pragma once
#include "dsp_block.h"
#include "dsp_source.h"

namespace OpenDSP
{
    class dsp_file_source : public dsp_block
    {
    private:
        std::string     m_file_name;
        char*           m_pBufferStart;
        char*           m_pBufferEnd;
        char*           m_pBufferRead;
        psignal_block   m_pSignalBlock;
        int             m_ndecimate;
    protected:
        bool load_file();
    public:
        dsp_file_source (const std::string &file_name);
        ~dsp_file_source();

        void set_decimation(int decimation){m_ndecimate = decimation;}

        dsp_block::state general_work();
        void reset();
    };
}