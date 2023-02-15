/****************************************************************************
** Meta object code from reading C++ file 'PMusrStep.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../src/musredit_qt6/musrStep/PMusrStep.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PMusrStep.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PModSelect_t {
    uint offsetsAndSizes[18];
    char stringdata0[11];
    char stringdata1[6];
    char stringdata2[1];
    char stringdata3[10];
    char stringdata4[7];
    char stringdata5[7];
    char stringdata6[26];
    char stringdata7[10];
    char stringdata8[10];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PModSelect_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PModSelect_t qt_meta_stringdata_PModSelect = {
    {
        QT_MOC_LITERAL(0, 10),  // "PModSelect"
        QT_MOC_LITERAL(11, 5),  // "scale"
        QT_MOC_LITERAL(17, 0),  // ""
        QT_MOC_LITERAL(18, 9),  // "automatic"
        QT_MOC_LITERAL(28, 6),  // "factor"
        QT_MOC_LITERAL(35, 6),  // "absVal"
        QT_MOC_LITERAL(42, 25),  // "absoluteValueStateChanged"
        QT_MOC_LITERAL(68, 9),  // "scaleAuto"
        QT_MOC_LITERAL(78, 9)   // "getFactor"
    },
    "PModSelect",
    "scale",
    "",
    "automatic",
    "factor",
    "absVal",
    "absoluteValueStateChanged",
    "scaleAuto",
    "getFactor"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PModSelect[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: name, argc, parameters, tag, flags, initial metatype offsets
       1,    3,   38,    2, 0x06,    1 /* Public */,

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       6,    1,   45,    2, 0x08,    5 /* Private */,
       7,    0,   48,    2, 0x08,    7 /* Private */,
       8,    0,   49,    2, 0x08,    8 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Bool, QMetaType::Double, QMetaType::Bool,    3,    4,    5,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject PModSelect::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PModSelect.offsetsAndSizes,
    qt_meta_data_PModSelect,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PModSelect_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PModSelect, std::true_type>,
        // method 'scale'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'absoluteValueStateChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'scaleAuto'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'getFactor'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void PModSelect::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PModSelect *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->scale((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        case 1: _t->absoluteValueStateChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->scaleAuto(); break;
        case 3: _t->getFactor(); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (PModSelect::*)(bool , double , bool );
            if (_t _q_method = &PModSelect::scale; *reinterpret_cast<_t *>(_a[1]) == _q_method) {
                *result = 0;
                return;
            }
        }
    }
}

const QMetaObject *PModSelect::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PModSelect::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PModSelect.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PModSelect::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void PModSelect::scale(bool _t1, double _t2, bool _t3)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t3))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
namespace {
struct qt_meta_stringdata_PMusrStep_t {
    uint offsetsAndSizes[30];
    char stringdata0[10];
    char stringdata1[18];
    char stringdata2[1];
    char stringdata3[4];
    char stringdata4[7];
    char stringdata5[14];
    char stringdata6[9];
    char stringdata7[11];
    char stringdata8[11];
    char stringdata9[14];
    char stringdata10[12];
    char stringdata11[16];
    char stringdata12[10];
    char stringdata13[7];
    char stringdata14[7];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PMusrStep_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PMusrStep_t qt_meta_stringdata_PMusrStep = {
    {
        QT_MOC_LITERAL(0, 9),  // "PMusrStep"
        QT_MOC_LITERAL(10, 17),  // "handleCellChanged"
        QT_MOC_LITERAL(28, 0),  // ""
        QT_MOC_LITERAL(29, 3),  // "row"
        QT_MOC_LITERAL(33, 6),  // "column"
        QT_MOC_LITERAL(40, 13),  // "checkSpecific"
        QT_MOC_LITERAL(54, 8),  // "checkAll"
        QT_MOC_LITERAL(63, 10),  // "unCheckAll"
        QT_MOC_LITERAL(74, 10),  // "modifyAuto"
        QT_MOC_LITERAL(85, 13),  // "modifyChecked"
        QT_MOC_LITERAL(99, 11),  // "saveAndQuit"
        QT_MOC_LITERAL(111, 15),  // "handleModSelect"
        QT_MOC_LITERAL(127, 9),  // "automatic"
        QT_MOC_LITERAL(137, 6),  // "factor"
        QT_MOC_LITERAL(144, 6)   // "absVal"
    },
    "PMusrStep",
    "handleCellChanged",
    "",
    "row",
    "column",
    "checkSpecific",
    "checkAll",
    "unCheckAll",
    "modifyAuto",
    "modifyChecked",
    "saveAndQuit",
    "handleModSelect",
    "automatic",
    "factor",
    "absVal"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PMusrStep[] = {

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
       1,    2,   62,    2, 0x08,    1 /* Private */,
       5,    0,   67,    2, 0x08,    4 /* Private */,
       6,    0,   68,    2, 0x08,    5 /* Private */,
       7,    0,   69,    2, 0x08,    6 /* Private */,
       8,    0,   70,    2, 0x08,    7 /* Private */,
       9,    0,   71,    2, 0x08,    8 /* Private */,
      10,    0,   72,    2, 0x08,    9 /* Private */,
      11,    3,   73,    2, 0x08,   10 /* Private */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int, QMetaType::Int,    3,    4,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool, QMetaType::Double, QMetaType::Bool,   12,   13,   14,

       0        // eod
};

Q_CONSTINIT const QMetaObject PMusrStep::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PMusrStep.offsetsAndSizes,
    qt_meta_data_PMusrStep,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PMusrStep_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PMusrStep, std::true_type>,
        // method 'handleCellChanged'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'checkSpecific'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'checkAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'unCheckAll'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'modifyAuto'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'modifyChecked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'saveAndQuit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleModSelect'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        QtPrivate::TypeAndForceComplete<double, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>
    >,
    nullptr
} };

void PMusrStep::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PMusrStep *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->handleCellChanged((*reinterpret_cast< std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<int>>(_a[2]))); break;
        case 1: _t->checkSpecific(); break;
        case 2: _t->checkAll(); break;
        case 3: _t->unCheckAll(); break;
        case 4: _t->modifyAuto(); break;
        case 5: _t->modifyChecked(); break;
        case 6: _t->saveAndQuit(); break;
        case 7: _t->handleModSelect((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<double>>(_a[2])),(*reinterpret_cast< std::add_pointer_t<bool>>(_a[3]))); break;
        default: ;
        }
    }
}

const QMetaObject *PMusrStep::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PMusrStep::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PMusrStep.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PMusrStep::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
