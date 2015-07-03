#include "NodeAttributePlotting.h"
#include "ui_NodeAttributePlotting.h"

#include <QDebug>
#include <QMessageBox>

static const QString TAG_CU_GET_DATASET = "cu_get_dataset";

using namespace chaos::metadata_service_client;
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

    //plot setup
    ui->qCustomPlotTimed->setBackground(this->palette().background().color());
    //set the x axis as timestamp
    ui->qCustomPlotTimed->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    ui->qCustomPlotTimed->xAxis->setDateTimeFormat("hh:mm:ss");
    ui->qCustomPlotTimed->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->qCustomPlotTimed->xAxis->setAutoTickStep(true);
    //ui->qCustomPlotTimed->xAxis->setTickStep(2);
    //set y axis as number
    ui->qCustomPlotTimed->yAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->qCustomPlotTimed->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->qCustomPlotTimed->axisRect()->setupFullAxesBox();
    ui->qCustomPlotTimed->legend->setVisible(true);

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
    //remove unremoved plot
    foreach (QString key, map_plot_info.keys()) {
        removedTimedGraphFor(key);
    }
    delete ui;
}

void NodeAttributePlotting::addTimedGraphFor(QSharedPointer<DatasetAttributeReader>& attribute_reader) {
    const QString attribute_name = attribute_reader->getName();
    if(map_plot_info.contains(attribute_name)) return;

    if(attribute_reader->getType() == chaos::DataType::TYPE_BYTEARRAY ||
            attribute_reader->getType() == chaos::DataType::TYPE_BOOLEAN ||
            attribute_reader->getType() == chaos::DataType::TYPE_STRING) {
        QMessageBox::information(this, tr("Create plot error"), QString("The type for attribute %1 can't be added to timed plot").arg( attribute_name));
        return;
    }

    bool ok = false;
    QSharedPointer<PlotInfo> plot_info(new PlotInfo());
    //get monitor handler for attribute
    plot_info->monitor_handler = getChaosAttributeHandlerForType(attribute_name,
                                                                 (plot_info->attribute_type = (chaos::DataType::DataType)attribute_reader->getType()),
                                                                 ok);
    if(!ok) {
        QMessageBox::information(this, tr("Create plot error"), QString("Error creating monitor for attrbiute %1").arg(attribute_name));
        return;
    }

    plot_info->quantum_multiplier = 5;
    plot_info->attribute_name = attribute_name;
    plot_info->direction = attribute_reader->getDirection();
    plot_info->graph = ui->qCustomPlotTimed->addGraph();
    plot_info->graph->setLineStyle(QCPGraph::lsLine);
    plot_info->graph->setName(attribute_name);
    plot_info->graph->addToLegend();

    _addRemoveToPlot(plot_info, true);
}


void NodeAttributePlotting::removedTimedGraphFor(const QString& attribute_name) {
    if(!map_plot_info.contains(attribute_name)) return;

    _addRemoveToPlot(map_plot_info[attribute_name], false);
}

void NodeAttributePlotting::_addRemoveToPlot(QSharedPointer<PlotInfo> plot_info, bool add) {
    const QString attribute_name = plot_info->attribute_name;

    //general check
    unsigned int dataset_type = 0;
    monitor_system::AbstractQuantumTSTaggedAttributeHandler *chaos_handler = dynamic_cast<monitor_system::AbstractQuantumTSTaggedAttributeHandler*>(plot_info->monitor_handler.data());
    switch(plot_info->direction) {
    case chaos::DataType::Input:
        dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;
        break;

    case chaos::DataType::Output:
        dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
        break;

    }

    if(add) {
        //ad plot to graph
        connect(plot_info->monitor_handler.data(),
                SIGNAL(valueUpdated(QString,QString,uint64_t,QVariant)),
                SLOT(valueUpdated(QString,QString,uint64_t,QVariant)));

        //activate monitoring
        monitor_system::AbstractQuantumTSTaggedAttributeHandler *chaos_handler = dynamic_cast<monitor_system::AbstractQuantumTSTaggedAttributeHandler*>(plot_info->monitor_handler.data());
        ChaosMetadataServiceClient::getInstance()->addKeyAttributeHandlerForDataset(node_uid.toStdString(),
                                                                                    dataset_type,
                                                                                    plot_info->quantum_multiplier,
                                                                                    chaos_handler);
        map_plot_info.insert(attribute_name, plot_info);
    } else {
        //remove plot from monitor
        ChaosMetadataServiceClient::getInstance()->removeKeyAttributeHandlerForDataset(node_uid.toStdString(),
                                                                                        dataset_type,
                                                                                        plot_info->quantum_multiplier,
                                                                                        chaos_handler);

        //we can remove the plot from graph and map
        ui->qCustomPlotTimed->removeGraph(plot_info->graph);
        map_plot_info.remove(attribute_name);
    }

}

void NodeAttributePlotting::addGraphAction() {
    QModelIndexList selected_row = ui->listViewPlottableAttribute->selectionModel()->selectedRows();
    foreach (QModelIndex row, selected_row) {
        QSharedPointer<DatasetAttributeReader> attribute_reader = row.data(Qt::UserRole).value< QSharedPointer<DatasetAttributeReader> >();
        addTimedGraphFor(attribute_reader);
    }
}

void NodeAttributePlotting::removePlot() {
    QModelIndexList selected_row = ui->listViewPlottableAttribute->selectionModel()->selectedRows();
    foreach (QModelIndex row, selected_row) {
        QSharedPointer<DatasetAttributeReader> attribute_reader = row.data(Qt::UserRole).value< QSharedPointer<DatasetAttributeReader> >();
        removedTimedGraphFor(attribute_reader->getName());
    }
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

QSharedPointer<AbstractTSTaggedAttributeHandler>
NodeAttributePlotting::getChaosAttributeHandlerForType(const QString& attribute_name, chaos::DataType::DataType chaos_type, bool& ok) {
    ok = true;
    switch(chaos_type) {
    case chaos::DataType::TYPE_BOOLEAN:
        return QSharedPointer<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedBoolAttributeHandler(attribute_name,
                                                                                                        chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_INT32:
        return QSharedPointer<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedInt32AttributeHandler(attribute_name,
                                                                                                         chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_INT64:
        return QSharedPointer<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedInt64AttributeHandler(attribute_name,
                                                                                                         chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_DOUBLE:
        return QSharedPointer<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedDoubleAttributeHandler(attribute_name,
                                                                                                          chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_STRING:
        return QSharedPointer<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedStringAttributeHandler(attribute_name,
                                                                                                          chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    case chaos::DataType::TYPE_BYTEARRAY:
        return QSharedPointer<AbstractTSTaggedAttributeHandler>(new MonitorTSTaggedBinaryAttributeHandler(attribute_name,
                                                                                                          chaos::DataPackCommonKey::DPCK_TIMESTAMP));
    default:
        ok = false;
        return QSharedPointer<AbstractTSTaggedAttributeHandler>();
        break;
    }
}

void NodeAttributePlotting::valueUpdated(const QString& node_uid,
                                         const QString& attribute_name,
                                         uint64_t timestamp,
                                         const QVariant& attribute_value) {
    qDebug() << QString("Updating plot for: %1 with value %2[%3]").arg(attribute_name, attribute_value.toString(), QString::number(timestamp));
    map_plot_info[attribute_name]->graph->addData((double)timestamp, attribute_value.toDouble());
    ui->qCustomPlotTimed->replot(QCustomPlot::rpImmediate);
}
