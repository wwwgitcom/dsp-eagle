#include "dsp_task.h"
#include "stdio.h"
#include "dsp_sysconfig.h"
#include "dsp_log.h"
namespace OpenDSP
{
    extern dsp_log& log;

    namespace dsp_task
    {
        DWORD WINAPI cpu_processor::processor_thread(LPVOID lpdwThreadParam)
        {
            cpu_processor* me = (cpu_processor*)lpdwThreadParam;

            SetThreadAffinityMask(GetCurrentThread(), me->m_affinity);

            me->Run();

            return 1;
        }

        void cpu_processor::wake_up()
        {
            SetEvent(m_event);
        }

        void cpu_processor::Run()
        {
            while (m_active)
            {
                task* t = NULL;

                t = Dequeue();
                if (t)
                {
                    do 
                    {
                        //while (t->m_status != t_run && t->m_status != t_cancel);
                        if (t->m_status == t_cancel)
                        {
                            //t->m_status = t_done;
                        }
                        else
                        {
                            log("[cpu_processor]********************************************\n");
                            log("[cpu_processor]¡¾%d¡¿: Run a task %p, status = %p\n", m_affinity, t, t->GetStatus());
                            t->Run();
                            log("[cpu_processor]********************************************\n");
                        }
                        t = Dequeue();
                    } while (t != NULL);
                    // no task to do, mark it as free
                    _InterlockedXor((volatile long*)m_status_mask, m_affinity);
                    log("[cpu_processor] cpu_status %p\n", *m_status_mask);
                }
                else
                {
                    //_mm_pause();
                    //Sleep(50);
                    // TODO: imp. as event based or APC based
                    // NOTE:: user mode APC works at passive level, no fundermental improvement
                    //SleepEx(10, TRUE);
                    // TODO: keep a timer, busy looping for a while then goto sleep
                    m_status = idle;
                    log("[cpu_processor][%d]no task, sleep ....\n", m_affinity);
                    WaitForSingleObject(m_event, INFINITE);
                    m_status = running;
                    
                }
            }

            log("[cpu_processor] cpu[%d] deactive...\n", m_affinity);
        }

        //////////////////////////////////////////////////////////////////////////

        cpu_manager::cpu_manager() : m_nCurrentIndex(0), m_nTotalProcessor(0), m_status_mask(0)
        {
            setup();
        }
        cpu_manager::~cpu_manager()
        {
            destroy();
        }

        cpu_manager* cpu_manager::Instance()
        {
            static cpu_manager s_cpu_manager;

            return &s_cpu_manager;
        }

        void cpu_manager::setup()
        {
            dsp_sysconfig* config = dsp_sysconfig::Instance();
            m_nTotalProcessor     = config->GetCPUProcessorCount();
            m_cpu_array           = new cpu_processor *[m_nTotalProcessor];
            
            for (int i = 1; i < m_nTotalProcessor; i++)
            {
                cpu_processor* cpu = new cpu_processor((1L << i));
                m_status_mask |= (1L << i);
                cpu->set_status_mask(&m_status_mask);
                cpu->Create();
            	m_cpu_array[i] = cpu;
            }
        }

        void cpu_manager::destroy()
        {
            for (int i = 1; i < m_nTotalProcessor; i++)
            {
                m_cpu_array[i]->Destroy();
                delete m_cpu_array[i];
            }

            delete[] m_cpu_array;
        }
        
        void cpu_manager::run_task(task* t)
        {
            DWORD dwFreeCpu = 0;

            // spin wait
            while (!m_status_mask);

            m_lock.Acquire();

            log("[cpu_man] status %p.\n", m_status_mask);
            _BitScanForward(&dwFreeCpu, m_status_mask);
            {
                //debug
                //dwFreeCpu = 1;
                _InterlockedXor((volatile long*)&m_status_mask, (1L << dwFreeCpu));
                m_cpu_array[dwFreeCpu]->Enqueue(t);
                if (m_cpu_array[dwFreeCpu]->processor_status() == cpu_processor::idle)
                {
                    m_cpu_array[dwFreeCpu]->wake_up();
                }
                log("[cpu_man] enqueue task %p to processor %d.\n", t, (1L << dwFreeCpu));
            }
#if 0
            else
            {
                // all cpu are busy, use random cpu
                m_cpu_array[m_nCurrentIndex]->Enqueue(t);
                log("[cpu_man] random enqueue task %p to processor %d.\n", t, (1L << m_nCurrentIndex));
                m_nCurrentIndex++;
                m_nCurrentIndex %= m_nTotalProcessor;
            }
#endif
            m_lock.Release();
        }
    }
}
