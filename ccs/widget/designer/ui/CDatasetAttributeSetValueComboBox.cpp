#include "CDatasetAttributeSetValueComboBox.h"

#include <QIcon>
#include <QAction>
#include <QHBoxLayout>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
CDatasetAttributeSetValueComboBox::CDatasetAttributeSetValueComboBox(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent) {
    //configure line edit
    combo_box = new QComboBox(this);
    QObject::connect(combo_box, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &CDatasetAttributeSetValueComboBox::currentIndexChanged);
    //        connect(line_edit, &QLineEdit::editingFinished, this, &CDatasetAttributeSetValueLineEdit::editFinisched);
    //        connect(line_edit, &QLineEdit::returnPressed, this, &CDatasetAttributeSetValueLineEdit::returnPressed);

    //add layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setMargin(0);
    layout->addWidget(combo_box);
    setLayout(layout);
    setDatasetType(ChaosBaseDatasetUI::Input);
}


QSize CDatasetAttributeSetValueComboBox::sizeHint() const {
    return QSize(120, 16);
}

QSize CDatasetAttributeSetValueComboBox::minimumSizeHint() const {
    return QSize(40, 16);
}

void CDatasetAttributeSetValueComboBox::setSetup(QString new_setup) {
    qDebug() << "CDatasetAttributeSetValueComboBox::setSetup";
    combo_box->blockSignals(true);
    p_setup = new_setup;
    combo_box->clear();
    map_value_label.clear();
    //load label into combo box
    QJsonDocument j_doc = QJsonDocument::fromJson(new_setup.toUtf8());
    if(j_doc.isNull() || !j_doc.isArray()) {return;}

    QJsonArray j_arr = j_doc.array();
    for (auto v : j_arr) {
        QJsonObject element = v.toObject();
        qDebug() << QString("CDatasetAttributeSetValueComboBox::setSetup -> %1-%2").arg(element["label"].toString()).arg(element["value"].toString());
        combo_box->addItem(element["label"].toString(), element["value"].toVariant());
        map_value_label.insert(element["value"].toVariant(), element["label"].toString());
    }
    combo_box->blockSignals(false);
}

QString CDatasetAttributeSetValueComboBox::setup() {
    qDebug() << "CDatasetAttributeSetValueComboBox::setup";
    return p_setup;
}

void CDatasetAttributeSetValueComboBox::reset() {}


void CDatasetAttributeSetValueComboBox::changeSetCommitted() {
    qDebug() << "CDatasetAttributeSetValueComboBox::changeSetCommitted" << deviceID() << ":" <<attributeName();
     combo_box->setStyleSheet("QComboBox {}");
    //    value_committed = false;
    //    line_edit->setText(QString());
    //    editFinisched();
}

void CDatasetAttributeSetValueComboBox::currentIndexChanged(int index) {
    combo_box->setStyleSheet("QComboBox { background-color: rgb(94,170,255); foreground-color: rgb(10,10,10);}");
    //called whrn user select an index
    emit attributeChangeSetUpdated(deviceID(),
                                   attributeName(),
                                   combo_box->itemData(index));
}

void CDatasetAttributeSetValueComboBox::updateOnline(ChaosBaseDatasetUI::OnlineState state) {

}

void CDatasetAttributeSetValueComboBox::updateValue(QVariant variant_value) {
    //check if the combo box has the value
    combo_box->blockSignals(true);
    if(!map_value_label.contains(variant_value)) {
        combo_box->setCurrentIndex(-1);
    } else {
        combo_box->setCurrentText(map_value_label.find(variant_value).value());
    }
    combo_box->blockSignals(false);
}


//void CDatasetAttributeSetValueComboBox:: returnPressed() {
//    qDebug("returnPressed");
//    if(line_edit->text().size()){
//        //emit signal for update data
//        emit attributeChangeSetUpdated(deviceID(),
//                                       attributeName(),
//                                       QVariant(line_edit->text()));
//        value_committed = true;
//    } else {
//        //clear changeset
//        emit attributeChangeSetClear(deviceID(),
//                                     attributeName());
//    }
//}
