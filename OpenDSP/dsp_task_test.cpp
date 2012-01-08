#include "stdafx.h"
//////////////////////////////////////////////////////////////////////////

struct my_task : dsp_task::task
{
private:
    const static int nSize = 16;
    int m_Array[nSize];
    int m_ID;

public:
    my_task(int ID) : m_ID(ID) {}
    __forceinline dsp_task::task* execute()
    {
        printf("[%d] execute==>\n", m_ID);

        for (int i = 0; i < 1000000000; i++)
        {
            //printf("%d\n", i);
            _mm_pause();
        }
        printf("[%d] executeM<==\n", m_ID);

        return this;
    }
};


void multi_task_test()
{
    DWORD dwProcessorCount = GetProcessorCount();

    vector<dsp_task::cpu_processor*> vecCPU;

    cout << "Create " << dwProcessorCount << " CPU processors!" << endl;

    for (DWORD dwIdx = 1; dwIdx < dwProcessorCount; dwIdx++)
    {
        dsp_task::cpu_processor* pCPU = new dsp_task::cpu_processor((DWORD)(1 << dwIdx));
        pCPU->Create();
        vecCPU.push_back(pCPU);
    }

    cout << "Create a task on each CPU and RUN async!" << endl;

    vector<dsp_task::task*> vecTask;
    for (DWORD dwIdx = 0; dwIdx < dwProcessorCount - 1; dwIdx++)
    {
        dsp_task::task *pTask = new my_task(dwIdx);
        vecTask.push_back(pTask);
        vecCPU[dwIdx]->Enqueue(pTask);
    }

    for (DWORD dwIdx = 0; dwIdx < dwProcessorCount - 1; dwIdx++)
    {
        dsp_task::task *pTask = vecTask[dwIdx];
        pTask->RunA();
    }

    for (int iLoop = 0; iLoop < 1000; iLoop++)
    {
        cout << "task loop test :: " << iLoop << endl;
        for (DWORD dwIdx = 0; dwIdx < dwProcessorCount - 1; dwIdx++)
        {
            dsp_task::task *pTask = vecTask[dwIdx];
            pTask->Wait();
            vecCPU[dwIdx]->Enqueue(pTask);
            pTask->RunA();
        }
    }


    getchar();
    cout << "Delete tasks and CPUs!" << endl;
    for (DWORD dwIdx = 0; dwIdx < dwProcessorCount - 1; dwIdx++)
    {
        dsp_task::task* pTask         = vecTask[dwIdx];
        delete pTask;

        dsp_task::cpu_processor* pCPU = vecCPU[dwIdx];
        pCPU->Destroy();
        delete pCPU;
    }

    vecCPU.clear();
    vecTask.clear();
}

