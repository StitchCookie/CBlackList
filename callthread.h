#ifndef CALLTHREAD_H
#define CALLTHREAD_H

#include <QObject>

class callThread : public QObject
{
    Q_OBJECT
public:
    explicit callThread(QObject *parent = 0);
    ~callThread();

signals:

public slots:
};

#endif // CALLTHREAD_H
