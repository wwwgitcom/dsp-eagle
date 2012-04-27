#include "stdafx.h"


void dot11n_tx_test();
void dot11n_rx_test(char** dumpfiles);
void dot11n_tx(int mcs, int length);
void dot11n_rx();

void multi_task_test();
void dsp_buffer_test();

void dsp_block_test();

void dsp_flowgraph_test();

void dsp_dot11a_test();

int dsp_testunit(int argc, _TCHAR* argv[])
{
#if 0
    dsp_dot11a_test();
    return 0;
#endif
#if 0
    multi_task_test();
    return 0;
#endif

#if 0
    if ( strcmp(argv[1], "tx") == 0 )
    {
        dot11n_tx( atoi(argv[2]), atoi(argv[3]) );
        txbuffer.toRxDumpFile20M("mimo");
    }
    else if ( strcmp(argv[1], "rx") == 0 )
    {
        dot11nphy.m_rxstream.load(argv + 2);
        dot11n_rx();
    }

    return 0;
#endif

#if 0
    dot11n_tx_test();
    txbuffer.toRxDumpFile20M("mimo");
    //txbuffer.toTxtFile("mimo");
#else
    if (argc < 3)
    {
        dot11n_rx_test(NULL);
    }
    else
    {
        dot11n_rx_test(argv + 1);
    }

#endif

    return 0;
}