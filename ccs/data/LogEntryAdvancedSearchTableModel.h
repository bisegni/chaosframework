#ifndef LOGENTRYADVANCEDSEARCHTABLEMODEL_H
#define LOGENTRYADVANCEDSEARCHTABLEMODEL_H

#include "LogEntryTableModel.h"
class LogEntryAdvancedSearchTableModel:
public LogEntryTableModel {
public:
    LogEntryAdvancedSearchTableModel(QObject *parent = 0);
    void updateEntriesList(const QString& _search_string,
                           const chaos::metadata_service_client::api_proxy::logging::LogDomainList& _domain_list,
                           const uint64_t start_ts,
                           const uint64_t end_ts);
};

#endif // LOGENTRYADVANCEDSEARCHTABLEMODEL_H
