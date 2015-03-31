#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "search/SearchNode.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    command_presenter(NULL),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//allocate the presenter
    command_presenter = new CommandPresenter(this, ui->mdiAreaEditor);


    ui->dockCommand->setStyleSheet("::title { position: relative;"
                                   "          text-align: center }");
    ui->dockCommandSpecific->setStyleSheet("::title { position: relative;"
                                   "          text-align: center }");
    ui->dockCommand->setWidget(new SearchNode(this, command_presenter));
    //tabifyDockWidget(ui->dockCommand,
    //                 ui->dockCommandSpecific);

}

MainWindow::~MainWindow()
{
    if(command_presenter) delete(command_presenter);
    delete ui;
}
