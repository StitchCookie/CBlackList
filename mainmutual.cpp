#include "mainmutual.h"

MainMutual::MainMutual(QObject *parent) : QObject(parent)
{
    CBlackListSql *sqlDatabase = new CBlackListSql;
}

MainMutual::~MainMutual()
{

}

