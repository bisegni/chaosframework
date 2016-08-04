#ifndef EditableSubtypeListWidget_H
#define EditableSubtypeListWidget_H

#include "../common/EditableSubtypeListTableModel.h"

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class EditableSubtypeListWidget;
}

class EditableSubtypeListWidget :
        public QWidget
{
    Q_OBJECT

public:
    explicit EditableSubtypeListWidget(QWidget *parent = 0);
    ~EditableSubtypeListWidget();
    void setSubtypeListPtr(chaos::common::data::structured::DatasetSubtypeList *_editable_subtypes_list);
private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonRemove_clicked();

    void tableSelectionChanged(const QItemSelection & selected, const QItemSelection & deselected);
private:

    EditableSubtypeListTableModel subtypes_table_model;
    Ui::EditableSubtypeListWidget *ui;
};

#endif // EditableSubtypeListWidget_H
