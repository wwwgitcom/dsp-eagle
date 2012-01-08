#pragma once
#include <vector>

namespace OpenDSP
{
    class dsp_io_signature;
    class dsp_buffer;
    class dsp_buffer_reader;
    class dsp_basic_block;
    class dsp_block;
    class dsp_block_detail;
    class dsp_switch_block;
    class dsp_flat_flowgraph;
    class dsp_flowgraph;
    class dsp_scheduler;
    class dsp_scheduler_sts;
    class dsp_single_threaded_scheduler;
    class dsp_endpoint;

    typedef dsp_io_signature* dsp_io_signature_ptr;
    typedef dsp_block* dsp_block_ptr;
    typedef std::vector<dsp_block_ptr> dsp_block_vector_t;
    typedef std::vector<dsp_block_ptr>::iterator dsp_block_viter_t;    
    typedef dsp_basic_block* dsp_basic_block_ptr;
    typedef dsp_block_detail* dsp_block_detail_ptr;
    typedef dsp_switch_block* dsp_block_switch_ptr;
    typedef dsp_buffer* dsp_buffer_ptr;
    typedef dsp_buffer_reader* dsp_buffer_reader_ptr;
    
    typedef dsp_flowgraph* dsp_flowgraph_ptr;
    typedef dsp_flat_flowgraph* dsp_flat_flowgraph_ptr;
    typedef dsp_scheduler* dsp_scheduler_ptr;
    typedef dsp_scheduler_sts* dsp_scheduler_sts_ptr;
    typedef class dsp_single_threaded_scheduler* dsp_single_threaded_scheduler_ptr;
    typedef dsp_endpoint* dsp_endpoint_ptr;
    typedef std::vector<dsp_endpoint>   dsp_vector_endpoint;

    typedef std::vector<int>            dsp_vector_int;
    typedef std::vector<int>            dsp_vector_int;
    typedef std::vector<float>          dsp_vector_float;
    typedef std::vector<double>         dsp_vector_double;
    typedef std::vector<void *>         dsp_vector_void_star;
    typedef std::vector<const void *>   dsp_vector_const_void_star;
}