#ifndef ChaosBaseDatasetAttributeUI_H
#define ChaosBaseDatasetAttributeUI_H

#include "ChaosBaseDatasetUI.h"

class ChaosBaseDatasetAttributeUI:
        public ChaosBaseDatasetUI {
    Q_OBJECT
    Q_PROPERTY(QString attributeName READ attributeName WRITE setAttributeName)
public:
    ChaosBaseDatasetAttributeUI(QWidget *parent = nullptr);

    QString attributeName() const;
    void setAttributeName(const QString& new_attribute_name);
public slots:
    void updateData(QString attribute_name,
                    QVariant attribute_value);

private:
    QString p_attribute_name;
};

#endif // ChaosBaseDatasetAttributeUI_H
