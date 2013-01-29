#include "stdafx.h"


int soft_demap(complexf *in_ary, unsigned char *soft_ary, int len)
{
  int i;
  char softvalue;
  //	float temp;
  for (i=0; i<len; i++)
  {
    if (in_ary[i].re > 0.0f)
      softvalue = (int)(in_ary[i].re*2.0f+0.5f);
    else
      softvalue = (int)(in_ary[i].re*2.0f-0.5f);
    // saturation
    if (softvalue < -4)
      softvalue = -4;
    else if (softvalue > 3)
      softvalue = 3;

    softvalue += 3;

    soft_ary[i] = (unsigned char)softvalue;
  }
  return 0;
}


/************************************************************************/
/* Code Generation Hint                                                 */
/************************************************************************/

#define cc_12 1
#define cc_23 2
#define cc_34 3

#define mod_bpsk 1
#define mod_qpsk 2
#define mod_16qam 3
#define mod_64qam 4

#define cc_now cc_34
#define mod_now mod_64qam

/************************************************************************/
/* Viterbi Template for (2, 1, 7)                                       */
/************************************************************************/

#pragma warning(disable: 4355)// disable warning: this used in init list
template<int nTraceBackLength = 36, int nTraceBackOutput = 48>
struct dsp_viterbi_64
{
  static_assert(nTraceBackOutput % 8 == 0, "Trace back output must a factor of 8!");

  static const int nTracebackDataCount  = nTraceBackLength + nTraceBackOutput;

  static_assert(nTracebackDataCount % 2 == 0, "(nTraceBackLength + nTraceBackOutput) % 2 != 0");

  static const int nTracebackOffset     = nTracebackDataCount - 1;
  static const int nTraceBackOutputByte = nTraceBackOutput / 8;

  // ACS
  v_align(64) v_ub m_vBM[64][4]; // 64 states, each state has two 32 BM
  v_align(64) v_ub m_vBM0[8][4]; // 64 states, each state has two 32 BM
  v_align(64) v_ub m_vBM1[8][4]; // 64 states, each state has two 32 BM

  // shared
  v_align(64) v_ub m_vShuffleMask;
  v_ub m_vNormMask;
  unsigned __int8 *m_pSoftBits;
  unsigned __int8 *m_pDecodedBytes;
  __int32          m_nDecodedBytes;

  // shared
#define RQ_MASK (1024 - 1)
  typedef v_ub TBState[4];
  typedef unsigned __int16 SurviorPath[4];
  typedef RingQ<SurviorPath, unsigned __int16, 1024> VitRQ;

  v_align(64) VitRQ     m_TBQ;
  v_align(64) TBState   m_MinAddress[1024];

  // TB
  v_align(64) v_ub      m_vStateIndex[4];

  //////////////////////////////////////////////////////////////////////////

  dsp_viterbi_64() : m_TBWorker(*this), m_ACSWorker(*this), m_Worker(*this), m_ACSCPU(2), m_TBCPU(2)
  {
    unsigned char v = 0;
    int i;

    v = 0;
    for (i = 0; i < 8; i++)
    {
      m_vShuffleMask[i] = v;
      v += 2;
    }
    v = 1;
    for (; i < 16; i++)
    {
      m_vShuffleMask[i] = v;
      v += 2;
    }

    v = 0;
    for (i = 0; i < 64; i++)
    {
      m_vStateIndex[0][i] = v++;
    }

    m_vNormMask.v_setall(80);


    unsigned __int8 bAddress;
    unsigned __int8 m00, m01, m10, m11;
    unsigned __int8 m0, m1;

    v_ub v14;
    v14.v_setall(14);

    v_ub v7;
    v7.v_setall(7);

    for (unsigned __int8 b0 = 0; b0 < 8; b0++)
    {
      for (unsigned __int8 b1 = 0; b1 < 8; b1++)
      {
        bAddress = ( (b0 & 0x7) | ((b1 & 0x7) << 3) );

        //printf("b0= %d, b1 = %d, Address = %d \nSoftBits", b0, b1, bAddress);

        m00 = b1 + b0;
        m01 = b1 + 7 - b0;
        m10 = 7 - b1 + b0;
        m11 = 7 - b1 + 7 - b0;

        //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\nSoftBits", m00, m01, m10, m11);

        m_vBM[bAddress][0].v_set(m00, m01, m00, m01, m11, m10, m11, m10, m11, m10, m11, m10, m00, m01, m00, m01);
        m_vBM[bAddress][1] = v_sub(v14, m_vBM[bAddress][0]);
        m_vBM[bAddress][2].v_set(m10, m11, m10, m11, m01, m00, m01, m00, m01, m00, m01, m00, m10, m11, m10, m11);
        m_vBM[bAddress][3] = v_sub(v14, m_vBM[bAddress][2]);
      }
    }

    for (unsigned __int8 b0 = 0; b0 < 8; b0++)
    {
      bAddress = (b0 & 0x7);

      //printf("b0= %d, b1 = %d, Address = %d \nSoftBits", b0, b1, bAddress);

      m0 = b0;
      m1 = 7 - b0;

      //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\nSoftBits", m00, m01, m10, m11);

      m_vBM0[bAddress][0].v_set(m0, m1, m0, m1, m1, m0, m1, m0, m1, m0, m1, m0, m0, m1, m0, m1);
      m_vBM0[bAddress][1] = v_sub(v7, m_vBM0[bAddress][0]);
      m_vBM0[bAddress][2].v_set(m0, m1, m0, m1, m1, m0, m1, m0, m1, m0, m1, m0, m0, m1, m0, m1);
      m_vBM0[bAddress][3] = v_sub(v7, m_vBM0[bAddress][2]);
    }

    for (unsigned __int8 b1 = 0; b1 < 8; b1++)
    {
      bAddress = (b1 & 0x7);

      //printf("b0= %d, b1 = %d, Address = %d \nSoftBits", b0, b1, bAddress);

      m0 = b1;
      m1 = 7 - b1;

      //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\nSoftBits", m00, m01, m10, m11);

      m_vBM1[bAddress][0].v_set(m0, m0, m0, m0, m1, m1, m1, m1, m1, m1, m1, m1, m0, m0, m0, m0);
      m_vBM1[bAddress][1] = v_sub(v7, m_vBM1[bAddress][0]);
      m_vBM1[bAddress][2].v_set(m1, m1, m1, m1, m0, m0, m0, m0, m0, m0, m0, m0, m1, m1, m1, m1);
      m_vBM1[bAddress][3] = v_sub(v7, m_vBM1[bAddress][2]);
    }
  }

  inline void v_print(v_ub &v)
  {
    for (int i = 0; i < v_ub::elem_cnt; i++)
    {
      printf("%4d ", v[i]);
    }

    printf("\n");
  }

  inline void v_print(v_ub &v1, v_ub &v2, v_ub &v3, v_ub &v4)
  {
    static int icount = 0;
    printf("%d: ", icount++);
    for (int i = 0; i < v_ub::elem_cnt; i++)
    {
      printf("%2d ", v1[i]);
    }

    printf("| ");

    for (int i = 0; i < v_ub::elem_cnt; i++)
    {
      printf("%2d ", v2[i]);
    }
    printf("| ");

    for (int i = 0; i < v_ub::elem_cnt; i++)
    {
      printf("%2d ", v3[i]);
    }
    printf("| ");
    for (int i = 0; i < v_ub::elem_cnt; i++)
    {
      printf("%2d ", v4[i]);
    }

    printf("\n");
  }

  inline void v_print(v_ub *v, int nv)
  {
    static int icount = 0;
    printf("%d: ", icount++);
    for (int j = 0; j < nv; j++)
    {
      for (int i = 0; i < v_ub::elem_cnt; i++)
      {
        printf("%4d ", v[j][i]);
      }                
    }
    printf("\n");
  }


  __forceinline v_ub ACS_Branch(v_ub& va, v_ub& vb, v_ub& vBM1, v_ub& vBM2, unsigned __int16& iPH)
  {
    v_ub vc    = v_add(va, vBM1);// from 0
    v_ub vd    = v_add(vb, vBM2);// from 1
    v_b vdiff  = v_sub(vd, vc); // difference

    iPH        = (unsigned __int16)vdiff.v_signmask();

    v_ub vmin  = v_min(vc, vd);

    return vmin;
  }

  __forceinline void Shuffle(v_ub& va, v_ub& vb, v_ub& vmask, v_ub& veven, v_ub&vodd)
  {
    va = va.v_shuffle(vmask);
    vb = vb.v_shuffle(vmask);

    (v_q&)veven = ((v_q&)va).v_unpack_lo((v_q&)vb);
    (v_q&)vodd  = ((v_q&)va).v_unpack_hi((v_q&)vb);
  }

