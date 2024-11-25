
#include "mysqlconn.h"


MysqlConn::MysqlConn(){
    mysqlConn = mysql_init(nullptr);
}
/*
@brief 移动构造
*/
MysqlConn::MysqlConn(MysqlConn && other){
    this->mysqlConn = other.mysqlConn;
    this->queryResult = other.queryResult;
    other.mysqlConn = NULL;
    other.queryResult = NULL;
}
MysqlConn& MysqlConn::operator = (MysqlConn && other){
    if(this->mysqlConn != other.mysqlConn){
        if(this->mysqlConn != NULL){
            mysql_close(mysqlConn);
            this->mysqlConn = NULL;
            this->mysqlConn = other.mysqlConn;
            other.mysqlConn = NULL;
            other.queryResult = NULL;
        }
        if(this->queryResult!=NULL){
            mysql_free_result(this->queryResult);
            this->queryResult = other.queryResult;
            this->fieldNames.swap(other.fieldNames);
        }
    }
    return *this;
}
MysqlConn::~MysqlConn(){
    if(this->mysqlConn != NULL){
        mysql_close(this->mysqlConn);
        freeQueryResult();//清空结果集和fieldNames字段
    }
}
/*
@brief 连接数据库
@param host ip地质
@param user 用户名
@param pwd 密码
@param dbname 连接的数据库
@param port 数据库端口
*/
bool MysqlConn::connect(string host,string user,string pwd,string dbname,unsigned int port){
    this->mysqlConn = mysql_real_connect(this->mysqlConn,host.c_str(),user.c_str(),pwd.c_str(),dbname.c_str(),port,nullptr,0);
    if(this->mysqlConn)return true;
    return false;
}
/*
@brief 断开数据库连接
*/
bool MysqlConn::disconnect(){
    if(!mysqlConn) return true;
    mysql_close(mysqlConn);
    mysqlConn = nullptr;
    freeQueryResult();
}
/*
@brief 更新操作
@param sql sql语句
*/
bool MysqlConn::update(string sql){
    return mysql_query(this->mysqlConn, sql.c_str()) == 0;
}
/*
@brief query操作
@param sql sql语句
*/
bool MysqlConn::query(string sql){
    if(mysql_query(this->mysqlConn, sql.c_str()) != 0) return false;
    freeQueryResult();//清除先前的查询结果
    queryResult = mysql_store_result(this->mysqlConn);
    fields();
    return true;
}
/*
@brief 查看所有字段名
*/
vector<string> MysqlConn::fields(){
    if(this->queryResult == nullptr) return fieldNames;
    vector<string>(0).swap(fieldNames);//清空上次查询记录的字段
    int colcnt = mysql_num_fields(queryResult);
    MYSQL_FIELD* fields = mysql_fetch_fields(queryResult);//获取字段名数组
    for(int i=0;i<colcnt;i++){
        fieldNames.emplace_back(fields[i].name);
    }
    return fieldNames;
}
/*
@brief 从结果集取下一条记录
*/
bool MysqlConn::next(){
    if(queryResult == NULL) return false;
    queryResultRow = NULL;//清空先前记录
    queryResultRow = mysql_fetch_row(queryResult);
    return queryResultRow!=NULL;
}
/*
@brief 获取指定index的值
*/
string MysqlConn::value(unsigned int index){
    if(queryResult == nullptr || queryResultRow == nullptr) return string();
    if(index <0 || index >= mysql_num_fields(queryResult)) return string();
    unsigned long* colRealLengths = mysql_fetch_lengths(queryResult);
    return string(queryResultRow[index],colRealLengths[index]);
}
/*
@brief 以string形式返回指定列的值
*/
string MysqlConn::value(string field){
    if(queryResult == nullptr || queryResultRow == nullptr) return string();
    if(field == "") return string();
    for(unsigned int i=0;i<fieldNames.size();i++){
        if(fieldNames[i] == field){
            return value(i);
        }
    }
    return string();
}
/*
@brief 开始一个事务，将自动提交关闭，进行一系列操作后需要显式调用commit和rollback
*/
bool MysqlConn::startTransaction(){
    return mysql_autocommit(this->mysqlConn,0)==0;
}
/*
@brief 提交
*/
bool MysqlConn::commit(){
    return mysql_commit(this->mysqlConn) == 0;
}
/*
@brief 回滚
*/
bool MysqlConn::rollback(){
    return mysql_rollback(this->mysqlConn) == 0;
}
/*
@brief 清空结果集
*/
void MysqlConn::freeQueryResult(){
    if(this->queryResult != NULL){
        mysql_free_result(this->queryResult);
        vector<string>(0).swap(fieldNames);
    }
}
/*
@brief 判断连接是否有效
*/
bool MysqlConn::isvalid(){
    if(!mysqlConn) return false;
    if(mysql_ping(mysqlConn)==0){
        return true;
    }
    return false;
}