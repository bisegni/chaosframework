#include "ChaosDatasetLabel.h"

ChaosDatasetLabel::ChaosDatasetLabel(QWidget *parent) :
    ChaosLabel(parent) {

}

ChaosDatasetLabel::Dataset ChaosDatasetLabel::dataset() {
 return p_dataset;
}

void ChaosDatasetLabel::setDataset(ChaosDatasetLabel::Dataset dataset) {
    if(dataset == p_dataset ||
            monitoring) return;
    emit datasetChanged(p_dataset,
                        dataset);
    p_dataset = dataset;
}

void ChaosDatasetLabel::startMonitoring() {

}

void ChaosDatasetLabel::stopMonitoring() {

}
