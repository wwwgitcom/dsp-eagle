#include "dsp_flowgraph.h"
#include "dsp_scheduler.h"
#include "stdio.h"
#include "stdlib.h"

#include "dsp_file_source.h"
#include "dsp_scope_sink.h"

using namespace OpenDSP;

class mysource : public dsp_block
{
private:
    int m_counter;
public:
    mysource()
        : dsp_block("mysource", dsp_io_signature(0, 0), dsp_io_signature(1, sizeof(int)))
    {
        m_counter = 0;
    }
    ~mysource(){};

    virtual dsp_block::state
        general_work()
    {
        int* op = output<int>(0);

        if (noutput(0) < 16)
        {
            return dsp_block::BLKD_OUT;
        }
        printf("mysource==>\n");
        for (int i = 0; i < 16; i++)
        {
            op[i] = m_counter++;
        }

        produce(0, 16);

        return dsp_block::READY;
    }

    virtual void reset(){}
};

class myfilter : public dsp_block
{
private:
    int m_counter;
public:
    myfilter()
        : dsp_block("myfilter", dsp_io_signature(1, sizeof(int)), dsp_io_signature(1, sizeof(int)))
    {
        m_counter = 0;
    }
    ~myfilter(){};

    virtual dsp_block::state
        general_work()
    {
        int* ip = input<int>(0);
        int* op = output<int>(0);

        if (ninput(0) < 16)
        {
            return dsp_block::BLKD_IN;
        }

        if (noutput(0) < 16)
        {
            return dsp_block::BLKD_OUT;
        }
        printf("myfilter==>\n");
        for (int i = 0; i < 16; i++)
        {
            op[i] = ip[i] * 2 + 1;
        }
        consume(0, 16);
        produce(0, 16);

        return dsp_block::READY;
    }

    virtual void reset(){}
};

class mysink : public dsp_block
{
private:
    int m_counter;
public:
    mysink()
        : dsp_block("mysink", dsp_io_signature(1, sizeof(complex16)), dsp_io_signature(0, 0))
    {
        m_counter = 0;
    }
    ~mysink(){};

    virtual dsp_block::state
        general_work()
    {
        complex16* ip = input<complex16>(0);

        if (ninput(0) < 16)
        {
            return dsp_block::BLKD_IN;
        }

        printf("mysink==>\n");
        for (int i = 0; i < 16; i++)
        {
            printf("[%d, %d] ", ip[i].re, ip[i].im);
        }
        printf("\n");
        consume(0, 16);

        return dsp_block::READY;
    }

    virtual void reset(){}
};

extern dsp_scheduler_ptr _scheduler;

void dsp_flowgraph_test()
{
    //mysource _source;
    //myfilter _filter;
    //mysink   _sink;

    //dsp_file_source _source("C:\\DiskD\\Danial\\KuaiPan\\OpenDSP\\11ndump\\11g\\54Mbps_3.dmp.new2.dmp");
    dsp_file_source _source("MiMoRx_0.dmp");
    dsp_scope_sink  _sink;
    mysink  _sink2;

    dsp_flat_flowgraph_ptr _fg = dsp_make_flat_flowgraph();

    _fg->connect((dsp_basic_block_ptr)&_source, 0, &_sink, 0);
    _fg->connect((dsp_basic_block_ptr)&_source, 0, &_sink2, 0);

    _fg->validate();

    _fg->setup_connections();

    _fg->dump();

    _scheduler = dsp_scheduler_sts::make(_fg);

    _scheduler->run();
    
    if (_scheduler) delete _scheduler;
    delete _fg;
}