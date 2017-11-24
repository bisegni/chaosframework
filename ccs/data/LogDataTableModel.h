#ifndef LOGDATATABLEMODEL_H
#define LOGDATATABLEMODEL_H

#include "ChaosAbstractTableModel.h"

#include <QMap>
#include <QPair>
#include <QVariant>

class LogDataTableModel:
        public ChaosAbstractTableModel {
public:
    LogDataTableModel(QObject *parent = 0);

    void setLogEntry(ChaosSharedPtr<chaos::metadata_service_client::api_proxy::logging::LogEntry> _log_entry_ptr);
    void clear();
protected:
    int getRowCount() const;
    int getColumnCount() const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getCellUserData(int row, int column) const;
    QVariant getTooltipTextForData(int row, int column) const;
    QVariant getTextAlignForData(int row, int column) const;
private:
    QMap<unsigned int, QPair<QString, QVariant> > log_data_map;
};

#endif // LOGDATATABLEMODEL_H
