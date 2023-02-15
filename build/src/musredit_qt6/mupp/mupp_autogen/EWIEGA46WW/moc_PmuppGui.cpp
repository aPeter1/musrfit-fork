/****************************************************************************
** Meta object code from reading C++ file 'PmuppGui.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../../../src/musredit_qt6/mupp/PmuppGui.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>
#include <QtCore/QList>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PmuppGui.h' doesn't include <QObject>."
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
struct qt_meta_stringdata_PVarErrorDialog_t {
    uint offsetsAndSizes[2];
    char stringdata0[16];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PVarErrorDialog_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PVarErrorDialog_t qt_meta_stringdata_PVarErrorDialog = {
    {
        QT_MOC_LITERAL(0, 15)   // "PVarErrorDialog"
    },
    "PVarErrorDialog"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PVarErrorDialog[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

Q_CONSTINIT const QMetaObject PVarErrorDialog::staticMetaObject = { {
    QMetaObject::SuperData::link<QDialog::staticMetaObject>(),
    qt_meta_stringdata_PVarErrorDialog.offsetsAndSizes,
    qt_meta_data_PVarErrorDialog,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PVarErrorDialog_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PVarErrorDialog, std::true_type>
    >,
    nullptr
} };

void PVarErrorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

const QMetaObject *PVarErrorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PVarErrorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PVarErrorDialog.stringdata0))
        return static_cast<void*>(this);
    return QDialog::qt_metacast(_clname);
}

int PVarErrorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    return _id;
}
namespace {
struct qt_meta_stringdata_PmuppGui_t {
    uint offsetsAndSizes[76];
    char stringdata0[9];
    char stringdata1[12];
    char stringdata2[1];
    char stringdata3[9];
    char stringdata4[15];
    char stringdata5[9];
    char stringdata6[20];
    char stringdata7[11];
    char stringdata8[7];
    char stringdata9[10];
    char stringdata10[9];
    char stringdata11[10];
    char stringdata12[12];
    char stringdata13[9];
    char stringdata14[5];
    char stringdata15[6];
    char stringdata16[5];
    char stringdata17[12];
    char stringdata18[11];
    char stringdata19[5];
    char stringdata20[8];
    char stringdata21[7];
    char stringdata22[8];
    char stringdata23[8];
    char stringdata24[14];
    char stringdata25[16];
    char stringdata26[11];
    char stringdata27[13];
    char stringdata28[17];
    char stringdata29[5];
    char stringdata30[12];
    char stringdata31[12];
    char stringdata32[9];
    char stringdata33[6];
    char stringdata34[7];
    char stringdata35[11];
    char stringdata36[4];
    char stringdata37[4];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_PmuppGui_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_PmuppGui_t qt_meta_stringdata_PmuppGui = {
    {
        QT_MOC_LITERAL(0, 8),  // "PmuppGui"
        QT_MOC_LITERAL(9, 11),  // "aboutToQuit"
        QT_MOC_LITERAL(21, 0),  // ""
        QT_MOC_LITERAL(22, 8),  // "fileOpen"
        QT_MOC_LITERAL(31, 14),  // "fileOpenRecent"
        QT_MOC_LITERAL(46, 8),  // "fileExit"
        QT_MOC_LITERAL(55, 19),  // "toolDumpCollections"
        QT_MOC_LITERAL(75, 10),  // "toolDumpXY"
        QT_MOC_LITERAL(86, 6),  // "addVar"
        QT_MOC_LITERAL(93, 9),  // "normalize"
        QT_MOC_LITERAL(103, 8),  // "helpCmds"
        QT_MOC_LITERAL(112, 9),  // "helpAbout"
        QT_MOC_LITERAL(122, 11),  // "helpAboutQt"
        QT_MOC_LITERAL(134, 8),  // "addDitto"
        QT_MOC_LITERAL(143, 4),  // "addX"
        QT_MOC_LITERAL(148, 5),  // "param"
        QT_MOC_LITERAL(154, 4),  // "addY"
        QT_MOC_LITERAL(159, 11),  // "createMacro"
        QT_MOC_LITERAL(171, 10),  // "handleCmds"
        QT_MOC_LITERAL(182, 4),  // "plot"
        QT_MOC_LITERAL(187, 7),  // "refresh"
        QT_MOC_LITERAL(195, 6),  // "remove"
        QT_MOC_LITERAL(202, 7),  // "removeX"
        QT_MOC_LITERAL(210, 7),  // "removeY"
        QT_MOC_LITERAL(218, 13),  // "handleNewData"
        QT_MOC_LITERAL(232, 15),  // "updateParamList"
        QT_MOC_LITERAL(248, 10),  // "currentRow"
        QT_MOC_LITERAL(259, 12),  // "editCollName"
        QT_MOC_LITERAL(272, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(289, 4),  // "item"
        QT_MOC_LITERAL(294, 11),  // "dropOnViewX"
        QT_MOC_LITERAL(306, 11),  // "dropOnViewY"
        QT_MOC_LITERAL(318, 8),  // "refreshY"
        QT_MOC_LITERAL(327, 5),  // "check"
        QT_MOC_LITERAL(333, 6),  // "varStr"
        QT_MOC_LITERAL(340, 10),  // "QList<int>"
        QT_MOC_LITERAL(351, 3),  // "idx"
        QT_MOC_LITERAL(355, 3)   // "add"
    },
    "PmuppGui",
    "aboutToQuit",
    "",
    "fileOpen",
    "fileOpenRecent",
    "fileExit",
    "toolDumpCollections",
    "toolDumpXY",
    "addVar",
    "normalize",
    "helpCmds",
    "helpAbout",
    "helpAboutQt",
    "addDitto",
    "addX",
    "param",
    "addY",
    "createMacro",
    "handleCmds",
    "plot",
    "refresh",
    "remove",
    "removeX",
    "removeY",
    "handleNewData",
    "updateParamList",
    "currentRow",
    "editCollName",
    "QListWidgetItem*",
    "item",
    "dropOnViewX",
    "dropOnViewY",
    "refreshY",
    "check",
    "varStr",
    "QList<int>",
    "idx",
    "add"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_PmuppGui[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,  212,    2, 0x0a,    1 /* Public */,
       3,    0,  213,    2, 0x0a,    2 /* Public */,
       4,    0,  214,    2, 0x0a,    3 /* Public */,
       5,    0,  215,    2, 0x0a,    4 /* Public */,
       6,    0,  216,    2, 0x0a,    5 /* Public */,
       7,    0,  217,    2, 0x0a,    6 /* Public */,
       8,    0,  218,    2, 0x0a,    7 /* Public */,
       9,    0,  219,    2, 0x0a,    8 /* Public */,
      10,    0,  220,    2, 0x0a,    9 /* Public */,
      11,    0,  221,    2, 0x0a,   10 /* Public */,
      12,    0,  222,    2, 0x0a,   11 /* Public */,
      13,    0,  223,    2, 0x08,   12 /* Private */,
      14,    1,  224,    2, 0x08,   13 /* Private */,
      14,    0,  227,    2, 0x28,   15 /* Private | MethodCloned */,
      16,    1,  228,    2, 0x08,   16 /* Private */,
      16,    0,  231,    2, 0x28,   18 /* Private | MethodCloned */,
      17,    0,  232,    2, 0x08,   19 /* Private */,
      18,    0,  233,    2, 0x08,   20 /* Private */,
      19,    0,  234,    2, 0x08,   21 /* Private */,
      20,    0,  235,    2, 0x08,   22 /* Private */,
      21,    0,  236,    2, 0x08,   23 /* Private */,
      22,    1,  237,    2, 0x08,   24 /* Private */,
      22,    0,  240,    2, 0x28,   26 /* Private | MethodCloned */,
      23,    1,  241,    2, 0x08,   27 /* Private */,
      23,    0,  244,    2, 0x28,   29 /* Private | MethodCloned */,
      24,    0,  245,    2, 0x08,   30 /* Private */,
      25,    1,  246,    2, 0x08,   31 /* Private */,
      27,    1,  249,    2, 0x08,   33 /* Private */,
      30,    1,  252,    2, 0x08,   35 /* Private */,
      31,    1,  255,    2, 0x08,   37 /* Private */,
      32,    0,  258,    2, 0x08,   39 /* Private */,
      33,    2,  259,    2, 0x08,   40 /* Private */,
      37,    2,  264,    2, 0x08,   43 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   15,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   26,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void, 0x80000000 | 28,   29,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 35,   34,   36,
    QMetaType::Void, QMetaType::QString, 0x80000000 | 35,   34,   36,

       0        // eod
};

