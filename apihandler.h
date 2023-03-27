#ifndef APIHANDLER_H
#define APIHANDLER_H

#include <QObject>
#include <QThread>
#include <QList>
#include <QDateTime>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include "HTTPServer/utilities.h"
#include "HTTPServer/requesthandler.h"
#include "icreadergui.h"
#include "logrecord/logrecord.h"
#include "Define.h"
class apihandler : public QObject, public enable_singleton<apihandler>
{
    Q_OBJECT
public:
    apihandler();
    ~apihandler();

    /* 请求处理函数 */
    static int handler(const Http_Request_Info & req, Http_Respone_Info & reply);
private:
    /* 解析报文内容 */
    static bool analysisHttpBody(const QByteArray &pBody, QJsonObject &json);
    static void respone(int nSubCode, const QByteArray &data, const QString &apiType, const QString &apiName, Http_Respone_Info &reply);

    QThread m_pThread;

};

#endif // APIHANDLER_H
