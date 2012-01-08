#pragma once

namespace OpenDSP
{
    namespace VBUFFER
    {
        #define MEM_ALIGN(_x) __declspec(align(_x))
        template<size_t VB_DCSIZE>
        struct VB_DCBLOCK
        {
            volatile MEM_ALIGN(64) unsigned int isValid;
            MEM_ALIGN(4) unsigned char data[VB_DCSIZE];

            const static size_t BlockPadding;
            const static size_t BlockOccupying;
        };

        // TODO: Padding length at the end of each VB_DCBLOCK, this make code implementation dependency, better removed.
        // 4 is the alignment of VB_DCBLOCK::data, ie. the allocated space for isValid, which is hard to capsulated by fancy
        // constant expression.
        template<size_t VB_DCSIZE>
        __declspec(selectany) const size_t VB_DCBLOCK<VB_DCSIZE>::BlockPadding = sizeof(VB_DCBLOCK<VB_DCSIZE>)/64*64 - 4 - VB_DCSIZE;
        template<size_t VB_DCSIZE>
        __declspec(selectany) const size_t VB_DCBLOCK<VB_DCSIZE>::BlockOccupying = 4 + VB_DCSIZE;

        template<size_t VB_DCSIZE, size_t VB_DCCOUNT>
        struct VB
        {
            typedef VB_DCBLOCK<VB_DCSIZE> VB_DCBLOCK;
            const static size_t BlockCount = VB_DCCOUNT;
        private:
            VB_DCBLOCK * w_currentBlock;
            VB_DCBLOCK * w_itBlock;

            MEM_ALIGN(64) VB_DCBLOCK blocks[VB_DCCOUNT];
        public:
            VB_DCBLOCK *BlocksBegin()
            {
                return blocks;
            }

            VB_DCBLOCK *BlocksEnd()
            {
                return blocks + VB_DCCOUNT;
            }

            void SpaceWait(size_t nBlock, volatile unsigned int *pbWorkIndicator)
            {
                VB_DCBLOCK *lastBlock = (w_currentBlock - blocks + nBlock - 1) % VB_DCCOUNT + blocks;

                while ((lastBlock->isValid == 1) && *pbWorkIndicator)
                    _mm_pause();
            }

            unsigned char *Push()
            {
                unsigned char *ret = w_currentBlock->data;

                //unsigned char *ret = w_itBlock->data;

                //w_itBlock++;
                //if (w_itBlock == blocks + VB_DCCOUNT)
                //    w_itBlock = blocks;
                return ret;
            }

            void Flush()
            {
                w_currentBlock->isValid = 1;
                w_currentBlock++;
                if (w_currentBlock == blocks + VB_DCCOUNT)
                    w_currentBlock = blocks;
            }

            void Clear()
            {
                unsigned int i;
                w_itBlock = blocks;
                w_currentBlock = blocks;
                for (i = 0; i < VB_DCCOUNT; i++)
                {
                    blocks[i].isValid = 0;
                }
            }
        };
    }


    template<typename _ElemType, typename _IterType, unsigned int _ElemCount>
    struct RingQ
    {
        static_assert(_ElemCount < INT_MAX, "RingQ cannot hold so many elements!");
        __declspec(align(64)) volatile unsigned int m_wcount;
        volatile unsigned int m_rcount;

        typedef _IterType iter;

        RingQ()
        {
            m_wcount = m_rcount = 0;
        }

        void Clear()
        {
            m_wcount = m_rcount = 0;
        }

        int DataCount()
        {
            return (int)(m_wcount - m_rcount);
        }

        bool WCheck(_IterType n)
        {
            return _ElemCount - (m_wcount - m_rcount) >= n;
        }

        __forceinline _ElemType& operator[](int index) { return m_Data[index]; }
        __forceinline _ElemType& operator[](iter index) { return m_Data[index]; }

    private:
        __declspec(align(64)) _ElemType m_Data[_ElemCount];
    };
}