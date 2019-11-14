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
    //        connect(combo_box, &QComboBox::currentIndexChanged, this, &CDatasetAttributeSetValueComboBox::currentIndexChanged);
    //        connect(line_edit, &QLineEdit::editingFinished, this, &CDatasetAttributeSetValueLineEdit::editFinisched);
    //        connect(line_edit, &QLineEdit::returnPressed, this, &CDatasetAttributeSetValueLineEdit::returnPressed);

    //add layout
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setSpacing(-1);
    layout->setMargin(0);
    layout->addWidget(combo_box);
    setLayout(layout);
    setDatasetType(ChaosBaseDatasetUI::Input);
    //        base_line_edit_color = line_edit->palette();
    //        edited_line_edit_color = QPalette(base_line_edit_color);
    //        edited_line_edit_color.setColor(QPalette::Base, QColor(94,170,255));
    //        edited_line_edit_color.setColor(QPalette::Foreground, QColor(10,10,10));
}


QSize CDatasetAttributeSetValueComboBox::sizeHint() const {
    return QSize(120, 16);
}

QSize CDatasetAttributeSetValueComboBox::minimumSizeHint() const {
    return QSize(40, 16);
}

void CDatasetAttributeSetValueComboBox::setSetup(QString new_setup) {
    qDebug() << "CDatasetAttributeSetValueComboBox::setSetup";
    p_setup = new_setup;
    combo_box->clear();
    //load label into combo box
    QJsonDocument j_doc = QJsonDocument::fromJson(new_setup.toUtf8());
    if(j_doc.isNull() || !j_doc.isArray()) {return;}

    QJsonArray j_arr = j_doc.array();
    for (auto v : j_arr) {
        QJsonObject element = v.toObject();
        qDebug() << QString("CDatasetAttributeSetValueComboBox::setSetup -> %1-%2").arg(element["label"].toString()).arg(element["value"].toString());
        combo_box->addItem(element["label"].toString(), element["value"].toVariant());
    }
    //    QMap<QString, QVariant> lable_values = new_setup.toMap();
    //    for (QMap<QString, QVariant>::iterator i = lable_values.begin(); i != lable_values.end(); ++i) {
    //        combo_box->addItem(i.key(), i.value());
    //    }
}

QString CDatasetAttributeSetValueComboBox::setup() {
    qDebug() << "CDatasetAttributeSetValueComboBox::setup";
    return p_setup;
}

void CDatasetAttributeSetValueComboBox::reset() {}


void CDatasetAttributeSetValueComboBox::changeSetCommitted() {
    qDebug()<< "CDatasetAttributeSetValueComboBox::changeSetCommitted" << deviceID() << ":" <<attributeName();
    //    value_committed = false;
    //    line_edit->setText(QString());
    //    editFinisched();
}

void CDatasetAttributeSetValueComboBox::currentIndexChanged(int index) {
    //called whrn user select an index
}

void CDatasetAttributeSetValueComboBox::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {}

void CDatasetAttributeSetValueComboBox::updateValue(QVariant variant_value) {
    //    line_edit->setPlaceholderText(variant_value.toString());
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
