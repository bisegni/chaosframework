#ifndef CDATASETATTRIBUTELABEL_H
#define CDATASETATTRIBUTELABEL_H

#include <QLabel>
#include "../ChaosBaseDatasetAttributeUI.h"

class CDatasetAttributeLabel:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
public:
    explicit CDatasetAttributeLabel(QWidget *parent = nullptr);
    ~CDatasetAttributeLabel();
private slots:
    void updateValue(QVariant new_value);

private:
    QLabel *labelValue;
};

#endif // CDATASETATTRIBUTELABEL_H
