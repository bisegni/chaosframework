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
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);

private:
    QLabel *labelValue;
};

#endif // CDATASETATTRIBUTELABEL_H
