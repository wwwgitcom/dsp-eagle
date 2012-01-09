#include <fstream>
#include <iostream>
#include "dsp_block.h"
#include "dsp_buffer.h"

namespace OpenDSP
{
#define ENABLE_LOGGING 1

#if (ENABLE_LOGGING)
#define LOG(x) do { x; } while(0)
#else
#define LOG(x) do {;} while(0)
#endif
    //////////////////////////////////////////////////////////////////////////
    dsp_io_signature::dsp_io_signature(int numberof_stream,
        const std::vector<int> &sizeof_stream_items)
    {
        m_numberof_streams    = numberof_stream;
        m_sizeof_stream_items = sizeof_stream_items;
    }

    dsp_io_signature::dsp_io_signature(int numberof_stream,
        int n0, int n1, int n2, int n3,
        int n4, int n5, int n6, int n7, 
        int n8, int n9, int n10, int n11, 
        int n12, int n13, int n14, int n15)
    {
        assert(numberof_stream >= 0 && numberof_stream <= 15);
        m_numberof_streams = numberof_stream;
        m_sizeof_stream_items.resize(16);
        m_sizeof_stream_items[0]  = n0;
        m_sizeof_stream_items[1]  = n1;
        m_sizeof_stream_items[2]  = n2;
        m_sizeof_stream_items[3]  = n3;
        m_sizeof_stream_items[4]  = n4;
        m_sizeof_stream_items[5]  = n5;
        m_sizeof_stream_items[6]  = n6;
        m_sizeof_stream_items[7]  = n7;
        m_sizeof_stream_items[8]  = n8;
        m_sizeof_stream_items[9]  = n9;
        m_sizeof_stream_items[10] = n10;
        m_sizeof_stream_items[11] = n11;
        m_sizeof_stream_items[12] = n12;
        m_sizeof_stream_items[13] = n13;
        m_sizeof_stream_items[14] = n14;
        m_sizeof_stream_items[15] = n15;
    }

    dsp_io_signature::~dsp_io_signature()
    {
    }

    int dsp_io_signature::numberof_streams() const
    {
        return m_numberof_streams;
    }

    int dsp_io_signature::sizeof_stream_item (int index) const
    {
        return m_sizeof_stream_items[index];
    }
    std::vector<int> dsp_io_signature::sizeof_stream_items() const
    {
        return m_sizeof_stream_items;
    }

    dsp_io_signature_ptr
        dsp_make_io_signaturev(int numberof_stream,
        const std::vector<int> &sizeof_stream_items)
    {
        return dsp_io_signature_ptr (new dsp_io_signature(numberof_stream, sizeof_stream_items));
    }
    //////////////////////////////////////////////////////////////////////////
    static long s_next_id              = 0;
    static long s_ncurrently_allocated = 0;

    dsp_basic_block::dsp_basic_block(const std::string &name,
        dsp_io_signature& input_signature,
        dsp_io_signature& output_signature)
    {
        m_name             = name;
        m_input_signature  = input_signature;
        m_output_signature = output_signature;
        m_unique_id        = s_next_id++;
        m_color            = WHITE;        
        s_ncurrently_allocated++;
    }

    dsp_basic_block::~dsp_basic_block()
    {
        s_ncurrently_allocated--;
    }
    //////////////////////////////////////////////////////////////////////////
    dsp_block::dsp_block(const std::string &name, dsp_io_signature& input_signature, dsp_io_signature& output_signature)
    : dsp_basic_block(name, input_signature, output_signature),
    m_fixed_rate(false), m_fixed_input_items(0), m_fixed_output_items(0)
    {
        m_type = BLOCK;
        m_input_blocks.resize(input_signature.numberof_streams());
        m_output_blocks.resize(input_signature.numberof_streams());
    }
    dsp_block::~dsp_block ()
    {
    }    

    void dsp_block::consume (int which_input, int how_many_items)
    {
        m_detail->consume(which_input, how_many_items);
    }
    
    void dsp_block::consume_each (int how_many_items)
    {
        m_detail->consume_each(how_many_items);
    }

    void dsp_block::produce (int which_output, int how_many_items)
    {
        m_detail->produce(which_output, how_many_items);
    }

    void dsp_block::produce_each (int how_many_items)
    {
        m_detail->produce_each(how_many_items);
    }


