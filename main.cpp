#include <QCoreApplication>
#include <HTTPServer/listen.h>
#include "apihandler.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    HttpStartListen::getInstance().start(6666,apihandler::getInstance().handler);
    return a.exec();
}
