#include "LogEntryTableModel.h"

#include <QDateTime>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::logging;

#define TS_TO_STR(ts) QDateTime::fromMSecsSinceEpoch(ts, Qt::LocalTime).toString("dd-MM-yy hh.mm.ss.zzz")

LogEntryTableModel::LogEntryTableModel(QObject *parent):
    ChaosAbstractTableModel(parent),
    ApiHandler(),
    api_submitter(this),
    number_of_max_result(0),
    show_source_column(false){}

//!update log entries for node uid as emitter and log domain list to inclue
void LogEntryTableModel::updateEntriesList(const QString& _node_uid,
                                           const LogDomainList& _domain_list) {
    api_submitter.submitApiResult("LogEntryTableModel::load_entry_list",
                                  GET_CHAOS_API_PTR(logging::GetLogForSourceUID)->execute(_node_uid.toStdString(),
                                                                                          _domain_list,
                                                                                          0,
                                                                                          number_of_max_result));
}

void LogEntryTableModel::onApiDone(const QString& tag,
                                   QSharedPointer<CDataWrapper> api_result) {
    //data received
    beginResetModel();
    helper = logging::GetLogForSourceUID::getHelper(api_result.data());
    //manage sequence id
    endResetModel();
    //emit signal that model has changed
    emit(dataChanged(LogEntryTableModel::index(0,0), LogEntryTableModel::index(helper->getLogEntryListSize(),3)));

}

void LogEntryTableModel::clear() {
    beginResetModel();
    helper.reset();
    endResetModel();
}

ChaosSharedPtr<logging::LogEntry> LogEntryTableModel::getLogEntryForRow(unsigned int row) {
    if(row >= helper->getLogEntryListSize()) return ChaosSharedPtr<logging::LogEntry>();
    return helper->getLogEntryList()[row];
}

void LogEntryTableModel::setMaxResultItem(uint32_t _number_of_max_result) {
    number_of_max_result = _number_of_max_result;
}

int LogEntryTableModel::getRowCount() const {
    return helper.get()?helper->getLogEntryListSize():0;
}

int LogEntryTableModel::getColumnCount() const {
    return show_source_column?4:3;
}

QString LogEntryTableModel::getHeaderForColumn(int column) const {
    QString result;
    if(show_source_column) {
        switch (column) {
        case 0:
            result = tr("Timestamp");
            break;
        case 1:
            result = tr("Emitter");
            break;
        case 2:
            result = tr("Domain");
            break;
        case 3:
            result = tr("Subject");
            break;
        default:
            break;
        }
    }else {
        switch (column) {
        case 0:
            result = tr("Timestamp");
            break;
        case 1:
            result = tr("Domain");
            break;
        case 2:
            result = tr("Subject");
            break;
        default:
            break;
        }
    }
    return result;
}

QVariant LogEntryTableModel::getCellData(int row, int column) const {
    QString result;
    if(show_source_column) {
        switch (column) {
        case 0:
            result = TS_TO_STR(helper->getLogEntryList()[row]->ts);
            break;
        case 1:
            result = QString::fromStdString(helper->getLogEntryList()[row]->source_identifier);
            break;
        case 2:
            result = QString::fromStdString(helper->getLogEntryList()[row]->domain);
            break;
        case 3:
            result = QString::fromStdString(helper->getLogEntryList()[row]->subject);
            break;
        default:
            break;
        }
    }else {
        switch (column) {
        case 0:
            result = TS_TO_STR(helper->getLogEntryList()[row]->ts);
            break;
        case 1:
            result = QString::fromStdString(helper->getLogEntryList()[row]->domain);
            break;
        case 2:
            result = QString::fromStdString(helper->getLogEntryList()[row]->subject);
            break;
        default:
            break;
        }
    }
    return result;
}

QVariant LogEntryTableModel::getCellUserData(int row, int column) const {
    QString result;
    if(show_source_column) {
        switch (column) {
        case 0:
            result = tr("Timestamp");
            break;
        case 1:
            result = tr("Emitter");
            break;
        case 2:
            result = tr("Domain");
            break;
        case 3:
            result = tr("Subject");
            break;
        default:
            break;
        }
    }else {
        switch (column) {
        case 0:
            result = tr("Timestamp");
            break;
        case 1:
            result = tr("Domain");
            break;
        case 2:
            result = tr("Subject");
            break;
        default:
            break;
        }
    }
    return result;
}

QVariant LogEntryTableModel::getTooltipTextForData(int row, int column) const {
    return getCellData(row, column);
}

QVariant LogEntryTableModel::getTextAlignForData(int row, int column) const {
    return Qt::AlignLeft+Qt::AlignVCenter;
}

bool LogEntryTableModel::isCellSelectable(const QModelIndex &index) const {
    return true;
}
