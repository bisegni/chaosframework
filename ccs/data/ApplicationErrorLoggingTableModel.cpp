#include "ApplicationErrorLoggingTableModel.h"

#include "../error/ErrorManager.h"

ApplicationErrorLoggingTableModel::ApplicationErrorLoggingTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    record_number(0){
    connect(&ErrorManager::getInstance()->signal_proxy,
            SIGNAL(errorEntryUpdated()),
            SLOT(onErrorEntryUpdated()));
}

void ApplicationErrorLoggingTableModel::updateLog() {
    beginResetModel();
    endResetModel();
}

void ApplicationErrorLoggingTableModel::onErrorEntryUpdated() {
    updateLog();
}

int ApplicationErrorLoggingTableModel::getRowCount() const {
    if(ErrorManager::getInstance()->getErrorCount(record_number) != 0) {
        record_number = 0;
    }
    return (int)record_number;
}

int ApplicationErrorLoggingTableModel::getColumnCount() const {
    return 4;
}

QVariant ApplicationErrorLoggingTableModel::getTooltipTextForData(int row,
                                                                  int column) const {
    QString toolTip = QString("<font>");
    toolTip += getCellData(row,
                           column).toString();
    toolTip += QString("</font>");
    return toolTip;
}

QString ApplicationErrorLoggingTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch(column) {
    case 0:
        result = "Timestamp";
        break;
    case 1:
        result = "Code";
        break;
    case 2:
        result = "Message";
        break;
    case 3:
        result = "Domain";
        break;
    default:
        break;
    }
    return result;
}

QVariant ApplicationErrorLoggingTableModel::getCellData(int row, int column) const {
    QVariant result;
    QSharedPointer<ErrorEntry> error_entry;
    if(ErrorManager::getInstance()->getErrorEntryByPosition(error_entry, (record_number-1)-row) != 0){
        return result;
    }
    switch(column) {
    case 0:
        result = error_entry->date_time;
        break;
    case 1:
        result =  error_entry->exception->errorCode;
        break;
    case 2:
        result = QString::fromStdString(error_entry->exception->errorMessage);
        break;
    case 3:
        result = QString::fromStdString(error_entry->exception->errorDomain);
        break;
    default:
        break;
    }
    return result;
}

QVariant ApplicationErrorLoggingTableModel::getUserData(int row, int column) const {
    return getCellData(row, column);
}

bool ApplicationErrorLoggingTableModel::isCellEditable(const QModelIndex &index) const {
    return false;
}

bool ApplicationErrorLoggingTableModel::isCellCheckable(const QModelIndex &index) const {
    return false;
}
