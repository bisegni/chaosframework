#include "BufferPlot.h"
#include "ui_BufferPlot.h"
#include <chaos/common/global.h>

#define RANDOM_COMPONENT() (qrand() % 256)

AttributeScanner::AttributeScanner(const QString& _attribute_name,
                                   QCustomPlot *_master_plot,
                                   QReadWriteLock& _global_lock,
                                   QCPRange &_global_y_range):
    attribute_name(_attribute_name),
    global_lock(_global_lock),
    master_plot(_master_plot),
    global_y_range(_global_y_range){}

AttributeScanner::~AttributeScanner() {
    //remove all graph
    foreach(QCPGraph* graph, graph_vector) {
        master_plot->removeGraph(graph);
    }
}

void AttributeScanner::updateData(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {
    global_lock.lockForWrite();
    SingleTypeBinaryPlotAdapter<double>::setData(_buffer_to_plot);
    //iterate on buffer
    for(int idx = 0;
        idx < SingleTypeBinaryPlotAdapter<double>::getChannelNumber();
        idx++) {
        SingleTypeBinaryPlotAdapter<double>::iterOnChannel(idx);
    }
    global_lock.unlock();
}

void AttributeScanner::setBinaryType(std::vector<chaos::DataType::BinarySubtype>& _bin_type) {
    global_lock.lockForWrite();
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

unsigned int AttributeScanner::getMaxXAxisSize() {
    return SingleTypeBinaryPlotAdapter<double>::getNumberOfElementPerChannel();
}

void AttributeScanner::channelElement(const unsigned int channel_index,
                                      const unsigned int element_index,
                                      const double &element_value) {
    //add value to y axis if needed
    if(global_y_range.contains(element_value) == false) global_y_range += element_value;
    QCPDataMap::iterator element_iterator = graph_vector[channel_index]->data()->find(element_index);
    if(element_iterator != graph_vector[channel_index]->data()->end()) {
        graph_vector[channel_index]->addData(element_index, element_value);
    } else {
        element_iterator.value().value = element_value;
    }
}

BufferPlot::BufferPlot(QReadWriteLock& _global_lock,
                       QWidget *parent):
    QWidget(parent),
    global_lock(_global_lock),
    ui(new Ui::BufferPlot) {
    ui->setupUi(this);
    ui->widgetBufferPlot->setBackground(this->palette().background().color());
    ui->widgetBufferPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);
    ui->widgetBufferPlot->legend->setFont(QFont(QFont().family(), 8));

    ui->widgetBufferPlot->xAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->widgetBufferPlot->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->widgetBufferPlot->xAxis->setLabel("x");
    ui->widgetBufferPlot->xAxis->setAutoTickStep(true);

    ui->widgetBufferPlot->yAxis->setTickLabelType(QCPAxis::ltNumber);
    ui->widgetBufferPlot->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->widgetBufferPlot->xAxis->setLabel("y");

    ui->widgetBufferPlot->axisRect()->setupFullAxesBox();
    ui->widgetBufferPlot->legend->setVisible(true);
}

BufferPlot::~BufferPlot() {
    delete ui;
}

void BufferPlot::addAttribute(const QString& channel_name) {
    if(map_attribute_scanners.contains(channel_name)) return;
    map_attribute_scanners.insert(channel_name, QSharedPointer<AttributeScanner>(new AttributeScanner(channel_name,
                                                                                                      ui->widgetBufferPlot,
                                                                                                      global_lock,
                                                                                                      global_y_range)));
}

void BufferPlot::updateAttributeData(const QString& channel_name,
                                     boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {
    if(map_attribute_scanners.contains(channel_name) == false) return;
    map_attribute_scanners[channel_name]->updateData(_buffer_to_plot);
}

void BufferPlot::removeAttribute(const QString& channel_name) {
    if(map_attribute_scanners.contains(channel_name) == false) return;
}

void BufferPlot::updateAttributeDataType(const QString& channel_name,
                                         std::vector<chaos::DataType::BinarySubtype>& _bin_type) {
    if(map_attribute_scanners.contains(channel_name) == false) return;
    map_attribute_scanners[channel_name]->setBinaryType(_bin_type);
}

void BufferPlot::updatePlot() {
    global_lock.lockForRead();
    ui->widgetBufferPlot->replot();
    global_lock.unlock();
}
