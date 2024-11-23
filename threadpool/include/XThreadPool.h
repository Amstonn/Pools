#ifndef XTHREADPOOL
#define XTHREADPOOL

#include<vector>
class XThread;
class XTask;
class XThreadPool
{
private:
    int threadcount;
    int lastThread = -1;
    std::vector<XThread *> threads;//所有线程数组
    XThreadPool(){};
public:
    ~XThreadPool(){};
    static XThreadPool *Get(){  //单例模式
        static XThreadPool p;
        return &p;
    }
    void Init(int threadcnt);//初始化线程
    void Dispatch(XTask*,int arg);//分配任务到线程池 arg是任务所带的参数
    void close();
};

#endif