#include "XThreadPool.h"
#include "XTask.h"
#include "XThread.h"
using namespace std;
//分配任务到线程池
void XThreadPool::Dispatch(XTask* task,int arg){
    if(!task) return;
    int tid = (lastThread + 1) % threadcount;
    lastThread = tid;
    XThread *th = threads[tid];
    th->AddTack(task, arg); //添加任务
}
//初始化线程池中的线程
void XThreadPool::Init(int threadcnt){
    this->threadcount = threadcnt;
    this->lastThread = -1;
    for(int i=0;i<threadcount;i++){
        cout << "Create thread " << i << endl;
        XThread *th = new XThread(i);
        th->Start();
        threads.push_back(th);
        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

//关闭线程池
void XThreadPool::close(){
    for(auto item : threads){
        item -> terminal();
    }
}