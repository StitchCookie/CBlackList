#include "httpconnectionhandlerpool.h"

using namespace httpConnectionHandlerPool;

HttpConnectionHandlerPool::HttpConnectionHandlerPool(RequestHandler *requestHandler)
    : QObject()
    , m_pRequestHandler(requestHandler)
{
    connect(&m_cleanupTimer, SIGNAL(timeout()), SLOT(slt_cleanup()));
    m_cleanupTimer.start(1000);
    vLogDebug("HttpConnectionHandlerPool (%p): 创建", this);
}

HttpConnectionHandlerPool::~HttpConnectionHandlerPool()
{
    foreach(HttpConnectionHandler* handler, m_pool)
        DEL_INSTANCE(handler)
                vLogDebug("HttpConnectionHandlerPool (%p): 销毁", this);
}

HttpConnectionHandler* HttpConnectionHandlerPool::getConnectionHandler()
{
    HttpConnectionHandler* pFreeHandler = nullptr;
    m_Mutex.lock();
    try
    {
        // 从线程池中找一个空闲的
        foreach(HttpConnectionHandler* handler, m_pool)
        {
            if (handler && !handler->isBusy())
            {
                pFreeHandler = handler;
                pFreeHandler->setBusy(true);
                break;
            }
        }
        // 找不到则创建
        if (!pFreeHandler)
        {
            if (m_pool.count() < MAX_CONNECT_THREAD)
            {
                pFreeHandler = new HttpConnectionHandler(m_pRequestHandler);
                pFreeHandler->setBusy();
                m_pool.append(pFreeHandler);
            }
            else
            {
                // vLogError("HttpConnectionHandlerPool: get free connection handler fail!");
            }
        }
    }
    catch(std::exception& e)
    {
        vLogFatal("HttpConnectionHandlerPool: get ConnectionHandler exception:%s.", e.what());
        pFreeHandler = nullptr;
    }
    catch(...)
    {
        vLogFatal("HttpConnectionHandlerPool: get ConnectionHandler exception:unkonwn.");
        pFreeHandler = nullptr;
    }

    m_Mutex.unlock();
    return pFreeHandler;
}
/*每秒钟对空闲的hander进行移除*/
void HttpConnectionHandlerPool::slt_cleanup()
{
    m_Mutex.lock();
    try
    {
        foreach(HttpConnectionHandler* handler, m_pool)
        {
            if (nullptr == handler)
            {
                m_pool.removeOne(handler);
                vLogFatal("HttpConnectionHandlerPool: 移除空handler, 当前剩余HttpConnectionHandler数：%d.", m_pool.size());
                break;
            }

            if (!handler->isBusy())
            {
                //在数据量小的情况下，队列尾的元素很可能长期处于空闲状态，可以适当销毁
                if ((m_pool.size() > MIN_CONNECT_THREAD) && (handler->idleTime() > CONNECT_IDLE_TIMEOUT))
                {
                    m_pool.removeOne(handler);
                    vLogDebug("HttpConnectionHandlerPool: 移除HttpConnectionHandler (%p), 当前剩余HttpConnectionHandler数：%d.",
                              handler, m_pool.size());
                    delete handler;
                    break;
                }
            }
            else if (handler->busyTime() > CONNECT_BUSY_TIMEOUT)
            {
                int nTime = static_cast<int>(handler->busyTime());
                m_pool.removeOne(handler);
                vLogFatal("HttpConnectionHandlerPool: HttpConnectionHandler (%p)处理超时[%d秒], 强制移除, 当前剩余HttpConnectionHandler数：%d.",
                          handler, nTime, m_pool.size());
                delete handler;
                break;
            }
        }
    }
    catch(std::exception& e)
    {
        vLogFatal("HttpConnectionHandlerPool: clean up ConnectionHandler exception occurred:%s.", e.what());
    }
    catch(...)
    {
        vLogFatal("HttpConnectionHandlerPool: clean up ConnectionHandler exception occurred:unkonwn.");
    }
    m_Mutex.unlock();
}
