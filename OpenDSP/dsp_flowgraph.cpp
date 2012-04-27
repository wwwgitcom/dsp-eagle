#include "dsp_block.h"
#include "dsp_flowgraph.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>

#define DSP_FLOWGRAPH_DEBUG 1

namespace OpenDSP
{
    // FIXME: move to libgruel as a utility function
    template<class T>
    static std::vector<T> unique_vector(std::vector<T> v)
    {
        std::vector<T> result;
        std::insert_iterator<std::vector<T> > inserter(result, result.begin());

        sort(v.begin(), v.end());
        unique_copy(v.begin(), v.end(), inserter);
        return result;
    }

    dsp_edge::~dsp_edge(){}

    dsp_flowgraph::dsp_flowgraph()
    {

    }

    dsp_flowgraph::~dsp_flowgraph()
    {

    }

    void dsp_flowgraph::connect(dsp_basic_block_ptr src_block, int src_port, dsp_basic_block_ptr dst_block, int dst_port)
    {
        connect(dsp_endpoint(src_block, src_port), dsp_endpoint(dst_block, dst_port));
    }

    void dsp_flowgraph::connect(const dsp_endpoint &src, const dsp_endpoint &dst)
    {
        dsp_edge edge = dsp_edge(src,dst);
        check_valid_port(src.block()->output_signature(), src.port());
        check_valid_port(dst.block()->input_signature(), dst.port());
        check_dst_not_used(dst);
        check_type_match(src, dst);

        src.block()->add_connection(edge);        
        m_edges.push_back(edge);
    }
    
    void dsp_flowgraph::disconnect(dsp_basic_block_ptr src_block, int src_port, dsp_basic_block_ptr dst_block, int dst_port)
    {
        disconnect(dsp_endpoint(src_block, src_port), dsp_endpoint(dst_block, dst_port));
    }

    void dsp_flowgraph::disconnect(const dsp_endpoint &src, const dsp_endpoint &dst)
    {
        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++) {
            if (src == p->src() && dst == p->dst()) {
                m_edges.erase(p);
                return;
            }
        }

