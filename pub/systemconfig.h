#ifndef SYSTEMCONFIG_H
#define SYSTEMCONFIG_H
#include <HTTPServer/utilities.h>

#include <QObject>
#include <QMutex>
#include <qatomic.h>
#include <QQueue>
#define SySConfigPATH "../c_black_Configinfo.ini"

#pragma pack(1)
/*Lane Info*/
struct LaneBase_Info
{
    int laneId;
    int provid;
    int statinid;
};
/*主线门架系统配置*/
struct GrantrayMysqlConfig
{
    QString ip;
    quint16 port;
    QString user;
    QString passwd;
    QString databaseName;
    QString tableName;
    quint8 intervalTime;         //数据库请求间隔时间
    quint8 scopeTime;             //请求距当前时间范围内数据
    GrantrayMysqlConfig(){
        ip.clear();
        port = 3306;
        user.clear();
        passwd.clear();
        databaseName.clear();
        tableName.clear();
        intervalTime = 1;
        scopeTime = 5;
    }
};
/*请求黑名单服务器信息配置*/
struct CBlackListRequestInfo
{
    QString requestUrl;
    quint8  requestOvertime;     //请求超时时间
    CBlackListRequestInfo(){
        requestUrl.clear();
        requestOvertime = 1;     //单位分钟
    }
};

/*http响应信配置信息*/
struct CBLackResponseInfo
{
    QString responseAddr;
    quint16 responsePort;
    CBLackResponseInfo(){
        responseAddr.clear();
        responsePort = 9527;
    }
};

/*本地Redis数据库配置*/
struct LoaclRedisInfo
{
    QString redisIp;
    int redisPort;
    QString redisPasswd;
    int  redisNo;           //Redis库 默认15号库
    int  existTime;         //Redis键值生存时间
    LoaclRedisInfo()
    {
        redisIp.clear();
        redisPort = 6379;
        redisNo = 15;
        existTime = 5;
        redisPasswd.clear();
    }
};
/*系统配置 开关*/
struct sys_switch{
    quint8 redisPasswdForbid;
    sys_switch(){
        redisPasswdForbid = 0;
    }
};
struct BaseInfo{
    GrantrayMysqlConfig      grantrayMysqlConfig;
    CBlackListRequestInfo    cBlackListRequest;
    CBLackResponseInfo       cBLackResponse;
    LoaclRedisInfo           loaclRedis;
    LaneBase_Info            laneBase_Info;
    sys_switch               sys_Switch;
};

struct LineGrantrySQLData{
    QString etcCardId;
    QString license;
    QString obuid;
    LineGrantrySQLData(){
        etcCardId.clear();
        license.clear();
        obuid.clear();
    }
};

extern QQueue<LineGrantrySQLData> m_qqueueData;
extern BaseInfo m_baseInfo;
#pragma pack()    //恢复对齐状态

class SystemConfig : public QObject
{
    Q_OBJECT
public:
    static SystemConfig* getInstance()
    {
        static SystemConfig systemConfig;
        return &systemConfig;
    }
    /* 读取本地配置 */
    static bool set_Mysql_info(BaseInfo& baseInfo);
    static bool set_HttpRequest_info(BaseInfo& baseInfo);
    static bool set_HttpResponse_info(BaseInfo& baseInfo);
    static bool set_Redis_info(BaseInfo& baseInfo);
    static bool setLanebaseInfo(BaseInfo& baseInfo);
private:
    SystemConfig() = default;
    SystemConfig(const SystemConfig& T) = delete;
    SystemConfig& operator=(const SystemConfig&T) = delete;
};

#endif // SYSTEMCONFIG_H
