// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <bitset>
#include <assert.h>
#include <string>
#include <vector>
#include <iterator>
#include <functional>
#include <algorithm>
#include <Windows.h>
#include <ppl.h>
#include <concrtrm.h>

#include "dsp_complex.h"
#include "dsp_vector1.h"
#include "dsp_fft.h"
#include "dsp_tickcount.h"
#include "dsp_scramble.h"
#include "dsp_crc.h"
#include "dsp_interleave.h"
#include "dsp_perf.h"
#include "dsp_task.h"
#include "dsp_channel.h"
#include "dsp_ber.h"
#include "dot11n_def.h"
#include "dot11n_plcp.h"
#include "dsp_draw.h"
#include "dsp_task.h"

using namespace std;
using namespace Concurrency;
using namespace OpenDSP;

// TODO: reference additional headers your program requires here
#define USER_MODE