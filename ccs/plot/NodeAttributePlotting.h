#ifndef NODEATTRIBUTEPLOTTING_H
#define NODEATTRIBUTEPLOTTING_H

#include "../node/data/DatasetAttributeListModel.h"
#include "../external_lib/qcustomplot.h"
#include "../api_async_processor/ApiAsyncProcessor.h"
#include "../monitor/monitor.h"

#include <QMap>
#include <QWidget>
#include <QSharedPointer>
#include <QReadWriteLock>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class NodeAttributePlotting;
}

struct PlotInfo {
    int graph_index;
    int direction;
    int quantum_multiplier;
    QSharedPointer<AbstractTSTaggedAttributeHandler> monitor_handler;
    QCPGraph *graph;
    QString attribute_name;
    chaos::DataType::DataType attribute_type;
};

class NodeAttributePlotting :
        public QWidget {
    Q_OBJECT

public:
    explicit NodeAttributePlotting(const QString& _node_uid,
                                   QWidget *parent = 0);
    ~NodeAttributePlotting();

protected:
    QSharedPointer<AbstractTSTaggedAttributeHandler>
    getChaosAttributeHandlerForType(const QString &attribute_name, chaos::DataType::DataType chaos_type, bool& ok);
    void addTimedGraphFor(QSharedPointer<DatasetAttributeReader>& attribute_reader);
    void removedTimedGraphFor(const QString &attribute_name);
private slots:
    void addGraphAction();
    void removePlot();
    void updatePlot();
    void asyncApiResult(const QString& tag, QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void asyncApiError(const QString&  tag, QSharedPointer<chaos::CException> api_exception);
    void asyncApiTimeout(const QString& tag);
    void on_pushButtonStartMonitoring_clicked();
    void on_pushButtonStopMonitoring_clicked();
    void valueUpdated(const QString& node_uid,
                      const QString& attribute_name,
                      uint64_t timestamp,
                      const QVariant& attribute_value);
    void on_lineEditTimeInterval_editingFinished();

    void on_lineEditRangeTo_editingFinished();

    void on_lineEditRangeFrom_editingFinished();

    void on_checkBoxLogScaleEnable_clicked();

private:
    const QString node_uid;
    uint64_t plot_ageing;                                                                    ;
    QReadWriteLock lock_read_write_for_plot;
    ApiAsyncProcessor api_processor;
    DatasetAttributeListModel list_model_dataset;
    QMap<QString, QSharedPointer<PlotInfo> > map_plot_info;
    Ui::NodeAttributePlotting *ui;

    void _addRemoveToPlot(QSharedPointer<PlotInfo> plot_info, bool add);
};

#endif // NODEATTRIBUTEPLOTTING_H
