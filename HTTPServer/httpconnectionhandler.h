#ifndef HTTPCONNECTIONHANDLER_H
#define HTTPCONNECTIONHANDLER_H

#include <QTcpSocket>
#include <QTimer>
#include <QThread>
#include "httprequest.h"
#include "requesthandler.h"

/* 实例释放 */
#define DEL_INSTANCE(INSTANCE) {if(INSTANCE) delete INSTANCE; INSTANCE = nullptr;}

/* 适配QT不同版本 */
#if QT_VERSION >= 0x050000
    typedef qintptr tSocketDescriptor;
#else
    typedef int tSocketDescriptor;
#endif

/* 超时时间 */
#define READ_TIMEOUT 5000
#define WAIT_NEXT_REQUEST_TIMEOUT 2000


class HttpConnectionHandler : public QObject
{
    Q_OBJECT
public:
    HttpConnectionHandler(RequestHandler* requestHandler);
    virtual ~HttpConnectionHandler();

    /* 设置线程状态: 忙碌/空闲 */
    bool isBusy();
    void setBusy(bool isBusy = true);
    /* 获取线程忙碌时间(超过一定阈值,很可能会出现异常,需要释放连接) */
    qint64 busyTime();
    qint64 idleTime();

private:
    /* 读取请求消息 */
	void readFromSocket();
    /* 回复响应消息 */
    void response();
    /* 断开连接 */
	void disconnectSocket();
    /* 判断是否需要关闭客户端 */
	bool isCloseConnection();

private:
    /* 客户端 */
    QTcpSocket* m_pSocket;

    /* 工作线程 */
    QThread* m_pThread;

    /* 数据读取超时,同时也用作保持长连接 */
    QTimer m_Timer;

    /* 存储请求数据 */
    HttpRequest* m_pCurrentRequest;

    /* 处理请求数据 */
    RequestHandler* m_pRequestHandler;

    /* 忙碌状态 */
    bool m_isBusy;
    QDateTime m_dtBusyBeginTime;
    QDateTime m_dtBusyEndTime;

    /* 客户端是否需要释放 */
    bool m_isCloseConnection;

    /* 客户端保持长连接 */
    bool m_isWaitingNextRequest;

public slots:
    /* 客户端主动断开连接 */
    void slt_handleConnection(const tSocketDescriptor socketDescriptor);

private slots:
    /* TcpSocket读取内容 */
    void slt_read();
    /* TcpSocket断开连接 */
    void slt_disconnected();
    /* QTimer定时器超时 */
    void slt_timeout();
    /* 实例释放 */
    void slt_threadFinished();
};

#endif // HTTPCONNECTIONHANDLER_H
