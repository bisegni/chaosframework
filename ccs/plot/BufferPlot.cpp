#include "BufferPlot.h"
#include "ui_BufferPlot.h"

#define RANDOM_COMPONENT() (qrand() % 256)

using namespace chaos::common::utility;
using namespace chaos::metadata_service_client;

AttributeScanner::AttributeScanner(const QString& _node_uid,
                                   const QString& _attribute_name,
                                   QCustomPlot *_master_plot,
                                   QReadWriteLock& _global_lock,
                                   QCPRange &_global_y_range,
                                   QCPRange &_global_x_range):
    node_uid(_node_uid),
    attribute_name(_attribute_name),
    global_lock(_global_lock),
    master_plot(_master_plot),
    global_y_range(_global_y_range),
    global_x_range(_global_x_range){
    ChaosMetadataServiceClient::getInstance()->addHandlerToNodeMonitor(node_uid.toStdString(),
                                                                       node_monitor::ControllerTypeNodeControlUnit,
                                                                       this);
}

AttributeScanner::~AttributeScanner() {
    ChaosMetadataServiceClient::getInstance()->removeHandlerToNodeMonitor(node_uid.toStdString(),
                                                                          node_monitor::ControllerTypeNodeControlUnit,
                                                                          this);
    //remove all graph
    foreach(QCPGraph* graph, graph_vector) {
        master_plot->removeGraph(graph);
    }
}

