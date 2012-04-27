#pragma once
#include "dsp_types.h"
#include "dsp_block.h"

namespace OpenDSP
{
    
    class dsp_flowgraph
    {
    private:
        void * m_context;
    public:
        // Destruct an arbitrary flow graph
        ~dsp_flowgraph();

        void bind(void * context);

        void shared_connect(const dsp_endpoint &src, const dsp_vector_endpoint &dsts);

        // Connect two endpoints
        void connect(const dsp_endpoint &src, const dsp_endpoint &dst);

        // Disconnect two endpoints
        void disconnect(const dsp_endpoint &src, const dsp_endpoint &dst);

        // Connect an output port to an input port (convenience)
        void connect(dsp_basic_block_ptr src_block, int src_port,
            dsp_basic_block_ptr dst_block, int dst_port);

        // Disconnect an input port from an output port (convenience)
        void disconnect(dsp_basic_block_ptr src_block, int src_port,
            dsp_basic_block_ptr dst_block, int dst_port);

        void validate();
        void clear();

        // Return vector of connected blocks
        dsp_basic_block_vector_t calc_used_blocks();

        // Return toplogically sorted vector of blocks.  All the sources come first.
        dsp_basic_block_vector_t topological_sort(dsp_basic_block_vector_t &blocks);

        // Return vector of vectors of disjointly connected blocks, topologically
        // sorted.
        std::vector<dsp_basic_block_vector_t> partition();
    protected:
        dsp_basic_block_vector_t    m_blocks;
        dsp_edge_vector_t           m_edges;

        dsp_flowgraph();

        std::vector<int> calc_used_ports(dsp_basic_block_ptr block, bool check_inputs); 
        dsp_basic_block_vector_t calc_downstream_blocks(dsp_basic_block_ptr block, int port);
        dsp_edge_vector_t calc_upstream_edges(dsp_basic_block_ptr block);
        bool has_block_p(dsp_basic_block_ptr block);
        dsp_edge calc_upstream_edge(dsp_basic_block_ptr block, int port);

    private:

        void check_valid_port(dsp_io_signature_ptr sig, int port);
        void check_dst_not_used(const dsp_endpoint &dst);
        void check_type_match(const dsp_endpoint &src, const dsp_endpoint &dst);
        dsp_edge_vector_t calc_connections(dsp_basic_block_ptr block, bool check_inputs); // false=use outputs
        void check_contiguity(dsp_basic_block_ptr block, const std::vector<int> &used_ports, bool check_inputs);

        dsp_basic_block_vector_t calc_downstream_blocks(dsp_basic_block_ptr block);
        dsp_basic_block_vector_t calc_reachable_blocks(dsp_basic_block_ptr block, dsp_basic_block_vector_t &blocks);
        void reachable_dfs_visit(dsp_basic_block_ptr block, dsp_basic_block_vector_t &blocks);
        dsp_basic_block_vector_t calc_adjacent_blocks(dsp_basic_block_ptr block, dsp_basic_block_vector_t &blocks);
        dsp_basic_block_vector_t sort_sources_first(dsp_basic_block_vector_t &blocks);
        bool source_p(dsp_basic_block_ptr block);
        void topological_dfs_visit(dsp_basic_block_ptr block, dsp_basic_block_vector_t &output);

        // TODO: impl. destroy function, we need to manually
        // free all resources related to one block: detail, buffer, buffer_reader...
        // put destroy into destructor....
        virtual void destroy() = 0;
    };
    
    // Create a shared pointer to a heap allocated dsp_flat_flowgraph
    // (types defined in dsp_runtime_types.h)
    dsp_flat_flowgraph_ptr dsp_make_flat_flowgraph();

    /*!
    *\brief Class specializing dsp_flat_flowgraph that has all nodes
    * as dsp_blocks, with no hierarchy
    * \ingroup internal
    */
    class dsp_flat_flowgraph : public dsp_flowgraph
    {
    public:
        friend dsp_flat_flowgraph_ptr dsp_make_flat_flowgraph();

        // Destruct an arbitrary dsp_flat_flowgraph
        ~dsp_flat_flowgraph();

        // Wire dsp_blocks together in new flat_flowgraph
        void setup_connections();

        void setup_connections2();

        // Merge applicable connections from existing flat flowgraph
        void merge_connections(dsp_flat_flowgraph_ptr sfg);

        void dump();

        void destroy();

        void connect_c(dsp_basic_block_ptr src_block, int src_port, dsp_basic_block_ptr dst_block, int dst_port);
        void connect_d(dsp_basic_block_ptr src_block, int src_port, dsp_basic_block_ptr dst_block, int dst_port);
        void connect_c(const dsp_endpoint &src, const dsp_endpoint &dst);
        void connect_d(const dsp_endpoint &src, const dsp_endpoint &dst);

        /*!
        * Make a vector of dsp_block from a vector of dsp_basic_block
        */
        static dsp_block_vector_t make_block_vector(dsp_basic_block_vector_t &blocks);

    private:
        dsp_flat_flowgraph();

        dsp_block_detail_ptr allocate_block_detail(dsp_basic_block_ptr block);
        dsp_buffer_ptr allocate_buffer(dsp_basic_block_ptr block, int port);
        void connect_block_inputs(dsp_basic_block_ptr block);
    };
}