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
signals:

private slots:
   void updateValue(QVariant variant_value);

private:
    QLabel *internalLabel;


};

#endif // CDATASETATTRIBUTELABEL_H
