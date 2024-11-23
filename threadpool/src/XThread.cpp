#include "XThread.h"
#include "XTask.h"
#include <chrono>

//启动一个线程，但是还不一定执行任务，以为可能没有任务可以执行
void XThread::Start(){
    printf("Thread [%d] created\n", this->id);
    std::thread th(&XThread::Main,this);
    th.detach();
}
//线程启动时执行的内容 这里可以改进 否则XThread的生命周期将影响线程的生命周期
void XThread::Main(){
    while(!isterminal){
        this->Activate(); 
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
//终止线程
void XThread::terminal(){
    this->isterminal = true;
}
//激活线程 有任务了就激活线程
void XThread::Activate(){
    XTask *task = NULL;
    tasks_mutex.lock();
    if(tasks.empty()){
        tasks_mutex.unlock();
        return;
    }
    task = tasks.front();
    tasks.pop_front();
    tasks_mutex.unlock();
    printf("Thread [%d]:", id);
    task->Init();
}
//向线程中添加任务
void XThread::AddTack(XTask *task, int task_id){
    if(!task) return;
    task->base = this->base;
    tasks_mutex.lock();
    tasks.push_back(task);
    tasks_mutex.unlock();
    printf("Task [%d] add to [%d] thread\n", task_id, id);
}
XThread::XThread(int id){
    this->id = id;
    this->isterminal = false;
}
XThread::~XThread(){
}