    int dsp_block::ninput(int which)
    {
        return detail()->input(which)->items_available();
    }

    int dsp_block::noutput(int which)
    {
        return detail()->output(which)->space_available();
    }

    void dsp_block::dump_output(int which)
    {
        detail()->output(which)->dump();
    }

    void dsp_block::dump_input(int which)
    {
        detail()->input(which)->buffer()->dump();
    }

    //////////////////////////////////////////////////////////////////////////

    static long s_block_detail_ncurrently_allocated = 0;

    long dsp_block_detail_ncurrently_allocated ()
    {
        return s_block_detail_ncurrently_allocated;
    }

    dsp_block_detail::dsp_block_detail (unsigned int ninputs, unsigned int noutputs)
        : m_ninputs(ninputs), m_noutputs(noutputs),
        m_input(ninputs), m_output(noutputs)
    {
        s_block_detail_ncurrently_allocated++;
    }

    dsp_block_detail::~dsp_block_detail ()
    {
        // should take care of itself
        s_block_detail_ncurrently_allocated--;
    }

    void dsp_block_detail::set_input (unsigned int which, dsp_buffer_reader_ptr reader)
    {
        m_input[which] = reader;
    }

    void dsp_block_detail::set_output (unsigned int which, dsp_buffer_ptr buffer)
    {
        m_output[which] = buffer;
    }

    dsp_block_detail_ptr
        dsp_make_block_detail (unsigned int ninputs, unsigned int noutputs)
    {
        return dsp_block_detail_ptr (new dsp_block_detail (ninputs, noutputs));
    }

    void dsp_block_detail::consume (int which_input, int how_many_items)
    {
        if (how_many_items > 0) {
            input (which_input)->update_read_pointer (how_many_items);
        }
    }

    void dsp_block_detail::consume_each (int how_many_items)
    {
        if (how_many_items > 0) {
            for (int i = 0; i < ninputs (); i++) {
                m_input[i]->update_read_pointer (how_many_items);
            }
        }
    }

    void dsp_block_detail::produce (int which_output, int how_many_items)
    {
        if (how_many_items > 0){
            m_output[which_output]->update_write_pointer (how_many_items);
        }
    }

    void dsp_block_detail::produce_each (int how_many_items)
    {
        if (how_many_items > 0) {
            for (int i = 0; i < noutputs (); i++) {
                m_output[i]->update_write_pointer (how_many_items);
            }
        }
    }
    
    unsigned __int64
        dsp_block_detail::nitems_read(unsigned int which_input) 
    {
        return m_input[which_input]->nitems_read();
    }

    unsigned __int64
        dsp_block_detail::nitems_written(unsigned int which_output) 
    {
        return m_output[which_output]->nitems_written();
    }
    //////////////////////////////////////////////////////////////////////////
    
    dsp_block_executor::dsp_block_executor(dsp_block_ptr block)
        : m_block(block), m_log(0)
    {
        char buf[1024];
        memset(buf, 0, 1024);
        sprintf_s(buf, "dsp_block_executor-%d.txt", block->unique_id());
        
        std::string name(buf);
        m_log = new std::ofstream(name.c_str());
        std::unitbuf(*m_log);		// make it unbuffered...
        *m_log << "dsp_block_executor: "
            << m_block->unique_id() << std::endl;
    }

    dsp_block_executor::~dsp_block_executor()
    {
        if (m_log)
        {
            delete m_log;
        }
    }

