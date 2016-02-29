#include "LogEntryAdvancedSearchTableModel.h"

#include <QDebug>

using namespace chaos::metadata_service_client::api_proxy::logging;

LogEntryAdvancedSearchTableModel::LogEntryAdvancedSearchTableModel(QObject *parent):
    LogEntryTableModel(parent){
    show_source_column = true;
}

//!update log entries for node uid as emitter and log domain list to inclue
void LogEntryAdvancedSearchTableModel::updateEntriesList(const QString& _search_string,
                                                         const LogDomainList& _domain_list,
                                                         const uint64_t start_ts,
                                                         const uint64_t end_ts) {
    domain_list = _domain_list;
    api_submitter.submitApiResult("LogEntryTableModel::load_entry_list",
                                  GET_CHAOS_API_PTR(SearchLogEntry)->execute(_search_string.toStdString(),
                                                                             _domain_list,
                                                                             start_ts,
                                                                             end_ts,
                                                                             0,
                                                                             number_of_max_result));
}
