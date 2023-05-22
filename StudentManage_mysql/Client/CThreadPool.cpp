#include "CThreadPool.h"

void CThreadPool::CreateThreadPool(int nThreadCnt)
{
  //创建信号量
  m_hSemphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
  // check....

  //创建临界区
  InitializeCriticalSection(&m_csForQueue);

  //创建线程
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
  m_quuTasks.push(pTask); //任务加入到队列
  LeaveCriticalSection(&m_csForQueue);

  ReleaseSemaphore(m_hSemphore, 1, NULL); //通知线程，有任务来了
}

DWORD __stdcall CThreadPool::WorkThreadFunc(LPVOID lpParam)
{
  CThreadPool* pThis = (CThreadPool*)lpParam;

  while (pThis->m_bRunning)
  {
    //等待任务
    WaitForSingleObject(pThis->m_hSemphore, INFINITE);

    //取任务
    EnterCriticalSection(&pThis->m_csForQueue);
    ITask* pTask = pThis->m_quuTasks.front();
    pThis->m_quuTasks.pop();
    LeaveCriticalSection(&pThis->m_csForQueue);

    //执行任务
    pTask->ExcuteTask();
  }
  return 0;
}
