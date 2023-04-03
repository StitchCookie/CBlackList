#include "listen.h"

HttpStartListen::HttpStartListen()
    : QObject()
{
    connect(&m_pThread, SIGNAL(started()), this, SLOT(slt_init()));
    connect(&m_pThread, SIGNAL(finished()), this, SLOT(slt_finish()));
    this->moveToThread(&m_pThread);
    qDebug() << "http 服务端构造成功";

}

HttpStartListen::~HttpStartListen()
{
    m_pThread.quit();
    m_pThread.wait();
    m_pThread.deleteLater();
}
/**
 * @brief HttpStartListen::start
 * @param port   监听端口
 * @param cbfun  回调函数 异步触发
 */
void HttpStartListen::start(int port, callback cbfun)
{
    if (m_pThread.isRunning())
        return;
    m_nPort = port;
    m_callback = cbfun;
    m_pThread.start();
}
/**
 * @brief HttpStartListen::slt_init
 * @note 开始进行监听 以及回调函数注册给实际处理的类
 */
void HttpStartListen::slt_init()
{
    m_pRequestHandler = new RequestHandler();
    m_pRequestHandler->m_callback = m_callback;
    m_pHttpListener = new HttpServer(m_nPort, m_pRequestHandler);
    while (true)
    {
        if (m_pHttpListener->startListen())
            break;
        QThread::sleep(3);
    }
}
/**
 * @brief HttpStartListen::slt_finish
 * @note 释放线程资源
 */
void HttpStartListen::slt_finish()
{
    DEL_INSTANCE(m_pHttpListener)
    DEL_INSTANCE(m_pRequestHandler)
}
