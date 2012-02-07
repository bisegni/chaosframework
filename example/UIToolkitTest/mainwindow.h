#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSettings>
#include <QMainWindow>
#include <graphwidget.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QSettings settings;
    GraphWidget *graphWdg;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void showContextMenuForWidget(const QPoint &pos);

private slots:

    void on_buttonDeleteDevice_clicked(bool checked);

    void on_actionShow_Info_triggered();

    void on_splitter_splitterMoved(int pos, int index);

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
    void readSettings();

};

#endif // MAINWINDOW_H
