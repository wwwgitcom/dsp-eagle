#pragma once
#include <vector>
#include <string>
#include "dsp_task.h"
#include "dsp_types.h"
#include "dsp_buffer.h"
using namespace std;

namespace OpenDSP
{
    dsp_io_signature_ptr
        dsp_make_io_signaturev(int numberof_stream,
        const std::vector<int> &sizeof_stream_items);

    class dsp_io_signature
    {
        int			        m_numberof_streams;
        std::vector<int>	m_sizeof_stream_items;
               
    public:
        friend dsp_io_signature_ptr
            dsp_make_io_signaturev(int numberof_stream,
            const std::vector<int> &sizeof_stream_items);

        dsp_io_signature(int numberof_stream,
            const std::vector<int> &sizeof_stream_items);

        dsp_io_signature(int numberof_stream,
            int n0 = 0, int n1 = 0, int n2 = 0, int n3 = 0,
            int n4 = 0, int n5 = 0, int n6 = 0, int n7 = 0, 
            int n8 = 0, int n9 = 0, int n10 = 0, int n11 = 0, 
            int n12 = 0, int n13 = 0, int n14 = 0, int n15 = 0);

        dsp_io_signature(): m_numberof_streams(0), m_sizeof_stream_items(1){}
        ~dsp_io_signature();

        int numberof_streams () const;
        int sizeof_stream_item (int index) const;
        std::vector<int> sizeof_stream_items() const;
    };

    class dsp_basic_block : public dsp_task::task
    {
    public:
        enum block_type { CONTROL, BLOCK, SWITCH, SPLIT };
    protected:
        friend class dsp_flowgraph;
        enum vcolor { WHITE, GREY, BLACK };
        

        std::string          m_name;
        dsp_io_signature     m_input_signature;
        dsp_io_signature     m_output_signature;
        long                 m_unique_id;
        vcolor               m_color;
        block_type           m_type;

        dsp_basic_block(void){} //allows pure virtual interface sub-classes

        //! Protected constructor prevents instantiation by non-derived classes
        dsp_basic_block(const std::string &name,
            dsp_io_signature &input_signature,
            dsp_io_signature &output_signature);

        __forceinline virtual task* execute() = 0;

        //! may only be called during constructor
        void set_input_signature(dsp_io_signature& iosig) {
            m_input_signature = iosig;
        }

        //! may only be called during constructor
        void set_output_signature(dsp_io_signature& iosig) {
            m_output_signature = iosig;
        }

        /*!
        * \brief Allow the flowgraph to set for sorting and partitioning
        */
        void set_color(vcolor color) { m_color = color; }
        vcolor color() const { return m_color; }
        
    public:
        virtual ~dsp_basic_block();
        long unique_id() const { return m_unique_id; }
        std::string name() const { return m_name; }
        dsp_io_signature_ptr input_signature() const  { return (dsp_io_signature_ptr)&m_input_signature; }
        dsp_io_signature_ptr output_signature() const { return (dsp_io_signature_ptr)&m_output_signature; }

        virtual bool check_topology(int ninputs, int noutputs) { return true; }
        block_type type() const {return m_type;}
    };

    inline std::ostream &operator << (std::ostream &os, dsp_basic_block_ptr block)
    {
        os << block->name() << "(" << block->unique_id() << ")";
        return os;
    }

    typedef std::vector<dsp_basic_block_ptr> dsp_basic_block_vector_t;
    typedef std::vector<dsp_basic_block_ptr>::iterator dsp_basic_block_viter_t;
    //////////////////////////////////////////////////////////////////////////

    
    class dsp_block : public dsp_basic_block
    {
    private:
        // if block is fixed rate, specify the number of items needed
        bool                  m_fixed_rate;
        unsigned int          m_fixed_input_items;
        unsigned int          m_fixed_output_items;
        dsp_block_detail_ptr  m_detail;		// implementation details

        std::vector<dsp_block_ptr> m_input_blocks;
        std::vector<dsp_block_ptr> m_output_blocks;

    protected:
        dsp_block (void){} //allows pure virtual interface sub-classes
        dsp_block (const std::string &name,
            dsp_io_signature& input_signature,
            dsp_io_signature& output_signature);
    public:
        ~dsp_block ();

        void set_fixed_rate(bool bfixed) {m_fixed_rate = bfixed;}
        void set_fixed_rate_input(int how_many_items){m_fixed_input_items = how_many_items;}
        void set_fixed_rate_output(int how_many_items){m_fixed_output_items = how_many_items;}
        int  nfixedinput(){return m_fixed_input_items;}
        int  nfixedoutput(){return m_fixed_output_items;}
        bool is_fixed_rate(){return m_fixed_rate;}
        dsp_block_ptr output_block(int which){return m_output_blocks[which];}
        void set_output_block(int which, dsp_block_ptr block){m_output_blocks[which] = block;}
        /*!
        * \brief Tell the scheduler \p how_many_items of input stream \p which_input were consumed.
        */
        void consume (int which_input, int how_many_items);

        /*!
        * \brief Tell the scheduler \p how_many_items were consumed on each input stream.
        */
        void consume_each (int how_many_items);

        /*!
        * \brief Tell the scheduler \p how_many_items were produced on output stream \p which_output.
        *
        * If the block's general_work method calls produce, \p general_work must return WORK_CALLEm_PRODUCE.
        */
        void produce (int which_output, int how_many_items);

        void produce_each (int how_many_items);

        int ninput(int which);

        void dump_output(int which);
        void dump_input(int which);

        template<class T>
        T* input(unsigned int which){return (T*)(detail()->input(which)->read_pointer());}
        
        int noutput(int which);

        template<class T>
        T* output(unsigned int which){return (T*)(detail()->output(which)->write_pointer());}