  __forceinline void ButterFly(v_ub& va, v_ub& vb, v_ub& vShuffleMask, v_ub& vBM1, v_ub& vBM2, unsigned __int16& iPH1, unsigned __int16& iPH2)
  {
    v_ub veven, vodd;
    Shuffle(va, vb, vShuffleMask, veven, vodd);

    va = ACS_Branch(veven, vodd, vBM1, vBM2, iPH1);//0-branch
    vb = ACS_Branch(veven, vodd, vBM2, vBM1, iPH2);//1-branch
  }

  __forceinline __int8 BMAddress(unsigned __int8 b0, unsigned __int8 b1)
  {
    return ( (b0 & 7) | ((b1 & 7) << 3) );
  }

  __forceinline unsigned __int8 BMAddress(unsigned __int8 b)
  {
    return b;
  }

  __forceinline __int8 PrevPHAddress(unsigned __int8* pvPath, unsigned __int8& Index)
  {
    unsigned __int8 AddressHi = Index >> 3;
    unsigned __int8 AddressLo = Index & 0x7;

    unsigned __int8 v = pvPath[AddressHi];
    return ( ((v >> AddressLo) & 1) | ((Index << 1) & 0x3F) );
  }

  __forceinline unsigned __int8 FindMinValueAddress(v_ub* pvTrellis)
  {
    v_ub vb0, vb1;
    v_us vs0, vs1, vs2, vs3;

    unsigned __int16 MinPos;

    vb0 = m_vStateIndex[0];
    vb1 = pvTrellis[0];

    vs1 = (v_us)v_unpack_lo( vb0, vb1 );
    vs2 = (v_us)v_unpack_hi( vb0, vb1 );

    vs0 = v_min(vs1, vs2);

    vb0 = m_vStateIndex[1];
    vb1 = pvTrellis[1];
    vs1 = (v_us)v_unpack_lo ( vb0, vb1 );
    vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

    vs3 = v_min (vs1, vs2);
    vs0 = v_min (vs0, vs3);

    vb0 = m_vStateIndex[2];
    vb1 = pvTrellis[2];
    vs1 = (v_us)v_unpack_lo ( vb0, vb1 );
    vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

    vs3 = v_min (vs1, vs2);
    vs0 = v_min (vs0, vs3);

    vb0 = m_vStateIndex[3];
    vb1 = pvTrellis[3];
    vs1 = (v_us)v_unpack_lo ( vb0, vb1 );
    vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

    vs3 = v_min (vs1, vs2);
    vs0 = v_min (vs0, vs3);

    vs0 = vs0.v_hminpos();

    MinPos = vs0.v_get_at<0>();// the minimum index in 7:0
    //MinPos &= 0xFF;

    return (unsigned __int8)MinPos;
  }

  __forceinline void Normalize2(v_ub* pvTrellis)
  {
    v_ub vb0, vb1, vb2, vb3, vb4, vb5;
    v_ub vm0, vm1, vm2;

    vb0 = pvTrellis[0];
    vb1 = pvTrellis[1];

    vb4 = v_min(vb0, vb1);

    vm0 = v_max(vb0, vb1);

    vb2 = pvTrellis[2];
    vb3 = pvTrellis[3];

    vb5 = v_min(vb2, vb3);
    vb4 = v_min(vb4, vb5);

    vm1 = v_max(vb2, vb3);
    vm2 = v_max(vm0, vb1);

    vb5 = ((v_i&)vb4).v_shuffle<2, 3, 0, 1>();

    vm1 = ((v_i&)vm2).v_shuffle<2, 3, 0, 1>();

    vb4 = v_min(vb4, vb5);

    vm2 = v_max(vm1, vm2);

    // vb0 contains 8 minimum values
    // cast into 8 shorts
    vb5 = v_unpack_lo(vb4, vb4);
    vm1 = v_unpack_lo(vm2, vm2);

    vb4 = ((v_us&)vb5).v_hminpos();
    vm2 = ((v_us&)vm1).v_hmaxpos();

    printf( "trellis min=%d, max=%d\n", vb4[0], vm2[0] );

    vb4 = ((v_s&)vb4).v_shuffle_lo<0, 0, 0, 0>();
    vb4 = v_unpack_lo(vb4, vb4);

    pvTrellis[0] = v_sub(vb0, vb4);
    pvTrellis[1] = v_sub(vb1, vb4);
    pvTrellis[2] = v_sub(vb2, vb4);
    pvTrellis[3] = v_sub(vb3, vb4);
  }


  __forceinline void Normalize(v_ub* pvTrellis)
  {
    v_ub vb0, vb1, vb2, vb3, vb4, vb5;

    vb0 = pvTrellis[0];
    vb1 = pvTrellis[1];

    vb4 = v_min(vb0, vb1);
    
    vb2 = pvTrellis[2];
    vb3 = pvTrellis[3];

    vb5 = v_min(vb2, vb3);
    vb4 = v_min(vb4, vb5);

    vb5 = ((v_i&)vb4).v_shuffle<2, 3, 0, 1>();

    vb4 = v_min(vb4, vb5);

    // vb0 contains 8 minimum values
    // cast into 8 shorts
    vb5 = v_unpack_lo(vb4, vb4);

    vb4 = ((v_us&)vb5).v_hminpos();
    
    vb4 = ((v_s&)vb4).v_shuffle_lo<0, 0, 0, 0>();
    vb4 = v_unpack_lo(vb4, vb4);

    pvTrellis[0] = v_sub(vb0, vb4);
    pvTrellis[1] = v_sub(vb1, vb4);
    pvTrellis[2] = v_sub(vb2, vb4);
    pvTrellis[3] = v_sub(vb3, vb4);
  }

  __forceinline void Normalize(v_ub* pvTrellis, v_ub& vNormConst)
  {
    pvTrellis[0] = v_sub(pvTrellis[0], vNormConst);
    pvTrellis[1] = v_sub(pvTrellis[1], vNormConst);
    pvTrellis[2] = v_sub(pvTrellis[2], vNormConst);
    pvTrellis[3] = v_sub(pvTrellis[3], vNormConst);
  }

  __forceinline void TraceBack(unsigned __int16 HistoryIndex, unsigned __int8 MinIndex, int TraceBackLength, int TraceOutputLength, unsigned __int8* pDecodedBytes)
  {
    unsigned __int8* pPathHistory;

    unsigned __int8 PHAddress;

    PHAddress = MinIndex;

    unsigned __int8 Output = 0;
    unsigned __int8* p = pDecodedBytes;

    for (int i = 0; i < TraceBackLength; i++)
    {
      pPathHistory = (unsigned __int8*)&m_TBQ[HistoryIndex];

      PHAddress = PrevPHAddress(pPathHistory, PHAddress);

      HistoryIndex -= 1;
      HistoryIndex &= RQ_MASK;
    }

    for (int i = 0; i < TraceOutputLength >> 3; i++)
    {
      for ( int j = 0; j < 8; j++)
      {
        Output <<= 1;
        Output |= (PHAddress >> 5);// since we know only 6 bits are valid

        pPathHistory = (unsigned __int8*)&m_TBQ[HistoryIndex];

        PHAddress = PrevPHAddress(pPathHistory, PHAddress);

        HistoryIndex -= 1;
        HistoryIndex &= RQ_MASK;
      }
      *p = Output;
      p--;
    }

#if 0
    // output decoded bytes
    p++;
    for (int l = 0; l < TraceOutputLength >> 3; l++)
    {
      printf("%02X ", p[l]);
    }
    printf("%\n");
#endif
  }


  //////////////////////////////////////////////////////////////////////////


  struct viterbi_traceback_worker : dsp_task::task 
  {
    dsp_viterbi_64& m_viterbi;

    viterbi_traceback_worker(dsp_viterbi_64& vit) : m_viterbi(vit)
    {

    }