    dsp_block::state dsp_block_executor::run_one_iteration()
    {
        dsp_block_detail_ptr d = m_block->detail();

        if (d->source_p())
        {
            m_ninput_items_required.resize(0);
            m_ninput_items.resize(0);
            m_input_items.resize(0);
            m_noutput_items.resize (d->noutputs());
            m_output_items.resize (d->noutputs());
            m_start_nitems_read.resize(0);

            for (int i = 0; i < d->noutputs(); i++)
            {
                m_noutput_items[i] = d->output(i)->space_available();
                m_output_items[i]  = d->output(i)->write_pointer();

                LOG(*m_log << " source\n  noutput_items on output_port " << i << " = " << m_noutput_items[i] << std::endl);
            }            
        }
        else if (d->sink_p())
        {
            m_ninput_items_required.resize(d->ninputs());
            m_ninput_items.resize(d->ninputs());
            m_input_items.resize(d->ninputs());
            m_noutput_items.resize (d->noutputs());
            m_output_items.resize(0);
            m_start_nitems_read.resize(d->ninputs());

            for (int i = 0; i < d->ninputs(); i++)
            {
                m_ninput_items[i] = d->input(i)->items_available();
                m_input_items[i]  = d->input(i)->read_pointer();

                LOG(*m_log << " sink\n  ninput_items on port " << i << " = " << m_ninput_items[i] << std::endl);
            }
        }
        else
        {
            // do the regular thing
            m_ninput_items_required.resize(d->ninputs());
            m_ninput_items.resize(d->ninputs());
            m_input_items.resize(d->ninputs());
            m_noutput_items.resize (d->noutputs());
            m_output_items.resize (d->noutputs ());
            m_start_nitems_read.resize(d->ninputs());

            LOG(*m_log << " regular\n");
            for (int i = 0; i < d->ninputs(); i++)
            {
                m_ninput_items[i] = d->input(i)->items_available();
                m_input_items[i]  = d->input(i)->read_pointer();

                LOG(*m_log << " ninput_items on port " << i << " = " << m_ninput_items[i] << std::endl);
            }

            for (int i = 0; i < d->noutputs(); i++)
            {
                m_noutput_items[i] = d->output(i)->space_available();
                m_output_items[i]  = d->output(i)->write_pointer();

                LOG(*m_log << " noutput_items on port " << i << " = " << m_noutput_items[i] << std::endl);
            }
        }

        //return m_block->general_work(m_ninput_items, m_input_items, m_noutput_items, m_output_items);
        return m_block->general_work();
    }

    //////////////////////////////////////////////////////////////////////////
    inline static unsigned int
        round_up (unsigned int n, unsigned int multiple)
    {
        return ((n + multiple - 1) / multiple) * multiple;
    }

    inline static unsigned int
        round_down (unsigned int n, unsigned int multiple)
    {
        return (n / multiple) * multiple;
    }

    int min_output_available_space (dsp_block_detail_ptr d)
    {
        int	min_space = INT_MAX;

        for (int i = 0; i < d->noutputs (); i++)
        {
            int n = d->output(i)->space_available();
            min_space = min(min_space, n);
        }
        return min_space;
    }

    int min_input_available_items(dsp_block_detail_ptr d)
    {
        int	min_nitems = INT_MAX;

        for (int i = 0; i < d->noutputs (); i++)
        {
            int n = d->input(i)->items_available();
            min_nitems = min(min_nitems, n);
        }
        return min_nitems;
    }

    //////////////////////////////////////////////////////////////////////////

    dsp_sync_block::dsp_sync_block(
        const std::string &name, 
        dsp_io_signature &input_signature, 
        dsp_io_signature &output_signature,
        int nfixed_rate_input,
        int nfixed_rate_output
        )
        : dsp_block(name, input_signature, output_signature)
    {
        set_fixed_rate(true);
        set_fixed_rate_input(nfixed_rate_input);
        set_fixed_rate_output(nfixed_rate_output);
    }
    dsp_sync_block::~dsp_sync_block()
    {

    }

    dsp_block::state dsp_sync_block::general_work()
    {
        dsp_sync_block::state st = work();
        if (st == dsp_sync_block::READY)
        {
            consume_each(nfixedinput());
            produce_each(nfixedoutput());
        }
        return st;
    }

    //////////////////////////////////////////////////////////////////////////

    dsp_control_block::dsp_control_block(const std::string &name)
        : dsp_block(name, dsp_io_signature(0, 0), dsp_io_signature(0, 0))
    {
        m_type = CONTROL;
    }

    dsp_control_block::~dsp_control_block()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    dsp_switch_block::dsp_switch_block(const std::string &name, dsp_io_signature &input_signature, dsp_io_signature &output_signature)
        : dsp_block(name, input_signature, output_signature)
    {
        assert(input_signature.numberof_streams() == 1);
        m_type = SWITCH;
        m_next = NULL;
    }
    dsp_switch_block::~dsp_switch_block(){}

    void dsp_switch_block::switch_to(int which)
    {
        m_next = output_block(which);
        detail()->input(0)->switch_link(m_next);
    }
}