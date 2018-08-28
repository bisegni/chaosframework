#include "NodeAttributePlotting.h"
#include "ui_NodeAttributePlotting.h"

#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QIntValidator>
#include <QVBoxLayout>
static const QString TAG_CU_GET_DATASET = "cu_get_dataset";

using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

PlotInfo::PlotInfo(NodeAttributePlotting& _plotting_class_ref):
    plotting_class_ref(_plotting_class_ref),
    last_received_value(0.0) {
    //let this handler to be update atevery read done by mnitor
    //also if dataset has not changed
    ControlUnitMonitorHandler::setSignalOnChange(false);
}

void PlotInfo::updatedDS(const std::string& control_unit_uid,
                         int dataset_type_signal,
                         chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type != dataset_type_signal) return;
    plotting_class_ref.lock_read_write_for_plot.lockForRead();
    double key = (QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0);
    double key_for_old = key-plotting_class_ref.plot_ageing;
    graph->addData(key, last_received_value = dataset_key_values[attribute_name.toStdString()].asDouble());
    graph->data()->removeBefore(key_for_old);
    plotting_class_ref.lock_read_write_for_plot.unlock();
}

void PlotInfo::noDSDataFound(const std::string& control_unit_uid,
                             int dataset_type_signal) {
    if(dataset_type != dataset_type_signal) return;
    plotting_class_ref.lock_read_write_for_plot.lockForRead();
    double key = (QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0);
    double key_for_old = key-plotting_class_ref.plot_ageing;
    graph->addData(key, last_received_value);
    graph->data()->removeBefore(key_for_old);
    plotting_class_ref.lock_read_write_for_plot.unlock();
}

NodeAttributePlotting::NodeAttributePlotting(const QString& _node_uid,
                                             QWidget *parent) :
    QWidget(parent),
    node_uid(_node_uid),
    plot_ageing(60),
    rng((const uint_fast32_t) std::time(0) ),
    zero_to_255( 0, 255 ),
    random_color_component(rng, zero_to_255),
    ui(new Ui::NodeAttributePlotting) {
    ui->setupUi(this);
    if(parent == NULL) {
        setAttribute(Qt::WA_DeleteOnClose);
    }

    //set window title name
    setWindowTitle(QString("Plot viewer for:%1").arg(node_uid));

    //set list model
    ui->listViewPlottableAttribute->setModel(&list_model_dataset);

    //plot setup
    ui->lineEditTimeInterval->setValidator(new QIntValidator(60, 600));
    ui->lineEditRangeFrom->setValidator(new QIntValidator());
    ui->lineEditRangeTo->setValidator(new QIntValidator());
    ui->qCustomPlotTimed->setBackground(this->palette().background().color());
    ui->qCustomPlotTimed->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->qCustomPlotTimed->legend->setFont(QFont(QFont().family(), 8));

    QSharedPointer<QCPAxisTickerDateTime> dt_tick(new QCPAxisTickerDateTime());
    dt_tick->setDateTimeFormat("hh:mm:ss");
    dt_tick->setTickCount(5);
    dt_tick->setTickStepStrategy(QCPAxisTickerDateTime::tssReadability);
    ui->qCustomPlotTimed->xAxis->setTicker(dt_tick);
    ui->qCustomPlotTimed->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->qCustomPlotTimed->xAxis->setTickLabelColor(QColor("lightGray"));
    ui->qCustomPlotTimed->xAxis->setLabel("Time");

    QSharedPointer<QCPAxisTickerFixed> f_tick(new QCPAxisTickerFixed());
    f_tick->setTickStep(1.0); // tick step shall be 1.0
    f_tick->setScaleStrategy(QCPAxisTickerFixed::ssMultiples);
    ui->qCustomPlotTimed->yAxis->setTicker(f_tick);
    ui->qCustomPlotTimed->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->qCustomPlotTimed->yAxis->setTickLabelColor(QColor("lightGray"));
    ui->qCustomPlotTimed->yAxis->setRange(0.0, 100.0);
    ui->qCustomPlotTimed->yAxis->setLabel("Value");

    ui->qCustomPlotTimed->axisRect()->setupFullAxesBox();
    ui->qCustomPlotTimed->legend->setVisible(true);
    ui->qCustomPlotTimed->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->qCustomPlotTimed,
            SIGNAL(mouseMove(QMouseEvent*)),
            SLOT(onMouseMoveGraph(QMouseEvent*)));

    connect(ui->qCustomPlotTimed,
            SIGNAL(customContextMenuRequested(QPoint)),
            SLOT(contextMenuRequest(QPoint)));

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

    ui->plotBuffer->setVisible(false);

    //load current dataset
    api_processor.submitApiResult(TAG_CU_GET_DATASET,
                                  GET_CHAOS_API_PTR(control_unit::GetCurrentDataset)->execute(node_uid.toStdString()),
                                  this,
                                  SLOT(asyncApiResult(QString, QSharedPointer<chaos::common::data::CDataWrapper>)),
                                  SLOT(asyncApiError(QString, QSharedPointer<chaos::CException>)),
                                  SLOT(asyncApiTimeout(QString)));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updatePlot()));
    timer->start(30);
}

