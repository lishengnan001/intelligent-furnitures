/****************************************************************************
** Meta object code from reading C++ file 'coorimpl.hpp'
**
** Created: Fri Jan 23 10:47:33 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../IntelliHouse_main/coorimpl.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'coorimpl.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CoorImpl[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,
      22,    9,    9,    9, 0x05,
      37,    9,    9,    9, 0x05,
      48,    9,    9,    9, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,    9,    9,    9, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CoorImpl[] = {
    "CoorImpl\0\0connected()\0disconnected()\0"
    "activate()\0deactivate()\0onActivate()\0"
};

const QMetaObject CoorImpl::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_CoorImpl,
      qt_meta_data_CoorImpl, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CoorImpl::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CoorImpl::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CoorImpl::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CoorImpl))
        return static_cast<void*>(const_cast< CoorImpl*>(this));
    if (!strcmp(_clname, "Coordinator"))
        return static_cast< Coordinator*>(const_cast< CoorImpl*>(this));
    return QObject::qt_metacast(_clname);
}

int CoorImpl::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: connected(); break;
        case 1: disconnected(); break;
        case 2: activate(); break;
        case 3: deactivate(); break;
        case 4: onActivate(); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void CoorImpl::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void CoorImpl::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void CoorImpl::activate()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void CoorImpl::deactivate()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}
QT_END_MOC_NAMESPACE
