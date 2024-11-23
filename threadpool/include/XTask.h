#ifndef XTASK
#define XTASK
#include <iostream>
#include <thread>
class XTask{
public:
    struct event_base *base = 0;
    int thread_id = 0;
    virtual bool Init() = 0;
    bool runflag = false;
};
class MyTask: public XTask{
public:
    bool Init();
};
#endif