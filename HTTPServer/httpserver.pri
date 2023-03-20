QT += network

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/httpconnectionhandlerpool.h \
    $$PWD/listen.h \
    $$PWD/requesthandler.h \
    $$PWD/httpserver.h \
    $$PWD/httpconnectionhandler.h \
    $$PWD/httprequest.h \
    $$PWD/httpresponse.h

contains( DEFINES, VLPR_DEV_TEST ){
    HEADERS +=
}

SOURCES += \
    $$PWD/httpconnectionhandlerpool.cpp \
    $$PWD/listen.cpp \
    $$PWD/requesthandler.cpp \
    $$PWD/httpserver.cpp \
    $$PWD/httpconnectionhandler.cpp \
    $$PWD/httprequest.cpp \
    $$PWD/httpresponse.cpp

contains( DEFINES, VLPR_DEV_TEST ){
    SOURCES +=
}
