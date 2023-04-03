#include "httpclienthandle.h"
#include <QEventLoop>
#include <iostream>
using namespace  std;
int httpClientHandle::restart_redis_count = 0;
httpClientHandle::httpClientHandle():
    m_NetManager(nullptr)
  ,m_redisStatus(false)
{
    qDebug()<<"http client 构造 success"<<QThread::currentThreadId();
    this->moveToThread(&m_pThread);
    setRedisDatabase(m_baseInfo.loaclRedis.redisIp
                     ,m_baseInfo.loaclRedis.redisPort
                     ,m_baseInfo.loaclRedis.redisNo
                     , m_baseInfo.loaclRedis.redisPasswd
                     ,m_baseInfo.loaclRedis.existTime);
    m_pThread.start();
    connect(&m_pThread,&QThread::started,this,&httpClientHandle::dealPost);
    qDebug()<<"http client 构造 success"<<QThread::currentThreadId();
}
/**
 * @brief httpClientHandle::packRequset
 * @param 对队列中的数据按照http请求的格式组装
 */
void httpClientHandle::packRequset(QByteArray &postdata)
{
    m_LineGrantrySQLData = m_qqueueData.dequeue();
    m_map["checkType"] = 7;
    m_map["cpcCardId"] = "";
    m_map["etcCardId"] = m_LineGrantrySQLData.etcCardId;
    m_map["laneId"] = m_baseInfo.laneBase_Info.laneId;
    m_map["license"] = m_LineGrantrySQLData.license;
    m_map["obuId"] = m_LineGrantrySQLData.obuid;
    m_map["stationId"] = m_baseInfo.laneBase_Info.statinid;
    postdata = QJsonDocument(QJsonObject::fromVariantMap(m_map)).toJson(QJsonDocument::Compact);
}
/**
 * @brief httpClientHandle::setRedisDatabase
 * @param m_ip              redis地址
 * @param m_port            redis端口
 * @param m_No              redis库号
 * @param m_Passwd          redis密码
 * @param m_exitTime        redisTTL
 */
void httpClientHandle::setRedisDatabase(QString m_ip, int m_port, int m_No, QString m_Passwd, int m_exitTime)
{
    this->m_ip = m_ip;
    this->m_port = m_port;
    this->m_No = m_No;
    this->m_Passwd = m_Passwd;
    this->m_exitTime = m_exitTime;

}
/**
 * @brief httpClientHandle::connectRedis
 * @return 返回true连接成功
 */
bool httpClientHandle::connectRedis()
{

    m_ctx = redisConnect(m_ip.toStdString().c_str(),m_port);
    if(m_ctx == nullptr || m_ctx->err)
    {
        if(m_ctx)
        {
            //QString str = QString("Redis Connect Error--->%1").arg(m_ctx->errstr);
            redisFree(m_ctx);
            m_ctx = nullptr;
        }else{
            //            QString str = "can't allocate redis context";
            //            qDebug()<<str;
        }
        return false;
    }
    if(m_baseInfo.sys_Switch.redisPasswdForbid == 1)
    {
        redisReply * reply = (redisReply *)redisCommand(m_ctx, "AUTH %s", m_Passwd.toStdString().c_str());
        qDebug()<<reply->str;
        freeReplyObject(reply);
    }
    QString selectNo = QString("select %1").arg(m_baseInfo.loaclRedis.redisNo);
    redisReply *reply = (redisReply*)redisCommand(m_ctx,selectNo.toStdString().c_str());
    if(reply->type == REDIS_REPLY_STATUS)
    {
        m_redisStatus = true;
        return true;
    }else{
        redisFree(m_ctx);
        m_ctx = nullptr;
        m_redisStatus = false;
        return false;
    }
    freeReplyObject(reply);
}
/**
 * @brief httpClientHandle::restartConnectRedis
 * @note  每秒连接一次 直到连接成功
 */
void httpClientHandle::restartConnectRedis()
{
    //restart_redis_count = 0;
    qDebug()<<"Redis重新连接中,请检查Redis配置信息....";
    qDebug()<<"Redis IP------>"<<m_baseInfo.loaclRedis.redisIp;
    qDebug()<<"Redis port------>"<<m_baseInfo.loaclRedis.redisPort;
    qDebug()<<"Redis passd------>"<<m_baseInfo.loaclRedis.redisPasswd;
    qDebug()<<"Redis exittime------>"<<m_baseInfo.loaclRedis.existTime;
    qDebug()<<"Redis 数据库号------>"<<m_baseInfo.loaclRedis.redisNo;
    qDebug()<<"Redis 是否使用密码------>"<<QString("%1").arg(m_baseInfo.sys_Switch.redisPasswdForbid == 0 ?"NO" : "YES");
    while(!m_ctx)
    {
        connectRedis();
        QThread::sleep(1);
        //  ++restart_redis_count;
        //        if(restart_redis_count >= 30)
        //        {
        //            qDebug()<<"Redis重新连接超时，线程退出，请检查Redis环境并重启程序";
        //            return;
        //        }
    }
    qDebug()<<"Redis重连成功";
    //restart_redis_count = 0;
}
/**
 * @brief httpClientHandle::dealData
 * @param pArray http响应的数据
 * @param pJson  传入Json对象
 * @return 如果转换成功返回0
 */
bool httpClientHandle::dealData(const QByteArray &pArray, QJsonObject &pJson)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(pArray,&error);
    if(error.error != QJsonParseError::NoError)
    {
        return false;
    }
    if(doc.isObject())
    {
        pJson = doc.object();
        return true;
    }
    return false;
}
/**
 * @brief httpClientHandle::Utf8hexqstringToGbkhexqstring
 * @param   传入按照gbk编码的 16进制
 * @return  返回字符串
 */
