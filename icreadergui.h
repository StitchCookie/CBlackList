#ifndef ICREADERGUI_H
#define ICREADERGUI_H

#include <QDebug>
#include <QDateTime>
#include <HTTPServer/listen.h>
#include <apihandler.h>
#include "Define.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include <QString>
#include <QVariantMap>
#include <QDateTime>
#include <QMutex>
#include <QDebug>

#include <QJsonObject>
#include <QJsonDocument>
QT_BEGIN_NAMESPACE

class ToolIcreader : public QObject, public enable_singleton<ToolIcreader>
{
    Q_OBJECT

public:
    explicit ToolIcreader(QObject *parent = nullptr);
    ~ToolIcreader();
    /* API接口 */
    int IC_API(const QString &APi_Name, const   QString &json, QByteArray& baRes);

    /*utf-8转GBK编码*/
    QString HexQString_to_QString(const QString &text);
    /*去掉尾部与编码无关的占位字符0*/
    QString removeZeroChar(QString text);
private:
    /* 工作线程 */
    QThread m_pThread;
    /* 互斥锁 */
    QMutex m_lock;
    QSqlDatabase DB;
    bool m_redisStatus;
    static int count ;
private:
    int m_nHandle;
};


#endif // ICREADERGUI_H
