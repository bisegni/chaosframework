#ifndef CHAOSDATASETLABEL_H
#define CHAOSDATASETLABEL_H

#include "ChaosLabel.h"
class ChaosDatasetLabel :
        public ChaosLabel{
    Q_OBJECT

    Q_PROPERTY(Dataset dataset READ dataset WRITE setDataset NOTIFY datasetChanged)
    Q_ENUMS(Dataset)

public:
    enum Dataset { DatasetOutput, DatasetInput, DatasetCustom, DatasetSystem };

    explicit ChaosDatasetLabel(QWidget * parent = 0);

    Dataset dataset();
    void setDataset(Dataset dataset);
    void startMonitoring();
    void stopMonitoring();
signals:
    void datasetChanged(const Dataset& last_dataset, const Dataset& new_dataset);
public slots:

private:
    Dataset p_dataset;

};

#endif // CHAOSDATASETLABEL_H
