#include "cblacklistsql.h"
#include <QDebug>
#include "pub/systemconfig.h"
#include <QSqlError>
#include <QSqlQuery>
CBlackListSql::CBlackListSql(QObject *parent) : QObject(parent)
{  
}
/**
 * @brief CBlackListSql::setConnectBase
 * @param ip
 * @param port
 * @param user
 * @param passwd
 * @param databaseName
 * @return 返回数据库驱动加载状态
 */
bool CBlackListSql::setConnectBase(QString ip, quint16 port, QString user, QString passwd, QString databaseName,QString type)
{
    QStringList sqlDrivers = QSqlDatabase::drivers();
    qDebug()<<sqlDrivers<<__FILE__;
    DB = QSqlDatabase::addDatabase(type);
    DB.setHostName(ip);
    DB.setPort(port);
    DB.setDatabaseName(databaseName);
    DB.setUserName(user);
    DB.setPassword(passwd);
    return DB.isValid();
}

/**
 * @brief CBlackListSql::getHandleData
 * @param selectSql
 * @return  返回查询结果
 */
QSqlQuery CBlackListSql::getHandleData(const QString selectSql,bool &ok)
{
    if(DB.open())
    {
        ok = true;
        qDebug()<<"数据库打开成功";
    }else{
        qDebug()<<"数据库打开error"<<DB.lastError().text();
        ok = false;
    }
    QSqlQuery query(DB);
    if(!query.exec(selectSql))
    {
        qDebug()<<"查询失败"<<DB.lastError().text();
        ok = false;
    }else{
        ok = true;
        qDebug()<<"success";
    }
    DB.close();
    return query;
}

CBlackListSql::~CBlackListSql()
{

}

