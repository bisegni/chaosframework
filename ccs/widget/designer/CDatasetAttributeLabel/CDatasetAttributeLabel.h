#ifndef CDATASETATTRIBUTELABEL_H
#define CDATASETATTRIBUTELABEL_H

#include <QLabel>
#include "../ChaosBaseDatasetUI.h"


class CDatasetAttributeLabel :
        public ChaosBaseDatasetUI {
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
