
    struct dsp_viterbi_64
    {
#define COMPACT_HISTORY 1
        static const int HistoryLength = 2000 * 8;
        v_ub m_vShuffleMask;
        v_ub m_vZero;
        v_ub m_vNormMask;
        v_ub m_vPadding[1];

        v_ub m_vStates[4];
        v_ub m_vStateIndex[4];
        v_ub m_vBM[64][4]; // 64 states, each state has two 32 BM

        v_ub m_v14;

        unsigned __int16 m_iPathHistory[HistoryLength][4];



        //total 8192 states can be held in RQ
        typedef RingQ<unsigned __int16, unsigned __int16, 65536> VitRQ;
        VitRQ m_TBQ;


        v_align(64) unsigned __int16  m_MinAddress[256];
        v_align(64) unsigned __int8  m_DecodedBytes[HistoryLength >> 3];

        v_align(64) volatile int m_trcaeback_thread_status;

        dsp_viterbi_64()
        {

            unsigned char v = 0;
            int i;
            for (i = 0; i < 64; i++)
            {
                m_vStates[0][i] = v++;
            }
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

            for (i = 0; i < 16; i++)
            {
                m_vNormMask[i] = __int8(0xFF);
            }

            unsigned __int8 bAddress;
            unsigned __int8 m00, m01, m10, m11;
            
            m_v14.v_setall(14);
            for (unsigned __int8 b0 = 0; b0 < 8; b0++)
            {
                for (unsigned __int8 b1 = 0; b1 < 8; b1++)
                {
                    bAddress = ( (b0 & 0x7) | ((b1 & 0x7) << 3) );

                    //printf("b0= %d, b1 = %d, Address = %d \n", b0, b1, bAddress);

                    m00 = b0 + b1;
                    m01 = b0 + 7 - b1;
                    m10 = 7 - b0 + b1;
                    m11 = 14 - b0 - b1;

                    //printf("m00 = %d, m01 = %d, m10 = %d, m11 = %d\n", m00, m01, m10, m11);

                    m_vBM[bAddress][0].v_set(m00, m01, m00, m01, m11, m10, m11, m10, m11, m10, m11, m10, m00, m01, m00, m01);
                    m_vBM[bAddress][1] = v_sub(m_v14, m_vBM[bAddress][0]);
                    m_vBM[bAddress][2].v_set(m10, m11, m10, m11, m01, m00, m01, m00, m01, m00, m01, m00, m10, m11, m10, m11);
                    m_vBM[bAddress][3] = v_sub(m_v14, m_vBM[bAddress][2]);
                }
            }


            m_vZero.v_zero();
        }


        inline void v_print(v_ub &v)
        {
            for (int i = 0; i < v_ub::elem_cnt; i++)
            {
                printf("%4d ", v[i]);
            }
            printf("\n");
        }


        __forceinline v_ub ACS_Branch(v_ub& va, v_ub& vb, v_ub& vBM1, v_ub& vBM2, unsigned __int16& iPH)
        {
            v_ub vc    = v_add(va, vBM1);// from 0
            v_ub vd    = v_add(vb, vBM2);// from 1
            v_b vdiff  = v_sub(vd, vc); // difference

            iPH       = (unsigned __int16)vdiff.v_signmask();

            v_ub vmin = v_min(vc, vd);
            //v_ub vmin = v_select(vc, vd, (v_ub&)vdiff); // performance degrade

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

            va = ACS_Branch(veven, vodd, vBM1, vBM2, iPH1);
            vb = ACS_Branch(veven, vodd, vBM2, vBM1, iPH2);
        }

        __forceinline __int8 BMAddress(unsigned __int8 b0, unsigned __int8 b1)
        {
            return ( (b0 & 7) | ((b1 & 7) << 3) );
        }

        __forceinline __int8 PrevPHAddress(unsigned __int16* pvPath, unsigned __int8& Index)
        {
            unsigned __int8 AddressHi = Index >> 4;
            unsigned __int8 AddressLo = Index & 0xF;

            unsigned __int16 v = pvPath[AddressHi];
            return ( ((v >> AddressLo) & 1) | ((Index << 1) & 0x3F) );
        }
        __forceinline unsigned __int8 PrevPHAddress(unsigned __int16* pvPath, unsigned __int16& Index, unsigned __int16& Value)
        {
            unsigned __int16 AddressHi = Index >> 4;
            unsigned __int16 AddressLo = Index & 0xF;

            Value = pvPath[AddressHi];
            Value = ((Value >> AddressLo) & 1);
            return ( Value | ((Index << 1) & 0x3F) );
        }


        __forceinline unsigned __int16 FindMinValueAddress(v_ub* pvTrellis)
        {
            v_ub vb0, vb1;
            v_us vs0, vs1, vs2, vs3;

            unsigned __int16 MinPos;

            vb0 = m_vStateIndex[0];
            vb1 = pvTrellis[0];

            vs1 = (v_us)v_unpack_lo( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi( vb0, vb1 );

            vs0       = v_min(vs1, vs2);

            vb0 = m_vStateIndex[1];
            vb1 = pvTrellis[1];
            vs1 = (v_us)v_unpack_lo  ( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

            vs3 = v_min (vs1, vs2);
            vs0 = v_min (vs0, vs3);

            vb0 = m_vStateIndex[2];
            vb1 = pvTrellis[2];
            vs1 = (v_us)v_unpack_lo  ( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

            vs3       = v_min (vs1, vs2);
            vs0       = v_min (vs0, vs3);

            vb0 = m_vStateIndex[3];
            vb1 = pvTrellis[3];
            vs1 = (v_us)v_unpack_lo  ( vb0, vb1 );
            vs2 = (v_us)v_unpack_hi ( vb0, vb1 );

            vs3 = v_min (vs1, vs2);
            vs0 = v_min (vs0, vs3);

            vs0 = vs0.v_hminpos();

            MinPos = vs0.v_get_at<1>();// the minimum index

            return MinPos;
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


        __forceinline int TraceBack(unsigned __int16 HistoryIndex, v_ub* pvTrellis, int TraceBackLength, int TraceOutputLength, unsigned __int8* pDecodedBytes)
        {
            //unsigned __int8* pPathHistory = (unsigned __int8*)&m_iPathHistory[HistoryIndex][0];

            unsigned __int16* pPathHistory;

            unsigned __int16 PHAddress, PHValue;

            PHAddress = FindMinValueAddress(pvTrellis);

            unsigned __int8 Output = 0;
            unsigned __int8* p = pDecodedBytes;

            for (int i = 0; i < TraceBackLength; i++)
            {
                pPathHistory = (unsigned __int16*)&m_TBQ[HistoryIndex];

                PHAddress = PrevPHAddress(pPathHistory, PHAddress, PHValue);

                HistoryIndex -= 4;
            }

            for (int i = 0; i < TraceOutputLength >> 3; i++)
            {
                for ( int j = 0; j < 8; j++)
                {
                    Output <<= 1;
                    Output |= PHValue;

                    pPathHistory = (unsigned __int16*)&m_TBQ[HistoryIndex];

                    PHAddress = PrevPHAddress(pPathHistory, PHAddress, PHValue);

                    HistoryIndex -= 4;
                }
                *p = Output;
                p++;
            }
            return (TraceOutputLength >> 3);
        }

        __forceinline int TraceBack(unsigned __int16 HistoryIndex, unsigned __int16 MinIndex, int TraceBackLength, int TraceOutputLength, unsigned __int8* pDecodedBytes)
        {
            unsigned __int16* pPathHistory;

            unsigned __int16 PHAddress, PHValue;

            PHAddress = MinIndex;

            unsigned __int8 Output = 0;
            unsigned __int8* p = pDecodedBytes;

            for (int i = 0; i < TraceBackLength; i++)
            {
                pPathHistory = (unsigned __int16*)&m_TBQ[HistoryIndex];

                PHAddress = PrevPHAddress(pPathHistory, PHAddress, PHValue);

                HistoryIndex -= 4;
            }

            for (int i = 0; i < TraceOutputLength >> 3; i++)
            {
                for ( int j = 0; j < 8; j++)
                {
                    Output <<= 1;
                    Output |= PHValue;

                    pPathHistory = (unsigned __int16*)&m_TBQ[HistoryIndex];

                    PHAddress = PrevPHAddress(pPathHistory, PHAddress, PHValue);

                    HistoryIndex -= 4;
                }
                *p = Output;
                p++;
            }
            return (TraceOutputLength >> 3);
        }

        void start_traceback();
        void stop_traceback();

        void traceback_worker()
        {
            unsigned __int16 qrit = 0;
            unsigned __int8  MinAddressIndex = 0;

            unsigned __int8* pDecodedBytes = m_DecodedBytes;
            unsigned __int16 tracbackpt = 36;

            int total_rounds = (10000 - 36 + 48) / 48;

            tick_count tbegin, tend, tend2, tduration, tduration2;
            tbegin = tick_count::now();

            while (total_rounds)
            {
                

                while( m_TBQ.DataCount() < 84 )
                {
                    //printf("RTB: w=%u, r=%u\n", m_TBQ.m_wcount, m_TBQ.m_rcount);
                }

                tracbackpt = qrit + 84;
                
                int nDecodedBytes = TraceBack(tracbackpt, m_MinAddress[MinAddressIndex++], 36, 48, pDecodedBytes);

                pDecodedBytes += nDecodedBytes;

                qrit           += 48;
                m_TBQ.m_rcount += 48;


                total_rounds--;
            }
            
            m_trcaeback_thread_status = 0;

            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("TBThread: %f us, %f Mbps\n", tduration.us(), 10000 / tduration.us());
            
        }


        void TraceBackTest()
        {
            unsigned __int16 qrit = 0;
            unsigned __int16 tracbackpt = 36;
            int total_rounds = (10000 - 36 + 48) / 48;

            
            unsigned __int8* pDecodedBytes = m_DecodedBytes;
            unsigned __int8  MinAddressIndex = 0;

            tick_count tbegin, tend, tend2, tduration, tduration2;
            tbegin = tick_count::now();

            while (total_rounds)
            {
                tracbackpt = qrit + 84;

                int nDecodedBytes = TraceBack(tracbackpt, m_MinAddress[MinAddressIndex++], 36, 48, pDecodedBytes);

                pDecodedBytes += nDecodedBytes;

                qrit           += 48;

                total_rounds--;
            }
            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("TBTest: %f us, %f Mbps\n", tduration.us(), 10000 / tduration.us());
        }



        void UnitTest()
        {
            v_ub vStates[4];
            v_ub vBM0, vBM1;
            v_ub vShuffleMask;
            v_ub vZero;
            v_ub vPH0, vPH1;
            int BMIndex = 4;
            unsigned __int16 iPH0, iPH1;
            unsigned __int8  MinAddressIndex = 0;

            m_TBQ.m_wcount = 36;

            vZero.v_zero();
            vShuffleMask = m_vShuffleMask;

            for (int i = 0; i < 4; i++)
            {
                vStates[i].v_zero();
            }

            int nTotalDecodedBytes = 0;
            unsigned __int16 qwit = 0;
            unsigned int iTrellis = 0;
            
            tick_count tbegin, tend, tend2, tduration, tduration2;

            BMIndex = 36;

            start_traceback();

            tbegin = tick_count::now();
#define TotalBits 10000
            for (int StageIndex = 0; StageIndex < TotalBits; StageIndex += 2)
            {
                // stage StageIndex
                BMIndex = StageIndex % 64;

                v_ub* pvBM = &m_vBM[BMIndex][0];

                vBM0 = pvBM[0];
                vBM1 = pvBM[1];

                ButterFly(vStates[0], vStates[1], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                m_TBQ[qwit]     = iPH0;
                m_TBQ[qwit + 2] = iPH1;
                //m_iPathHistory[StageIndex][0] = iPH0;
                //m_iPathHistory[StageIndex][2] = iPH1;

                vBM0 = pvBM[2];
                vBM1 = pvBM[3];
                ButterFly(vStates[2], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                m_TBQ[qwit + 1] = iPH0;
                m_TBQ[qwit + 3] = iPH1;
                //m_iPathHistory[StageIndex][1] = iPH0;
                //m_iPathHistory[StageIndex][3] = iPH1;

                qwit += 4;
                // stage StageIndex + 1
                BMIndex = (StageIndex + 1) % 64;
                pvBM = &m_vBM[BMIndex][0];

                vBM0 = pvBM[0];
                vBM1 = pvBM[1];
                ButterFly(vStates[0], vStates[2], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                m_TBQ[qwit + 0] = iPH0;
                m_TBQ[qwit + 1] = iPH1;
                //m_iPathHistory[StageIndex + 1][0] = iPH0;
                //m_iPathHistory[StageIndex + 1][1] = iPH1;

                vBM0 = pvBM[2];
                vBM1 = pvBM[3];
                ButterFly(vStates[1], vStates[3], vShuffleMask, vBM0, vBM1, iPH0, iPH1);
                m_TBQ[qwit + 2] = iPH0;
                m_TBQ[qwit + 3] = iPH1;
                //m_iPathHistory[StageIndex + 1][2] = iPH0;
                //m_iPathHistory[StageIndex + 1][3] = iPH1;

                qwit     += 4;
                iTrellis += 2;

                if (((iTrellis + 2) % 32) == 0)
                {
                    Normalize(vStates);
                }
                
                if (iTrellis == 84)
                {
                    unsigned __int16 PHAddress = FindMinValueAddress(vStates);
                    m_MinAddress[MinAddressIndex++] = PHAddress;

                    m_TBQ.m_wcount += 48;
                    iTrellis       -= 48;

                    //printf("WTB: w=%u, r=%u\n", m_TBQ.m_wcount, m_TBQ.m_rcount);

                    while ( !m_TBQ.WCheck(48 * 4) );
                }



                //if (iTrellis >= 84)
                //{
                //    int nDecodedBytes   = TraceBack(StageIndex, vStates, 36, 48, pDecodedBytes);
                //    pDecodedBytes      += nDecodedBytes;
                //    nTotalDecodedBytes += nDecodedBytes;
                //    iTrellis -= 48;
                //}
            }

            if ( iTrellis > 0 )
            {
                while ( !m_TBQ.WCheck((84 - iTrellis) * 4) );

                for (unsigned int i = iTrellis; i < 84; i++)
                {
                    m_TBQ[qwit] = m_TBQ[qwit + 1] = m_TBQ[qwit + 2] = m_TBQ[qwit + 3] = 0;
                    qwit += 4;
                    m_MinAddress[MinAddressIndex++] = 0;
                }
                
                m_TBQ.m_wcount += 48;
                //printf("TB: w flush %d\n", m_TBQ.m_wcount);
            }

            tend = tick_count::now();
            tduration = tend - tbegin;
            
            while(m_trcaeback_thread_status)
            {
                //printf("tb thread status: %d\n", m_trcaeback_thread_status);
            };

            //printf("tb thread status: %d\n", m_trcaeback_thread_status);

            tend2 = tick_count::now();
            tduration2 = tend2 - tbegin;
            printf("%f us, %f Mbps\n", tduration.us(), TotalBits / tduration.us());
            printf("%f us, %f Mbps\n", tduration2.us(), TotalBits / tduration2.us());

            //for (int i = 0; i < nTotalDecodedBytes; i++)
            //{
            //    printf("%02X ", m_DecodedBytes[i]);
            //}
        }




        void UnitTest2()
        {
            tick_count tbegin, tend, tduration;

            tbegin = tick_count::now();

            __asm
            {
                mov ecx, 100000;
donext:
                psubb       xmm0,xmm2;
                pblendvb    xmm1,xmm2,xmm0;
                dec ecx;
                jnz donext;
            }

            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us\n", tduration.us());


            tbegin = tick_count::now();
            __asm
            {
                mov ecx, 100000;
donext2:
                psubb       xmm0,xmm2;
                pmaxub      xmm1,xmm2;
                dec ecx;
                jnz donext2;
            }
            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us\n", tduration.us());

            tbegin = tick_count::now();
            __asm
            {
                mov ecx, 100000;
donext3:
                pcmpgtb     xmm0,xmm2;
                pand        xmm1,xmm0;
                pandn       xmm0,xmm2;
                dec ecx;
                jnz donext3;
            }
            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us\n", tduration.us());
        }
        void UnitTest3()
        {
            int BMIndex = 4;

            unsigned __int16* pHistoryPath = &m_iPathHistory[0][0];

            v_ub vShuffleMask;
            register v_ub va, vb, vc, vd;

            va.v_zero();
            vb.v_zero();
            vc.v_zero();
            vd.v_zero();

            vShuffleMask = m_vShuffleMask;

            tick_count tbegin, tend, tduration;

            tbegin = tick_count::now();

#define TotalBits 10000
            for (int StageIndex = 0; StageIndex < TotalBits; StageIndex += 2)
            {
                v_ub veven, vodd;

                BMIndex = StageIndex % 64;

                //! ===== stage i =====

                v_ub* pvBM = &m_vBM[BMIndex][0];

                v_ub vBM0 = pvBM[0];
                v_ub vBM1 = pvBM[1];

                //-- first half
                Shuffle(va, vb, vShuffleMask, veven, vodd);

                // butterfly 0
                v_ub ve               = v_add(veven, vBM0);// from 0
                v_ub vf               = v_add(vodd,  vBM1);// from 1
                v_ub vg               = v_add(veven, vBM1);// from 0
                v_ub vh               = v_add(vodd,  vBM0);// from 1

                va                    = v_min(ve, vf);
                vb                    = v_min(vg, vh);

                v_b  vdiff0           = v_sub(vf, ve); // difference
                v_b  vdiff1           = v_sub(vh, vg); // difference

                unsigned __int16 iPH0 = (unsigned __int16)vdiff0.v_signmask();
                unsigned __int16 iPH1 = (unsigned __int16)vdiff1.v_signmask();

                pHistoryPath[0]       = iPH0;
                pHistoryPath[2]       = iPH1;

                //-- second half

                vBM0 = pvBM[2];
                vBM1 = pvBM[3];

                Shuffle(vc, vd, vShuffleMask, veven, vodd);

                // butterfly 0
                ve              = v_add(veven, vBM0);// from 0
                vf              = v_add(vodd,  vBM1);// from 1

                vg              = v_add(veven, vBM1);// from 0
                vh              = v_add(vodd,  vBM0);// from 1

                vc              = v_min(vf, ve);
                vd              = v_min(vh, vg);

                vdiff0          = v_sub(ve, vf); // difference
                vdiff1          = v_sub(vg, vh); // difference

                iPH0            = (unsigned __int16)vdiff0.v_signmask();
                iPH1            = (unsigned __int16)vdiff1.v_signmask();

                pHistoryPath[1] = iPH0;
                pHistoryPath[3] = iPH1;

                pHistoryPath   += 4;

                //! ===== stage i + 1 =====
                BMIndex = (StageIndex + 1) % 64;

                pvBM = &m_vBM[BMIndex][0];

                vBM0 = pvBM[0];
                vBM1 = pvBM[1];

                //-- first half
                Shuffle(va, vc, vShuffleMask, veven, vodd);

                // butterfly 0
                ve                    = v_add(veven, vBM0);// from 0
                vf                    = v_add(vodd,  vBM1);// from 1
                vg                    = v_add(veven, vBM1);// from 0
                vh                    = v_add(vodd,  vBM0);// from 1

                va                    = v_min(ve, vf);
                vc                    = v_min(vg, vh);

                vdiff0                = v_sub(vf, ve); // difference
                vdiff1                = v_sub(vh, vg); // difference

                iPH0 = (unsigned __int16)vdiff0.v_signmask();
                iPH1 = (unsigned __int16)vdiff1.v_signmask();

                pHistoryPath[0]       = iPH0;
                pHistoryPath[1]       = iPH1;

                //-- second half

                vBM0 = pvBM[2];
                vBM1 = pvBM[3];

                Shuffle(vb, vd, vShuffleMask, veven, vodd);

                // butterfly 0
                ve              = v_add(veven, vBM0);// from 0
                vf              = v_add(vodd,  vBM1);// from 1
                vg              = v_add(veven, vBM1);// from 0
                vh              = v_add(vodd,  vBM0);// from 1

                vb              = v_min(ve, vf);
                vd              = v_min(vg, vh);

                vdiff0          = v_sub(vf, ve); // difference
                vdiff1          = v_sub(vh, vg); // difference

                iPH0            = (unsigned __int16)vdiff0.v_signmask();
                iPH1            = (unsigned __int16)vdiff1.v_signmask();

                pHistoryPath[2] = iPH0;
                pHistoryPath[3] = iPH1;

                pHistoryPath   += 4;
            }

            tend = tick_count::now();
            tduration = tend - tbegin;
            printf("%f us, %f Mbps\n", tduration.us(), TotalBits / tduration.us());

        }



        static DWORD WINAPI traceback_thread(LPVOID lpThreadParam);

    };