NodeAttributePlotting::~NodeAttributePlotting() {
    //remove unremoved plot
    foreach (QString key, map_plot_info.keys()) {
        removedTimedGraphFor(key);
    }
    delete ui;
}

void NodeAttributePlotting::contextMenuRequest(const QPoint& point){

    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);
    // context menu on legend requested
    if (ui->qCustomPlotTimed->legend->selectTest(point, false) >= 0)  {
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
        menu->addAction("Hide/Show", this, SLOT(moveLegend()))->setData(QString("show/hide"));
    }

    menu->popup(ui->qCustomPlotTimed->mapToGlobal(point));
}

void NodeAttributePlotting::moveLegend() {
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) {

        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)        {
            ui->qCustomPlotTimed->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            ui->qCustomPlotTimed->replot();
        }else {
            QString action_string = contextAction->data().toString();
            if(action_string.compare("show/hide") == 0){
                ui->qCustomPlotTimed->legend->setVisible(!ui->qCustomPlotTimed->legend->visible());
            }
        }
    }
}

void NodeAttributePlotting::onMouseMoveGraph(QMouseEvent *event) {

    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->qCustomPlotTimed->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->qCustomPlotTimed->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        ui->qCustomPlotTimed->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->qCustomPlotTimed->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      ui->qCustomPlotTimed->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      ui->qCustomPlotTimed->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->qCustomPlotTimed->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->qCustomPlotTimed->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
        ui->qCustomPlotTimed->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->qCustomPlotTimed->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
      ui->qCustomPlotTimed->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
      ui->qCustomPlotTimed->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<ui->qCustomPlotTimed->graphCount(); ++i)
    {
      QCPGraph *graph = ui->qCustomPlotTimed->graph(i);
      QCPPlottableLegendItem *item = ui->qCustomPlotTimed->legend->itemWithPlottable(graph);
      if (item->selected() || graph->selected())
      {
        item->setSelected(true);
        graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
      }
    }
}

void NodeAttributePlotting::addTimedGraphFor(QSharedPointer<DatasetAttributeReader>& attribute_reader) {
    const QString attribute_name = attribute_reader->getName();
    if(map_plot_info.contains(attribute_name)) return;

    if((attribute_reader->getType() == chaos::DataType::TYPE_STRING)||(attribute_reader->getType() == chaos::DataType::TYPE_CLUSTER)) {
        QMessageBox::information(this, tr("Create plot error"), QString("The type for attribute %1 can't be added to timed plot").arg( attribute_name));
        return;
    } else if(attribute_reader->getType() == chaos::DataType::TYPE_BYTEARRAY){
        const std::vector<unsigned int> sub_types = attribute_reader->getSubtype();
        if(sub_types.size() == 0) {
            QMessageBox::information(this, tr("Create plot error"), QString("The binary attribute %1 has not subtype defined").arg( attribute_name));
            return;
        }
        //add buffer attribute to buffer plot
        ui->plotBuffer->setVisible(true);
        if(ui->plotBuffer->hasAttribute(node_uid,
                                        attribute_name) == false){
            ui->plotBuffer->addAttribute(node_uid,
                                         attribute_name);
            ui->plotBuffer->updateAttributeDataType(node_uid,
                                                    attribute_name,
                                                    sub_types);
        }
    } else {
        bool ok = false;
        switch(attribute_reader->getDirection()) {
        case chaos::DataType::Input:{
            QSharedPointer<PlotInfo> plot_info(new PlotInfo(*this));
            plot_info->dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;
            plot_info->attribute_name = attribute_name;
            plot_info->graph = ui->qCustomPlotTimed->addGraph();
            plot_info->graph->setLineStyle(QCPGraph::lsLine);
            plot_info->graph->setName(attribute_name);
            plot_info->graph->setPen(QPen(QColor(random_color_component(), random_color_component(), random_color_component())));
            plot_info->graph->setAntialiased(true);
            plot_info->graph->addToLegend();
            _addRemoveToPlot(plot_info, true);
            break;
        }
        case chaos::DataType::Output:{
            QSharedPointer<PlotInfo> plot_info(new PlotInfo(*this));
            plot_info->dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
            plot_info->attribute_name = attribute_name;
            plot_info->graph = ui->qCustomPlotTimed->addGraph();
            plot_info->graph->setLineStyle(QCPGraph::lsLine);
            plot_info->graph->setName(attribute_name);
            plot_info->graph->setPen(QPen(QColor(random_color_component(), random_color_component(), random_color_component())));
            plot_info->graph->setAntialiased(true);
            plot_info->graph->addToLegend();
            _addRemoveToPlot(plot_info, true);
            break;
        }
        case chaos::DataType::Bidirectional: {
            QSharedPointer<PlotInfo> plot_info_in(new PlotInfo(*this));
            plot_info_in->attribute_name = attribute_name;
            plot_info_in->dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;
            plot_info_in->graph = ui->qCustomPlotTimed->addGraph();
            plot_info_in->graph->setLineStyle(QCPGraph::lsLine);
            plot_info_in->graph->setName(attribute_name+"_in");
            plot_info_in->graph->setPen(QPen(QColor(random_color_component(), random_color_component(), random_color_component())));
            plot_info_in->graph->setAntialiased(true);
            plot_info_in->graph->addToLegend();
            _addRemoveToPlot(plot_info_in, true);
            QSharedPointer<PlotInfo> plot_info_out(new PlotInfo(*this));
            plot_info_out->attribute_name = attribute_name;
            plot_info_out->dataset_type = chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
            plot_info_out->graph = ui->qCustomPlotTimed->addGraph();
            plot_info_out->graph->setLineStyle(QCPGraph::lsLine);
            plot_info_out->graph->setName(attribute_name+"_out");
            plot_info_out->graph->setPen(QPen(QColor(random_color_component(), random_color_component(), random_color_component())));
            plot_info_out->graph->setAntialiased(true);
            plot_info_out->graph->addToLegend();
            _addRemoveToPlot(plot_info_out, true);
            break;
        }
        }
    }
}


