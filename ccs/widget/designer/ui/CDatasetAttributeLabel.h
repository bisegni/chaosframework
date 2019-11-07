#ifndef CDATASETATTRIBUTELABEL_H
#define CDATASETATTRIBUTELABEL_H

#include <QLabel>

#include "../ChaosBaseDatasetAttributeUI.h"

class CDatasetAttributeLabel:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
    Q_PROPERTY(AttributeType attributeType READ attributeType WRITE setAttributeType)
public:
    enum AttributeType {  Timestamp,
                          DeviceID,
                          SequenceID,
                          RunID,
                          Custom};

    Q_ENUM(AttributeType)
    explicit CDatasetAttributeLabel(QWidget *parent = nullptr);
    void setAttributeType(const AttributeType &new_attribute_type);
    AttributeType attributeType() const;
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);

private:
    QLabel *labelValue;
    AttributeType p_attribute_type;
};

#endif // CDATASETATTRIBUTELABEL_H
