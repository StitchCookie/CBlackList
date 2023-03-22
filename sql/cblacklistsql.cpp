#include "cblacklistsql.h"
#include <QDebug>
#include "pub/systemconfig.h"
#include <QSqlError>
CBlackListSql::CBlackListSql(QObject *parent) : QObject(parent)
{
    QStringList sqlDrivers = QSqlDatabase::drivers();
    qDebug()<<sqlDrivers<<__FILE__;
    DB.addDatabase("QMYSQL");
    if(DB.isValid())
    {
        qDebug()<<"MYSQL数据库驱动无效";
    }
    DB.setHostName(m_baseInfo.grantrayMysqlConfig.ip);
    DB.setPort(m_baseInfo.grantrayMysqlConfig.port);
    DB.setDatabaseName(m_baseInfo.grantrayMysqlConfig.databaseName);
    DB.setUserName(m_baseInfo.grantrayMysqlConfig.user);
    DB.setPassword(m_baseInfo.grantrayMysqlConfig.passwd);
    qDebug()<<m_baseInfo.grantrayMysqlConfig.ip<<m_baseInfo.grantrayMysqlConfig.port<<m_baseInfo.grantrayMysqlConfig.databaseName
           <<m_baseInfo.grantrayMysqlConfig.user<<m_baseInfo.grantrayMysqlConfig.passwd;
    if(DB.open())
    {
        qDebug()<<"数据库打开成功";
    }else{
         qDebug()<<"数据库打开error"<<DB.lastError().text();
    }
    //    if(DB.close())
    //    {
    //        qDebug()<<"数据库关闭成功";
    //    }
}

CBlackListSql::~CBlackListSql()
{

}

