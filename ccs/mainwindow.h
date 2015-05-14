#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "monitor/healt/HealtMonitorWidget.h"
#include "presenter/CommandPresenter.h"

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

private slots:
    void on_actionOpenNode_triggered();

    void on_actionSearch_Node_triggered();

    void on_actionData_Services_triggered();

    void on_actionPreferences_triggered();
    void on_actionShow_Monitor_View_triggered();

public slots:
    void reconfigure();
private:
    HealtMonitorWidget *healt_widget;
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
