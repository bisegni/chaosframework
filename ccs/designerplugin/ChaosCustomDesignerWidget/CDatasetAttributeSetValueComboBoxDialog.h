#ifndef CDATASETATTRIBUTESETVALUECOMBOBOXDIALOG_H
#define CDATASETATTRIBUTESETVALUECOMBOBOXDIALOG_H

#include <QMap>
#include <QDialog>
#include <QVariant>
#include <QTableView>
#include <QAbstractTableModel>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
QT_END_NAMESPACE

class CDatasetAttributeSetValueComboBox;
class SetValueComboBoxDialogTableModel;

class CDatasetAttributeSetValueComboBoxDialog :
        public QDialog {
    Q_OBJECT

public:
    explicit CDatasetAttributeSetValueComboBoxDialog(CDatasetAttributeSetValueComboBox *_combo_widget = nullptr, QWidget *parent = nullptr);

    QSize sizeHint() const override;

private slots:
    void resetState();
    void saveState();
    void addRow(bool checked);
    void removeRow(bool checked);
private:
    QTableView *tableView;
    SetValueComboBoxDialogTableModel *table_model;
    //    CDatasetAttributeSetValueComboBox *editor;
    CDatasetAttributeSetValueComboBox *combo_widget;
    QDialogButtonBox *buttonBox;
    QMap<QString, QVariant> combo_label_value;
};


class SetValueComboBoxDialogTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct Element {
        QString label;
        QVariant value;

        bool operator==(const Element &other) const {
            return label == other.label && value == other.value;
        }
    };

    SetValueComboBoxDialogTableModel(QObject *parent = nullptr);
    SetValueComboBoxDialogTableModel(const QVector<Element> &contacts, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    const QVector<Element> &getElements() const;
    QMap<QString, QVariant> getElementsAsMap() const;
private:
    QVector<Element> elements;
};
#endif // CDATASETATTRIBUTESETVALUECOMBOBOXDIALOG_H