void NodeAttributePlotting::removedTimedGraphFor(const QString& attribute_name) {
    if(ui->plotBuffer->hasAttribute(node_uid,
                                    attribute_name) == true){
        ui->plotBuffer->removeAttribute(node_uid,
                                        attribute_name);
        ui->plotBuffer->setVisible(ui->plotBuffer->isEmpty() == false);
    } else {
        if(!map_plot_info.contains(attribute_name)) return;

        _addRemoveToPlot(map_plot_info[attribute_name], false);
    }
}

void NodeAttributePlotting::_addRemoveToPlot(QSharedPointer<PlotInfo> plot_info, bool add) {
    if(add) {
        //activate monitoring
        ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(node_uid.toStdString(),
                                                                           chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                           plot_info.data());
        map_plot_info.insert(plot_info->graph->name(), plot_info);
    } else {
        //remove plot from monitor
        ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(node_uid.toStdString(),
                                                                              chaos::metadata_service_client::node_monitor::ControllerTypeNodeControlUnit,
                                                                              plot_info.data());

        //we can remove the plot from graph and map
        map_plot_info.remove(plot_info->graph->name());
        ui->qCustomPlotTimed->removeGraph(plot_info->graph);
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
        switch(attribute_reader->getDirection()) {
        case chaos::DataType::Input:
        case chaos::DataType::Output:
            removedTimedGraphFor(attribute_reader->getName());
            break;
        case chaos::DataType::Bidirectional:
            removedTimedGraphFor(attribute_reader->getName()+"_in");
            removedTimedGraphFor(attribute_reader->getName()+"_out");
            break;

        }


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

void NodeAttributePlotting::updatePlot() {
    lock_read_write_for_plot.lockForWrite();
    double now = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    double key_min = QDateTime::currentDateTime().addSecs(-plot_ageing).toMSecsSinceEpoch()/1000.0;
    ui->qCustomPlotTimed->xAxis->setRange(key_min, now);
    ui->qCustomPlotTimed->replot();
    lock_read_write_for_plot.unlock();
    ui->plotBuffer->updatePlot();
}

void NodeAttributePlotting::on_lineEditTimeInterval_editingFinished() {
    qDebug() << "Update the plot ageing display";
    if(ui->lineEditTimeInterval->text().compare("") == 0) {
        plot_ageing = 60;
    } else {
        plot_ageing = ui->lineEditTimeInterval->text().toDouble();
    }
}

void NodeAttributePlotting::on_lineEditRangeTo_editingFinished() {
    lock_read_write_for_plot.lockForWrite();
    if(ui->lineEditRangeTo->text().compare("") == 0) {
        ui->qCustomPlotTimed->yAxis->setRangeUpper(100.0);
    } else {
        ui->qCustomPlotTimed->yAxis->setRangeUpper(ui->lineEditRangeTo->text().toDouble());
    }
    lock_read_write_for_plot.unlock();
}

void NodeAttributePlotting::on_lineEditRangeFrom_editingFinished() {
    lock_read_write_for_plot.lockForWrite();
    if(ui->lineEditRangeFrom->text().compare("") == 0) {
        ui->qCustomPlotTimed->yAxis->setRangeLower(0.0);
    } else {
        ui->qCustomPlotTimed->yAxis->setRangeLower(ui->lineEditRangeFrom->text().toDouble());
    }
    lock_read_write_for_plot.unlock();
}

void NodeAttributePlotting::on_checkBoxLogScaleEnable_clicked() {
    lock_read_write_for_plot.lockForWrite();
    ui->qCustomPlotTimed->yAxis->setScaleType(ui->checkBoxLogScaleEnable->isChecked()?QCPAxis::stLogarithmic:QCPAxis::stLinear);
    lock_read_write_for_plot.unlock();
}
