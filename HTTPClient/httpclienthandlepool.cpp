#include "httpclienthandlepool.h"

httpClientHandlePool::httpClientHandlePool(QObject *parent) : QObject(parent)
{
    connect(&m_cleanHanderTimer,SIGNAL(timeout()),this,SLOT(slt_cleanup()));
    m_cleanHanderTimer.start(1000);
    qDebug()<<"线程池地址:"<<QThread::currentThreadId();
}

httpClientHandle *httpClientHandlePool::getHttpClientHandle()
{
    qDebug()<<"start allot http thread";
    httpClientHandle* currentFreeHandler = nullptr;
    m_Mutex.lock();
    try
    {
        foreach (httpClientHandle *handle, m_httpclientPool) {
            if(handle && !handle->isBusy())
            {
                qDebug()<<"pool allot success address = "<<handle;
                currentFreeHandler = handle;
                currentFreeHandler->setBusy(true);
                break;
            }
        }
        if(!currentFreeHandler)
        {
            if(m_httpclientPool.count() < MAX_CONNECT_THREAD)
            {

                currentFreeHandler = new httpClientHandle();
                currentFreeHandler->setBusy(true);
                qDebug()<<"pool reallot success address = "<<currentFreeHandler;
                m_httpclientPool.append(currentFreeHandler);
            }
            else
            {
                qDebug()<<"线程池分配线程失败";
            }
        }
    }

    catch(std::exception& err)
    {
        currentFreeHandler = nullptr;
        qDebug()<<"捕获异常"<<err.what();
    }
    catch(...)
    {
        qDebug()<<"不可预期的错误";
    }
    m_Mutex.unlock();
    return currentFreeHandler;
}

httpClientHandlePool::~httpClientHandlePool()
{
    foreach(httpClientHandle* handle,m_httpclientPool)
        DEL_OBJECT(handle)
}

void httpClientHandlePool::slt_cleanup()
{
    m_Mutex.lock();
    foreach (httpClientHandle* handle,m_httpclientPool) {
        if(nullptr == handle)
        {
            m_httpclientPool.removeOne(handle);
            break;
        }
        if(!handle->isBusy())
        {
            //在数据量小的情况下，队列尾的元素很可能长期处于空闲状态，可以适当销毁
            if((m_httpclientPool.size() > MIN_CONNECT_THREAD) && (handle->idleTime() > CONNECT_IDLE_TIMEOUT))
            {
                qDebug()<<"release idle thread that too long time"<<handle->idleTime()<<"current size"<<m_httpclientPool.size();
                m_httpclientPool.removeOne(handle);
                DEL_OBJECT(handle);
                break;
            }
        }
        /*本程序暂不对超时线程进行销毁*/
//        else if(handle->busyTime() > CONNECT_BUSY_TIMEOUT)
    }
    m_Mutex.unlock();
}


