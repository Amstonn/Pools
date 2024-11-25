#ifndef CONNECTIONPOOL
#define CONNECTIONPOOL

#include "mysqlconn.h"
#include <thread>
#include <assert.h>
#include <vector>
#include <chrono>
using namespace std;

class MysqlConnPool{
private:
    int p_minsize;
    int p_maxsize;
    int p_size;
    int p_cntlocked;
    bool managestop = false;
    vector<MysqlConn> p_conns;
    vector<pthread_mutex_t> p_mutexs;
    thread pthManage;
    pthread_mutex_t mutex_cnt;
    void manage(void *arg);//连接池size调整
public:
    MysqlConnPool(int min_size,int max_size,int size);
    ~MysqlConnPool();
    bool Initconns();
    MysqlConn* getconn();
    void freeconn(MysqlConn*);
    void freeconns();
};
#endif