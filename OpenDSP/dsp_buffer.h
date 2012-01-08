#pragma once
#include <assert.h>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
#include <Windows.h>
#include "dsp_types.h"

namespace OpenDSP
{
    struct dsp_vmcircbuf 
    {
        int	        m_size; // in bytes
        char	   *m_base;

        HANDLE      m_hMapFile;
        LPCTSTR     m_pFirstCopy;
        LPCTSTR     m_pSecondCopy;
        BOOL        m_bLocked;
        
        static const  int  cNameSize = 1024;
        char        m_szName[cNameSize];

        static int  s_bufcount;
    
        // CREATORS
        dsp_vmcircbuf (int size);
        ~dsp_vmcircbuf();

        // ACCESSORS
        void *pointer_to_first_copy  () const { return m_base; }
        void *pointer_to_second_copy () const { return m_base + m_size; }

        static dsp_vmcircbuf* create(int size);
    };
    
    dsp_buffer_ptr dsp_make_buffer (int nitems, size_t sizeof_item, dsp_block_ptr link);

    /*!
    * \brief Allocate a buffer that holds at least \p nitems of size \p sizeof_item.
    *
    * The total size of the buffer will be rounded up to a system
    * dependent boundary.  This is typically the system page size, but
    * under MS windows is 64KB.
    *
    * \param nitems is the minimum number of items the buffer will hold.
    * \param sizeof_item is the size of an item in bytes.
    * \param link is the block that writes to this buffer.
    */

    /*!
    * \brief Single writer, multiple reader FIFO.
    */
    class dsp_buffer 
    {
    public:
        char				       *m_base;		// base address of buffer
        unsigned int	 			m_bufsize;	// in items
        size_t	 			        m_sizeof_item; // in bytes
        dsp_vmcircbuf              *m_vmcircbuf;

        volatile unsigned int       m_write_index;	// in items [0,d_bufsize)
        unsigned __int64            m_abs_write_offset;
        unsigned __int64            m_last_min_items_read;

        dsp_block_ptr               m_link; // block that writes to this buffer

        vector<dsp_buffer_reader *>	m_readers;

        dsp_buffer(int nitems, size_t sizeof_item, dsp_block_ptr link);
        ~dsp_buffer();

        bool allocate_buffer (int nitems, size_t sizeof_item);

        /*!
        * \brief return number of items worth of space available for writing
        */
        int space_available ();

        /*!
        * \brief return size of this buffer in items
        */
        int bufsize() const { return m_bufsize; }

        /*!
        * \brief return pointer to write buffer.
        *
        * The return value points at space that can hold at least
        * space_available() items.
        */
        void *write_pointer ();

        /*!
        * \brief tell buffer that we wrote \p nitems into it
        */
        void update_write_pointer (int nitems);

        size_t nreaders() const { return m_readers.size(); }
        dsp_buffer_reader* reader(size_t index) { return m_readers[index]; }
        void drop_reader (dsp_buffer_reader *reader);

        /*!
        * \brief Return the block that writes to this buffer.
        */
        dsp_block_ptr link() { return m_link; }

        unsigned __int64 nitems_written() { return m_abs_write_offset; }

        unsigned int index_add (unsigned int a, unsigned int b);
        unsigned int index_sub (unsigned int a, unsigned int b);

        void dump();

        friend dsp_buffer_ptr dsp_make_buffer (int nitems, size_t sizeof_item, dsp_block_ptr link);
    };

    //////////////////////////////////////////////////////////////////////////

    class dsp_buffer_reader
    {
    public:
        dsp_buffer_ptr          m_buffer;
        volatile unsigned int   m_read_index;	// in items [0,d->buffer.d_bufsize)
        unsigned __int64        m_abs_read_offset;
        dsp_block_ptr           m_link;

        dsp_buffer_reader(dsp_buffer_ptr buffer, unsigned int read_index, dsp_block_ptr link);
        ~dsp_buffer_reader ();

        dsp_buffer_ptr buffer () const { return m_buffer; }

        int max_possible_items_available () const { return m_buffer->m_bufsize - 1; }

        const void *read_pointer ();
        void update_read_pointer (int nitems);

        int items_available () const;
        unsigned __int64 nitems_read() { return m_abs_read_offset; }

        /*!
        * \brief Return the block that read from this buffer.
        */
        dsp_block_ptr link() { return m_link; }
        // change the reader
        void switch_link(dsp_block_ptr new_link) { m_link = new_link; }
    };

    dsp_buffer_reader_ptr dsp_buffer_add_reader (dsp_buffer_ptr buf, dsp_block_ptr link);
}