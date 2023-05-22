#include "CThreadPool.h"

void CThreadPool::CreateThreadPool(int nThreadCnt)
{
  //�����ź���
  m_hSemphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
  // check....

  //�����ٽ���
  InitializeCriticalSection(&m_csForQueue);

  //�����߳�
  m_bRunning = TRUE;
  m_vctThreads.resize(nThreadCnt);
  for (size_t i = 0; i < nThreadCnt; i++)
  {
    m_vctThreads[i] = CreateThread(
      NULL,                        // no security attributes 
      0,                           // use default stack size  
      WorkThreadFunc,                  // thread function 
      this,                // argument to thread function 
      0,                           // use default creation flags 
      NULL);                // returns the thread identifier 
    //check ....
  }
}

void CThreadPool::AddTask(ITask* pTask)
{
  EnterCriticalSection(&m_csForQueue);
  m_quuTasks.push(pTask); //������뵽����
  LeaveCriticalSection(&m_csForQueue);

  ReleaseSemaphore(m_hSemphore, 1, NULL); //֪ͨ�̣߳�����������
}

DWORD __stdcall CThreadPool::WorkThreadFunc(LPVOID lpParam)
{
  CThreadPool* pThis = (CThreadPool*)lpParam;

  while (pThis->m_bRunning)
  {
    //�ȴ�����
    WaitForSingleObject(pThis->m_hSemphore, INFINITE);

    //ȡ����
    EnterCriticalSection(&pThis->m_csForQueue);
    ITask* pTask = pThis->m_quuTasks.front();
    pThis->m_quuTasks.pop();
    LeaveCriticalSection(&pThis->m_csForQueue);

    //ִ������
    pTask->ExcuteTask();
  }
  return 0;
}
