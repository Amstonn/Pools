#ifndef XTHREAD
#define XTHREAD
#include<list>
#include<mutex>
#include <thread>
#include <iostream>
class XTask;
struct event_base;
class XThread{
public:
    void Start();//启动线程
    void Main();//线程入口函数
    void Activate();//线程激活
    void AddTack(XTask *, int);//添加任务 一个线程可以同时处理多个任务 公用一个event_base
    void terminal();
    XThread(int id);
    ~XThread();
    int id = 0;//线程编号
private:
    event_base *base = 0;
    std::list<XTask*> tasks;//任务链表
    std::mutex tasks_mutex;//任务链表互斥锁
    bool isterminal;
};
#endif