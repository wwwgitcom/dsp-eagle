#include "dsp_vector1.h"
#include "dsp_file_source.h"
#include <iostream>

namespace OpenDSP
{
    dsp_file_source::dsp_file_source(
        const std::string &file_name)
        : dsp_block("dsp_file_source", dsp_io_signature(0), dsp_io_signature(1, sizeof(complex16))),m_ndecimate(1),
        m_file_name(file_name), m_pBufferStart(NULL), m_pSignalBlock(NULL)
    {
        if (!load_file())
        {
            throw::invalid_argument(m_name + " can not load file " + file_name);
        }
    }

    dsp_file_source::~dsp_file_source()
    {
        if (m_pBufferStart)
        {
            _aligned_free(m_pBufferStart);
            m_pBufferStart = NULL;
        }
    }

    bool dsp_file_source::load_file()
    {
        FILE* hFile;
        fopen_s(&hFile, m_file_name.c_str(), "rb");

        if (NULL == hFile)
        {
            return false;
        }

        fseek(hFile, 0, SEEK_END);

        long file_length = ftell(hFile);

        assert(file_length > 0);

        m_pBufferStart = (char*)_aligned_malloc(file_length, 64);
        assert(m_pBufferStart != NULL);
        
        fseek(hFile, 0, SEEK_SET);
        size_t read_length = fread(m_pBufferStart, sizeof(char), file_length, hFile);

        assert(read_length == file_length);

        m_pBufferRead  = m_pBufferStart;
        m_pBufferEnd   = m_pBufferStart + file_length;
        m_pSignalBlock = (psignal_block)m_pBufferStart;

        std::cout << this->m_name << " read from " << m_file_name << " " << file_length << " bytes." << endl;

        return true;
    }

    void dsp_file_source::reset()
    {

    }

    dsp_block::state dsp_file_source::general_work()
    {
        dsp_block::state st = dsp_block::READY;

        do 
        {
            if (noutput(0) < 28 / m_ndecimate)
            {
                st = dsp_block::BLKD_OUT;
                break;
            }

            complex16* op = output<complex16>(0);
            complex16* p = (complex16*)(((char*)m_pSignalBlock) + 16);
            for (int i = 0; i < 28; i += m_ndecimate)
            {
                *op = p[i];
                op++;
            }
            m_pSignalBlock++;

            if ((char*)m_pSignalBlock >= m_pBufferEnd)
            {
                m_pSignalBlock = (psignal_block)m_pBufferStart;
                std::cout << "source wrap once..." << endl;
            }
            produce(0, 28 / m_ndecimate);
        } while (false);

        return st;
    }

}