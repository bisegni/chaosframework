/*
 *	MainWindow.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <graphwidget.h>
#include <QModelIndex>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <chaos/common/thread/ChaosThread.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <qwt_system_clock.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSettings settings;
    GraphWidget *graphWdg;
    chaos::MDSMessageChannel *mdsChannel;
    boost::shared_ptr<chaos::ui::DeviceController> deviceController;
    chaos::CThread *trackThread;
    int d_timerId;
    boost::shared_ptr<boost::thread> schedThread;
    bool runThread;
    std::string checkSequentialIDKey;
    int64_t lostPack;
    int32_t lastID;
    int64_t oversampling;
protected:
    void executeOnThread();
    void updateDeviceState();
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showContextMenuForWidget(const QPoint &pos);
private slots:

    void on_buttonDeleteDevice_clicked(bool checked);

    void on_actionShow_Info_triggered();

    void on_splitter_splitterMoved(int pos, int index);

    void on_buttonUpdateDeviceList_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_tableView_doubleClicked(const QModelIndex &index);

    void on_buttonInit_clicked();

    void on_buttonDeinit_clicked();

    void on_buttonStart_clicked();

    void on_buttonStop_clicked();

    void on_buttonStartTracking_clicked();

    void on_buttonStopTracking_clicked();

    void on_dialTrackSpeed_valueChanged(int value);

    void on_dialScheduleDevice_valueChanged(int value);

    void on_spinDeviceSchedule_valueChanged(int arg1);

    void on_spinTrackSpeed_valueChanged(int arg1);

    void on_pushButton_clicked();
    void timerEvent(QTimerEvent *event);
    void on_spinBox_valueChanged(int arg1);

    void on_lineEdit_returnPressed();

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
    void readSettings();
    void cleanCurrentDevice();

};

#endif // MAINWINDOW_H
