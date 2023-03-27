#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H
// **********************************************************
// projectName: Httpclient
// description: 本类似于SOCKET中的readReady时触发
//
// **********************************************************
#include <QObject>

#include <pub/systemconfig.h>
#include "httpclienthandle.h"
#include "httpclienthandlepool.h"
#include <QThread>
class HttpClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpClient(QObject *parent = 0);
    ~HttpClient();

signals:

public slots:
   virtual void incoming_MysqlData(QByteArray lineGrantryCarData);
private:

    httpClientHandlePool* m_pool;
};

#endif // HTTPCLIENT_H
