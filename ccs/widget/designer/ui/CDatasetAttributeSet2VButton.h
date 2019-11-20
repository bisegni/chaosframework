#ifndef CDATASETATTRIBUTESET2VBUTTON_H
#define CDATASETATTRIBUTESET2VBUTTON_H

#include <QLabel>

#include "../ChaosBaseDatasetAttributeUI.h"
#include "customwidget/CStatePushButton.h"

class CDatasetAttributeSet2VButton:
        public ChaosBaseDatasetAttributeUI {
    typedef enum {
        StateUndeterminated,
        StateOff,
        StateOn
    } State;
    Q_OBJECT
    Q_PROPERTY(QString attributeSetValueOn READ attributeSetValueOn WRITE setAttributeSetValueOn)
    Q_PROPERTY(QString attributeSetValueOff READ attributeSetValueOff WRITE setAttributeSetValueOff)
public:
    explicit CDatasetAttributeSet2VButton(QWidget *parent = nullptr);

    QString attributeSetValueOn();
    void setAttributeSetValueOn(QString& new_p_attribute_setValue_on);

    QString attributeSetValueOff();
    void setAttributeSetValueOff(QString& new_p_attribute_setValue_off);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;
public slots:
    virtual void reset();
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private slots:
    void on_pushButton_clicked(bool cliecked);
    void changeSetCommitted();
    void changeSetRollback();
private:
    State _internal_state;
    QString p_attribute_setValue_on;
    QString p_attribute_setValue_off;
    CStatePushButton *cstate_push_button;
};


#endif // CDATASETATTRIBUTESET2VBUTTON_H
