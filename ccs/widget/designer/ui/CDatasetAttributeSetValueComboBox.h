#ifndef CDATASETATTRIBUTESETVALUECOMBOBOX_H
#define CDATASETATTRIBUTESETVALUECOMBOBOX_H


#include "../ChaosBaseDatasetAttributeUI.h"

#include <QMap>
#include <QSize>
#include <QVariant>
#include <QComboBox>


class CDatasetAttributeSetValueComboBox:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
    Q_PROPERTY(QVariant setup READ setup WRITE setSetup)
public:
    explicit CDatasetAttributeSetValueComboBox(QWidget *parent = nullptr);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void setSetup(QVariant new_combo_label_value);
    QVariant setup();
private slots:
    void changeSetCommitted();
    void currentIndexChanged(int index);
public slots:
    virtual void reset();
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private:
    bool value_committed;
    QComboBox *combo_box;
    QVariant p_setup;
};

#endif // CDATASETATTRIBUTESETVALUECOMBOBOX_H
