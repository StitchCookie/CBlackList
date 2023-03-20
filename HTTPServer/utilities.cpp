#include "utilities.h"

CRunSingleFunc::CRunSingleFunc() : m_intervalUsecs(1000), m_func(nullptr)
{
}

CRunSingleFunc::~CRunSingleFunc()
{
    if (this->isRunning())
    {
        this->requestInterruption();
        this->quit();
        this->wait();
    }
}

void CRunSingleFunc::init(std::function<void ()> func, QString threadName, quint32 timeout)
{
    m_func = func;
    m_threadName = threadName;
    m_intervalUsecs = timeout;
}

void CRunSingleFunc::run()
{
    if (m_func == nullptr)
    {
        vLogWarn("CRunSingleFunc[%s]->m_func为空,启动失败", m_threadName.toUtf8().data());
        return ;
    }

    if (!m_threadName.isEmpty())
    {
        vLogInfo("%s已启动", m_threadName.toUtf8().data());
    }

    while (!this->isInterruptionRequested())
    {
        m_func();
        usleep(m_intervalUsecs);
    }

    if (!m_threadName.isEmpty())
    {
        vLogInfo("%s已停止", m_threadName.toUtf8().data());
    }
}


CTimeHelper::CTimeHelper()
{
    reset();
}

qint64 CTimeHelper::getTimePoint()
{
    return m_ElapsedTimer.elapsed();
}

void CTimeHelper::reset()
{
    m_ElapsedTimer.restart();
}
