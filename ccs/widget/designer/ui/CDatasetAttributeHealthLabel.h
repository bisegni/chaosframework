#ifndef CDATASETATTRIBUTEHEALTHLABEL_H
#define CDATASETATTRIBUTEHEALTHLABEL_H

#include <QLabel>

#include "../ChaosBaseDatasetAttributeUI.h"

class CDatasetAttributeHealthLabel:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
    Q_PROPERTY(HealthValueType healthValueType READ healthValueType WRITE setHealthValueType)
public:
    enum HealthValueType {HealthValueTypeTimeStamp,
                          HealthValueTypeMDSTimeStamp,
                          HealthValueTypeUserTime,
                          HealthValueTypeSystemTime,
                          HealthValueTypeProcessSwap,
                          HealthValueTypeUptime,
                          HealthValueTypeStatus,
                          HealthValueTypeError,
                          HealthValueTypeErrorMessage,
                          HealthValueTypeErrorDomain};
    Q_ENUM(HealthValueType)
    explicit CDatasetAttributeHealthLabel(QWidget *parent = nullptr);
    void setHealthValueType(const HealthValueType &new_health_type);
    HealthValueType healthValueType() const;
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);

private:
    QLabel *labelValue;
    HealthValueType p_health_value_type;
};

#endif // CDATASETATTRIBUTEHEALTHLABEL_H