    dsp_task::task* execute()
    {
      VitRQ::iter      TBQrit            = 0;
      unsigned __int16 MinAddressIndex   = 0;

      unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
      __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      unsigned __int16 TraceBackPt       = 0;

      tick_count tbegin, tend, tend2, tduration, tduration2;
      tbegin = tick_count::now();

      while (TotalDecodedBytes > 0)
      {
        while( m_viterbi.m_TBQ.DataCount() < nTracebackDataCount )
        {
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(&m_viterbi.m_MinAddress[MinAddressIndex++][0]);
        MinAddressIndex &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit                   += nTraceBackOutput;
        TBQrit                   &= RQ_MASK;
        m_viterbi.m_TBQ.m_rcount += nTraceBackOutput;
      }

      tend = tick_count::now();
      tduration = tend - tbegin;
      printf("TBThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());

      return this;
    }
  };

  struct viterbi_acs_worker : dsp_task::task 
  {
    dsp_viterbi_64& m_viterbi;

    viterbi_acs_worker(dsp_viterbi_64& vit) : m_viterbi(vit)
    {

    }


    void execute_1_2()
    {
      v_ub vStates[4];
      v_ub vBM0, vBM1;
      v_ub vShuffleMask;
      int  BMIndex = 4;
      unsigned __int16 iPH0, iPH1;
      unsigned __int16 MinAddressIndex = 0;

      vShuffleMask = m_viterbi.m_vShuffleMask;

      for (int i = 0; i < 4; i++)
      {
        vStates[i].v_setall(80);
      }
      vStates[0].v_set_at<0>(0);

      VitRQ::iter TBQwit       = 0;
      unsigned int iTrellis    = 0;

      unsigned __int8 *pSoftBits = m_viterbi.m_pSoftBits;
      __int32 nTotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      __int32 nTotalSoftBits     = nTotalDecodedBytes * 8 * 2; // 1/2 coding

      m_viterbi.m_TBQ.m_wcount = nTraceBackLength;
      m_viterbi.m_TBWorker.RunA();

      tick_count tbegin, tend, tend2, tduration, tduration2;
      tbegin = tick_count::now();

      for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 4)
      {
        // stage StageIndex
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

        v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;
        // stage StageIndex + 1
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2], pSoftBits[nSoftBits + 3]);

        pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit &= RQ_MASK;
        iTrellis += 2;

        //m_viterbi.v_print(vStates, 4);

        if (vStates[0].v_get_at<0>() > 170)
        {
          //m_viterbi.v_print(vStates, 1);
          m_viterbi.Normalize(vStates);
        }

        if (iTrellis >= nTracebackDataCount)
        {
          //unsigned __int8 PHAddress = m_viterbi.FindMinValueAddress(vStates);
          //m_viterbi.m_MinAddress[MinAddressIndex++] = PHAddress;
          m_viterbi.m_MinAddress[MinAddressIndex][0] = vStates[0];
          m_viterbi.m_MinAddress[MinAddressIndex][1] = vStates[1];
          m_viterbi.m_MinAddress[MinAddressIndex][2] = vStates[2];
          m_viterbi.m_MinAddress[MinAddressIndex][3] = vStates[3];

          MinAddressIndex++;
          MinAddressIndex &= RQ_MASK;
          //printf("TB MinAddress=%d, Index=%d\nSoftBits", PHAddress, MinAddressIndex);

          iTrellis                 -= nTraceBackOutput;
          nTotalDecodedBytes       -= nTraceBackOutputByte;
          m_viterbi.m_TBQ.m_wcount += nTraceBackOutput;

          //printf("WTB: w=%u, r=%u\nSoftBits", m_viterbi.m_TBQ.m_wcount, m_viterbi.m_TBQ.m_rcount);

          while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
        }
      }

      v_ub vZero;
      vZero.v_zero();

      unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);
      if ( iTrellis >= nTraceBackLength )
      {
        for (unsigned int i = 0; i < iTrellisPadding; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;

        m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
        m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
        MinAddressIndex++;
        MinAddressIndex &= RQ_MASK;

        nTotalDecodedBytes                       -= nTraceBackOutputByte;
        m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
      }

      while (nTotalDecodedBytes > 0)
      {
        // pad traceback length zeros
        while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
        for (unsigned int i = 0; i < nTraceBackOutput; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;
        m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
        m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
        MinAddressIndex++;
        MinAddressIndex &= RQ_MASK;

        nTotalDecodedBytes        -= nTraceBackOutputByte;
        m_viterbi.m_TBQ.m_wcount  += nTraceBackOutput;
      }

      tend = tick_count::now();
      tduration = tend - tbegin;
      printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
    }

    // A0 A1 A2 A3
    // B0 *  B2 *
    // A0 B0 A1 A2 B2 A3
    void execute_2_3()
    {
      v_ub vStates[4];
      v_ub vBM0, vBM1;
      v_ub vShuffleMask;
      int  BMIndex = 4;
      unsigned __int16 iPH0, iPH1;
      unsigned __int16 MinAddressIndex = 0;

      vShuffleMask = m_viterbi.m_vShuffleMask;

      for (int i = 0; i < 4; i++)
      {
        vStates[i].v_setall(80);
      }
      vStates[0].v_set_at<0>(0);

      VitRQ::iter TBQwit       = 0;
      unsigned int iTrellis    = 0;

      unsigned __int8 *pSoftBits = m_viterbi.m_pSoftBits;
      __int32 nTotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      __int32 nTotalSoftBits     = nTotalDecodedBytes * 8 * 3 / 2; // 2/3 coding

      m_viterbi.m_TBQ.m_wcount = nTraceBackLength;
      m_viterbi.m_TBWorker.RunA();

      tick_count tbegin, tend, tend2, tduration, tduration2;
      tbegin = tick_count::now();
      for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 3)
      {
        // stage StageIndex
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

        v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;
        // stage StageIndex + 1
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

        pvBM = &m_viterbi.m_vBM0[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit &= RQ_MASK;
        iTrellis += 2;

        //m_viterbi.v_print(vStates, 4);

        if (vStates[0].v_get_at<0>() > 170)
        {
          //m_viterbi.v_print(vStates, 1);
          m_viterbi.Normalize(vStates);
        }

        if (iTrellis >= nTracebackDataCount)
        {
          //unsigned __int8 PHAddress = m_viterbi.FindMinValueAddress(vStates);
          //m_viterbi.m_MinAddress[MinAddressIndex++] = PHAddress;
          m_viterbi.m_MinAddress[MinAddressIndex][0] = vStates[0];
          m_viterbi.m_MinAddress[MinAddressIndex][1] = vStates[1];
          m_viterbi.m_MinAddress[MinAddressIndex][2] = vStates[2];
          m_viterbi.m_MinAddress[MinAddressIndex][3] = vStates[3];

          MinAddressIndex++;
          MinAddressIndex &= RQ_MASK;
          //printf("TB MinAddress=%d, Index=%d\nSoftBits", PHAddress, MinAddressIndex);

          iTrellis                 -= nTraceBackOutput;
          nTotalDecodedBytes       -= nTraceBackOutputByte;
          m_viterbi.m_TBQ.m_wcount += nTraceBackOutput;

          //printf("WTB: w=%u, r=%u\nSoftBits", m_viterbi.m_TBQ.m_wcount, m_viterbi.m_TBQ.m_rcount);

          while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
        }
      }

      v_ub vZero;
      vZero.v_zero();

      unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);
      if ( iTrellis >= nTraceBackLength )
      {
        for (unsigned int i = 0; i < iTrellisPadding; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;

        m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
        m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
        MinAddressIndex++;
        MinAddressIndex &= RQ_MASK;

        nTotalDecodedBytes                       -= nTraceBackOutputByte;
        m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
      }

      while (nTotalDecodedBytes > 0)
      {
        // pad traceback length zeros
        while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
        for (unsigned int i = 0; i < nTraceBackOutput; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        //m_viterbi.m_MinAddress[MinAddressIndex++] = 0;
        m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
        m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
        MinAddressIndex++;
        MinAddressIndex &= RQ_MASK;

        nTotalDecodedBytes                       -= nTraceBackOutputByte;
        m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
      }

      tend = tick_count::now();
      tduration = tend - tbegin;
      //printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
    }

    // A0 A1 *  A3 A4 *
    // B0 *  B2 B3 *  B5
    // A0 B0 A1 B2 A3 B3 A4 B5
    void execute_3_4()
    {
      v_ub vStates[4];
      v_ub vBM0, vBM1;
      v_ub vShuffleMask;
      int  BMIndex = 4;
      unsigned __int16 iPH0, iPH1;
      unsigned __int16 MinAddressIndex = 0;

      vShuffleMask = m_viterbi.m_vShuffleMask;

      for (int i = 0; i < 4; i++)
      {
        vStates[i].v_setall(80);
      }
      vStates[0].v_set_at<0>(0);

      VitRQ::iter TBQwit       = 0;
      unsigned int iTrellis    = 0;

      unsigned __int8 *pSoftBits = m_viterbi.m_pSoftBits;
      __int32 nTotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      __int32 nTotalSoftBits     = nTotalDecodedBytes * 8 * 4 / 3; // 3/4 coding

      m_viterbi.m_TBQ.m_wcount = nTraceBackLength;
      m_viterbi.m_TBWorker.RunA();

      tick_count tbegin, tend, tend2, tduration, tduration2;
      tbegin = tick_count::now();
      for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 8)
      {
        // stage StageIndex
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

        v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;

        // stage StageIndex + 1
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

        pvBM = &m_viterbi.m_vBM0[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;
        // stage StageIndex + 2
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 3]);

        pvBM = &m_viterbi.m_vBM1[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;

        // stage StageIndex + 3
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 4], pSoftBits[nSoftBits + 5]);

        pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;
        // stage StageIndex + 4
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 6]);

        pvBM = &m_viterbi.m_vBM0[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;
        // stage StageIndex + 5
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 7]);

        pvBM = &m_viterbi.m_vBM1[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;
        iTrellis += 6;

        //m_viterbi.v_print(vStates, 4);

        if (vStates[0].v_get_at<0>() > 170)
        {
          //m_viterbi.v_print(vStates, 1);
          m_viterbi.Normalize(vStates);
        }

        if (iTrellis >= nTracebackDataCount)
        {
          //unsigned __int8 PHAddress = m_viterbi.FindMinValueAddress(vStates);
          //m_viterbi.m_MinAddress[MinAddressIndex++] = PHAddress;
          m_viterbi.m_MinAddress[MinAddressIndex][0] = vStates[0];
          m_viterbi.m_MinAddress[MinAddressIndex][1] = vStates[1];
          m_viterbi.m_MinAddress[MinAddressIndex][2] = vStates[2];
          m_viterbi.m_MinAddress[MinAddressIndex][3] = vStates[3];

          MinAddressIndex++;
          MinAddressIndex &= RQ_MASK;

          //printf("TB MinAddress=%d, Index=%d\nSoftBits", PHAddress, MinAddressIndex);

          iTrellis                 -= nTraceBackOutput;
          nTotalDecodedBytes       -= nTraceBackOutputByte;
          m_viterbi.m_TBQ.m_wcount += nTraceBackOutput;

          //printf("WTB: w=%u, r=%u, bytes=%d\n", m_viterbi.m_TBQ.m_wcount, m_viterbi.m_TBQ.m_rcount, nTotalDecodedBytes);

          while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
        }
      }

      v_ub vZero;
      vZero.v_zero();

      unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);
      if ( iTrellis >= nTraceBackLength )
      {
        for (unsigned int i = 0; i < iTrellisPadding; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
        m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
        MinAddressIndex++;
        MinAddressIndex &= RQ_MASK;

        nTotalDecodedBytes                       -= nTraceBackOutputByte;
        m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
      }

      while (nTotalDecodedBytes > 0)
      {
        // pad traceback length zeros
        while ( !m_viterbi.m_TBQ.WCheck(nTraceBackOutput) );
        for (unsigned int i = 0; i < nTraceBackOutput; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        m_viterbi.m_MinAddress[MinAddressIndex][0] = m_viterbi.m_MinAddress[MinAddressIndex][1] = vZero;
        m_viterbi.m_MinAddress[MinAddressIndex][2] = m_viterbi.m_MinAddress[MinAddressIndex][3] = vZero;
        MinAddressIndex++;
        MinAddressIndex &= RQ_MASK;

        nTotalDecodedBytes                       -= nTraceBackOutputByte;
        m_viterbi.m_TBQ.m_wcount                 += nTraceBackOutput;
      }

      tend = tick_count::now();
      tduration = tend - tbegin;
      printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
    }


    dsp_task::task* execute()
    {

#if cc_now == cc_12
      execute_1_2();
#elif cc_now == cc_23
      execute_2_3();
#elif cc_now == cc_34
      execute_3_4();
#endif

      return this;
    }
  };


  struct viterbi_worker : dsp_task::task 
  {
    dsp_viterbi_64& m_viterbi;

    viterbi_worker(dsp_viterbi_64& vit) : m_viterbi(vit)
    {

    }

    void execute_1_2()
    {
      v_ub vShuffleMask;
      v_ub vNormMask;
      v_ub vStates[4];
      v_ub vBM0, vBM1;

      int  BMIndex = 4;
      unsigned __int16 iPH0, iPH1;
      unsigned __int16 MinAddressIndex = 0;

      vShuffleMask = m_viterbi.m_vShuffleMask;
      vNormMask    = m_viterbi.m_vNormMask;

      vNormMask.v_setall(80);

      for (int i = 0; i < 4; i++)
      {
        vStates[i].v_setall(80);
      }
      vStates[0].v_set_at<0>(0);


      VitRQ::iter  TBQwit     = 0;
      VitRQ::iter  TBQrit     = 0;
      unsigned int iTrellis   = 0;

      unsigned __int8 *pSoftBits         = m_viterbi.m_pSoftBits;
      unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
      __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      unsigned __int16 TraceBackPt       = 0;

      tick_count tbegin, tend, tend2, tduration, tduration2;

      __int32 nTotalSoftBits = m_viterbi.m_nDecodedBytes * 8 * 2; // 1/2 coding

      tbegin = tick_count::now();

      for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 4)
      {
        // stage StageIndex
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

        v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        //m_viterbi.v_print(vStates, 4);

        TBQwit += 1;
        TBQwit &= RQ_MASK;
        // stage StageIndex + 1
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2], pSoftBits[nSoftBits + 3]);

        pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;
        iTrellis += 2;

        //m_viterbi.v_print(vStates, 4);

        if (vStates[0].v_get_at<0>() > 200)
        {
          //printf("===Normalization===\n");
          //m_viterbi.v_print(vStates, 4);
          //m_viterbi.Normalize(vStates);
          //m_viterbi.Normalize(vStates, vNormMask);
          m_viterbi.Normalize(vStates);
          //m_viterbi.v_print(vStates, 4);
        }

        if (iTrellis == nTracebackDataCount)
        {
          unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(vStates);
          //unsigned __int8 MinAddress = 0;

          TraceBackPt = TBQrit + nTracebackOffset;
          TraceBackPt &= RQ_MASK;

          m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

          pDecodedBytes     += nTraceBackOutputByte;
          TotalDecodedBytes -= nTraceBackOutputByte;

          TBQrit            += nTraceBackOutput;
          TBQrit            &= RQ_MASK;
          iTrellis          -= nTraceBackOutput;
        }
      }


      //m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

      if ( iTrellis >= nTraceBackLength )
      {
        unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);

        for (unsigned int i = 0; i < iTrellisPadding; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit            += nTraceBackOutput;
        TBQrit            &= RQ_MASK;
      }

      while (TotalDecodedBytes > 0)
      {
        // pad traceback length zeros
        for (unsigned int i = 0; i < nTraceBackOutput; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit            += nTraceBackOutput;
        TBQrit            &= RQ_MASK;
      }


      tend = tick_count::now();
      tduration = tend - tbegin;
      printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
    }

    void execute_2_3()
    {
      v_ub vStates[4];
      v_ub vBM0, vBM1;
      v_ub vShuffleMask;
      v_ub vNormMask;
      int  BMIndex = 4;
      unsigned __int16 iPH0, iPH1;
      unsigned __int16 MinAddressIndex = 0;

      vShuffleMask = m_viterbi.m_vShuffleMask;

      vNormMask.v_setall(80);
      for (int i = 0; i < 4; i++)
      {
        vStates[i].v_setall(42);
      }
      vStates[0].v_set_at<0>(0);


      VitRQ::iter  TBQwit     = 0;
      VitRQ::iter  TBQrit     = 0;
      unsigned int iTrellis   = 0;

      unsigned __int8 *pSoftBits         = m_viterbi.m_pSoftBits;
      unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
      __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      unsigned __int16 TraceBackPt       = 0;

      tick_count tbegin, tend, tend2, tduration, tduration2;

      __int32 nTotalSoftBits = m_viterbi.m_nDecodedBytes * 8 * 3 / 2; // 2/3 coding

      tbegin = tick_count::now();

      for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 3)
      {
        // stage StageIndex
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

        v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;

        // stage StageIndex + 1
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);

        pvBM = &m_viterbi.m_vBM0[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;
        iTrellis += 2;

        //m_viterbi.v_print(vStates, 4);

        //if (vStates[0][0] > 200)
        //if (nSoftBits % 21 == 0)
        {
          //m_viterbi.v_print(vStates, 4);
          //m_viterbi.Normalize(vStates, vNormMask);
          m_viterbi.Normalize(vStates);
        }

        if (iTrellis >= nTracebackDataCount)
        {
          unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(vStates);
          //unsigned __int8 MinAddress = 0;

          TraceBackPt = TBQrit + nTracebackOffset;
          TraceBackPt &= RQ_MASK;

          m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

          pDecodedBytes     += nTraceBackOutputByte;
          TotalDecodedBytes -= nTraceBackOutputByte;

          TBQrit            += nTraceBackOutput;
          TBQrit            &= RQ_MASK;
          iTrellis          -= nTraceBackOutput;
        }
      }

      if ( iTrellis >= nTraceBackLength )
      {
        unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);

        for (unsigned int i = 0; i < iTrellisPadding; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit            += nTraceBackOutput;
        TBQrit            &= RQ_MASK;
      }

      while (TotalDecodedBytes > 0)
      {
        // pad traceback length zeros
        for (unsigned int i = 0; i < nTraceBackOutput; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit            += nTraceBackOutput;
        TBQrit            &= RQ_MASK;
      }


      tend = tick_count::now();
      tduration = tend - tbegin;
      printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
    }

    void execute_3_4()
    {
      v_ub vStates[4];
      v_ub vBM0, vBM1;
      v_ub vShuffleMask;
      v_ub vNormMask;
      int  BMIndex = 4;
      unsigned __int16 iPH0, iPH1;
      unsigned __int16 MinAddressIndex = 0;

      vShuffleMask = m_viterbi.m_vShuffleMask;

      vNormMask.v_setall(80);
      for (int i = 0; i < 4; i++)
      {
        vStates[i].v_setall(50);
      }
      vStates[0].v_set_at<0>(0);


      VitRQ::iter  TBQwit     = 0;
      VitRQ::iter  TBQrit     = 0;
      unsigned int iTrellis   = 0;

      unsigned __int8 *pSoftBits         = m_viterbi.m_pSoftBits;
      unsigned __int8* pDecodedBytes     = m_viterbi.m_pDecodedBytes;
      __int32          TotalDecodedBytes = m_viterbi.m_nDecodedBytes;
      unsigned __int16 TraceBackPt       = 0;

      tick_count tbegin, tend, tend2, tduration, tduration2;

      __int32 nTotalSoftBits = m_viterbi.m_nDecodedBytes * 8 * 4 / 3; // 3/4 coding

      tbegin = tick_count::now();

      for (__int32 nSoftBits = 0; nSoftBits < nTotalSoftBits; nSoftBits += 8)
      {
        // stage StageIndex
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits], pSoftBits[nSoftBits + 1]);

        v_ub* pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        //m_viterbi.v_print(vStates, 4);

        TBQwit += 1;
        TBQwit &= RQ_MASK;
        
        // stage StageIndex + 1
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2]);
        pvBM = &m_viterbi.m_vBM0[BMIndex][0];

        //BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 2], 4);
        //pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;

        //m_viterbi.v_print(vStates, 4);

        // stage StageIndex + 2
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 3]);
        pvBM = &m_viterbi.m_vBM1[BMIndex][0];

        //BMIndex = m_viterbi.BMAddress(4, pSoftBits[nSoftBits + 3]);
        //pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;

        //m_viterbi.v_print(vStates, 4);

        // stage StageIndex + 3
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 4], pSoftBits[nSoftBits + 5]);

        pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;

        //m_viterbi.v_print(vStates, 4);

        // stage StageIndex + 4
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 6]);
        pvBM = &m_viterbi.m_vBM0[BMIndex][0];

        //BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 6], 4);
        //pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][2] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][1] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit += 1;
        TBQwit &= RQ_MASK;

        //m_viterbi.v_print(vStates, 4);

        // stage StageIndex + 5
        BMIndex = m_viterbi.BMAddress(pSoftBits[nSoftBits + 7]);
        pvBM = &m_viterbi.m_vBM1[BMIndex][0];

        //BMIndex = m_viterbi.BMAddress(4, pSoftBits[nSoftBits + 7]);
        //pvBM = &m_viterbi.m_vBM[BMIndex][0];

        vBM0 = pvBM[0];
        vBM1 = pvBM[1];
        m_viterbi.ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][0] = iPH0;
        m_viterbi.m_TBQ[TBQwit][1] = iPH1;

        vBM0 = pvBM[2];
        vBM1 = pvBM[3];
        m_viterbi.ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
        m_viterbi.m_TBQ[TBQwit][2] = iPH0;
        m_viterbi.m_TBQ[TBQwit][3] = iPH1;

        TBQwit   += 1;
        TBQwit   &= RQ_MASK;
        iTrellis += 6;

        //m_viterbi.v_print(vStates, 4);

        if (vStates[0][0] > 200)
        //if (nSoftBits % 16 == 0)
        {
          //printf("\n-----\n");
          //m_viterbi.v_print(vStates, 4);
          //printf("--===---\n");
          //m_viterbi.Normalize(vStates, vNormMask);
          m_viterbi.Normalize(vStates);
          //m_viterbi.v_print(vStates, 4);
          //printf("--===---\n");
        }

        if (iTrellis >= nTracebackDataCount)
        {
          unsigned __int8 MinAddress = m_viterbi.FindMinValueAddress(vStates);
          //unsigned __int8 MinAddress = 0;

          TraceBackPt = TBQrit + nTracebackOffset;
          TraceBackPt &= RQ_MASK;

          m_viterbi.TraceBack(TraceBackPt, MinAddress, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

          pDecodedBytes     += nTraceBackOutputByte;
          TotalDecodedBytes -= nTraceBackOutputByte;

          TBQrit            += nTraceBackOutput;
          TBQrit            &= RQ_MASK;
          iTrellis          -= nTraceBackOutput;
        }
      }

      if ( iTrellis >= nTraceBackLength )
      {
        unsigned int iTrellisPadding = (nTracebackDataCount - iTrellis);

        for (unsigned int i = 0; i < iTrellisPadding; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit            += nTraceBackOutput;
        TBQrit            &= RQ_MASK;
      }

      while (TotalDecodedBytes > 0)
      {
        // pad traceback length zeros
        for (unsigned int i = 0; i < nTraceBackOutput; i++)
        {
          m_viterbi.m_TBQ[TBQwit][0] = m_viterbi.m_TBQ[TBQwit][1] = m_viterbi.m_TBQ[TBQwit][2] = m_viterbi.m_TBQ[TBQwit][3] = 0;                        
          TBQwit += 1;
          TBQwit &= RQ_MASK;
        }

        TraceBackPt = TBQrit + nTracebackOffset;
        TraceBackPt &= RQ_MASK;

        m_viterbi.TraceBack(TraceBackPt, 0, nTraceBackLength, nTraceBackOutput, pDecodedBytes + nTraceBackOutputByte - 1);

        pDecodedBytes     += nTraceBackOutputByte;
        TotalDecodedBytes -= nTraceBackOutputByte;

        TBQrit            += nTraceBackOutput;
        TBQrit            &= RQ_MASK;
      }

      tend = tick_count::now();
      tduration = tend - tbegin;
      //printf("ACSThread: %f us, %f Mbps\n", tduration.us(), m_viterbi.m_nDecodedBytes * 8 / tduration.us());
    }



    dsp_task::task* execute()
    {
#if cc_now == cc_12
      execute_1_2();
#elif cc_now == cc_23
      execute_2_3();
#elif cc_now == cc_34
      execute_3_4();
#endif

      return this;
    }
  };


  viterbi_traceback_worker m_TBWorker;
  viterbi_acs_worker       m_ACSWorker;
  viterbi_worker           m_Worker;

  dsp_task::cpu_processor  m_TBCPU;
  dsp_task::cpu_processor  m_ACSCPU;


  void Run1()
  {
    m_Worker.execute();
  }


  void Run()
  {
    m_TBQ.Clear();

    m_TBCPU.Enqueue(&m_TBWorker);
    //m_ACSCPU.Enqueue(&m_ACSWorker);

    m_TBCPU.Create();
    //m_ACSCPU.Create();

    //m_ACSWorker.RunA();
    //m_TBWorker.RunA();

    m_ACSWorker.execute();
    //m_ACSWorker.Wait();
    m_TBWorker.Wait();

    m_TBCPU.Destroy();
    m_ACSCPU.Destroy();
  }


  //  void UnitTest2()
  //  {
  //    tick_count tbegin, tend, tduration;
  //
  //    tbegin = tick_count::now();
  //
  //    __asm
  //    {
  //      mov ecx, 100000000;
  //donext:
  //      psubb       xmm0,xmm2;
  //      pblendvb    xmm1,xmm2,xmm0;
  //      dec ecx;
  //      jnz donext;
  //    }
  //
  //    tend = tick_count::now();
  //    tduration = tend - tbegin;
  //    printf("%f us\n", tduration.us());
  //
  //
  //    tbegin = tick_count::now();
  //    __asm
  //    {
  //      mov ecx, 100000000;
  //donext2:
  //      psubb       xmm0,xmm2;
  //      pminub      xmm1,xmm2;
  //      dec ecx;
  //      jnz donext2;
  //    }
  //    tend = tick_count::now();
  //    tduration = tend - tbegin;
  //    printf("%f us\n", tduration.us());
  //
  //    tbegin = tick_count::now();
  //    __asm
  //    {
  //      mov ecx, 100000000;
  //donext3:
  //      pcmpgtb     xmm0,xmm2;
  //      pand        xmm1,xmm0;
  //      pandn       xmm0,xmm2;
  //      dec ecx;
  //      jnz donext3;
  //    }
  //    tend = tick_count::now();
  //    tduration = tend - tbegin;
  //    printf("%f us\n", tduration.us());
  //  }

};
#pragma warning(default: 4355)





