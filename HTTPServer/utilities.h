#ifndef UTILITIES_H
#define UTILITIES_H

#include <QObject>
#include <QThread>
#include <functional>
#include <QElapsedTimer>
#include "logrecord/logrecord.h"
template <typename T>
class enable_singleton
{
protected:
    enable_singleton() {}
public:
    static T &getInstance() { static T obj; return obj; }
};

//=====================================================================================

class CRunSingleFunc: public QThread
{
    Q_OBJECT
public:
    explicit CRunSingleFunc();
    ~CRunSingleFunc();
    void init(std::function<void()> func, QString threadName = "", quint32 timeout = 1000);
protected:
    void run();
private:
    quint32 m_intervalUsecs;    //单位微秒
    QString m_threadName;
    std::function<void()> m_func;
};

#define LAMBDA_PACKING(callback) [&](){callback;}

//=====================================================================================

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> onExitScope) : onExitScope_(onExitScope), dismissed_(false) {}

    ~ScopeGuard() { if (!dismissed_) {onExitScope_();} }

    void Dismiss() { dismissed_ = true; }

private:
    std::function<void()> onExitScope_;
    bool dismissed_;

private:
    ScopeGuard(ScopeGuard const &);
    ScopeGuard &operator=(ScopeGuard const &);
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback);
#define defer(callback) ON_SCOPE_EXIT([&](){callback;})

//=====================================================================================

class CTimeHelper : public enable_singleton<CTimeHelper>
{
public:
    CTimeHelper();
    qint64 getTimePoint();
    void reset();
private:
    QElapsedTimer m_ElapsedTimer;
};

#endif // UTILITIES_H
