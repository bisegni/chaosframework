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
        public chaos::metadata_service_client::node_monitor::ControlUnitMonitorHandler,
        protected SingleTypeBinaryPlotAdapter<double> {
public:
    AttributeScanner(const QString &_node_uid,
                     const QString& _attribute_name,
                     QCustomPlot *_master_plot,
                     QReadWriteLock& _global_lock,
                     QCPRange& _global_y_range, QCPRange &_global_x_range);
    ~AttributeScanner();
    //update attribute graph data
    void updateData(const ChaosSharedPtr<chaos::common::data::CDataBuffer> &_buffer_to_plot);
    //update attribute data type
    void setBinaryType(const std::vector<unsigned int>& _bin_type);
    //get the max element
    unsigned int getMaxXAxisSize();
protected:
    void channelElement(const unsigned int channel_index,
                        const unsigned int element_index,
                        const double &element_value);
    void updatedDS(const std::string& control_unit_uid,
                   int dataset_type_signal,
                   chaos::metadata_service_client::node_monitor::MapDatasetKeyValues& dataset_key_values);
    void noDSDataFound(const std::string& control_unit_uid,
                       int dataset_type_signal);
private:
    const QString node_uid;
    const QString attribute_name;
    QCPRange& global_y_range;
    QCPRange& global_x_range;
    QReadWriteLock& global_lock;
    QCustomPlot *master_plot;
    QVector<QCPGraph*> graph_vector;
    std::vector<chaos::DataType::BinarySubtype> bin_type;
};

typedef QPair<QString,QString> AttributeMapKey;
typedef QMap< AttributeMapKey, QSharedPointer<AttributeScanner> > AttributeMap;
typedef chaos::common::utility::LockableObject< AttributeMap > LockableAttributeMap;

class BufferPlot:
        public QWidget {
    Q_OBJECT

public:
    explicit BufferPlot(QWidget *parent = 0);
    ~BufferPlot();
    void addAttribute(const QString& node_uid,
                      const QString& attribute_name);

    bool hasAttribute(const QString& node_uid,
                      const QString& attribute_name);

    void updateAttributeData(const QString& node_uid,
                             const QString& attribute_name,
                             ChaosSharedPtr<chaos::common::data::CDataBuffer>& _buffer_to_plot);
    void removeAttribute(const QString& node_uid,
                         const QString& attribute_name);

    void updateAttributeDataType(const QString& node_uid,
                                 const QString& attribute_name,
                                 const std::vector<unsigned int> &_bin_type);
    void updatePlot();

    bool isEmpty();
private:
    Ui::BufferPlot *ui;
    QCPRange global_y_range;
    QCPRange global_x_range;
    //contains all attribute scanners
    LockableAttributeMap map_attribute_scanners;
    QVector<QCPGraph*> graph_vector;
    QReadWriteLock global_lock;
};

#endif // BUFFERPLOT_H
