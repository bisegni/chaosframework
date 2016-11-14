#ifndef BUFFERPLOT_H
#define BUFFERPLOT_H

#include "../external_lib/qcustomplot.h"
#include "AbstractBinaryPlotAdapter.h"

#include <QWidget>
#include <QReadWriteLock>

namespace Ui {
class BufferPlot;
}

class BufferPlot :
        public QWidget,
        public SingleTypeBinaryPlotAdapter<double> {
    Q_OBJECT

public:
    explicit BufferPlot(QWidget *parent = 0);
    ~BufferPlot();
    void setChannelData(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot,
                 std::vector<chaos::DataType::BinarySubtype>& _bin_type);
    void updatePlot();
protected:
    void channelElement(const unsigned int channel_index,
                        const unsigned int element_index,
                        const double &element_value);
private:
    Ui::BufferPlot *ui;
    QCPRange global_y_range;
    QVector<QCPGraph*> graph_vector;
    QReadWriteLock lock_read_write_for_plot;
};

#endif // BUFFERPLOT_H
