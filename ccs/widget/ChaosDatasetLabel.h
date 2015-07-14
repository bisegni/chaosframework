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
    ~ChaosDatasetLabel();
    Dataset dataset();
    void setDataset(Dataset dataset);
    int startMonitoring();
    int stopMonitoring();
    //force this method to set ever false value to property
    void setLabelValueShowTrackStatus(bool label_value_show_track_status);
signals:
    void datasetChanged(const Dataset& last_dataset, const Dataset& new_dataset);
public slots:

protected slots:
    void valueUpdated(const QString& node_uid,
                      const QString& attribute_name,
                      uint64_t timestamp,
                      const QVariant& attribute_value);
    private:
        Dataset p_dataset;
        unsigned int getChaosDataset();
        boost::shared_ptr<AbstractTSTaggedAttributeHandler>
        getChaosAttributeHandlerForType(ChaosDataType chaos_type, bool &ok);

};

#endif // CHAOSDATASETLABEL_H
