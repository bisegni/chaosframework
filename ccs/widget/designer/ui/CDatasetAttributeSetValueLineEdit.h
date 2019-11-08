#ifndef CDATASETATTRIBUTESETVALUELINEEDIT_H
#define CDATASETATTRIBUTESETVALUELINEEDIT_H

#include "../ChaosBaseDatasetAttributeUI.h"
#include <QLineEdit>
#include <QSize>
#include <QPalette>

class CDatasetAttributeSetValueLineEdit:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
     Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask)
public:
    explicit CDatasetAttributeSetValueLineEdit(QWidget *parent = nullptr);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void setInputMask(const QString &new_input_mask);
    QString inputMask() const;
public slots:
    virtual void reset();
private slots:
    void valueUpdated(const QString &);
    void editFinisched();
    void returnPressed();
    void changeSetCommitted();
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private:
    bool value_committed;
    QLineEdit *line_edit;
    QPalette base_line_edit_color;
    QPalette edited_line_edit_color;
    QString p_input_mask;
};
#endif // CDATASETATTRIBUTESETVALUELINEEDIT_H