void AttributeScanner::updateData(const ChaosSharedPtr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {
    global_lock.lockForWrite();
    SingleTypeBinaryPlotAdapter<double>::setData(_buffer_to_plot);
    //iterate on buffer
    for(int idx = 0;
        idx < SingleTypeBinaryPlotAdapter<double>::getChannelNumber();
        idx++) {
        //clear all achnnel data
        graph_vector[idx]->clearData();
        SingleTypeBinaryPlotAdapter<double>::iterOnChannel(idx);
    }
    global_lock.unlock();
}

void AttributeScanner::setBinaryType(const std::vector<unsigned int> &_bin_type) {
    global_lock.lockForRead();
    SingleTypeBinaryPlotAdapter<double>::setBinaryType(_bin_type);
    //if this value is < 0 means that we need to remove graph
    int channel_to_add = SingleTypeBinaryPlotAdapter<double>::getChannelNumber() - graph_vector.size();
    if(channel_to_add > 0) {
        //we need to add graph
        for(int idx = channel_to_add-1;
            idx < SingleTypeBinaryPlotAdapter<double>::getChannelNumber();
            idx++) {
            graph_vector.push_back(master_plot->addGraph());
            graph_vector[idx]->setLineStyle(QCPGraph::lsLine);
            graph_vector[idx]->setName(QString("%1_Ch%2").arg(attribute_name).arg(idx));
            graph_vector[idx]->setPen(QPen(QColor(RANDOM_COMPONENT(), RANDOM_COMPONENT(), RANDOM_COMPONENT())));
            graph_vector[idx]->setAntialiased(true);
            graph_vector[idx]->addToLegend();
        }
    } else if(channel_to_add < 0) {
        //we need to remove graph
        for(int idx = channel_to_add;
            idx != 0;
            idx--){
            graph_vector.remove(graph_vector.size()-1);
        }
    }

    //set graph range
    global_lock.unlock();
}

void AttributeScanner::updatedDS(const std::string& control_unit_uid,
                                 int dataset_type_signal,
                                 chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values) {
    if(dataset_type_signal != chaos::DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT) return;
    if(dataset_key_values.count(attribute_name.toStdString()) == 0) return;
    updateData(dataset_key_values[attribute_name.toStdString()].asCDataBufferShrdPtr());
}

void AttributeScanner::noDSDataFound(const std::string& control_unit_uid,
                                     int dataset_type_signal) {}

unsigned int AttributeScanner::getMaxXAxisSize() {
    return SingleTypeBinaryPlotAdapter<double>::getNumberOfElementPerChannel();
}

void AttributeScanner::channelElement(const unsigned int channel_index,
                                      const unsigned int element_index,
                                      const double &element_value) {
    //add value to y axis if needed
    global_y_range.upper = std::max(global_y_range.upper, element_value);
    global_y_range.lower = std::min(global_y_range.lower, element_value);
    global_x_range.upper = std::max(global_x_range.upper, (double)element_index);
    //QCPDataMap::iterator element_iterator = graph_vector[channel_index]->data()->find(element_index);
    //if(element_iterator != graph_vector[channel_index]->data()->end()) {
    graph_vector[channel_index]->addData(element_index, element_value);
    //} else {
    //    element_iterator.value().value = element_value;
    //}
}

BufferPlot::BufferPlot(QWidget *parent):
    QWidget(parent),
    ui(new Ui::BufferPlot) {
    ui->setupUi(this);
    ui->widgetBufferPlot->setBackground(this->palette().background().color());
    ui->widgetBufferPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->widgetBufferPlot->legend->setFont(QFont(QFont().family(), 8));

    ui->widgetBufferPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->widgetBufferPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->widgetBufferPlot->xAxis->setAutoTickStep(true);

    ui->widgetBufferPlot->yAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->widgetBufferPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->widgetBufferPlot->yAxis->setAutoTickStep(true);

    ui->widgetBufferPlot->axisRect()->setupFullAxesBox();
    ui->widgetBufferPlot->legend->setVisible(true);
}

BufferPlot::~BufferPlot() {
    map_attribute_scanners().clear();
    delete ui;
}

void BufferPlot::addAttribute(const QString& node_uid,
                              const QString& attribute_name) {
    LockableAttributeMap::LockableObjectWriteLock wl = map_attribute_scanners.getWriteLockObject();
    AttributeMapKey key(node_uid,attribute_name);
    if(map_attribute_scanners().contains(key)) return;
    map_attribute_scanners().insert(key, QSharedPointer<AttributeScanner>(new AttributeScanner(node_uid,
                                                                                               attribute_name,
                                                                                               ui->widgetBufferPlot,
                                                                                               global_lock,
                                                                                               global_y_range,
                                                                                               global_x_range)));
}

bool BufferPlot::hasAttribute(const QString& node_uid,
                              const QString& attribute_name) {
    LockableAttributeMap::LockableObjectReadLock wl = map_attribute_scanners.getReadLockObject();
    AttributeMapKey key(node_uid,
                        attribute_name);
    return map_attribute_scanners().contains(key);
}

void BufferPlot::updateAttributeData(const QString& node_uid,
                                     const QString& attribute_name,
                                     ChaosSharedPtr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {
    LockableAttributeMap::LockableObjectReadLock wl = map_attribute_scanners.getReadLockObject();
    AttributeMapKey key(node_uid,attribute_name);
    if(map_attribute_scanners().contains(key) == false) return;
    map_attribute_scanners()[key]->updateData(_buffer_to_plot);
}

void BufferPlot::removeAttribute(const QString& node_uid,
                                 const QString& attribute_name) {
    LockableAttributeMap::LockableObjectWriteLock wl = map_attribute_scanners.getWriteLockObject();
    AttributeMapKey key(node_uid,attribute_name);
    AttributeMap::iterator it = map_attribute_scanners().find(key);
    if(it == map_attribute_scanners().end()) return;
    map_attribute_scanners().erase(it);
}

void BufferPlot::updateAttributeDataType(const QString &node_uid,
                                         const QString& attribute_name,
                                         const std::vector<unsigned int>& _bin_type) {
    LockableAttributeMap::LockableObjectReadLock wl = map_attribute_scanners.getReadLockObject();
    AttributeMapKey key(node_uid,attribute_name);
    if(map_attribute_scanners().contains(key) == false) return;
    map_attribute_scanners()[key]->setBinaryType(_bin_type);
}

void BufferPlot::updatePlot() {
    global_lock.lockForRead();
    ui->widgetBufferPlot->yAxis->setRange(global_y_range);
    ui->widgetBufferPlot->xAxis->setRange(global_x_range);

    ui->widgetBufferPlot->replot();
    global_lock.unlock();
}

bool BufferPlot::isEmpty() {
    return map_attribute_scanners().size() == 0;
}
