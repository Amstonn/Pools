#include "XTask.h"
#include "XThreadPool.h"
using namespace std;

int main(){
    XThreadPool* threadpool = XThreadPool::Get();
    threadpool->Init(5);
    for(int i=0;i<100;i++){
        MyTask *xt = new MyTask();
        threadpool->Dispatch(xt,i);
        this_thread::sleep_for(chrono::milliseconds(1));
    }
    // this_thread::sleep_for(chrono::milliseconds(10));
    // threadpool->close(); 
    return 0;
}