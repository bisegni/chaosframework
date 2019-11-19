#ifndef CDATASETATTRIBUTESET2VBUTTON_H
#define CDATASETATTRIBUTESET2VBUTTON_H

#include <QLabel>

#include "../ChaosBaseDatasetAttributeUI.h"
#include "customwidget/CStatePushButton.h"

class CDatasetAttributeSet2VButton:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
    Q_PROPERTY(QString attributeSetValue READ attributeSetValue WRITE setAttributeSetValue)
public:
    explicit CDatasetAttributeSet2VButton(QWidget *parent = nullptr);

    QString attributeSetValue();
    void setAttributeSetValue(QString& new_p_attribute_setValue);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private slots:
    void on_pushButton_clicked(bool cliecked);
    void changeSetCommitted();
private:
    QString p_attribute_setValue;
    CStatePushButton *cstate_push_button;
};


#endif // CDATASETATTRIBUTESET2VBUTTON_H