/*
1）如果你的软比特从【0，7】（3bits），你添0的话显然有问题。
如果你的软比特是从【－3，3】（3bits），添0没有问题。

2）注意你的SNR。不要用同样的SNR来比较1/2 , 2/3 , 3/4 , 5/6 的性能。
Guass channel   QPSK   BER = 2*10^-4
1/2      3.1dB
2/3      4.9dB
3/4      5.9dB
5/6      6.9dB
7/8      7.7dB
according to DVB-T EN300744 pages 40.
*/





class ViterbiTest 
{
public:
  //static const size_t m_nSource  = 300; // max 536870910
  static const size_t m_nSource  = 1 * 1024 * 1024; // max 536870910
#if cc_now == cc_12
  static const size_t m_nEncoded = m_nSource * 2;// 1/2 coding
#elif cc_now == cc_23
  static const int m_nEncoded = m_nSource * 3 / 2;// 2/3 coding
#elif cc_now == cc_34
  static const int m_nEncoded = m_nSource * 4 / 3;// 3/4 coding
#endif  

#if mod_now == mod_bpsk
  static const size_t m_nMapped   = m_nEncoded * 8;//bpsk
  static const size_t m_nDemapped = m_nMapped;//bpsk
#elif mod_now == mod_qpsk
  static const int m_nMapped  = m_nEncoded * 4;//qpsk
  static const int m_nDemapped  = m_nMapped * 2;//qpsk
#elif mod_now == mod_16qam
  static const int m_nMapped    = m_nEncoded * 2;//16qam
  static const int m_nDemapped  = m_nMapped * 4;//16qam
#elif mod_now == mod_64qam
  static const int m_nMapped    = m_nEncoded * 8 / 6;//64qam
  static const int m_nDemapped  = m_nMapped * 6;//64qam
#endif
  
