#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "presenter/CommandPresenter.h"
#include "monitor/healt/HealtMonitorWidget.h"
#include "api_async_processor/ApiAsyncProcessor.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    CommandPresenter *command_presenter;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void disposeResource();
private slots:
    void on_actionOpenNode_triggered();
    void on_actionSearch_Node_triggered();
    void on_actionData_Services_triggered();
    void on_actionPreferences_triggered();
    void on_actionShow_Monitor_View_triggered();
    void on_actionAdd_New_Unit_Server_triggered();
public slots:
    void reconfigure();
private slots:
    void asyncApiTimeout(const QString& tag);
    void asyncApiError(const QString& tag, QSharedPointer<chaos::CException> api_exception);
    void asyncApiResult(const QString& tag, QSharedPointer<chaos::common::data::CDataWrapper> api_data);

    void on_actionEnable_Monitoring_triggered();

    void on_actionDisable_Monitoring_triggered();

private:
    ApiAsyncProcessor api_processor;
    HealtMonitorWidget *healt_widget;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
