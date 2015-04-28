#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "MainController.h"
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
    MainController *main_controller;
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpenNode_triggered();

    void on_actionSearch_Node_triggered();

    void on_actionData_Services_triggered();

    void on_actionPreferences_triggered();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
