#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <QObject>

class requestHandler : public QObject
{
    Q_OBJECT
public:
    explicit requestHandler(QObject *parent = 0);
    ~requestHandler();

signals:

public slots:
};

#endif // REQUESTHANDLER_H
