/****************************************************************************
** Meta object code from reading C++ file 'paintwindow.hpp'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.4.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "paintwindow.hpp"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'paintwindow.hpp' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.4.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_PaintWindow_t {
    QByteArrayData data[13];
    char stringdata[118];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_PaintWindow_t, stringdata) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_PaintWindow_t qt_meta_stringdata_PaintWindow = {
    {
QT_MOC_LITERAL(0, 0, 11), // "PaintWindow"
QT_MOC_LITERAL(1, 12, 9), // "help_line"
QT_MOC_LITERAL(2, 22, 0), // ""
QT_MOC_LITERAL(3, 23, 14), // "help_rectangle"
QT_MOC_LITERAL(4, 38, 9), // "help_oval"
QT_MOC_LITERAL(5, 48, 8), // "set_line"
QT_MOC_LITERAL(6, 57, 8), // "set_rect"
QT_MOC_LITERAL(7, 66, 8), // "set_oval"
QT_MOC_LITERAL(8, 75, 9), // "set_black"
QT_MOC_LITERAL(9, 85, 7), // "set_red"
QT_MOC_LITERAL(10, 93, 9), // "set_green"
QT_MOC_LITERAL(11, 103, 8), // "set_blue"
QT_MOC_LITERAL(12, 112, 5) // "clear"

    },
    "PaintWindow\0help_line\0\0help_rectangle\0"
    "help_oval\0set_line\0set_rect\0set_oval\0"
    "set_black\0set_red\0set_green\0set_blue\0"
    "clear"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_PaintWindow[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   69,    2, 0x08 /* Private */,
       3,    0,   70,    2, 0x08 /* Private */,
       4,    0,   71,    2, 0x08 /* Private */,
       5,    0,   72,    2, 0x08 /* Private */,
       6,    0,   73,    2, 0x08 /* Private */,
       7,    0,   74,    2, 0x08 /* Private */,
       8,    0,   75,    2, 0x08 /* Private */,
       9,    0,   76,    2, 0x08 /* Private */,
      10,    0,   77,    2, 0x08 /* Private */,
      11,    0,   78,    2, 0x08 /* Private */,
      12,    0,   79,    2, 0x08 /* Private */,

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

       0        // eod
};

void PaintWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        PaintWindow *_t = static_cast<PaintWindow *>(_o);
        switch (_id) {
        case 0: _t->help_line(); break;
        case 1: _t->help_rectangle(); break;
        case 2: _t->help_oval(); break;
        case 3: _t->set_line(); break;
        case 4: _t->set_rect(); break;
        case 5: _t->set_oval(); break;
        case 6: _t->set_black(); break;
        case 7: _t->set_red(); break;
        case 8: _t->set_green(); break;
        case 9: _t->set_blue(); break;
        case 10: _t->clear(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject PaintWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_PaintWindow.data,
      qt_meta_data_PaintWindow,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *PaintWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *PaintWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_PaintWindow.stringdata))
        return static_cast<void*>(const_cast< PaintWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int PaintWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
