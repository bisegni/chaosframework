#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <graphwidget.h>
#include <QModelIndex>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/ui_toolkit/HighLevelApi/DeviceController.h>
#include <chaos/common/thread/ChaosThread.h>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, public chaos::CThreadExecutionTask
{
    Q_OBJECT
    QSettings settings;
    GraphWidget *graphWdg;
    chaos::MDSMessageChannel *mdsChannel;
    std::auto_ptr<chaos::ui::DeviceController> deviceController;
    chaos::CThread *trackThread;
protected:
    void executeOnThread(const std::string&) throw(chaos::CException);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showContextMenuForWidget(const QPoint &pos);
    void updatePlot();
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

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
    void readSettings();
    void cleanCurrentDevice();

};

#endif // MAINWINDOW_H
