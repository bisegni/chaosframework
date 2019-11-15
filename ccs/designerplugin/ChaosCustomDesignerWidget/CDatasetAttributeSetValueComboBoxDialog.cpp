#include "../../widget/designer/ui/CDatasetAttributeSetValueComboBox.h"
#include "CDatasetAttributeSetValueComboBoxDialog.h"

#include <QtDesigner/QDesignerFormWindowInterface>
#include <QtDesigner/QDesignerFormWindowCursorInterface>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QSpacerItem>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

CDatasetAttributeSetValueComboBoxDialog::CDatasetAttributeSetValueComboBoxDialog(CDatasetAttributeSetValueComboBox *_combo_widget, QWidget *parent)
    : QDialog(parent)
    //    , editor(new CDatasetAttributeSetValueComboBox())
    , combo_widget(_combo_widget)
    , buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok
                                     | QDialogButtonBox::Cancel
                                     | QDialogButtonBox::Reset)) {

    QJsonDocument j_doc = QJsonDocument::fromJson(_combo_widget->setup().toUtf8());

    tableView = new QTableView();
    tableView->setModel(new SetValueComboBoxDialogTableModel(j_doc, this));
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    tableView->horizontalHeader()->setStretchLastSection(true);
    tableView->verticalHeader()->hide();
    tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSortingEnabled(true);

    //    editor->setSetup(combo_widget->setup());
    connect(buttonBox->button(QDialogButtonBox::Reset), &QAbstractButton::clicked,
            this, &CDatasetAttributeSetValueComboBoxDialog::resetState);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &CDatasetAttributeSetValueComboBoxDialog::saveState);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(tableView);

    QHBoxLayout *manageLayout = new QHBoxLayout;
    QPushButton *removeButton = new QPushButton(this);
    removeButton->setText("Remove");
    connect(removeButton, &QAbstractButton::clicked, this, &CDatasetAttributeSetValueComboBoxDialog::removeRow);
    manageLayout->addWidget(removeButton);

    QPushButton *addButton = new QPushButton(this);
    addButton->setText("Add");
    connect(addButton, &QAbstractButton::clicked, this, &CDatasetAttributeSetValueComboBoxDialog::addRow);
    manageLayout->addWidget(addButton);

    mainLayout->addLayout(manageLayout);

    mainLayout->addWidget(buttonBox);



    setLayout(mainLayout);
    setWindowTitle(tr("Edit Combobox Label/Value"));
}

QSize CDatasetAttributeSetValueComboBoxDialog::sizeHint() const {
    return QSize(250, 250);
}

void CDatasetAttributeSetValueComboBoxDialog::resetState() {

}

void CDatasetAttributeSetValueComboBoxDialog::addRow(bool checked) {
    table_model->insertRow(table_model->rowCount(QModelIndex()));
}

void CDatasetAttributeSetValueComboBoxDialog::removeRow(bool checked) {
    foreach(QModelIndex i, tableView->selectionModel()->selectedRows()) {
        table_model->removeRow(i.row());
    }
}

void CDatasetAttributeSetValueComboBoxDialog::saveState() {

    if (QDesignerFormWindowInterface *formWindow = QDesignerFormWindowInterface::findFormWindow(combo_widget)) {
        qDebug() << "CDatasetAttributeSetValueComboBoxDialog::saveState";
        formWindow->cursor()->setProperty("setup", table_model->getElementsAsJsonDocument().toJson(QJsonDocument::Compact));
    }
    accept();
}

//tablemodel
SetValueComboBoxDialogTableModel::SetValueComboBoxDialogTableModel(QObject *parent)
    : QAbstractTableModel(parent){}

SetValueComboBoxDialogTableModel::SetValueComboBoxDialogTableModel(const QVector<Element> &_elements, QObject *parent)
    : QAbstractTableModel(parent),
      elements(_elements) {}

SetValueComboBoxDialogTableModel::SetValueComboBoxDialogTableModel(const QJsonDocument& _elements, QObject *parent)
    : QAbstractTableModel(parent) {
    if(_elements.isNull() || !_elements.isArray()) {return;}

    QJsonArray j_arr = _elements.array();
    for (auto v : j_arr) {
        QJsonObject element = v.toObject();
        qDebug() << QString("SetValueComboBoxDialogTableModel -> %1-%2").arg(element["label"].toString()).arg(element["value"].toString());
        Element e;
        e.label = element["label"].toString();
        e.value = element["value"].toVariant();
        elements.push_back(e);
    }
}

int SetValueComboBoxDialogTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : elements.size();
}

int SetValueComboBoxDialogTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : 2;
}

QVariant SetValueComboBoxDialogTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= elements.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &element = elements.at(index.row());

        switch (index.column()) {
        case 0:
            return element.label;
        case 1:
            return element.value;
        default:
            break;
        }
    }
    return QVariant();
}

QVariant SetValueComboBoxDialogTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Label");
        case 1:
            return tr("Value");
        default:
            break;
        }
    }
    return QVariant();
}

bool SetValueComboBoxDialogTableModel::insertRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index)
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        elements.insert(position, { QString(), QVariant() });

    endInsertRows();
    return true;
}

bool SetValueComboBoxDialogTableModel::removeRows(int position, int rows, const QModelIndex &index) {
    Q_UNUSED(index)
    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        elements.removeAt(position);

    endRemoveRows();
    return true;
}

bool SetValueComboBoxDialogTableModel::setData(const QModelIndex &index, const QVariant &value, int role) {
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        auto element = elements.value(row);

        switch (index.column()) {
        case 0:
            element.label = value.toString();
            break;
        case 1:
            element.value = value.toString();
            break;
        default:
            return false;
        }
        elements.replace(row, element);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});
        return true;
    }
    return false;
}

Qt::ItemFlags SetValueComboBoxDialogTableModel::flags(const QModelIndex &index) const {
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}

const QVector<SetValueComboBoxDialogTableModel::Element> &SetValueComboBoxDialogTableModel::getElements() const {
    return elements;
}

QMap<QString, QVariant> SetValueComboBoxDialogTableModel::getElementsAsMap() const {
    QMap<QString, QVariant> result;
    QVectorIterator<Element> it(elements);
    while (it.hasNext()) {
        Element e = it.next();
        result.insert(e.label, e.value);
    }
    return result;
}

QJsonDocument SetValueComboBoxDialogTableModel::getElementsAsJsonDocument() const {
    QJsonDocument doc;
    QJsonArray arr;

    QVectorIterator<Element> it(elements);
    while (it.hasNext()) {
        Element e = it.next();
        QJsonObject o;
        o.insert("label", e.label);
        o.insert("value", QJsonValue::fromVariant(e.value));
        arr.push_back(o);
    }
    doc.setArray(arr);
    return doc;
}
