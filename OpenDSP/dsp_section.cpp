#include "stdafx.h"

#if 1
#pragma section("dsp_shared_section", read, write, shared)

#define declare_shared_var(...)\
    __declspec(allocate("dsp_shared_section")) __VA_ARGS__

declare_shared_var(int idsp = 0;);
declare_shared_var(int idsp2 = 0;);
#else

#pragma comment(linker,"/SECTION:dsp_shared_section,RWS")

#pragma data_seg("dsp_shared_section")
    int idsp = 0;
    int idsp2 = 1;
#pragma data_seg()

#endif