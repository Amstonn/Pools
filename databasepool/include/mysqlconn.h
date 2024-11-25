#ifndef MYSQLCONN
#define MYSQLCONN
#include <iostream>
#include <mysql.h>
#include <vector>
using namespace std;
class MysqlConn{
private:
    MYSQL * mysqlConn = NULL;//Mysql连接
    MYSQL_RES* queryResult = NULL;//查询结果集
    std::vector<std::string> fieldNames; //当前结果集的所有字段名
    MYSQL_ROW queryResultRow = NULL; //当前查询记录
    void freeQueryResult();
public:
    MysqlConn();
    MysqlConn(MysqlConn& other) = delete;
    MysqlConn &operator = (MysqlConn& other) = delete; //删除赋值和拷贝构造 避免重复管理并确保连接的但一所有权
    MysqlConn(MysqlConn && other); //移动构造，剥夺原有所有权
    MysqlConn& operator = (MysqlConn && other);//移动赋值
    ~MysqlConn();
    bool connect(string host,string user,string pwd,string dbname,unsigned int port);
    bool disconnect();
    bool update(string sql);
    bool query(string sql);
    vector<string> fields();
    bool next();
    string value(unsigned int index);
    string value(string field);
    bool startTransaction();
    bool commit();
    bool rollback();
    bool isvalid();
};
#endif