#ifndef CDATASETATTRIBUTESETVALUECOMBOBOX_H
#define CDATASETATTRIBUTESETVALUECOMBOBOX_H


#include "../ChaosBaseDatasetAttributeUI.h"

#include <QComboBox>
#include <QSize>


class CDatasetAttributeSetValueComboBox:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
public:
    explicit CDatasetAttributeSetValueComboBox(QWidget *parent = nullptr);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
private slots:
    void changeSetCommitted();
public slots:
    virtual void reset();
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private:
    bool value_committed;
    QComboBox *combo_box;
};

#endif // CDATASETATTRIBUTESETVALUECOMBOBOX_H