  convolution::encoder_1_2 m_conv12;
  convolution::encoder_2_3 m_conv23;
  convolution::encoder_3_4 m_conv34;
  scrambler::dot11n_scrambler m_scrambler;
  mapper::dsp_mapper_bpsk<complex16>  m_mapbpsk;
  mapper::dsp_mapper_qpsk<complex16>  m_mapqpsk;
  mapper::dsp_mapper_16qam<complex16>  m_map16qam;
  mapper::dsp_mapper_64qam<complex16>  m_map64qam;

  demapper::dsp_demapper m_demapper;

  unsigned __int8 m_Source[m_nSource];
  unsigned __int8 m_ScrambledSource[m_nSource];
  unsigned __int8 m_EncodedSource[m_nEncoded];

  unsigned __int8 m_Decoded[m_nSource + 64];
  unsigned __int8 m_Descrambled[m_nSource];

  v_align(16) complexf  m_Mapped_f[m_nMapped];
  v_align(16) complex16 m_Mapped_i[m_nMapped];
  v_align(16) unsigned __int8 m_SoftBits[m_nMapped];
  v_align(16) unsigned __int8 m_Demapped[m_nDemapped];

  dsp_channel::AWGN m_AWGNChannel;

  dsp_plot m_plot;

  unsigned __int8 m_Demap_BPSK_LUT[256];


  ViterbiTest() : m_conv12(0x00), m_conv23(0x00), m_conv34(0x00), m_scrambler(0xCC), 
    m_mapbpsk(complex16(127.0f, 0.0f)),
    m_mapqpsk(complex16(127.0f / sqrt(2.0f), 127.0f / sqrt(2.0f))),
    m_map16qam(complex16(127.0f / sqrt(10.0f), 127.0f / sqrt(10.0f))),
    m_map64qam(complex16(127.0f / sqrt(42.0f), 127.0f / sqrt(42.0f)))
  {
  }

  void RandomSource()
  {
    srand(GetTickCount());
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_Source[i] = rand();
    }
    m_Source[m_nSource - 1] = 0;
  }

  void DemapBPSK(complex16* pcInput, unsigned __int8* pOutput, size_t nInput)
  {
    m_demapper.demap_limit_bpsk((v_cs*)pcInput, nInput / 4);

    for (size_t i = 0; i < nInput; i++)
    {
      m_demapper.demap_bpsk_i(pcInput[i], &pOutput[i]);
      //pOutput[i] = m_Demap_BPSK_LUT[pcInput[i].re];
    }
  }

  void DemapQPSK(complex16* pcInput, unsigned __int8* pOutput, size_t nInput)
  {
    m_demapper.demap_limit_qpsk((v_cs*)pcInput, nInput / 4);

    size_t j = 0;
    for (size_t i = 0; i < nInput; i++)
    {
      m_demapper.demap_qpsk(pcInput[i], &pOutput[j]);
      j += 2;
    }
  }

  void Demap16QAM(complex16* pcInput, unsigned __int8* pOutput, size_t nInput)
  {
    m_demapper.demap_limit_16qam((v_cs*)pcInput, nInput / 4);

    size_t j = 0;
    for (size_t i = 0; i < nInput; i++)
    {
      m_demapper.demap_16qam(pcInput[i], &pOutput[j]);
      j += 4;
    }
  }

  void Demap64QAM(complex16* pcInput, unsigned __int8* pOutput, size_t nInput)
  {
    m_demapper.demap_limit_64qam((v_cs*)pcInput, nInput / 4);

    size_t j = 0;
    for (size_t i = 0; i < nInput; i++)
    {
      m_demapper.demap_64qam(pcInput[i], &pOutput[j]);
      j += 6;
    }
  }


  void Run_Test0()
  {
    m_Source[0] = 0x0a;
    m_Source[1] = 0xdc;
    m_Source[2] = 0x05;
    m_Source[3] = 0x03;
    m_Source[4] = 0x10;
    m_Source[5] = 0x00;
    unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
    // conv encode
    m_conv12.reset(0);
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_conv12(m_Source[i], pEncoded[i]);
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
      Mapped = m_mapbpsk[m_EncodedSource[i]];
    }

    DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // bpsk 1/2
  void Run_Test1(float EbN0)
  {
    RandomSource();

    unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
    // conv encode
    m_conv12.reset(0);
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_conv12(m_Source[i], pEncoded[i]);
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
      Mapped = m_mapbpsk[m_EncodedSource[i]];
    }

    float CodingRate = 1.0f / 2.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
    //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

    //for (int i = 0; i < m_nMapped; i++)
    //{
    //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
    //}
    //demap
    DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // bpsk 2/3
  void Run_Test2(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv23.reset(0);
    for (size_t i = 0; i < m_nSource; i += 2)
    {
      m_conv23(&m_Source[i], pEncoded);
      pEncoded += 3;
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
      Mapped = m_mapbpsk[m_EncodedSource[i]];
    }

    float CodingRate = 2.0f / 3.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
    //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

    //for (int i = 0; i < m_nMapped; i++)
    //{
    //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
    //}
    //demap
    DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // bpsk 3/4
  void Run_Test3(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv34.reset(0);
    for (size_t i = 0; i < m_nSource; i += 3)
    {
      m_conv34(&m_Source[i], pEncoded);
      pEncoded += 4;
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_i[i * 8]);
      Mapped = m_mapbpsk[m_EncodedSource[i]];
    }

    float CodingRate = 3.0f / 4.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
    //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

    //for (int i = 0; i < m_nMapped; i++)
    //{
    //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
    //}
    //demap
    DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  void Run(float EbN0)
  {
    // source
    //memset(m_Source, 0xF5, m_nSource);
    //m_Source[m_nSource - 1] = 0;
    RandomSource();
    // scramble
#if 0
    for (int i = 0; i < m_nSource; i++)
    {
      m_scrambler(m_Source[i], m_ScrambledSource[i]);
    }
#endif

#define _CC12

#ifdef _CC12
    unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
    // conv encode
    m_conv12.reset(0);
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_conv12(m_Source[i], pEncoded[i]);
      //m_conv12(m_ScrambledSource[i], pEncoded[i]);
    }
#endif
#ifdef _CC23
    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv23.reset(0);
    for (size_t i = 0; i < m_nSource; i += 2)
    {
      m_conv23(&m_Source[i], pEncoded);
      pEncoded += 3;
    }
#endif
#ifdef _CC34
    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv34.reset(0);
    for (size_t i = 0; i < m_nSource; i += 3)
    {
      m_conv34(&m_Source[i], pEncoded);
      pEncoded += 4;
    }
#endif


    //for (size_t i = 0; i < m_nEncoded; i++)
    //{
    //    if (i % 16 == 0)
    //    {
    //        printf("%8d: ", i);
    //    }
    //    printf("%02X ", m_EncodedSource[i]);

    //    if (((i + 1) % 16) == 0)
    //    {
    //        printf("\n");
    //    }
    //}
    //printf("\n");

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_bpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_bpsk<complex16>::output_type&>(m_Mapped_f[i * 8]);
      Mapped = m_mapbpsk[m_EncodedSource[i]];
    }



