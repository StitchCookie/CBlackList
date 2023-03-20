#include "listen.h"

HttpStartListen::HttpStartListen()
    : QObject()
{
    connect(&m_pThread, SIGNAL(started()), this, SLOT(slt_init()));
    connect(&m_pThread, SIGNAL(finished()), this, SLOT(slt_finish()));
    this->moveToThread(&m_pThread);
    qDebug() << "http 构造成功 end";

}

HttpStartListen::~HttpStartListen()
{
    m_pThread.quit();
    m_pThread.wait();
    m_pThread.deleteLater();
}

void HttpStartListen::start(int port, callback cbfun)
{
    if (m_pThread.isRunning())
        return;
    m_nPort = port;
    m_callback = cbfun;
    m_pThread.start();
}

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

void HttpStartListen::slt_finish()
{
    DEL_INSTANCE(m_pHttpListener)
    DEL_INSTANCE(m_pRequestHandler)
}
