#ifndef CNODERESOURCEWIDGET_H
#define CNODERESOURCEWIDGET_H

#include "ChaosReadDatasetWidgetCompanion.h"

#include <QWidget>

namespace Ui {
class CNodeResourceWidget;
}

class CNodeResourceWidget :
        public QWidget,
        public ChaosReadDatasetWidgetCompanion {
    Q_OBJECT

public:
    explicit CNodeResourceWidget(QWidget *parent = 0);
    ~CNodeResourceWidget();
    void initChaosContent();
    void deinitChaosContent();
    void updateChaosContent();
protected:
    void quantumSlotHasData(const std::string& key,
                            const chaos::metadata_service_client::monitor_system::KeyValue& value);
    void quantumSlotHasNoData(const std::string& key);
    Dataset dataset();
    void setDataset(Dataset dataset);

protected slots:
    void updateUIWithData(const chaos::metadata_service_client::monitor_system::KeyValue value);
    void updateUIWithNoData();

private:
    Ui::CNodeResourceWidget *ui;
    QString secondsToDHMS(uint64_t duration);
};

#endif // CNODERESOURCEWIDGET_H
