#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QtGui/QApplication>
#include <QMessageBox>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <cmath>
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), settings("it.infn", "CHAOS")
{
    QSettings settings;
    ui->setupUi(this);
    readSettings();

    QVBoxLayout *gL = new QVBoxLayout();
    graphWdg = new GraphWidget();
    gL->addWidget(graphWdg, 1);
    ui->graphWidget->setLayout(gL);

    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint &)), SLOT(showContextMenuForWidget(const QPoint &)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonDeleteDevice_clicked(bool checked)
{
    qDebug() << "received signal";
}

void MainWindow::on_actionShow_Info_triggered()
{
    qDebug() << "Show info";
    QMessageBox* msg = new QMessageBox(this);
    msg->setText("!CHAOS Technology Test!");
    msg->show();
}

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    settings.setValue("main_window/splitterSizes", ui->splitter->saveState());
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    settings.setValue("main_window", saveGeometry());
    settings.setValue("main_window", saveState());
    QMainWindow::closeEvent(event);
}
void MainWindow::readSettings()
{
    restoreGeometry(settings.value("main_window/geometry").toByteArray());
    restoreState(settings.value("main_window/windowState").toByteArray());
     ui->splitter->restoreState(settings.value("main_window/splitterSizes").toByteArray());
}

void MainWindow::showContextMenuForWidget(const QPoint &pos) {
    QMenu contextMenu(tr("Context menu"), this);
    contextMenu.addAction(new QAction(tr("&Hello"), this));
    contextMenu.exec(mapToGlobal(pos));
}
