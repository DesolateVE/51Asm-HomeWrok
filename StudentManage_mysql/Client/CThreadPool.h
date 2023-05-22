#pragma once
#include <queue>
#include <vector>
using namespace std;

#include <Windows.h>

class ITask
{
public:
  virtual void ExcuteTask() = 0;
};

class CThreadPool
{
public:
  void CreateThreadPool(int nThreadCnt);
  void AddTask(ITask* pTask);

private:
  static DWORD WINAPI WorkThreadFunc(LPVOID lpParam);

private:
  vector<HANDLE> m_vctThreads;
  BOOL m_bRunning = FALSE;
  HANDLE m_hSemphore = NULL; //信号量
  
  CRITICAL_SECTION m_csForQueue = {};
  queue<ITask*> m_quuTasks;  //存放任务的任务队列
};

