#include "mysqlconnpool.h"

MysqlConnPool::MysqlConnPool(int min_size,int max_size,int size):p_minsize(min_size),p_maxsize(max_size),p_size(size){
    assert(size>=min_size && size <= max_size);
    p_cntlocked = 0;
    mutex_cnt = PTHREAD_MUTEX_INITIALIZER;
}
MysqlConnPool::~MysqlConnPool(){
    freeconns();
    pthManage.join();
}
/*
@brief 初始化连接池中的所有连接
*/
bool MysqlConnPool::Initconns(){
    for(int i=0;i < p_size;i++){
        MysqlConn conn;
        if(conn.connect("127.0.0.1","admin","admin123","testdb",3306) == false) return false;
        p_conns.push_back(move(conn));
        p_mutexs.push_back(pthread_mutex_t());
        pthread_mutex_init(&p_mutexs[i],NULL);
        if(!p_conns[i].isvalid()) return false;
    }
    printf("%d new mysql connection is created!\n", p_size);
    pthManage = thread([this]() { this->manage(this); });
    return true;
}
/*
@brief 获取一个连接
*/
MysqlConn* MysqlConnPool::getconn(){
    if(p_cntlocked == p_size) return nullptr;
    //便利所有连接，返回可以上锁的连接
    for(int i=0;i<p_size;i++){
        if(pthread_mutex_trylock(&p_mutexs[i]) == 0){
            pthread_mutex_lock(&mutex_cnt);
            p_cntlocked ++;
            pthread_mutex_unlock(&mutex_cnt);
            return &p_conns[i];
        }
    }
    return NULL;
}
/*
@brief 释放指定连接
*/
void MysqlConnPool::freeconn(MysqlConn* conn){
    if(conn != nullptr){
        for(int i=0;i<p_size;i++){
            if(&p_conns[i] == conn){
                if (pthread_mutex_trylock(&p_mutexs[i]) != 0) {
                    pthread_mutex_unlock(&p_mutexs[i]);
                }else{
                    pthread_mutex_unlock(&p_mutexs[i]);
                }
                pthread_mutex_lock(&mutex_cnt);
                p_cntlocked --;
                pthread_mutex_unlock(&mutex_cnt);
                break;
            }
        }
    }
}
/*
@brief 释放所有连接
*/
void MysqlConnPool::freeconns(){
    managestop = true;
    for(int i=0;i<p_size;i++){
        p_conns[i].disconnect();
        if(pthread_mutex_trylock(&p_mutexs[i]) != 0){
            pthread_mutex_unlock(&p_mutexs[i]);
        }else{
            pthread_mutex_unlock(&p_mutexs[i]);
        }
        pthread_mutex_destroy(&p_mutexs[i]);
    }
    p_conns.clear();
    p_mutexs.clear();
    p_cntlocked = 0;
}
/*
@brief 管理连接池 调整连接池大小
*/
void MysqlConnPool::manage(void *arg){
    this_thread::sleep_for(chrono::milliseconds(5));
    while(!managestop){
        // if(p_cntlocked > 0.8*p_size && 1.2*p_size<=p_maxsize){
        //     //增加连接
        //     printf("add new connect\n");
        //     int i=p_size;
        //     p_size = 1.2*p_size;
        //     for(;i<p_size;i++){
        //         MysqlConn conn;
        //         while(!conn.connect("127.0.0.1","admin","admin123","testdb",3306)){continue;};
        //         p_conns.push_back(move(conn));
        //         pthread_mutex_t mtx  = PTHREAD_MUTEX_INITIALIZER;
        //         p_mutexs.push_back(mtx);
        //     }
        // }
        // else if(p_cntlocked < 0.4*p_size && 0.8*p_size >= p_minsize && 0.8*p_size<=p_maxsize){
        //     //减少连接
        //     printf("delete some connect\n");
        //     int newSize = max(static_cast<int>(p_size * 0.8), p_minsize);
        //     while (p_size > newSize) {
        //         auto it1 = p_conns.begin();
        //         auto it2 = p_mutexs.begin();
        //         for (; it1 != p_conns.end() && it2 != p_mutexs.end();) {
        //             if (pthread_mutex_trylock(&(*it2)) == 0) {
        //                 (*it1).disconnect();
        //                 it1 = p_conns.erase(it1);
        //                 pthread_mutex_destroy(&(*it2));
        //                 it2 = p_mutexs.erase(it2);
        //                 break; // Exit loop
        //             } else {
        //                 ++it1;
        //                 ++it2;
        //             }
        //         }
        //         // If no more connections left to release
        //         if (p_conns.size() <= newSize) break; 
        //     }
        //     p_size = newSize; // Update size
        // }
    }
}