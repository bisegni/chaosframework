#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "search/SearchNode.h"
#include "node/unit_server/UnitServerEditor.h"
#include "node/data_service/DataServiceEditor.h"
#include "search/SearchNodeResult.h"
#include "preference/PreferenceDialog.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QInputDialog>

using namespace chaos::metadata_service_client;

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

void MainWindow::on_actionOpenNode_triggered()
{
    //opena a specified node, knowing the unique identifier
    bool ok;
    QStringList node_types;
    node_types << tr("Unit Server");
    QString type =  QInputDialog::getItem(this,
                                         tr("Select node type"),
                                         tr("Type:"),
                                         node_types,
                                         0,
                                         false,
                                         &ok);
    if(ok) {
        QString node_uid = QInputDialog::getText(this,
                                                 tr("Open a node by unique ID"),
                                                 tr("Unique ID:"),
                                                 QLineEdit::Normal,
                                                 tr(""), &ok);
        if (ok && !node_uid.isEmpty()) {
            command_presenter->showCommandPresenter(new UnitServerEditor(node_uid));
        }
    }
}

void MainWindow::on_actionSearch_Node_triggered()
{
    command_presenter->showCommandPresenter(new SearchNodeResult());
}

void MainWindow::on_actionData_Services_triggered()
{
    command_presenter->showCommandPresenter(new DataServiceEditor());
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferenceDialog pref_dialog(this);
    //connect changed signal to reconfiguration slot of main controller
    connect(&pref_dialog,
            SIGNAL(changedConfiguration()),
            SLOT(reconfigure()));
    pref_dialog.exec();
}

void MainWindow::reconfigure() {
    ChaosMetadataServiceClient::getInstance()->clearServerList();
    QSettings settings;
    settings.beginGroup("network");
    int size = settings.beginReadArray("mds_address");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        ChaosMetadataServiceClient::getInstance()->addServerAddress(settings.value("address").toString().toStdString());
    }
    settings.endArray();
    settings.endGroup();
}