        std::stringstream msg;
        msg << "cannot disconnect edge " << dsp_edge(src, dst) << ", not found";
        throw std::invalid_argument(msg.str());
    }

    // tbd...
    void dsp_flowgraph::shared_connect(const dsp_endpoint &src, const dsp_vector_endpoint &dsts)
    {
        size_t nshared_reader     = dsts.size();
        dsp_block_ptr  src_block  = cast_to_block_ptr(src.block());
        dsp_buffer_ptr src_buffer = src_block->detail()->output(src.port());
        dsp_block_ptr  dst_block  = cast_to_block_ptr(dsts[0].block());
        dsp_buffer_reader_ptr dst_buffer_reader = dst_block->detail()->input(dsts[0].port());

        for (int i = 1; i < dsts.size(); i++)
        {
            dst_block  = cast_to_block_ptr(dsts[i].block());
            dsp_buffer_reader_ptr buffer_reader = dst_block->detail()->input(dsts[0].port());
            delete buffer_reader;
            dst_block->detail()->set_input(dsts[i].port(), dst_buffer_reader);
        }
    }

    
    void dsp_flowgraph::validate()
    {
        m_blocks = calc_used_blocks();

        for (dsp_basic_block_viter_t p = m_blocks.begin(); p != m_blocks.end(); p++) {
            std::vector<int> used_ports;
            int ninputs, noutputs;

            if (DSP_FLOWGRAPH_DEBUG)
                std::cout << "Validating block: " << (*p) << std::endl;

            used_ports = calc_used_ports(*p, true); // inputs
            ninputs = used_ports.size();
            check_contiguity(*p, used_ports, true); // inputs

            used_ports = calc_used_ports(*p, false); // outputs
            noutputs = used_ports.size();
            check_contiguity(*p, used_ports, false); // outputs

            if (!((*p)->check_topology(ninputs, noutputs))) {
                std::stringstream msg;
                msg << "check topology failed on " << (*p)
                    << " using ninputs=" << ninputs 
                    << ", noutputs=" << noutputs;
                throw std::runtime_error(msg.str());
            }
        }
    }

    void dsp_flowgraph::check_contiguity(dsp_basic_block_ptr block,
        const std::vector<int> &used_ports, bool check_inputs)
    {
        std::stringstream msg;

        dsp_io_signature_ptr sig =
            check_inputs ? block->input_signature() : block->output_signature();

        int nports = used_ports.size();

        if (nports == 0 && sig->numberof_streams() == 0)
        {
            return; // no input/output ports, simply return.
        }
        
        if (used_ports[nports-1]+1 != nports)
        {
            for (int i = 0; i < nports; i++)
            {
                if (used_ports[i] != i)
                {
                    msg << block << ": missing connection " 
                        << (check_inputs ? "to input port " : "from output port ")
                        << i;
                    throw std::runtime_error(msg.str());
                }
            }
        }
    }

    void dsp_flowgraph::clear()
    {
        m_edges.clear();
        m_blocks.clear();
    }

    void dsp_flowgraph::check_valid_port(dsp_io_signature_ptr sig, int port)
    {
        std::stringstream msg;

        if (port < 0)
        {
            msg << "negative port number " << port << " is invalid";
            throw std::invalid_argument(msg.str());
        }

        int max = sig->numberof_streams();
        if (port >= max) 
        {
            msg << "port number " << port << " exceeds max of ";
            if (max == 0)
                msg << "(none)";
            else
                msg << max-1;
            throw std::invalid_argument(msg.str());
        }
    }

    void dsp_flowgraph::check_dst_not_used(const dsp_endpoint &dst)
    {
        // A destination is in use if it is already on the edge list
        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
        {
            if (p->dst() == dst)
            {
                std::stringstream msg;
                msg << "destination already in use by other edge";
                throw std::invalid_argument(msg.str());
            }
        }
    }

    void dsp_flowgraph::check_type_match(const dsp_endpoint &src, const dsp_endpoint &dst)
    {
        int src_size = src.block()->output_signature()->sizeof_stream_item(src.port());
        int dst_size = dst.block()->input_signature()->sizeof_stream_item(dst.port());

        if (src_size != dst_size)
        {
            std::stringstream msg;
            msg << "item size mismatch: " << &src << " using " << src_size
                << ", " << &dst << " using " << dst_size;
            throw std::invalid_argument(msg.str());
        }
    }

    bool dsp_flowgraph::has_block_p(dsp_basic_block_ptr block)
    {
        dsp_basic_block_viter_t result;
        result = std::find(m_blocks.begin(), m_blocks.end(), block);
        return (result != m_blocks.end());
    }

    dsp_edge dsp_flowgraph::calc_upstream_edge(dsp_basic_block_ptr block, int port)
    {
        dsp_edge result;

        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
        {
            if (p->dst() == dsp_endpoint(block, port))
            {
                result = (*p);
                break;
            }
        }

        return result;
    }

    dsp_edge_vector_t dsp_flowgraph::calc_upstream_edges(dsp_basic_block_ptr block)
    {
        dsp_edge_vector_t result;

        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
            if (p->dst().block() == block)
                result.push_back(*p);

        return result; // Assume no duplicates
    }
    
    dsp_basic_block_vector_t
        dsp_flowgraph::calc_downstream_blocks(dsp_basic_block_ptr block, int port)
    {
        dsp_basic_block_vector_t tmp;

        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
            if (p->src() == dsp_endpoint(block, port))
                tmp.push_back(p->dst().block());

        return unique_vector<dsp_basic_block_ptr>(tmp);
    }
        
    dsp_basic_block_vector_t
        dsp_flowgraph::calc_downstream_blocks(dsp_basic_block_ptr block)
    {
        dsp_basic_block_vector_t tmp;

        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
            if (p->src().block() == block)
                tmp.push_back(p->dst().block());

        return unique_vector<dsp_basic_block_ptr>(tmp);
    }

    bool dsp_flowgraph::source_p(dsp_basic_block_ptr block)
    {
        return (calc_upstream_edges(block).size() == 0);
    }

    dsp_basic_block_vector_t
        dsp_flowgraph::sort_sources_first(dsp_basic_block_vector_t &blocks)
    {
        dsp_basic_block_vector_t sources, nonsources, result;

        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
            if (source_p(*p))
                sources.push_back(*p);
            else
                nonsources.push_back(*p);
        }

        for (dsp_basic_block_viter_t p = sources.begin(); p != sources.end(); p++)
            result.push_back(*p);

        for (dsp_basic_block_viter_t p = nonsources.begin(); p != nonsources.end(); p++)
            result.push_back(*p);

        return result;
    }

    // Return a list of block adjacent to a given block along any edge
    dsp_basic_block_vector_t 
        dsp_flowgraph::calc_adjacent_blocks(dsp_basic_block_ptr block, dsp_basic_block_vector_t &blocks)
    {
        dsp_basic_block_vector_t tmp;

        // Find any blocks that are inputs or outputs
        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++) {
            if (p->src().block() == block)
                tmp.push_back(p->dst().block());
            if (p->dst().block() == block)
                tmp.push_back(p->src().block());
        }    

        return unique_vector<dsp_basic_block_ptr>(tmp);
    }

    std::vector<int>
        dsp_flowgraph::calc_used_ports(dsp_basic_block_ptr block, bool check_inputs)
    {
        std::vector<int> tmp;

        // Collect all seen ports 
        dsp_edge_vector_t edges = calc_connections(block, check_inputs);
        for (dsp_edge_viter_t p = edges.begin(); p != edges.end(); p++)
        {
            if (check_inputs == true)
                tmp.push_back(p->dst().port());
            else
                tmp.push_back(p->src().port());
        }

        return unique_vector<int>(tmp);
    }
    
    dsp_basic_block_vector_t
        dsp_flowgraph::calc_used_blocks()
    {
        dsp_basic_block_vector_t tmp;

        // Collect all blocks in the edge list
        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
        {
            tmp.push_back(p->src().block());
            tmp.push_back(p->dst().block());
        }

        return unique_vector<dsp_basic_block_ptr>(tmp);
    }

    void dsp_flowgraph::bind(void * context)
    {
        m_context = context;
        m_blocks  = calc_used_blocks();
        for (int i = 0; i < m_blocks.size(); i++)
        {
            m_blocks[i]->bind(context);
        }
    }

    dsp_edge_vector_t
        dsp_flowgraph::calc_connections(dsp_basic_block_ptr block, bool check_inputs)
    {
        dsp_edge_vector_t result;

        for (dsp_edge_viter_t p = m_edges.begin(); p != m_edges.end(); p++)
        {
            if (check_inputs)
            {
                if (p->dst().block() == block)
                    result.push_back(*p);
            }
            else
            {
                if (p->src().block() == block)
                    result.push_back(*p);
            }
        }

        return result; // assumes no duplicates
    }
    
    std::vector<dsp_basic_block_vector_t>
        dsp_flowgraph::partition()
    {
        std::vector<dsp_basic_block_vector_t> result;
        dsp_basic_block_vector_t blocks = calc_used_blocks();
        dsp_basic_block_vector_t graph;

        while (blocks.size() > 0) {
            graph = calc_reachable_blocks(blocks[0], blocks);
            assert(graph.size());
            result.push_back(topological_sort(graph));

            for (dsp_basic_block_viter_t p = graph.begin(); p != graph.end(); p++)
                blocks.erase(find(blocks.begin(), blocks.end(), *p));
        }

        return result;
    }

    dsp_basic_block_vector_t
        dsp_flowgraph::calc_reachable_blocks(dsp_basic_block_ptr block, dsp_basic_block_vector_t &blocks)
    {
        dsp_basic_block_vector_t result;

        // Mark all blocks as unvisited
        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
            (*p)->set_color(dsp_basic_block::WHITE);

        // Recursively mark all reachable blocks
        reachable_dfs_visit(block, blocks);

        // Collect all the blocks that have been visited
        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
            if ((*p)->color() == dsp_basic_block::BLACK)
                result.push_back(*p);

        return result;
    }

    // Recursively mark all reachable blocks from given block and block list
    void dsp_flowgraph::reachable_dfs_visit(dsp_basic_block_ptr block, dsp_basic_block_vector_t &blocks)
    {
        cout << "dfs visit =>" << block->name() << endl;
        // Mark the current one as visited
        block->set_color(dsp_basic_block::BLACK);

        // Recurse into adjacent vertices
        dsp_basic_block_vector_t adjacent = calc_adjacent_blocks(block, blocks);

        for (dsp_basic_block_viter_t p = adjacent.begin(); p != adjacent.end(); p++)
            if ((*p)->color() == dsp_basic_block::WHITE)
                reachable_dfs_visit(*p, blocks);
    }
    
    dsp_basic_block_vector_t
        dsp_flowgraph::topological_sort(dsp_basic_block_vector_t &blocks)
    {
        dsp_basic_block_vector_t tmp;
        dsp_basic_block_vector_t result;
        tmp = sort_sources_first(blocks);

        // Start 'em all white
        for (dsp_basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++)
            (*p)->set_color(dsp_basic_block::WHITE);

        for (dsp_basic_block_viter_t p = tmp.begin(); p != tmp.end(); p++)
        {
            if ((*p)->color() == dsp_basic_block::WHITE)
                topological_dfs_visit(*p, result);
        }

        reverse(result.begin(), result.end());
        return result;
    }
    
    void
        dsp_flowgraph::topological_dfs_visit(dsp_basic_block_ptr block, dsp_basic_block_vector_t &output)
    {
        cout << " topological dfs visit " << block->name() << endl;
        block->set_color(dsp_basic_block::GREY);
        dsp_basic_block_vector_t blocks(calc_downstream_blocks(block));

        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
            switch ((*p)->color()) {
            case dsp_basic_block::WHITE:
                topological_dfs_visit(*p, output);
                break;

            case dsp_basic_block::GREY:            
                throw std::runtime_error("flow graph has loops!");

            case dsp_basic_block::BLACK:
                continue;

            default:
                throw std::runtime_error("invalid color on block!");
            }
        }

        block->set_color(dsp_basic_block::BLACK);
        output.push_back(block);
    }

    //////////////////////////////////////////////////////////////////////////

