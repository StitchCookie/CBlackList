#include "httpclient.h"
#include "pub/systemconfig.h"
#include <QDebug>

HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    m_pool = new httpClientHandlePool;

}

HttpClient::~HttpClient()
{
    DEL_OBJECT(m_pool)
}


//
void HttpClient::incoming_MysqlData(QByteArray lineGrantryCarData)
{
    qDebug()<<"收到mysql扫描的门架数据 开始处理"<<"当前线程id"<<QThread::currentThreadId();
    httpClientHandle *handle = nullptr;
    if(m_pool)
    {
        handle = m_pool->getHttpClientHandle();
    }
    if(handle)
    {
        QMetaObject::invokeMethod(handle,"dealPost",Qt::QueuedConnection,Q_ARG(QByteArray,lineGrantryCarData));
    }else{

        qDebug()<<"请求数过多 无法分配空余线程处理";
    }
}