        enum state {
            READY,	      // We made progress; everything's cool.
            READY_NO_OUTPUT,  // We consumed some input, but produced no output.
            BLKD_IN,	      // no progress; we're blocked waiting for input data.
            BLKD_OUT,	      // no progress; we're blocked waiting for output buffer space.
            DONE,	      // we're done; don't call me again.
        };
    private:
        state m_state;

    public:
        state get_state() const {return m_state;}
        /* if it can produce some items, return true
        * otherwise, return false. 
        * general_work must call consume or consume_each to indicate how many items
        * were consumed on each input stream.
        */
        virtual state general_work() = 0;
        __forceinline task* execute() {m_state = general_work(); return this;}
        // reset internal data
        virtual void reset() = 0;

        dsp_block_detail_ptr detail () const { return m_detail; }
        void set_detail (dsp_block_detail_ptr detail) { m_detail = detail; }
    };

    inline std::ostream &operator << (std::ostream &os, dsp_block_ptr block)
    {
        os << block->name() << "(" << block->unique_id() << ")";
        return os;
    }

    inline dsp_block_ptr cast_to_block_ptr(dsp_basic_block_ptr p)
    {
        return reinterpret_cast<dsp_block_ptr>(p);
    }
    
    //////////////////////////////////////////////////////////////////////////

    dsp_block_detail_ptr
        dsp_make_block_detail (unsigned int ninputs, unsigned int noutputs);

    class dsp_block_detail 
    {
    public:
        ~dsp_block_detail ();

        int ninputs () const { return m_ninputs; }
        int noutputs () const { return m_noutputs; }
        bool sink_p () const { return m_noutputs == 0; }
        bool source_p () const { return m_ninputs == 0; }

        void set_done (bool done);
        bool done () const { return m_done; }

        void set_input (unsigned int which, dsp_buffer_reader_ptr reader);
        dsp_buffer_reader_ptr input (unsigned int which)
        {
            return m_input[which];
        }

        void set_output (unsigned int which, dsp_buffer_ptr buffer);
        dsp_buffer_ptr output (unsigned int which)
        {
            return m_output[which];
        }

        /*!
        * \brief Tell the scheduler \p how_many_items of input stream \p which_input were consumed.
        */
        void consume (int which_input, int how_many_items);

        /*!
        * \brief Tell the scheduler \p how_many_items were consumed on each input stream.
        */
        void consume_each (int how_many_items);

        /*!
        * \brief Tell the scheduler \p how_many_items were produced on output stream \p which_output.
        */
        void produce (int which_output, int how_many_items);

        /*!
        * \brief Tell the scheduler \p how_many_items were produced on each output stream.
        */
        void produce_each (int how_many_items);
        
        // Return the number of items read on input stream which_input
        unsigned __int64 nitems_read(unsigned int which_input);

        // Return the number of items written on output stream which_output
        unsigned __int64 nitems_written(unsigned int which_output);
        // ----------------------------------------------------------------------------

    private:
        unsigned int                       m_ninputs;
        unsigned int                       m_noutputs;
        std::vector<dsp_buffer_reader_ptr> m_input;
        std::vector<dsp_buffer_ptr>	       m_output;
        bool                               m_done;

        dsp_block_detail (unsigned int ninputs, unsigned int noutputs);

        friend struct dsp_tpb_detail;

        friend dsp_block_detail_ptr
            dsp_make_block_detail (unsigned int ninputs, unsigned int noutputs);
    };
    
    //////////////////////////////////////////////////////////////////////////
    class dsp_block_executor {
    protected:
        dsp_block_ptr			        m_block;	// The block we're trying to run
        std::ofstream	       	       *m_log;

        // These are allocated here so we don't have to on each iteration

        dsp_vector_int			        m_ninput_items_required;
        dsp_vector_int			        m_ninput_items;
        dsp_vector_const_void_star	    m_input_items;
        dsp_vector_int			        m_noutput_items;
        dsp_vector_void_star		    m_output_items;
        std::vector<unsigned __int64>   m_start_nitems_read; //stores where tag counts are before work

    public:
        dsp_block_executor(dsp_block_ptr block);
        ~dsp_block_executor();

        /*
        * \brief Run one iteration.
        */
        dsp_block::state run_one_iteration();
    };

    int min_input_available_items(dsp_block_ptr d);
    int min_output_available_space(dsp_block_ptr d);

    class dsp_sync_block : public dsp_block
    {
    protected:
        dsp_sync_block (void){} //allows pure virtual interface sub-classes
        dsp_sync_block (const std::string &name,
            dsp_io_signature &input_signature,
            dsp_io_signature &output_signature,
            int nfixed_rate_input,
            int nfixed_rate_output);

    public:
        ~dsp_sync_block();
        /*!
        * \brief just like gr_block::general_work, only this arranges to call consume_each for you
        *
        * The user must override work to define the signal processing code
        */
        virtual dsp_block::state work () = 0;

        virtual void reset() = 0;

        dsp_block::state general_work();
    };

    // 1 input
    class dsp_control_block : public dsp_block
    {
    protected:
        dsp_control_block (void){} //allows pure virtual interface sub-classes
        dsp_control_block (const std::string &name);

    public:
        ~dsp_control_block();

        virtual void reset() = 0;
        virtual dsp_block::state general_work() = 0;
    };

    class dsp_switch_block : public dsp_block
    {
    protected:
        dsp_switch_block (void){} //allows pure virtual interface sub-classes
        dsp_switch_block (const std::string &name, dsp_io_signature &input_signature, dsp_io_signature &output_signature);

        dsp_block * m_next;

        void switch_to(int which);

    public:
        ~dsp_switch_block();

        virtual void reset() = 0;
        virtual dsp_block::state general_work() = 0;
    };
}