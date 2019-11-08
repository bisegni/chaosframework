#ifndef CHAOSBASEDATASETATTRIBUTESETVALUEUI_H
#define CHAOSBASEDATASETATTRIBUTESETVALUEUI_H

#include "ChaosBaseDatasetAttributeUI.h"

//!Base class for the set of input attribute
class ChaosBaseDatasetAttributeSetValueUI:
        public ChaosBaseDatasetAttributeUI {
       Q_OBJECT
public:
    ChaosBaseDatasetAttributeSetValueUI(QWidget *parent = nullptr);
public slots:
    virtual void reset() = 0;
signals:
    void attributeChangeSetUpdated(QString attribute_name,
                                    QVariant attribute_value);
};

#endif // CHAOSBASEDATASETATTRIBUTESETVALUEUI_H
