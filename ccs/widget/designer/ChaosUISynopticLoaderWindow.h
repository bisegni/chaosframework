#ifndef CHAOSUISYNOPTICLOADERWINDOW_H
#define CHAOSUISYNOPTICLOADERWINDOW_H

#include <QMainWindow>

namespace Ui {
class ChaosUISynopticLoaderWindow;
}

class ChaosUISynopticLoaderWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ChaosUISynopticLoaderWindow(QWidget *parent = nullptr);
    ~ChaosUISynopticLoaderWindow();

private slots:
    void on_actionLoad_UI_File_triggered();

private:
    Ui::ChaosUISynopticLoaderWindow *ui;
    QWidget *loadUiFile(QWidget *parent, QString filePath);
};

#endif // CHAOSUISYNOPTICLOADERWINDOW_H