#if 0
    m_plot.command("set title 'ideal mapped symbols'");
    m_plot.begin_plot("w p 7");
    for (size_t i = 0; i < m_nMapped; i++)
    {
      m_plot.data("%d\t%d", m_Mapped[i].re, m_Mapped[i].im);
    }
    m_plot.end_plot();
#endif
    // noise
    //% Define number of bits per symbol (k).
    //M = 4;  % or 2
    //k = log2(M);
    //codeRate = 1/2;
    //Adjust SNR for coded bits and multi-bit symbols.
    //adjSNR = EbNo - 10*log10(1/codeRate) + 10*log10(k);

    float CodingRate = 3.0f / 4.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
    //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

    //printf("EbN0=%f, EsN0=%f\n", EbN0, EsN0);

    //soft_demap(m_Mapped_f, m_Demapped, m_nMapped);

    demapper::dsp_demapper::build_soft_bits_bpsk(EsN0, m_Demap_BPSK_LUT);

    //demapper::dsp_demapper::demodulate_soft_bits_bpsk(m_Mapped, m_nMapped, EbN0, m_SoftBits_i);
#if 0
    m_plot.command("set title 'AWGN mapped symbols'");
    m_plot.begin_plot("w p 7");
    for (size_t i = 0; i < m_nMapped; i++)
    {
      //m_plot.data("%f\t%f", m_Mapped[i].re, m_Mapped[i].im);
      m_plot.data("%d", m_SoftBits_i[i]);
    }
    m_plot.end_plot();
    getchar();
