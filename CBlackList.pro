QT -= gui
QT += core gui sql network

CONFIG += c++11
CONFIG -= app_bundle
CONFIG +=debug_and_release
CONFIG(debug,debug|release){
    TARGET = BlackListd
}else{
    TARGET = BlackList
}
# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        HTTPServer/httpconnectionhandler.cpp \
        HTTPServer/httpconnectionhandlerpool.cpp \
        HTTPServer/httprequest.cpp \
        HTTPServer/httpresponse.cpp \
        HTTPServer/httpserver.cpp \
        HTTPServer/listen.cpp \
        HTTPServer/requesthandler.cpp \
        HTTPServer/utilities.cpp \
        apihandler.cpp \
        icreadergui.cpp \
        logrecord/logrecord.cpp \
        main.cpp \
        pub/systemconfig.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    HTTPServer/httpserver.pri

HEADERS += \
    HTTPServer/httpconnectionhandler.h \
    HTTPServer/httpconnectionhandlerpool.h \
    HTTPServer/httprequest.h \
    HTTPServer/httpresponse.h \
    HTTPServer/httpserver.h \
    HTTPServer/listen.h \
    HTTPServer/requesthandler.h \
    HTTPServer/utilities.h \
    apihandler.h \
    icreadergui.h \
    logrecord/logrecord.h \
    pub/systemconfig.h
