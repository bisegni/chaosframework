#ifndef BUFFERPLOT_H
#define BUFFERPLOT_H

#include "../external_lib/qcustomplot.h"
#include "AbstractBinaryPlotAdapter.h"

#include <QMap>
#include <QWidget>
#include <QSharedPointer>
#include <QReadWriteLock>

namespace Ui {
class BufferPlot;
}

class AttributeScanner:
        public SingleTypeBinaryPlotAdapter<double> {
    AttributeScanner(QCustomPlot *_master_plot,
                     QReadWriteLock& _global_lock);
    //update attribute graph data
    void updateData(boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot);
    //update attribute data type
    void setBinaryType(std::vector<chaos::DataType::BinarySubtype>& _bin_type);
protected:
    void channelElement(const unsigned int channel_index,
                        const unsigned int element_index,
                        const double &element_value);
private:
    QReadWriteLock& global_lock;
    QCustomPlot *master_plot;
    QVector<QCPGraph*> graph_vector;
    std::vector<chaos::DataType::BinarySubtype> bin_type;
};

class BufferPlot:
        public QWidget,
        public SingleTypeBinaryPlotAdapter<double> {
    Q_OBJECT

public:
    explicit BufferPlot(QReadWriteLock& _global_lock,
                        QWidget *parent = 0);
    ~BufferPlot();
    void addAttribute(const QString& channel_name,
                      std::vector<chaos::DataType::BinarySubtype>& _bin_type);
    void updateAttributeData(const QString& channel_name,
                             boost::shared_ptr<chaos::common::data::CDataBuffer>& _buffer_to_plot);
    void removeAttribute(const QString& channel_name);

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

    //contains all attribute scanners
    QMap<QString, QSharedPointer<AttributeScanner> > map_attribute_scanners;

    QVector<QCPGraph*> graph_vector;
    QReadWriteLock& global_lock;
};

#endif // BUFFERPLOT_H
