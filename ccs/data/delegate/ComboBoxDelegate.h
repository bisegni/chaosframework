#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QStringList>
#include <QItemDelegate>

class ComboBoxDelegate:
        public QItemDelegate {
public:
    ComboBoxDelegate(QStringList _combo_box_choices, QObject *parent = 0);
    QWidget* createEditor(QWidget *parent,
                          const QStyleOptionViewItem &option,
                          const QModelIndex &index) const;

    void setEditorData(QWidget *editor,
                       const QModelIndex &index) const;

    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const;

    void updateEditorGeometry(QWidget *editor,
                              const QStyleOptionViewItem &option, const QModelIndex & index) const;
private:
    QStringList combo_box_choices;
};

#endif // COMBOBOXDELEGATE_H