#endif

    //for (size_t i = 0; i < m_nMapped; i++)
    //{
    //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127.0f;
    //}
    //demap
    DemapBPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // qpsk 1/2
  void Run_Test4(float EbN0)
  {
    RandomSource();

    unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
    // conv encode
    m_conv12.reset(0);
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_conv12(m_Source[i], pEncoded[i]);
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_qpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_qpsk<complex16>::output_type&>(m_Mapped_i[i * 4]);
      Mapped = m_mapqpsk[m_EncodedSource[i]];
    }

    float CodingRate = 1.0f / 2.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate) + 10.0f * log10(2.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);
    //m_AWGNChannel.pass_channel(m_Mapped_f, m_nMapped, EsN0);

    //for (int i = 0; i < m_nMapped; i++)
    //{
    //    m_Mapped_i[i].re = m_Mapped_f[i].re * 127;
    //    m_Mapped_i[i].im = m_Mapped_f[i].im * 127;
    //}
    //demap
    DemapQPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // qpsk 2/3
  void Run_Test5(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv23.reset(0);
    for (size_t i = 0; i < m_nSource; i += 2)
    {
      m_conv23(&m_Source[i], pEncoded);
      pEncoded += 3;
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_qpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_qpsk<complex16>::output_type&>(m_Mapped_i[i * 4]);
      Mapped = m_mapqpsk[m_EncodedSource[i]];
    }

    float CodingRate = 2.0f / 3.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate) + 10.0f * log10(2.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    DemapQPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // qpsk 3/4
  void Run_Test6(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv34.reset(0);
    for (size_t i = 0; i < m_nSource; i += 3)
    {
      m_conv34(&m_Source[i], pEncoded);
      pEncoded += 4;
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_qpsk<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_qpsk<complex16>::output_type&>(m_Mapped_i[i * 4]);
      Mapped = m_mapqpsk[m_EncodedSource[i]];
    }

    float CodingRate = 3.0f / 4.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate) + 10.0f * log10(2.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    DemapQPSK(m_Mapped_i, m_Demapped, m_nMapped);
  }

  //////////////////////////////////////////////////////////////////////////
  // 16QAM 1/2
  void Run_Test7(float EbN0)
  {
    RandomSource();

    unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
    // conv encode
    m_conv12.reset(0);
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_conv12(m_Source[i], pEncoded[i]);
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_16qam<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_Mapped_i[i * 2]);
      Mapped = m_map16qam[m_EncodedSource[i]];
    }

    float CodingRate = 1.0f / 2.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(4.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    Demap16QAM(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // 16QAM 2/3
  void Run_Test8(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv23.reset(0);
    for (size_t i = 0; i < m_nSource; i += 2)
    {
      m_conv23(&m_Source[i], pEncoded);
      pEncoded += 3;
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_16qam<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_Mapped_i[i * 2]);
      Mapped = m_map16qam[m_EncodedSource[i]];
    }

    float CodingRate = 2.0f / 3.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(4.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    Demap16QAM(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // 16QAM 3/4
  void Run_Test9(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv34.reset(0);        
    for (size_t i = 0; i < m_nSource; i += 3)
    {
      m_conv34(&m_Source[i], pEncoded);
      pEncoded += 4;
    }

    // map
    for (size_t i = 0; i < m_nEncoded; i++)
    {
      mapper::dsp_mapper_16qam<complex16>::output_type &Mapped = 
        reinterpret_cast<mapper::dsp_mapper_16qam<complex16>::output_type&>(m_Mapped_i[i * 2]);
      Mapped = m_map16qam[m_EncodedSource[i]];
    }

    float CodingRate = 3.0f / 4.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(4.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    Demap16QAM(m_Mapped_i, m_Demapped, m_nMapped);
  }

  //////////////////////////////////////////////////////////////////////////

  // 64QAM 1/2
  void Run_Test10(float EbN0)
  {
    RandomSource();

    unsigned __int16* pEncoded = (unsigned __int16*)m_EncodedSource;
    // conv encode
    m_conv12.reset(0);
    for (size_t i = 0; i < m_nSource; i++)
    {
      m_conv12(m_Source[i], pEncoded[i]);
    }

    // map
    size_t j = 0;
    for (size_t i = 0; i < m_nEncoded; i += 3)
    {
      mapper::dsp_mapper_64qam<complex16>::output_type *Mapped = 
        reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

      unsigned __int32 * p = reinterpret_cast<unsigned __int32*>(&m_EncodedSource[i]);

      *Mapped = m_map64qam[*p & 0x0FFF];

      j += 2;

      Mapped = reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

      *Mapped = m_map64qam[(*p & 0x00FFF000) >> 12];

      j += 2;
    }

    float CodingRate = 1.0f / 2.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(6.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    Demap64QAM(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // 64QAM 2/3
  void Run_Test11(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv23.reset(0);
    for (size_t i = 0; i < m_nSource; i += 2)
    {
      m_conv23(&m_Source[i], pEncoded);
      pEncoded += 3;
    }

    // map
    size_t j = 0;
    for (size_t i = 0; i < m_nEncoded; i += 3)
    {
      mapper::dsp_mapper_64qam<complex16>::output_type *Mapped = 
        reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

      unsigned __int32 * p = reinterpret_cast<unsigned __int32*>(&m_EncodedSource[i]);

      *Mapped = m_map64qam[*p & 0x0FFF];

      j += 2;

      Mapped = reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

      *Mapped = m_map64qam[(*p & 0x00FFF000) >> 12];

      j += 2;
    }

    float CodingRate = 2.0f / 3.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(6.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    Demap64QAM(m_Mapped_i, m_Demapped, m_nMapped);
  }

  // 64QAM 3/4
  void Run_Test12(float EbN0)
  {
    RandomSource();

    unsigned __int8* pEncoded = m_EncodedSource;
    // conv encode
    m_conv34.reset(0);        
    for (size_t i = 0; i < m_nSource; i += 3)
    {
      m_conv34(&m_Source[i], pEncoded);
      pEncoded += 4;
    }

    // map
    size_t j = 0;
    for (size_t i = 0; i < m_nEncoded; i += 3)
    {
      mapper::dsp_mapper_64qam<complex16>::output_type *Mapped = 
        reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

      unsigned __int32 * p = reinterpret_cast<unsigned __int32*>(&m_EncodedSource[i]);

      *Mapped = m_map64qam[*p & 0x0FFF];

      j += 2;

      Mapped = reinterpret_cast<mapper::dsp_mapper_64qam<complex16>::output_type*>(&m_Mapped_i[j]);

      *Mapped = m_map64qam[(*p & 0x00FFF000) >> 12];

      j += 2;
    }

    float CodingRate = 3.0f / 4.0f;
    float EsN0 = EbN0 + 10.0f * log10(CodingRate)  + 10.0f * log10(6.0f);
    m_AWGNChannel.pass_channel(m_Mapped_i, m_nMapped, 127.0f, EsN0);

    //demap
    Demap64QAM(m_Mapped_i, m_Demapped, m_nMapped);
  }
};

// Ref from www.dsplog.com
//    |BER for BPSK in AWGN with hard/soft Viterbi decoder
//    |----------------------------------------------
//    |Eb/N0 | BER (sim) | BER (sim) | BER (theory) |
//    | (dB) | (hard)    | (soft)    | (uncoded)    |
//    |----------------------------------------------
//    |  0   | 0.198044  | 0.096864  | 0.078650     |
//    |  1   | 0.128787  | 0.045707  | 0.056282     |
//    |  2   | 0.072306  | 0.017853  | 0.037506     |
//    |  3   | 0.032038  | 0.005107  | 0.022878     |
//    |  4   | 0.011104  | 0.001269  | 0.012501     |
//    |  5   | 0.002886  | 0.000277  | 0.005954     |
//    |  6   | 0.000696  | 0.000054  | 0.002388     |
//    |  7   | 0.000100  | 0.000005  | 0.000773     |
//    |  8   | 0.000014  | 0.000000  | 0.000191     |
//    |  9   | 0.000000  | 0.000000  | 0.000034     |


/*
Data rate(Mbits/s) Modulation Coding rate(R) Coded bits per subcarrier(NBPSC) Coded bits per OFDM symbol(NCBPS) Data bits per OFDM symbol(NDBPS)
6 BPSK 1/2 1 48 24
9 BPSK 3/4 1 48 36
12 QPSK 1/2 2 96 48
18 QPSK 3/4 2 96 72
24 16-QAM 1/2 4 192 96
36 16-QAM 3/4 4 192 144
48 64-QAM 2/3 6 288 192
54 64-QAM 3/4 6 288 216
*/
#define ObjectNew new(_aligned_malloc(sizeof(T), __alignof(T)))
template<typename T>
T * CreateObject()
{
  return (T*)(ObjectNew T());
}


int viterbi_check()
{
  ViterbiTest *vt = CreateObject<ViterbiTest>();

  dsp_viterbi_64<36, 48> *dv = CreateObject<dsp_viterbi_64<36, 48>>();

  dv->m_pSoftBits     = vt->m_Demapped;
  dv->m_pDecodedBytes = vt->m_Decoded;
  dv->m_nDecodedBytes = vt->m_nSource;

  vt->Run_Test0();
  dv->Run1();

  return 1;
}


int viterbi_test(float EbN0)
{
  ViterbiTest *vt = CreateObject<ViterbiTest>();

#if cc_now == cc_12
  dsp_viterbi_64<60, 128> *dv = CreateObject<dsp_viterbi_64<60, 128>>();
#elif cc_now == cc_23
  dsp_viterbi_64<60, 128> *dv = CreateObject<dsp_viterbi_64<60, 128>>();
#elif cc_now == cc_34
  dsp_viterbi_64<60, 144> *dv = CreateObject<dsp_viterbi_64<60, 144>>();
#endif

  dv->m_pSoftBits     = vt->m_Demapped;
  dv->m_pDecodedBytes = vt->m_Decoded;
  dv->m_nDecodedBytes = vt->m_nSource;

  double SumBer = 0.0;
  double MinBer = 100.0f;

  //dv.UnitTest2();
  //return 1;

#if 0
  //vt.Run(EbN0);
#if cc_now == cc_12 && mod_now == mod_bpsk
  vt->Run_Test1(EbN0);
#elif cc_now == cc_23 && mod_now == mod_bpsk
  vt->Run_Test2(EbN0);
#elif cc_now == cc_34 && mod_now == mod_bpsk
  vt->Run_Test3(EbN0);
#elif cc_now == cc_12 && mod_now == mod_qpsk
  vt->Run_Test4(EbN0);
#elif cc_now == cc_12 && mod_now == mod_16qam
  vt->Run_Test5(EbN0);
#elif cc_now == cc_34 && mod_now == mod_16qam
  vt->Run_Test6(EbN0);
#elif cc_now == cc_23 && mod_now == mod_64qam
  vt->Run_Test7(EbN0);
#endif
  dv->Run1();

  //vt.m_scrambler.reset(vt.m_Decoded[1]);
  //for (int i = 2; i < vt.m_nSource; i++)
  //{
  //    vt.m_Descrambled[i] = vt.m_scrambler(vt.m_Decoded[i]);
  //}


  double fber = dsp_ber::ber(vt->m_Source, vt->m_Decoded, vt->m_nSource - 16);

  SumBer += fber;
  printf("BER: %f : %f\n", EbN0, SumBer);
#else
  printf("EbN0(dB)  :  AvgBER  :  MinBER\n");
#if mod_now == mod_64qam
  for (size_t i = 0; i < 80; i++)
#else
  for (size_t i = 0; i < 40; i++)
#endif
  {
    SumBer = 0.0;
    MinBer = 100.0f;
#define N 2
    for (size_t i = 0; i < N; i++)
    {
#if cc_now == cc_12 && mod_now == mod_bpsk
      vt->Run_Test1(EbN0);
#elif cc_now == cc_23 && mod_now == mod_bpsk
      vt->Run_Test2(EbN0);
#elif cc_now == cc_34 && mod_now == mod_bpsk
      vt->Run_Test3(EbN0);
#elif cc_now == cc_12 && mod_now == mod_qpsk
      vt->Run_Test4(EbN0);
#elif cc_now == cc_23 && mod_now == mod_qpsk
      vt->Run_Test5(EbN0);
#elif cc_now == cc_34 && mod_now == mod_qpsk
      vt->Run_Test6(EbN0);
#elif cc_now == cc_12 && mod_now == mod_16qam
      vt->Run_Test7(EbN0);
#elif cc_now == cc_23 && mod_now == mod_16qam
      vt->Run_Test8(EbN0);
#elif cc_now == cc_34 && mod_now == mod_16qam
      vt->Run_Test9(EbN0);
#elif cc_now == cc_12 && mod_now == mod_64qam
      vt->Run_Test10(EbN0);
#elif cc_now == cc_23 && mod_now == mod_64qam
      vt->Run_Test11(EbN0);
#elif cc_now == cc_34 && mod_now == mod_64qam
      vt->Run_Test12(EbN0);
#endif

      //vt.Run_Test2(EbN0);
      //vt.Run(EbN0);
      dv->Run1();
      //dv.UnitTest();

      double fber = dsp_ber::ber(vt->m_Source, vt->m_Decoded, vt->m_nSource - 16);

      if (fber < MinBer)
      {
        MinBer = fber;
      }

      SumBer += fber;
    }
    SumBer /= N;
    printf("BER: %.2f : %.10f : %.10f\n", EbN0, SumBer, MinBer);
    EbN0 += .5f;
  }
#endif


#if 0
  for (size_t i = 0; i < vt.m_nSource - 1; i++)
  {
    if (i % 16 == 0)
    {
      printf("%8d: ", i);
    }
    printf("%02X ", vt.m_Decoded[i] ^ vt.m_Source[i]);

    if (((i + 1) % 16) == 0)
    {
      printf("\n");
    }
  }
  printf("\n");
#endif

  return 1;
}
