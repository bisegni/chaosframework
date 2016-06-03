#include "ComboBoxDelegate.h"

#include <QComboBox>

ComboBoxDelegate::ComboBoxDelegate(QStringList _combo_box_choices,
                                   QObject *parent) :
    QItemDelegate(parent),
    combo_box_choices(_combo_box_choices){}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent,
    const QStyleOptionViewItem &option,
    const QModelIndex &index) const {
    QComboBox *editor = new QComboBox(parent);
    //fill combobox with default value
    editor->addItems(combo_box_choices);
    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor,
                                    const QModelIndex &index) const {
//    QString value = index.model()->data(index, Qt::EditRole).toString();

    QComboBox *comboBox = static_cast<QComboBox*>(editor);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                   const QModelIndex &index) const {
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    model->setData(index, comboBox->currentText(), Qt::EditRole);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor,
    const QStyleOptionViewItem &option, const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}
