#include "CDatasetAttributeSetValueLineEdit.h"

#include <QIcon>
#include <QAction>
#include <QHBoxLayout>
#include <QDebug>

CDatasetAttributeSetValueLineEdit::CDatasetAttributeSetValueLineEdit(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    value_committed(false) {
    //configure line edit
    line_edit = new QLineEdit(this);
    line_edit->setAlignment(Qt::AlignCenter);
    line_edit->setClearButtonEnabled(true);
    connect(line_edit, &QLineEdit::textChanged, this, &CDatasetAttributeSetValueLineEdit::valueUpdated);
    connect(line_edit, &QLineEdit::editingFinished, this, &CDatasetAttributeSetValueLineEdit::editFinisched);
    connect(line_edit, &QLineEdit::returnPressed, this, &CDatasetAttributeSetValueLineEdit::returnPressed);

    //add layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setMargin(0);
    layout->addWidget(line_edit);
    setLayout(layout);
    setDatasetType(ChaosBaseDatasetUI::Input);
    base_line_edit_color = line_edit->palette();
    edited_line_edit_color = QPalette(base_line_edit_color);
    edited_line_edit_color.setColor(QPalette::Base, QColor(94,170,255));
    edited_line_edit_color.setColor(QPalette::Foreground, QColor(10,10,10));
}

QSize CDatasetAttributeSetValueLineEdit::sizeHint() const {
    return QSize(40, 16);
}

QSize CDatasetAttributeSetValueLineEdit::minimumSizeHint() const {
    return QSize(40, 16);
}

void CDatasetAttributeSetValueLineEdit::setInputMask(const QString &new_input_mask) {
    p_input_mask = new_input_mask;
}

QString CDatasetAttributeSetValueLineEdit::inputMask() const {
    return p_input_mask;
}

void CDatasetAttributeSetValueLineEdit::reset() {}

void CDatasetAttributeSetValueLineEdit::valueUpdated(const QString &/*text*/) {
    line_edit->setPalette(edited_line_edit_color);
    value_committed = false;
}

void CDatasetAttributeSetValueLineEdit:: returnPressed() {
    qDebug("returnPressed");
    if(line_edit->text().size()){
        //emit signal for update data
        emit attributeChangeSetUpdated(deviceID(),
                                       attributeName(),
                                       QVariant(line_edit->text()));
        value_committed = true;
    } else {
        //clear changeset
        emit attributeChangeSetClear(deviceID(),
                                     attributeName());
    }

}

void CDatasetAttributeSetValueLineEdit::editFinisched() {
    qDebug("editFinisched");
    if(!value_committed || (line_edit->text().size() == 0)){
        line_edit->setPalette(base_line_edit_color);
    }
    line_edit->clearFocus();
}

void CDatasetAttributeSetValueLineEdit::changeSetCommitted() {
    qDebug()<< "CDatasetAttributeSetValueLineEdit::changeSetCommitted" << deviceID() << ":" <<attributeName();
    value_committed = false;
    line_edit->setText(QString());
    editFinisched();
}

void CDatasetAttributeSetValueLineEdit::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {}

void CDatasetAttributeSetValueLineEdit::updateValue(QVariant variant_value) {
    line_edit->setPlaceholderText(variant_value.toString());
}
