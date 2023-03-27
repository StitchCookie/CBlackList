#include "systemconfig.h"
#include <QSettings>
#include "logrecord/logrecord.h"
bool SystemConfig::set_Mysql_info(BaseInfo& baseInfo)
{
    QSettings *sysconfig = new QSettings(SySConfigPATH, QSettings::IniFormat);
    baseInfo.grantrayMysqlConfig.ip = sysconfig->value("MYSQL/ip").toString();
    baseInfo.grantrayMysqlConfig.port = static_cast<quint16>(sysconfig->value("MYSQL/port").toUInt());
    baseInfo.grantrayMysqlConfig.user = sysconfig->value("MYSQL/user").toString();
    baseInfo.grantrayMysqlConfig.passwd = sysconfig->value("MYSQL/passwd").toString();
    baseInfo.grantrayMysqlConfig.databaseName = sysconfig->value("MYSQL/databaseName").toString();
    baseInfo.grantrayMysqlConfig.tableName = sysconfig->value("MYSQL/tableName").toString();
    baseInfo.grantrayMysqlConfig.intervalTime = static_cast<quint8>(sysconfig->value("MYSQL/intervalTime").toUInt());
    baseInfo.grantrayMysqlConfig.scopeTime = static_cast<quint8>(sysconfig->value("MYSQL/scopeTime").toUInt());
    delete  sysconfig;
    if(baseInfo.grantrayMysqlConfig.ip.isEmpty() || baseInfo.grantrayMysqlConfig.user.isEmpty()
            || baseInfo.grantrayMysqlConfig.passwd.isEmpty() || baseInfo.grantrayMysqlConfig.databaseName.isEmpty()
            || baseInfo.grantrayMysqlConfig.tableName.isEmpty())
    {
        vLogDebug("[set_Mysql_info]:请检查MYSQL数据库配置信息");
        return false;
    }
    baseInfo.grantrayMysqlConfig.intervalTime = baseInfo.grantrayMysqlConfig.intervalTime > 3 ? 3 : baseInfo.grantrayMysqlConfig.intervalTime;
    baseInfo.grantrayMysqlConfig.scopeTime    = baseInfo.grantrayMysqlConfig.scopeTime    > 10 ? 10 : baseInfo.grantrayMysqlConfig.scopeTime;

    return true;

}

bool SystemConfig::set_HttpRequest_info(BaseInfo& baseInfo)
{
    QSettings *sysconfig =new QSettings(SySConfigPATH,QSettings::IniFormat);
    baseInfo.cBlackListRequest.requestUrl=sysconfig->value("HTTPREQUSET/url").toString();
    baseInfo.cBlackListRequest.requestOvertime=static_cast<quint8>(sysconfig->value("HTTPREQUSET/requestOvertime").toUInt());
    delete  sysconfig;
    if(baseInfo.cBlackListRequest.requestUrl.isEmpty())
    {
        vLogDebug("[set_HttpRequest_info]:请检查黑名单请求服务配置信息");

        return false;
    }
    baseInfo.cBlackListRequest.requestOvertime = baseInfo.cBlackListRequest.requestOvertime > 3 ? 3 : baseInfo.cBlackListRequest.requestOvertime;
    return  true;
}

bool SystemConfig::set_HttpResponse_info(BaseInfo& baseInfo)
{
    QSettings *sysconfig =new QSettings(SySConfigPATH,QSettings::IniFormat);
    baseInfo.cBLackResponse.responseAddr=sysconfig->value("HTTPRESPONCE/responseAddr").toString();
    baseInfo.cBLackResponse.responsePort=static_cast<quint16>(sysconfig->value("HTTPRESPONCE/responseIp").toUInt());
    delete  sysconfig;
    if(baseInfo.cBLackResponse.responseAddr.isEmpty())
    {
        vLogDebug("[set_HttpResponse_info]:请检查黑名单请求响应配置信息");
        return false;
    }
    return true;
}

bool SystemConfig::set_Redis_info(BaseInfo& baseInfo)
{
    QSettings *sysconfig =new QSettings(SySConfigPATH,QSettings::IniFormat);
    baseInfo.loaclRedis.redisIp = sysconfig->value("REDIS/ip").toString();
    baseInfo.loaclRedis.redisPort = static_cast<int>(sysconfig->value("REDIS/port").toInt());
    baseInfo.loaclRedis.redisPasswd = sysconfig->value("REDIS/passwd").toString();
    baseInfo.loaclRedis.redisNo = static_cast<int>(sysconfig->value("REDIS/redisNo").toInt());
    baseInfo.loaclRedis.existTime = static_cast<int>(sysconfig->value("REDIS/exitTime").toInt());
    baseInfo.sys_Switch.redisPasswdForbid = static_cast<quint8>(sysconfig->value("SWITCH/redis_UsePassd").toUInt());
    delete  sysconfig;
    if(baseInfo.loaclRedis.redisIp.isEmpty() ||  baseInfo.loaclRedis.redisNo>15)
    {
        vLogDebug("[set_Redis_info]:请检查Redis配置信息");
        return false;
    }
    baseInfo.loaclRedis.existTime = baseInfo.loaclRedis.existTime > 15 ? 15 : baseInfo.loaclRedis.existTime;
    return true;
}

bool SystemConfig::setLanebaseInfo(BaseInfo &baseInfo)
{
    QSettings *sysconfig =new QSettings(SySConfigPATH,QSettings::IniFormat);
    baseInfo.laneBase_Info.provid = sysconfig->value("BASELINE/provId").toInt();
    baseInfo.laneBase_Info.laneId = sysconfig->value("BASELINE/laneid").toInt();
    baseInfo.laneBase_Info.statinid = sysconfig->value("BASELINE/stationId").toInt();
    delete  sysconfig;
    return true;
}
