#include "httpclienthandle.h"
#include <QEventLoop>

httpClientHandle::httpClientHandle():
    m_pThread(nullptr)
  ,m_NetManager(nullptr)
  ,m_redisStatus(false)
{
    m_pThread = new QThread();
    this->moveToThread(m_pThread);
    setRedisDatabase(m_baseInfo.loaclRedis.redisIp
                     ,m_baseInfo.loaclRedis.redisPort
                     ,m_baseInfo.loaclRedis.redisNo
                     , m_baseInfo.loaclRedis.redisPasswd

                     ,m_baseInfo.loaclRedis.existTime);

    if(connectRedis())
        qDebug()<<"Redis connect successed!!!";

    connect(m_pThread, SIGNAL(started()), this, SLOT(dealPost()));
    qDebug()<<"http request success";
    m_pThread->start();
}

void httpClientHandle::packRequset(QByteArray &postdata)
{
    qDebug()<<"-0-----------------0"<<m_qqueueData.size();
    m_LineGrantrySQLData = m_qqueueData.dequeue();
    m_map["checkType"] = 7;
    m_map["cpcCardId"] = "";
    m_map["etcCardId"] = m_LineGrantrySQLData.etcCardId;
    m_map["laneId"] = m_baseInfo.laneBase_Info.laneId;
    m_map["license"] = m_LineGrantrySQLData.license;
    m_map["obuId"] = m_LineGrantrySQLData.obuid;
    m_map["stationId"] = m_baseInfo.laneBase_Info.statinid;
    postdata = QJsonDocument(QJsonObject::fromVariantMap(m_map)).toJson(QJsonDocument::Compact);
    qDebug()<<"current json request data:"<<postdata;
    qDebug()<<"current json request  plate car data:"<<m_LineGrantrySQLData.license;
    qDebug()<<"-0-----------------0"<<m_qqueueData.size();
}

void httpClientHandle::setRedisDatabase(QString m_ip, int m_port, int m_No, QString m_Passwd, int m_exitTime)
{
    this->m_ip = m_ip;
    this->m_port = m_port;
    this->m_No = m_No;
    this->m_Passwd = m_Passwd;
    this->m_exitTime = m_exitTime;

}

bool httpClientHandle::connectRedis()
{
    m_ctx = redisConnect(m_ip.toStdString().c_str(),m_port);
    qDebug()<<"ip"<<m_ip<<"port"<<m_port;
    if(m_ctx == nullptr || m_ctx->err)
    {
        if(m_ctx)
        {
            QString str = QString("Error%q").arg(m_ctx->errstr);
            qDebug()<<str;
        }else{
            QString str = "can't allocate redis context";
            qDebug()<<str;
        }
        return false;
    }
    if(m_baseInfo.sys_Switch.redisPasswdForbid == 1)
    {
        redisReply * reply = (redisReply *)redisCommand(m_ctx, "AUTH %s", m_Passwd.toStdString().c_str());
        freeReplyObject(reply);
    }
    QString selectNo = QString("select %1").arg(m_baseInfo.loaclRedis.redisNo);
    redisReply *reply = (redisReply*)redisCommand(m_ctx,selectNo.toStdString().c_str());
    if(reply->type == REDIS_REPLY_STATUS)
    {
        m_redisStatus = true;
    }else{
        m_redisStatus = false;
    }
    freeReplyObject(reply);
    return m_redisStatus;
}

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
}

QString httpClientHandle::Utf8hexqstringToGbkhexqstring(const QString &text)
{
    {
        QTextCodec *tc = QTextCodec::codecForName("GBK");
        // 将Hex字符串转换为Ascii字符串
        QString temp;
        QByteArray ascii_data;
        // QStringList temp_list = text.split('0', QString::SkipEmptyParts);
        foreach(QString str, text)
        {
            ascii_data.append(str);
        }
        // 将ascii_data中的16进制数据转化为对应的字符串，比如\x31转换为"1"
        temp = tc->toUnicode(QByteArray::fromHex(ascii_data));
        return temp;
    }
}
void httpClientHandle::dealPost()
{
    qDebug()<<"dealPost m_qqueueData isEmpty:"<<m_qqueueData.isEmpty();
    qDebug()<<"dealPost m_qqueueData isSize:"<<m_qqueueData.size();
    while(true){
        while(!m_qqueueData.isEmpty()){
            QByteArray m_bytearray;
            packRequset(m_bytearray);
            qDebug()<<"http client Thread id:"<<QThread::currentThreadId();
            QTimer timer;
            timer.setInterval(1000 * 60 * m_baseInfo.cBlackListRequest.requestOvertime);
            timer.setSingleShot(true);
            if(m_NetManager == nullptr)
            {
                qDebug()<<"start allot networkmanager addr";
                m_NetManager = new QNetworkAccessManager;
            }
            m_request.setUrl(m_baseInfo.cBlackListRequest.requestUrl);
            m_request.setRawHeader(QByteArray("Content-Type"),QByteArray("application/json;charset=utf-8"));
            m_request.setRawHeader(QByteArray("Accept-Encoding"),QByteArray("deflate"));
            m_request.setRawHeader(QByteArray("Connection:"),QByteArray("Keep-alive"));
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
                        qDebug()<<"response data : "<<replyCount;
                        qDebug()<<"request Data :"<<m_bytearray;

                        if(false == dealData(replyCount,jsondata)){

                        }else{
                            QString str = QString("setex %1 %2 %3").arg(m_LineGrantrySQLData.license)
                                    .arg(m_baseInfo.loaclRedis.existTime * 60)
                                    .arg(jsondata.value("result").toInt());
                            redisReply *reply = (redisReply *)redisCommand(m_ctx,str.toStdString().c_str());
                            if(reply != NULL && reply->type == REDIS_REPLY_STATUS)
                            {
                                qDebug()<<"Redis insert Status:"<<reply->str;
                            }
                            freeReplyObject(reply);
                        }
                    }
                }
            }
            else{ //超时处理
                disconnect(m_NetManager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
                m_reply->abort();
            }
            DEL_OBJECT(m_reply);
        }
    }
}
httpClientHandle::~httpClientHandle()
{
    redisFree(m_ctx);
    m_pThread->quit();
    m_pThread->wait();
    m_pThread->deleteLater();
}


