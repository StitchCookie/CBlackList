#include "mainmutual.h"
#include "HTTPClient/httpclient.h"
#include "HTTPServer/listen.h"
#include "apihandler.h"
#include <QThread>
MainMutual::MainMutual(QObject *parent) : QObject(parent)
{
    qDebug()<<"当前主线程地址:"<< QThread::currentThreadId();


}
MainMutual::~MainMutual()
{

}

