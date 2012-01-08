#pragma once

#include "windows.h"
#include "dsp_helper.h"
#include <stdio.h>
namespace OpenDSP
{
    namespace dsp_task
    {
        static const size_t CacheLineSize = 64;

        template<class T, int S>
        struct padded_base : T {
            char pad[CacheLineSize - sizeof(T) % CacheLineSize];
        };
        template<class T> struct padded_base<T, 0> : T {};

        //! Pads type T to fill out to a multiple of cache line size.
        template<class T>
        struct padded : padded_base<T, sizeof(T)> {};

        class no_assign {
            // Deny assignment
            void operator=( const no_assign& );
        public:
        };

        //! Base class for types that should not be copied or assigned.
        class no_copy: no_assign {
            //! Deny copy construction
            no_copy( const no_copy& );
        public:
            //! Allow default construction
            no_copy() {}
        };

        enum task_status
        {
            t_wait,// not ready to be run
            t_queued,// in the queue, not executed yet
            t_run,// executing
            t_done, // done
            t_cancel// canceled by external signal
        };

        struct __declspec(align(64)) task_base : no_copy
        {
        public:
            void SetAffinity(DWORD affinity)
            {
                this->m_affinity = affinity;
            }

            void SetStatus(task_status status)
            {
                this->m_status = status;
            }

            task_status GetStatus()
            {
                return this->m_status;
            }

            void SetContext(PVOID ctx)
            {
                this->m_context = ctx;
            }

            void Cancel()
            {
                this->m_status = t_cancel;
            }


            volatile task_status  m_status;
            DWORD                 m_affinity;
            PVOID                 m_context;
            LIST_ENTRY            m_ListEntry;
            ULONG                 m_pad[11];
        };

        struct task : task_base
        {
            task()
            {
                this->m_affinity = NULL;
                this->m_context  = NULL;
                this->m_status   = t_done;
                //InitializeListHead(&this->m_ListEntry);
            }

            task(DWORD affinity, PVOID ctx)
            {
                this->m_affinity = affinity;
                this->m_context  = ctx;
                this->m_status   = t_done;
                //InitializeListHead(&this->m_ListEntry);
            }

            PLIST_ENTRY GetListEntry()
            {
                return &this->m_ListEntry;
            }

            template<typename T>
            T* GetContext()
            {
                return (T*)(this->m_context);
            }

            void SetContext(PVOID ctx)
            {
                this->m_context = ctx;
            }

            void RunA()
            {
            }

            void Run()
            {
                this->m_status = t_run;
                this->execute();
                this->m_status = t_done;
            }

            bool HasDone()
            {
                return (this->m_status == t_done || this->m_status == t_cancel);
            }

            void Wait()
            {
                while (this->m_status != t_done && this->m_status != t_cancel);
            }

            //! Should be overridden by derived classes.
            __forceinline virtual task* execute() = 0;
        };



        struct cpu_processor
        {
            cpu_processor(DWORD affinity = 0xFFFFFFFF)
            {
                m_hThread      = INVALID_HANDLE_VALUE;
                m_affinity     = affinity;
                m_active       = true;
                m_status       = idle;
                m_event        = CreateEvent(NULL, FALSE, FALSE, NULL);
                InitializeListHead(&m_TaskList);
            }

            static DWORD WINAPI processor_thread(LPVOID lpThreadParam);

            void Create()
            {
                m_active  = true;
                m_hThread = CreateThread(NULL, 0, cpu_processor::processor_thread, this, 0, NULL);
                SetThreadAffinityMask(m_hThread, m_affinity);
                SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);
            }

            void Destroy()
            {
                m_active = false;
                CloseHandle(m_hThread);
                m_hThread = INVALID_HANDLE_VALUE;
            }

            void Enqueue(task* t)
            {
                m_spinlock.Acquire();
                InsertTailList(&this->m_TaskList, t->GetListEntry());
                m_spinlock.Release();
            }

            task* Dequeue()
            {
                task* t                = NULL;
                PLIST_ENTRY pListEntry = NULL;

                do 
                {                    
                    if (IsListEmpty(&this->m_TaskList))
                    {
                        break;
                    }

                    m_spinlock.Acquire();
                    pListEntry = RemoveHeadList(&this->m_TaskList);
                    m_spinlock.Release();

                    t          = CONTAINING_RECORD(pListEntry, task, m_ListEntry);
                } while (FALSE);

                return t;
            }
            
            void Run();

            void Stop()
            {
                m_active = false;
                TerminateThread(m_hThread, 0);
            }

            void set_status_mask(volatile unsigned int* mask){m_status_mask = mask;}

            LIST_ENTRY      m_ListEntry;

            enum status
            {
                running, idle
            };
            status processor_status() const {return m_status;}
            void wake_up();

        private:
            volatile ULONG  m_active;
            volatile status m_status;

            HANDLE          m_hThread;
            HANDLE          m_event;
            DWORD           m_affinity;
            dsp_spin_lock   m_spinlock;
            LIST_ENTRY      m_TaskList;
            volatile unsigned int* m_status_mask;
        };


        struct cpu_manager
        {
        private:
            cpu_processor **m_cpu_array;
            unsigned int volatile m_status_mask;
            int             m_nTotalProcessor;
            int             m_nCurrentIndex;
            dsp_spin_lock   m_lock;
        
            cpu_manager();
            cpu_manager(const cpu_manager&) ;
            cpu_manager& operator=(const cpu_manager&) ;
            void setup();
            void destroy();
        public:
            ~cpu_manager();
            static cpu_manager* Instance();
            void run_task(task* t);
        };


        struct dsp_processor
        {
            enum
            {
                CPU, GPU, FPAG, DSP, SoC
            };
        };


        struct task_scheduler
        {
            task_scheduler(){}

        private:
            
        };
    }
    
}