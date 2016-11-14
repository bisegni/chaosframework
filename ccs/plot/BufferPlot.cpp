#include "BufferPlot.h"
#include "ui_BufferPlot.h"
#include <chaos/common/global.h>

#define RANDOM_COMPONENT() (qrand() % 256)

AttributeScanner::AttributeScanner(QCustomPlot *_master_plot,
                                   QReadWriteLock& _global_lock):
master_plot(_master_plot),
global_lock(_global_lock){}

void AttributeScanner::updateData(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {

}

void AttributeScanner::setBinaryType(std::vector<chaos::DataType::BinarySubtype>& _bin_type) {

}

void AttributeScanner::channelElement(const unsigned int channel_index,
                                      const unsigned int element_index,
                                      const double &element_value) {

}

BufferPlot::BufferPlot(QWidget *parent) :
    QWidget(parent),
    SingleTypeBinaryPlotAdapter<double>(),
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

void BufferPlot::addAttribute(const QString& channel_name,
                              std::vector<chaos::DataType::BinarySubtype>& _bin_type) {

}

void BufferPlot::updateAttributeData(const QString& channel_name,
                                     boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot) {

}

void BufferPlot::removeAttribute(const QString& channel_name) {

}

void BufferPlot::setChannelData(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot,
                                std::vector<chaos::DataType::BinarySubtype>& _bin_type) {
    lock_read_write_for_plot.lockForWrite();
    //set data into scanner
    SingleTypeBinaryPlotAdapter<double>::setData(_buffer_to_plot, _bin_type);

    //if this value is < 0 means that we need to remove graph
    int channel_to_add = SingleTypeBinaryPlotAdapter<double>::getChannelNumber() - graph_vector.size();
    if(channel_to_add > 0) {
        //we need to add graph
        for(int idx = channel_to_add-1;
            idx < SingleTypeBinaryPlotAdapter<double>::getChannelNumber();
            idx++) {
            graph_vector.push_back(ui->widgetBufferPlot->addGraph());
            graph_vector[idx]->setLineStyle(QCPGraph::lsLine);
            graph_vector[idx]->setName(QString("Channel %1").arg(idx));
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

    //iterate on buffer
    for(int idx = 0;
        idx < SingleTypeBinaryPlotAdapter<double>::getChannelNumber();
        idx++) {
        SingleTypeBinaryPlotAdapter<double>::iterOnChannel(idx);
    }

    //set graph range
    ui->widgetBufferPlot->xAxis->setRange(0, SingleTypeBinaryPlotAdapter<double>::getNumberOfElementPerChannel());
    ui->widgetBufferPlot->yAxis->setRange(global_y_range);
    lock_read_write_for_plot.unlock();
}

void BufferPlot::channelElement(const unsigned int channel_index,
                                const unsigned int element_index,
                                const double& element_value) {
    //add value to y axis if needed
    if(global_y_range.contains(element_value) == false) global_y_range += element_value;
    QCPDataMap::iterator element_iterator = graph_vector[channel_index]->data()->find(element_index);
    if(element_iterator != graph_vector[channel_index]->data()->end()) {
        graph_vector[channel_index]->addData(element_index, element_value);
    } else {
        element_iterator.value().value = element_value;
    }
}

void BufferPlot::updatePlot() {
    lock_read_write_for_plot.lockForRead();
    ui->widgetBufferPlot->replot();
    lock_read_write_for_plot.unlock();
}