QString httpClientHandle::Utf8hexqstringToGbkhexqstring(const QString &text)
{
    {
        QTextCodec *tc = QTextCodec::codecForName("GBK");
        // 将Hex字符串转换为Ascii字符串
        QString temp;
        QByteArray ascii_data;
        foreach(QString str, text)
        {
            ascii_data.append(str);
        }
        // 将ascii_data中的16进制数据转化为对应的字符串，比如\x31转换为"1"
        temp = tc->toUnicode(QByteArray::fromHex(ascii_data));
        return temp;
    }
}
/**
 * @brief httpClientHandle::execRedisCommand
 * @param                    redis连接上下文
 * @param cmd                redis执行命令
 * @return                   返回true执行成功
 */
int httpClientHandle::execRedisCommand(redisContext *connect_handle, const char *cmd)
{
    redisReply *reply = (redisReply *)redisCommand(connect_handle,cmd);
    if(!reply)
    {
        /*在未执行成功的情况下 千万不要free reply*/
        qDebug()<<"车牌黑名单插入Redis数据库失败，车牌号： "<<m_LineGrantrySQLData.license;
        return 0;
    }else if(reply->type == REDIS_REPLY_ERROR)
    {
        qDebug()<<"车牌黑名单插入Redis数据库失败，车牌号： "<<m_LineGrantrySQLData.license;
        freeReplyObject(reply);
        reply = nullptr;
        return 0;
    }else
    {
        freeReplyObject(reply);
        reply = nullptr;
        return 1;
    }
}
/**
 * @brief httpClientHandle::dealPost
 * @note  从全局队列中读取队头元素 进行http请求 并将返回结果插入Redis数据库中
 */
void httpClientHandle::dealPost()
{
    //等待redis成功
    if(connectRedis())
    {
        qDebug()<<"本地Redis 连接成功（长连接）!!!";
    }else
    {
        restartConnectRedis();
        if(m_ctx == nullptr)
            return;
    }
    //如果redis连接失败则不会进行 下面大http操作
    m_request.setUrl(m_baseInfo.cBlackListRequest.requestUrl);
    m_request.setRawHeader(QByteArray("Content-Type"),QByteArray("application/json;charset=utf-8"));
    m_request.setRawHeader(QByteArray("Accept-Encoding"),QByteArray("deflate"));
    m_request.setRawHeader(QByteArray("Connection:"),QByteArray("Keep-alive"));
    //m_request.setRawHeader(QByteArray("Connection:"),QByteArray("close"));
    m_request.setRawHeader(QByteArray("Keep-Alive:"),QByteArray("timeout=60"));
    //每5毫秒 读取对头元素 并弹出  进行http请求 每次请求都是阻塞的----必须要有结果 超时 或者失败 或者成功
    while(true){
        QThread::msleep(5);
        if(!m_qqueueData.isEmpty()){
            QByteArray m_bytearray;
            packRequset(m_bytearray);
            QTimer timer;
            timer.setInterval(1000 * 60 * m_baseInfo.cBlackListRequest.requestOvertime);
            timer.setSingleShot(true);
            //对网络管理类只分配一次内存 并且不能和request跨线程 网络管理类会自动设置为request类的父对象
            if(m_NetManager == nullptr)
            {
                qDebug()<<"start allot networkmanager addr";
                m_NetManager = new QNetworkAccessManager;
            }
            m_reply = m_NetManager->post(m_request,m_bytearray);
            QEventLoop loop;
            connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);
            connect(m_reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
            timer.start();
            loop.exec();
            QByteArray replyCount;
            QJsonObject jsondata;
            if(timer.isActive())
            {
                timer.stop();
                if(m_reply->error() != QNetworkReply::NoError)
                {
                }else
                {
                    if(m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 200)
                    {
                        replyCount = m_reply->readAll();
                        if((true == dealData(replyCount,jsondata)) && (m_ctx != nullptr)){
                            QString str = QString("setex %1 %2 %3").arg(m_LineGrantrySQLData.license)
                                    .arg(m_baseInfo.loaclRedis.existTime * 60)
                                    .arg(jsondata.value("result").toInt());
                            int execresult = execRedisCommand(m_ctx, str.toStdString().c_str());
                            if(!execresult)
                            {
                                redisFree(m_ctx);
                                m_ctx = nullptr;
                                restartConnectRedis();
                                //*按理来说这里不需要在做重连接机制 *//
                                execRedisCommand(m_ctx, str.toStdString().c_str());
                            }

                        }else if( m_ctx == nullptr)
                        {
                            restartConnectRedis();
                            QString str = QString("setex %1 %2 %3").arg(m_LineGrantrySQLData.license)
                                    .arg(m_baseInfo.loaclRedis.existTime * 60)
                                    .arg(jsondata.value("result").toInt());
                            //*按理来说这里不需要在做重连接机制 *//
                            execRedisCommand(m_ctx, str.toStdString().c_str());
                        }
                    }
                }
            }
            else{ //超时处理
                disconnect(m_NetManager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
            }
            m_reply->abort();
            DEL_OBJECT(m_reply);
        }
    }
}
httpClientHandle::~httpClientHandle()
{
    if(m_NetManager!=nullptr)
    {
        DEL_OBJECT(m_NetManager)
    }
    if(m_ctx != nullptr)
    {
        redisFree(m_ctx);
        m_ctx = nullptr;
    }
}


