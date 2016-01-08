/****************************************************************************
** Meta object code from reading C++ file 'picturebutton.hpp'
**
** Created: Fri Jan 23 10:47:29 2015
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../IntelliHouse_main/picturebutton.hpp"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'picturebutton.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PictureButton[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      25,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PictureButton[] = {
    "PictureButton\0\0enabled()\0disabled()\0"
};

const QMetaObject PictureButton::staticMetaObject = {
    { &QAbstractButton::staticMetaObject, qt_meta_stringdata_PictureButton,
      qt_meta_data_PictureButton, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PictureButton::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PictureButton::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PictureButton::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PictureButton))
        return static_cast<void*>(const_cast< PictureButton*>(this));
    return QAbstractButton::qt_metacast(_clname);
}

int PictureButton::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QAbstractButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: enabled(); break;
        case 1: disabled(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
