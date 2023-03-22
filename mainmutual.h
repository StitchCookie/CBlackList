#ifndef MAINMUTUAL_H
#define MAINMUTUAL_H

#include <QObject>
#include "sql/cblacklistsql.h"
class MainMutual : public QObject
{
    Q_OBJECT
public:
    explicit MainMutual(QObject *parent = 0);
    ~MainMutual();

signals:

public slots:
};

#endif // MAINMUTUAL_H