#define DSP_FLAT_FLOWGRAPH_DEBUG 1

    // 64Kbyte buffer size between blocks
#define DSP_FIXED_BUFFER_SIZE (64*(1L<<10))

    static const unsigned int s_fixed_buffer_size = DSP_FIXED_BUFFER_SIZE;

    dsp_flat_flowgraph_ptr
        dsp_make_flat_flowgraph()
    {
        return dsp_flat_flowgraph_ptr(new dsp_flat_flowgraph());
    }

    dsp_flat_flowgraph::dsp_flat_flowgraph()
    {
    }

    dsp_flat_flowgraph::~dsp_flat_flowgraph()
    {
        destroy();
    }

    void dsp_flat_flowgraph::destroy()
    {
        dsp_block_ptr m;
        dsp_block_detail_ptr d;
        std::vector<dsp_buffer_reader_ptr> all_buffer_readers;
        std::vector<dsp_buffer_reader_ptr> unique_buffer_readers;

        // delete all readers
        for (dsp_basic_block_viter_t it = m_blocks.begin(); it < m_blocks.end(); it++)
        {
            m = cast_to_block_ptr(*it);
            d = m->detail();

            if (!d) continue;

            for (int i = 0; i < d->ninputs(); i++)
            {
                dsp_buffer_reader_ptr reader = d->input(i);
                all_buffer_readers.push_back(reader);
            }
        }

        unique_buffer_readers = unique_vector<dsp_buffer_reader_ptr>(all_buffer_readers);
        for (std::vector<dsp_buffer_reader_ptr>::iterator it = unique_buffer_readers.begin();
            it != unique_buffer_readers.end(); it++)
        {
            delete (*it);
        }

        // delete all buffers
        for (dsp_basic_block_viter_t it = m_blocks.begin(); it < m_blocks.end(); it++)
        {
            m = cast_to_block_ptr(*it);
            d = m->detail();

            if (!d) continue;

            for (int i = 0; i < d->noutputs(); i++)
            {
                dsp_buffer_ptr buffer = d->output(i);
                if (!buffer) continue;
                delete buffer;
                d->set_output(i, NULL);
            }
        }

        // delete all block details
        for (dsp_basic_block_viter_t it = m_blocks.begin(); it < m_blocks.end(); it++)
        {
            m = cast_to_block_ptr(*it);
            d = m->detail();

            if (!d) continue;

            std::cout << " deleting block detail " << d << endl;

            delete d;
        }
    }

    void
        dsp_flat_flowgraph::setup_connections()
    {
        dsp_basic_block_vector_t blocks = calc_used_blocks();

        // Assign block details to blocks
        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
            cast_to_block_ptr(*p)->set_detail(allocate_block_detail(*p));

        // Connect inputs to outputs for each block
        for(dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
            connect_block_inputs(*p);
    }

    void dsp_flat_flowgraph::setup_connections2()
    {
        dsp_basic_block_vector_t blocks = calc_used_blocks();

        // Assign block details to blocks
        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
            cast_to_block_ptr(*p)->set_detail(allocate_block_detail(*p));

        // Connect inputs to outputs for each block
        for(dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++)
            connect_block_inputs(*p);
    }

    dsp_block_detail_ptr
        dsp_flat_flowgraph::allocate_block_detail(dsp_basic_block_ptr block)
    {
        int ninputs                 = calc_used_ports(block, true).size();
        int noutputs                = calc_used_ports(block, false).size();
        dsp_block_detail_ptr detail = dsp_make_block_detail(ninputs, noutputs);

        if (DSP_FLAT_FLOWGRAPH_DEBUG)
            std::cout << "Creating block detail for " << block << std::endl;

        for (int i = 0; i < noutputs; i++)
        {
            dsp_buffer_ptr buffer = allocate_buffer(block, i);
            if (DSP_FLAT_FLOWGRAPH_DEBUG)
                std::cout << "Allocated buffer for output " << block << ":" << i << std::endl;
            detail->set_output(i, buffer);
        }

        return detail;
    }

    dsp_buffer_ptr
        dsp_flat_flowgraph::allocate_buffer(dsp_basic_block_ptr block, int port)
    {
        dsp_block_ptr grblock = cast_to_block_ptr(block);
        if (!grblock)
            throw std::runtime_error("allocate_buffer found non-dsp_block");
        
        int item_size = block->output_signature()->sizeof_stream_item(port);
        int nitems    = s_fixed_buffer_size / item_size;

        return dsp_make_buffer(nitems, item_size, grblock);
    }

    void dsp_flat_flowgraph::connect_block_inputs(dsp_basic_block_ptr block)
    {
        dsp_block_ptr grblock = cast_to_block_ptr(block);
        if (!grblock)
            throw std::runtime_error("connect_block_inputs found non-dsp_block");

        // Get its detail and edges that feed into it
        dsp_block_detail_ptr detail = grblock->detail();
        dsp_edge_vector_t in_edges  = calc_upstream_edges(block);

        // For each edge that feeds into it
        for (dsp_edge_viter_t e = in_edges.begin(); e != in_edges.end(); e++)
        {
            // Set the buffer reader on the destination port to the output
            // buffer on the source port
            int dst_port = e->dst().port();
            int src_port = e->src().port();
            dsp_basic_block_ptr src_block = e->src().block();
            dsp_block_ptr src_grblock = cast_to_block_ptr(src_block);
            if (!src_grblock)
                throw std::runtime_error("connect_block_inputs found non-dsp_block");

            dsp_buffer_ptr src_buffer = src_grblock->detail()->output(src_port);
            
            if (DSP_FLAT_FLOWGRAPH_DEBUG)
                std::cout << "Setting input " << dst_port << " from edge " << (*e) << std::endl;

            detail->set_input(dst_port, dsp_buffer_add_reader(src_buffer, grblock));
        }
    }

    void dsp_flat_flowgraph::merge_connections(dsp_flat_flowgraph_ptr old_ffg)
    {
        // Allocate block details if needed.  Only new blocks that aren't pruned out
        // by flattening will need one; existing blocks still in the new flowgraph will
        // already have one.
        for (dsp_basic_block_viter_t p = m_blocks.begin(); p != m_blocks.end(); p++) {
            dsp_block_ptr block = cast_to_block_ptr(*p);

            if (!block->detail()) {
                if (DSP_FLAT_FLOWGRAPH_DEBUG)
                    std::cout << "merge: allocating new detail for block " << (*p) << std::endl;
                block->set_detail(allocate_block_detail(block));
            }
            else
                if (DSP_FLAT_FLOWGRAPH_DEBUG)
                    std::cout << "merge: reusing original detail for block " << (*p) << std::endl;
        }

        // Calculate the old edges that will be going away, and clear the buffer readers
        // on the RHS.
        for (dsp_edge_viter_t old_edge = old_ffg->m_edges.begin(); old_edge != old_ffg->m_edges.end(); old_edge++) {
            if (DSP_FLAT_FLOWGRAPH_DEBUG)
                std::cout << "merge: testing old edge " << (*old_edge) << "...";

            dsp_edge_viter_t new_edge;
            for (new_edge = m_edges.begin(); new_edge != m_edges.end(); new_edge++)
                if (new_edge->src() == old_edge->src() &&
                    new_edge->dst() == old_edge->dst())
                    break;

            if (new_edge == m_edges.end()) { // not found in new edge list
                if (DSP_FLAT_FLOWGRAPH_DEBUG)
                    std::cout << "not in new edge list" << std::endl;
                // zero the buffer reader on RHS of old edge
                dsp_block_ptr block(cast_to_block_ptr(old_edge->dst().block()));
                int port = old_edge->dst().port();
                block->detail()->set_input(port, dsp_buffer_reader_ptr());
            }
            else {
                if (DSP_FLAT_FLOWGRAPH_DEBUG)
                    std::cout << "found in new edge list" << std::endl;
            }
        }  

        // Now connect inputs to outputs, reusing old buffer readers if they exist
        for (dsp_basic_block_viter_t p = m_blocks.begin(); p != m_blocks.end(); p++) {
            dsp_block_ptr block = cast_to_block_ptr(*p);

            if (DSP_FLAT_FLOWGRAPH_DEBUG)
                std::cout << "merge: merging " << (*p) << "...";

            if (old_ffg->has_block_p(*p)) {
                // Block exists in old flow graph
                if (DSP_FLAT_FLOWGRAPH_DEBUG)
                    std::cout << "used in old flow graph" << std::endl;
                dsp_block_detail_ptr detail = block->detail();

                // Iterate through the inputs and see what needs to be done
                int ninputs = calc_used_ports(block, true).size(); // Might be different now
                for (int i = 0; i < ninputs; i++) {
                    if (DSP_FLAT_FLOWGRAPH_DEBUG)
                        std::cout << "Checking input " << block << ":" << i << "...";
                    dsp_edge edge = calc_upstream_edge(*p, i);

                    // Fish out old buffer reader and see if it matches correct buffer from edge list
                    dsp_block_ptr src_block = cast_to_block_ptr(edge.src().block());
                    dsp_block_detail_ptr src_detail = src_block->detail();
                    dsp_buffer_ptr src_buffer = src_detail->output(edge.src().port());
                    dsp_buffer_reader_ptr old_reader;
                    if (i < detail->ninputs()) // Don't exceed what the original detail has 
                        old_reader = detail->input(i);

                    // If there's a match, use it
                    if (old_reader && (src_buffer == old_reader->buffer())) {
                        if (DSP_FLAT_FLOWGRAPH_DEBUG)
                            std::cout << "matched, reusing" << std::endl;
                    }
                    else {
                        if (DSP_FLAT_FLOWGRAPH_DEBUG)
                            std::cout << "needs a new reader" << std::endl;

                        // Create new buffer reader and assign
                        detail->set_input(i, dsp_buffer_add_reader(src_buffer, block));
                    }
                }
            }
            else {
                // Block is new, it just needs buffer readers at this point
                if (DSP_FLAT_FLOWGRAPH_DEBUG)
                    std::cout << "new block" << std::endl;
                connect_block_inputs(block);
            }

            // Now deal with the fact that the block details might have changed numbers of 
            // inputs and outputs vs. in the old flowgraph.
        }  
    }

    void dsp_flat_flowgraph::dump()
    {
        for (dsp_edge_viter_t e = m_edges.begin(); e != m_edges.end(); e++)
            std::cout << " edge: " << (*e) << std::endl;

        for (dsp_basic_block_viter_t p = m_blocks.begin(); p != m_blocks.end(); p++) {
            std::cout << " block: " << (*p) << std::endl;
            dsp_block_detail_ptr detail = cast_to_block_ptr(*p)->detail();
            std::cout << "  detail @" << detail << ":" << std::endl;

            int ni = detail->ninputs();
            int no = detail->noutputs();
            for (int i = 0; i < no; i++) {
                dsp_buffer_ptr buffer = detail->output(i);
                std::cout << "   output " << i << ": " << buffer << std::endl;
            }

            for (int i = 0; i < ni; i++) {
                dsp_buffer_reader_ptr reader = detail->input(i);
                std::cout << "   reader " <<  i << ": " << reader
                    << " reading from buffer=" << reader->buffer() << std::endl;
            }
        }

    }

    dsp_block_vector_t
        dsp_flat_flowgraph::make_block_vector(dsp_basic_block_vector_t &blocks)
    {
        dsp_block_vector_t result;
        for (dsp_basic_block_viter_t p = blocks.begin(); p != blocks.end(); p++) {
            result.push_back(cast_to_block_ptr(*p));
        }

        return result;
    }

    void dsp_flat_flowgraph::connect_d(dsp_basic_block_ptr src_block, int src_port, dsp_basic_block_ptr dst_block, int dst_port)
    {
        connect_d(dsp_endpoint(src_block, src_port), dsp_endpoint(dst_block, dst_port));
    }

    void dsp_flat_flowgraph::connect_d(const dsp_endpoint &src, const dsp_endpoint &dst)
    {
        dsp_edge edge = dsp_edge(src,dst);
        src.block()->add_connection(edge);

        dsp_block_ptr src_block = cast_to_block_ptr( src.block() );
        dsp_block_ptr dst_block = cast_to_block_ptr( dst.block() );

        dsp_block_detail_ptr src_detail = src_block->detail();
        dsp_buffer_ptr src_buffer = allocate_buffer(src_block, src.port());
        src_detail->set_output(src.port(), src_buffer);

        dsp_block_detail_ptr dst_detail = dst_block->detail();
        dst_detail->set_input(dst.port(), dsp_buffer_add_reader(src_buffer, dst_block));

        m_edges.push_back(edge);
    }

    void dsp_flat_flowgraph::connect_c(dsp_basic_block_ptr src_block, int src_port, dsp_basic_block_ptr dst_block, int dst_port)
    {
        connect_c(dsp_endpoint(src_block, src_port), dsp_endpoint(dst_block, dst_port));
    }

    void dsp_flat_flowgraph::connect_c(const dsp_endpoint &src, const dsp_endpoint &dst)
    {
        dsp_edge edge = dsp_edge(src,dst);
        src.block()->add_connection(edge);

        m_edges.push_back(edge);
    }
}