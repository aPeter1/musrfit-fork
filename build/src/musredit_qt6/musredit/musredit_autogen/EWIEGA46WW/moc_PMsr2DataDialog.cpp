/****************************************************************************
** Meta object code from reading C++ file 'PMsr2DataDialog.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../src/musredit_qt6/musredit/PMsr2DataDialog.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PMsr2DataDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_PMsr2DataDialog_t {
    uint offsetsAndSizes[22];
    char stringdata0[16];
    char stringdata1[15];
    char stringdata2[1];
    char stringdata3[23];
    char stringdata4[19];
    char stringdata5[25];
    char stringdata6[15];
    char stringdata7[12];
    char stringdata8[16];
    char stringdata9[8];
    char stringdata10[20];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PMsr2DataDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PMsr2DataDialog_t qt_meta_stringdata_PMsr2DataDialog = {
    {
        QT_MOC_LITERAL(0, 15),  // "PMsr2DataDialog"
        QT_MOC_LITERAL(16, 14),  // "runListEntered"
        QT_MOC_LITERAL(31, 0),  // ""
        QT_MOC_LITERAL(32, 22),  // "runListFileNameEntered"
        QT_MOC_LITERAL(55, 18),  // "templateRunEntered"
        QT_MOC_LITERAL(74, 24),  // "createMsrFileOnlyChanged"
        QT_MOC_LITERAL(99, 14),  // "fitOnlyChanged"
        QT_MOC_LITERAL(114, 11),  // "helpContent"
        QT_MOC_LITERAL(126, 15),  // "globalOptionSet"
        QT_MOC_LITERAL(142, 7),  // "checked"
        QT_MOC_LITERAL(150, 19)   // "globalPlusOptionSet"
    },
    "PMsr2DataDialog",
    "runListEntered",
    "",
    "runListFileNameEntered",
    "templateRunEntered",
    "createMsrFileOnlyChanged",
    "fitOnlyChanged",
    "helpContent",
    "globalOptionSet",
    "checked",
    "globalPlusOptionSet"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PMsr2DataDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   62,    2, 0x0a,    1 /* Public */,
       3,    1,   65,    2, 0x0a,    3 /* Public */,
       4,    1,   68,    2, 0x0a,    5 /* Public */,
       5,    1,   71,    2, 0x0a,    7 /* Public */,
       6,    1,   74,    2, 0x0a,    9 /* Public */,
       7,    0,   77,    2, 0x0a,   11 /* Public */,
       8,    1,   78,    2, 0x08,   12 /* Private */,
      10,    1,   81,    2, 0x08,   14 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::QString,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    9,
    QMetaType::Void, QMetaType::Bool,    9,

       0        // eod
};

Q_CONSTINIT const QMetaObject PMsr2DataDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PMsr2DataDialog.offsetsAndSizes,
    qt_meta_data_PMsr2DataDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PMsr2DataDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PMsr2DataDialog, std::true_type>,
        // method 'runListEntered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'runListFileNameEntered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'templateRunEntered'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>,
        // method 'createMsrFileOnlyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'fitOnlyChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'helpContent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'globalOptionSet'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'globalPlusOptionSet'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void PMsr2DataDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PMsr2DataDialog *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->runListEntered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->runListFileNameEntered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->templateRunEntered((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->createMsrFileOnlyChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->fitOnlyChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->helpContent(); break;
        case 6: _t->globalOptionSet((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 7: _t->globalPlusOptionSet((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *PMsr2DataDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PMsr2DataDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PMsr2DataDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PMsr2DataDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 8)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 8;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
