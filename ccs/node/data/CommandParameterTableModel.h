#ifndef COMMANDPARAMETERTABLEMODEL_H
#define COMMANDPARAMETERTABLEMODEL_H

#include "ChaosAbstractTableModel.h"
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QVector>

class CommandParameterTableModel:
        public ChaosAbstractTableModel {
    Q_OBJECT
public:
    CommandParameterTableModel(QObject *parent = 0);
    ~CommandParameterTableModel();
    void updateAttribute(const QSharedPointer<chaos::common::data::CDataWrapper>& command_description);
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
    QVariant getBackgroundForData(int row, int column) const;
    QVariant getTextColorForData(int row, int column) const;
    bool setCellData(const QModelIndex &index, const QVariant &value);
    bool isCellEditable(const QModelIndex &index) const;
private:
    //contains the attribute
    QVector< QSharedPointer<chaos::common::data::CDataWrapper> >command_attribute_description;

};
#endif // COMMANDPARAMETERTABLEMODEL_H
