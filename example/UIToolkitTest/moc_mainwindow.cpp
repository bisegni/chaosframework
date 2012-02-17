/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created: Sun Feb 12 13:52:55 2012
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "mainwindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x0a,
      49,   11,   11,   11, 0x0a,
      70,   62,   11,   11, 0x08,
     106,   11,   11,   11, 0x08,
     147,  137,   11,   11, 0x08,
     182,   11,   11,   11, 0x08,
     224,  218,   11,   11, 0x08,
     263,  218,   11,   11, 0x08,
     303,   11,   11,   11, 0x08,
     327,   11,   11,   11, 0x08,
     353,   11,   11,   11, 0x08,
     378,   11,   11,   11, 0x08,
     402,   11,   11,   11, 0x08,
     435,   11,   11,   11, 0x08,
     473,  467,   11,   11, 0x08,
     509,  467,   11,   11, 0x08,
     554,  549,   11,   11, 0x08,
     594,  549,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0pos\0showContextMenuForWidget(QPoint)\0"
    "updatePlot()\0checked\0"
    "on_buttonDeleteDevice_clicked(bool)\0"
    "on_actionShow_Info_triggered()\0pos,index\0"
    "on_splitter_splitterMoved(int,int)\0"
    "on_buttonUpdateDeviceList_clicked()\0"
    "index\0on_listView_doubleClicked(QModelIndex)\0"
    "on_tableView_doubleClicked(QModelIndex)\0"
    "on_buttonInit_clicked()\0"
    "on_buttonDeinit_clicked()\0"
    "on_buttonStart_clicked()\0"
    "on_buttonStop_clicked()\0"
    "on_buttonStartTracking_clicked()\0"
    "on_buttonStopTracking_clicked()\0value\0"
    "on_dialTrackSpeed_valueChanged(int)\0"
    "on_dialScheduleDevice_valueChanged(int)\0"
    "arg1\0on_spinDeviceSchedule_valueChanged(int)\0"
    "on_spinTrackSpeed_valueChanged(int)\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    if (!strcmp(_clname, "chaos::CThreadExecutionTask"))
        return static_cast< chaos::CThreadExecutionTask*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showContextMenuForWidget((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 1: updatePlot(); break;
        case 2: on_buttonDeleteDevice_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: on_actionShow_Info_triggered(); break;
        case 4: on_splitter_splitterMoved((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 5: on_buttonUpdateDeviceList_clicked(); break;
        case 6: on_listView_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: on_tableView_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 8: on_buttonInit_clicked(); break;
        case 9: on_buttonDeinit_clicked(); break;
        case 10: on_buttonStart_clicked(); break;
        case 11: on_buttonStop_clicked(); break;
        case 12: on_buttonStartTracking_clicked(); break;
        case 13: on_buttonStopTracking_clicked(); break;
        case 14: on_dialTrackSpeed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: on_dialScheduleDevice_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: on_spinDeviceSchedule_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: on_spinTrackSpeed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 18;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
