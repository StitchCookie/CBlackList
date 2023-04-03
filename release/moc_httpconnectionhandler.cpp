/****************************************************************************
** Meta object code from reading C++ file 'httpconnectionhandler.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../HTTPServer/httpconnectionhandler.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'httpconnectionhandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_HttpConnectionHandler_t {
    QByteArrayData data[9];
    char stringdata[136];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_HttpConnectionHandler_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_HttpConnectionHandler_t qt_meta_stringdata_HttpConnectionHandler = {
    {
QT_MOC_LITERAL(0, 0, 21), // "HttpConnectionHandler"
QT_MOC_LITERAL(1, 22, 20), // "slt_handleConnection"
QT_MOC_LITERAL(2, 43, 0), // ""
QT_MOC_LITERAL(3, 44, 17), // "tSocketDescriptor"
QT_MOC_LITERAL(4, 62, 16), // "socketDescriptor"
QT_MOC_LITERAL(5, 79, 8), // "slt_read"
QT_MOC_LITERAL(6, 88, 16), // "slt_disconnected"
QT_MOC_LITERAL(7, 105, 11), // "slt_timeout"
QT_MOC_LITERAL(8, 117, 18) // "slt_threadFinished"

    },
    "HttpConnectionHandler\0slt_handleConnection\0"
    "\0tSocketDescriptor\0socketDescriptor\0"
    "slt_read\0slt_disconnected\0slt_timeout\0"
    "slt_threadFinished"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_HttpConnectionHandler[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x0a /* Public */,
       5,    0,   42,    2, 0x08 /* Private */,
       6,    0,   43,    2, 0x08 /* Private */,
       7,    0,   44,    2, 0x08 /* Private */,
       8,    0,   45,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void, 0x80000000 | 3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void HttpConnectionHandler::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        HttpConnectionHandler *_t = static_cast<HttpConnectionHandler *>(_o);
        switch (_id) {
        case 0: _t->slt_handleConnection((*reinterpret_cast< const tSocketDescriptor(*)>(_a[1]))); break;
        case 1: _t->slt_read(); break;
        case 2: _t->slt_disconnected(); break;
        case 3: _t->slt_timeout(); break;
        case 4: _t->slt_threadFinished(); break;
        default: ;
        }
    }
}

const QMetaObject HttpConnectionHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_HttpConnectionHandler.data,
      qt_meta_data_HttpConnectionHandler,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *HttpConnectionHandler::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *HttpConnectionHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_HttpConnectionHandler.stringdata))
        return static_cast<void*>(const_cast< HttpConnectionHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int HttpConnectionHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
