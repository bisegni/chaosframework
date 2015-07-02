#include "NodeAttributePlotting.h"
#include "ui_NodeAttributePlotting.h"

#include <QMessageBox>

static const QString TAG_CU_GET_DATASET = "cu_get_dataset";

using namespace chaos::metadata_service_client::api_proxy;

NodeAttributePlotting::NodeAttributePlotting(const QString& _node_uid,
                                             QWidget *parent) :
    QWidget(parent),
    node_uid(_node_uid),
    ui(new Ui::NodeAttributePlotting) {
    ui->setupUi(this);
    if(parent == NULL) {
        setAttribute(Qt::WA_DeleteOnClose);
    }

    //set window title name
    setWindowTitle(QString("Plot viewer for:%1").arg(node_uid));

    ui->listViewPlottableAttribute->setModel(&list_model_dataset);

    ui->qCustomPlotTimed->setBackground(this->palette().background().color());
    //set the x axis as timestamp
    ui->qCustomPlotTimed->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->qCustomPlotTimed->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->qCustomPlotTimed->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    //set y axis as number
    ui->qCustomPlotTimed->yAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->qCustomPlotTimed->yAxis->setTickLabelFont(QFont(QFont().family(), 8));

    ui->listViewPlottableAttribute->setContextMenuPolicy(Qt::ActionsContextMenu);
    QAction *add_plot_action = new QAction("Add to plot", ui->listViewPlottableAttribute);
    connect(add_plot_action,
            SIGNAL(triggered()),
            SLOT(addGraphAction()));
    ui->listViewPlottableAttribute->addAction(add_plot_action);

    QAction *remove_plot_action = new QAction("Remove from plot", ui->listViewPlottableAttribute);
    connect(remove_plot_action,
            SIGNAL(triggered()),
            SLOT(removePlot()));
    ui->listViewPlottableAttribute->addAction(remove_plot_action);


    api_processor.submitApiResult(TAG_CU_GET_DATASET,
                                  GET_CHAOS_API_PTR(control_unit::GetCurrentDataset)->execute(node_uid.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));
}

NodeAttributePlotting::~NodeAttributePlotting() {
    delete ui;
}

void NodeAttributePlotting::addTimedGraphFor(const QString& attribute_name) {
    QSharedPointer<PlotInfo> plot_info(new PlotInfo());
    plot_info->attribute_name = attribute_name;
    plot_info->graph = ui->qCustomPlotTimed->addGraph();
    plot_info->graph->setLineStyle(QCPGraph::lsLine);
}

void NodeAttributePlotting::addGraphAction() {

}

void NodeAttributePlotting::removePlot() {

}

void NodeAttributePlotting::asyncApiResult(const QString& tag, QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    if(tag.compare(TAG_CU_GET_DATASET)==0) {
        //we have received the dataset of the control unit
        list_model_dataset.updateDataset(QSharedPointer<DatasetReader>(new DatasetReader(api_result)));
    }
}

void NodeAttributePlotting::asyncApiError(const QString&  tag, QSharedPointer<chaos::CException> api_exception) {
    QMessageBox::information(this, "Api call error", QString::fromStdString(api_exception->what()));
}

void NodeAttributePlotting::asyncApiTimeout(const QString& tag) {
    QMessageBox::information(this, "Api call timeout", QString("Timeout on tag:%1").arg(tag));
}

void NodeAttributePlotting::on_pushButtonStartMonitoring_clicked() {

}

void NodeAttributePlotting::on_pushButtonStopMonitoring_clicked() {

}