Q_CONSTINIT const QMetaObject PmuppGui::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_PmuppGui.offsetsAndSizes,
    qt_meta_data_PmuppGui,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_PmuppGui_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PmuppGui, std::true_type>,
        // method 'aboutToQuit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fileOpen'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fileOpenRecent'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'fileExit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toolDumpCollections'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toolDumpXY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addVar'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'normalize'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'helpCmds'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'helpAbout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'helpAboutQt'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addDitto'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addX'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'addX'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'addY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'addY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'createMacro'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleCmds'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'plot'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'refresh'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'remove'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeX'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'removeX'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'removeY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        // method 'removeY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'handleNewData'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'updateParamList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<int, std::false_type>,
        // method 'editCollName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'dropOnViewX'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'dropOnViewY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'refreshY'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'check'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<int>, std::false_type>,
        // method 'add'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QString, std::false_type>,
        QtPrivate::TypeAndForceComplete<QVector<int>, std::false_type>
    >,
    nullptr
} };

void PmuppGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<PmuppGui *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->aboutToQuit(); break;
        case 1: _t->fileOpen(); break;
        case 2: _t->fileOpenRecent(); break;
        case 3: _t->fileExit(); break;
        case 4: _t->toolDumpCollections(); break;
        case 5: _t->toolDumpXY(); break;
        case 6: _t->addVar(); break;
        case 7: _t->normalize(); break;
        case 8: _t->helpCmds(); break;
        case 9: _t->helpAbout(); break;
        case 10: _t->helpAboutQt(); break;
        case 11: _t->addDitto(); break;
        case 12: _t->addX((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 13: _t->addX(); break;
        case 14: _t->addY((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 15: _t->addY(); break;
        case 16: _t->createMacro(); break;
        case 17: _t->handleCmds(); break;
        case 18: _t->plot(); break;
        case 19: _t->refresh(); break;
        case 20: _t->remove(); break;
        case 21: _t->removeX((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 22: _t->removeX(); break;
        case 23: _t->removeY((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        case 24: _t->removeY(); break;
        case 25: _t->handleNewData(); break;
        case 26: _t->updateParamList((*reinterpret_cast< std::add_pointer_t<int>>(_a[1]))); break;
        case 27: _t->editCollName((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 28: _t->dropOnViewX((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 29: _t->dropOnViewY((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 30: _t->refreshY(); break;
        case 31: _t->check((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[2]))); break;
        case 32: _t->add((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast< std::add_pointer_t<QList<int>>>(_a[2]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 31:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        case 32:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 1:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QList<int> >(); break;
            }
            break;
        }
    }
}

const QMetaObject *PmuppGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PmuppGui::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_PmuppGui.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int PmuppGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
