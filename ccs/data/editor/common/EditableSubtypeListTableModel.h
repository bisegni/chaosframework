#ifndef EditableSubtypeListTableModel_H
#define EditableSubtypeListTableModel_H

#include "../../ChaosAbstractTableModel.h"

#include <chaos_service_common/data/dataset/DatasetAttribute.h>

class EditableSubtypeListTableModel:
public ChaosAbstractTableModel {
public:
    EditableSubtypeListTableModel(QObject *parent=0);
    void setSubtypesList(std::vector<unsigned int> *subtype_list);
    void addNew();
    void removeAtIndex(unsigned int idx);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getCheckeable(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    bool setCellData(const QModelIndex &index, const QVariant &value);
    bool isCellEditable(const QModelIndex &index) const;
    bool isCellCheckable(const QModelIndex &index) const;
    QVariant getTextAlignForData(int row, int column) const;
private:
    std::vector<unsigned int> *subtype_list_ptr;
};

#endif // EditableSubtypeListTableModel_H
