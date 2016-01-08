/****************************************************************************
** Meta object code from reading C++ file 'netservice.hpp'
**
** Created: Fri Jan 23 14:44:18 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../IntelliHouse_main/netservice.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'netservice.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetService[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      31,   11,   11,   11, 0x0a,
      48,   43,   11,   11, 0x0a,
      74,   71,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetService[] = {
    "NetService\0\0acceptConnection()\0"
    "startRead()\0data\0startWrite(QByteArray)\0"
    "ba\0serviceDispatch(QByteArray)\0"
};

const QMetaObject NetService::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetService,
      qt_meta_data_NetService, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetService::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetService::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetService))
        return static_cast<void*>(const_cast< NetService*>(this));
    if (!strcmp(_clname, "AbstractNetService"))
        return static_cast< AbstractNetService*>(const_cast< NetService*>(this));
    return QObject::qt_metacast(_clname);
}

int NetService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: acceptConnection(); break;
        case 1: startRead(); break;
        case 2: startWrite((*reinterpret_cast< const QByteArray(*)>(_a[1]))); break;
        case 3: serviceDispatch((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
