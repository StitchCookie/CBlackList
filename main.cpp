#include <QCoreApplication>
#include <HTTPServer/listen.h>
#include "apihandler.h"
#include "pub/systemconfig.h"
BaseInfo m_baseInfo;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    if(initLogRecord(LL_DEBUG, "checkBlackLog","./checkBlackLog/",true) < 0)
    {
        qDebug() << "系统系统初始化失败";
    }
    if(!SystemConfig::getInstance()->set_Mysql_info(m_baseInfo)
            ||!SystemConfig::getInstance()->set_HttpRequest_info(m_baseInfo)
            ||!SystemConfig::getInstance()->set_HttpResponse_info(m_baseInfo)
            ||!SystemConfig::getInstance()->set_Redis_info(m_baseInfo))
    {
        qDebug()<<m_baseInfo.grantrayMysqlConfig.ip;
        vLogDebug("系统配置文件读取失败--程序退出");
        return 0;
    }
    vLogDebug("系统配置文件读取成功");
    HttpStartListen::getInstance().start(6666,apihandler::getInstance().handler);
    return a.exec();
}
