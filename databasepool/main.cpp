#include "mysqlconnpool.h"
#include<iostream>
#include <thread>
#include <vector>
using namespace std;

void clientFunc(MysqlConnPool * connpool, int id){
    string sql = "select name,age from students;";
    
    MysqlConn *conn = NULL;
    while(true){
        conn = connpool->getconn();
        if(conn != nullptr) break;
    }
    conn->query(sql);
    if(conn->next()){
        printf("Thread [%d], Query result: [%s:%s]\n",id,conn->value("name").c_str(),conn->value("age").c_str());
    }
    connpool->freeconn(conn);
}

int main(){
    MysqlConnPool connpool(5,50,10);
    connpool.Initconns();
    vector<thread> tdvec;
    for(int i=0;i<50;i++){
        tdvec.emplace_back(thread(clientFunc,&connpool,i));
    }
    for(int i=0;i<tdvec.size();i++){
        tdvec[i].detach();
    }
    connpool.freeconns();
    // MysqlConn conn;
    // if(conn.connect("127.0.0.1","admin","admin123","testdb",3306) == false) {
    //     cout << "mysql connect failed!\n";
    //     return 0;
    // }
    // if(conn.isvalid()){
    //      if (conn.query("SELECT * FROM students;")) {
    //         cout << "Query executed successfully." << endl;
    //         while (conn.next()) {
    //             cout << "Name: " << conn.value("name") << ", Age: " << conn.value("age") << endl;
    //         }
    //     } else {
    //         cout << "Query execution failed: " << endl;
    //     }
    // }else cout << "conn not valid\n" << endl;
    // conn.disconnect();
    return 0; 
}