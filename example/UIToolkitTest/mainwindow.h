/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <graphwidget.h>
#include <QModelIndex>

#ifndef Q_MOC_RUN
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#endif

#include <qwt_system_clock.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSettings settings;
    GraphWidget *graphWdg;
    chaos::common::network::MDSMessageChannel *mdsChannel;
    chaos::ui::DeviceController *deviceController;
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
    void cleanLastDevice();
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

    void startTracking();

    void stopTracking();

    void on_dialTrackSpeed_valueChanged(int value);

    void on_dialScheduleDevice_valueChanged(int value);

    void on_spinDeviceSchedule_valueChanged(int arg1);

    void on_spinTrackSpeed_valueChanged(int arg1);

    void on_pushButton_clicked();
    void timerEvent(QTimerEvent *event);
    void on_spinBox_valueChanged(int arg1);

    void on_lineEdit_returnPressed();

    void on_pushButtonResetStatistic_clicked();

    void on_spinBoxMaxYGrid_valueChanged(int arg1);

    void on_spinBoxMinYGrid_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QString returnAttributeTypeInString(string& attributeName);
    void closeEvent(QCloseEvent *event);
    void readSettings();
    void cleanCurrentDevice();

};

#endif // MAINWINDOW_H
