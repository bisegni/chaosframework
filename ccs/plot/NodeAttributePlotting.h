#ifndef NODEATTRIBUTEPLOTTING_H
#define NODEATTRIBUTEPLOTTING_H

#include "../node/data/DatasetAttributeListModel.h"
#include "../external_lib/qcustomplot.h"
#include "../api_async_processor/ApiAsyncProcessor.h"

#include <QMap>
#include <QWidget>
#include <QSharedPointer>

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class NodeAttributePlotting;
}

struct PlotInfo {
    int     graph_index;
    QCPGraph *graph;
    QString attribute_name;
};

class NodeAttributePlotting :
        public QWidget {
    Q_OBJECT

public:
    explicit NodeAttributePlotting(const QString& _node_uid,
                                   QWidget *parent = 0);
    ~NodeAttributePlotting();

protected:
    void addTimedGraphFor(const QString& attribute_name);

private slots:
    void addGraphAction();
    void removePlot();
    void asyncApiResult(const QString& tag, QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void asyncApiError(const QString&  tag, QSharedPointer<chaos::CException> api_exception);
    void asyncApiTimeout(const QString& tag);
    void on_pushButtonStartMonitoring_clicked();

    void on_pushButtonStopMonitoring_clicked();

private:
    const QString node_uid;
    ApiAsyncProcessor api_processor;
    DatasetAttributeListModel list_model_dataset;
    QMap<QString, QSharedPointer<PlotInfo> > map_plot_info;
    Ui::NodeAttributePlotting *ui;
};

#endif // NODEATTRIBUTEPLOTTING_H
