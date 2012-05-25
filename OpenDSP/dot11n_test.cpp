#include "stdafx.h"


//
dot11n_phy       dot11nphy;
dot11n_tx_buffer txbuffer;
//
unsigned __int8 srcbytes[2000];
unsigned __int8 dstbytes[2000];
crc::crc32 dot11n_tx_crc32;

template<typename _Fn>
void mcs_test(_Fn& fn)
{
    tick_count t1, t2;
    t1 = tick_count::now();
    for (int i = 0; i < 1; i++)
    {
        fn();
    }
    t2 = tick_count::now();
    cout << (t2 - t1).ms() << "ms \t"
        << dot11nphy.m_srcsize * 8 / (double)(t2 - t1).ms()
        << "Mbps"
        << endl;
};

#define testbytes 1500

void dot11n_tx_test()
{
    dot11nphy.m_psrc    = srcbytes;
    dot11nphy.m_srcsize = testbytes;
    dot11nphy.m_txmcs   = 12;
    dot11n_tx_crc32.reset();

    for (int i = 0; i < testbytes - 4; i++)
    {
        srcbytes[i] = 3;
        dot11n_tx_crc32(srcbytes[i]);
    }

    *((unsigned __int32*)&srcbytes[testbytes - 4]) = dot11n_tx_crc32.value();

    for (int i = testbytes; i < testbytes + 256; i++)
    {
        srcbytes[i] = 0;
    }

    //cout << "mcs8  \n";    mcs_test([&](){ dot11nphy.mcs8_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;
    //cout << "mcs9  \n";    mcs_test([&](){ dot11nphy.mcs9_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;
    //cout << "mcs10 ";    mcs_test([&](){ dot11nphy.mcs10_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;
    //cout << "mcs11 ";    mcs_test([&](){ dot11nphy.mcs11_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;
    cout << "mcs12 ";    mcs_test([&](){ dot11nphy.mcs12_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;
    //cout << "mcs13 \n";    mcs_test([&](){ dot11nphy.mcs13_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;
    //cout << "mcs14 ";    mcs_test([&](){ dot11nphy.mcs14_entry(txbuffer); });
    //cout << dot11nphy.m_symbolcount << " symbols" << endl;

    txbuffer.toRxDumpFile20M("mimo");
}

void dot11n_rx_test(char** dumpfiles)
{
#if 1
    //SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
    //SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    SetThreadAffinityMask(GetCurrentThread(), 0x1);
#endif

    if (NULL == dumpfiles)
    {
        //char* teststreams[] = {"c:\\MiMoRx_0.dmp", "c:\\MiMoRx_1.dmp"};
        char* teststreams[] = {"c:\\mimo_0.dmp", "c:\\mimo_1.dmp"};
        //char* teststreams[] = {"miso2.dmp", "miso2.dmp"};
        //char* teststreams[] = {"54Mbps.dmp", "54Mbps.dmp"};
        dot11nphy.m_rxstream.load(teststreams);
    }
    else
    {
        dot11nphy.m_rxstream.load(dumpfiles);
    }
    dot11nphy.rx_scheduler();
}

void dot11n_tx(int mcs, int length)
{
    unsigned __int8 * psrc = new unsigned __int8[length + 256];
    dot11nphy.m_psrc    = psrc;
    dot11nphy.m_srcsize = length;
    dot11nphy.m_txmcs   = mcs;

    dot11n_tx_crc32.reset();
    for (int i = 0; i < length - 4; i++)
    {
        psrc[i] = 3;
        dot11n_tx_crc32(psrc[i]);
    }

    *((unsigned __int32*)&psrc[length - 4]) = dot11n_tx_crc32.value();

    printf("CRC32 tx=%p\n", dot11n_tx_crc32.value());

    for (int i = length; i < length + 256; i++)
    {
        psrc[i] = 0;
    }

    switch (mcs)
    {
    case 8:
        dot11nphy.mcs8_entry(txbuffer);
        break;
    case 9:
        dot11nphy.mcs9_entry(txbuffer);
        break;
    case 10:
        dot11nphy.mcs10_entry(txbuffer);
        break;
    case 11:
        dot11nphy.mcs11_entry(txbuffer);
        break;
    case 12:
        dot11nphy.mcs12_entry(txbuffer);
        break;
    case 13:
        dot11nphy.mcs13_entry(txbuffer);
        break;
    case 14:
        dot11nphy.mcs14_entry(txbuffer);
        break;
    }

    delete[] psrc;
}

void dot11n_rx()
{
    dot11nphy.rx_scheduler();
}

