#ifndef HTTPREQUESTHANDLER_H
#define HTTPREQUESTHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QThread>

class httpRequestHandler : public QObject
{
    Q_OBJECT
public:
    explicit httpRequestHandler(QObject *parent = 0);
    ~httpRequestHandler();


    QNetworkAccessManager *m_manager;
    QNetworkReply *reply;
    QNetworkRequest * m_request;

signals:

public slots:
};

#endif // HTTPREQUESTHANDLER_H
