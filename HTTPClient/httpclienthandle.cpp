#include "httpclienthandle.h"
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
httpClientHandle::httpClientHandle():
    m_pThread(nullptr)
  ,m_isBusy(false)
  ,m_dtBusyBeginTime(QDateTime::currentDateTime())
  ,m_dtBusyEndTime(QDateTime::currentDateTime())
  ,m_NetManager(nullptr)
  ,m_redisStatus(false)
{
    m_pThread = new QThread();
    this->moveToThread(m_pThread);

    m_pThread->start();
    m_ip = m_baseInfo.loaclRedis.redisIp;
    m_port = m_baseInfo.loaclRedis.redisPort;
    m_No = m_baseInfo.loaclRedis.redisNo;
    m_Passwd = m_baseInfo.loaclRedis.redisPasswd;
    m_exitTime = m_baseInfo.loaclRedis.existTime;
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
        return;
    }
    if(m_baseInfo.sys_Switch.redisPasswdForbid == 1)
    {
        redisReply * reply = (redisReply *)redisCommand(m_ctx, "AUTH %s", m_Passwd.toStdString().c_str());
        if (reply->type == REDIS_REPLY_ERROR) {
            qDebug()<<"Redis认证失败！";
        }
        else
        {
            qDebug()<<"Redis认证成功！";
        }
        freeReplyObject(reply);
    }
    QString selectNo = QString("select %1").arg(m_baseInfo.loaclRedis.redisNo);
    redisReply *reply = (redisReply*)redisCommand(m_ctx,selectNo.toStdString().c_str());
    if(reply->type == REDIS_REPLY_STATUS)
    {
        m_redisStatus = true;
        qDebug()<<"Redis 15 success";
    }else{
        m_redisStatus = false;
        qDebug()<<"Redis 15 failed";
    }
    freeReplyObject(reply);
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
void httpClientHandle::dealPost(QByteArray lineGrantryCarData)
{
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
    m_reply = m_NetManager->post(m_request,lineGrantryCarData);
    QEventLoop loop;
    connect(&timer,&QTimer::timeout,&loop,&QEventLoop::quit);
    connect(m_reply,&QNetworkReply::finished,&loop,&QEventLoop::quit);
    timer.start();
    loop.exec();
    QJsonObject jsondata;
    QByteArray array = lineGrantryCarData;
    QByteArray replyCount;
    if(timer.isActive())
    {
        timer.stop();
        if(m_reply->error() != QNetworkReply::NoError){
            qDebug() << "request error:" << m_reply->errorString();
            QString str = QString("setex %1 %2 %3").arg(Utf8hexqstringToGbkhexqstring(jsondata.value("license").toString()))
                    .arg(m_baseInfo.loaclRedis.existTime * 60)
                    .arg(-1);
            redisReply *reply = (redisReply *)redisCommand(m_ctx,str.toStdString().c_str());
            if(reply != NULL && reply->type == REDIS_REPLY_STATUS)
            {
                qDebug()<<"Redis insert Status:"<<reply->str;
            }
            freeReplyObject(reply);
        }else{
            if(m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute) == 200)
            {
                replyCount = m_reply->readAll();
                qDebug()<<"response data : "<<replyCount;
                qDebug()<<"request Data :"<<lineGrantryCarData;

                if(false == dealData(array,jsondata)){

                }else{
                    qDebug()<<"gbk :"<<Utf8hexqstringToGbkhexqstring(jsondata.value("license").toString());
                    qDebug()<<"UTF-8 "<<QByteArray::fromHex(jsondata.value("license").toString().toUtf8());
                    QJsonObject object;
                    dealData(replyCount,object);
                    QString str = QString("setex %1 %2 %3").arg(Utf8hexqstringToGbkhexqstring(jsondata.value("license").toString()))
                            .arg(m_baseInfo.loaclRedis.existTime * 60)
                            .arg(object.value("result").toInt());
                    redisReply *reply = (redisReply *)redisCommand(m_ctx,str.toStdString().c_str());
                    if(reply != NULL && reply->type == REDIS_REPLY_STATUS)
                    {
                        qDebug()<<"Redis insert Status:"<<reply->str;
                    }
                    freeReplyObject(reply);
                }
            }
        }
    }else{ //超时处理
        disconnect(m_NetManager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
        qDebug()<<"overTime"<<"--------------------"<<QThread::currentThreadId();
        //m_reply->abort();
        QString str = QString("setex %1 %2 %3").arg(Utf8hexqstringToGbkhexqstring(jsondata.value("license").toString()))
                .arg(m_baseInfo.loaclRedis.existTime * 60)
                .arg(-1);
        redisReply *reply = (redisReply *)redisCommand(m_ctx,str.toStdString().c_str());
        if(reply != NULL && reply->type == REDIS_REPLY_STATUS)
        {
            qDebug()<<"Redis insert Status:"<<reply->str;
        }
        freeReplyObject(reply);
    }
    setBusy(false);
    //DEL_OBJECT(m_reply)
}
bool httpClientHandle::isBusy()
{
    return m_isBusy;
}

void httpClientHandle::setBusy(bool isBusy)
{
    m_isBusy = isBusy;
    if (m_isBusy)
        m_dtBusyBeginTime = QDateTime::currentDateTime();
    else
        m_dtBusyEndTime = QDateTime::currentDateTime();
}

httpClientHandle::~httpClientHandle()
{
    redisFree(m_ctx);
    m_pThread->quit();
    m_pThread->wait();
    m_pThread->deleteLater();
}

quint64 httpClientHandle::busyTime()
{
    return m_dtBusyBeginTime.secsTo(QDateTime::currentDateTime());
}

quint64 httpClientHandle::idleTime()
{
    return m_dtBusyEndTime.secsTo(QDateTime::currentDateTime());
}

