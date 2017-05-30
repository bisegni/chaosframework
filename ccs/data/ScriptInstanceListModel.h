#ifndef SCRIPTINSTANCELISTMODEL_H
#define SCRIPTINSTANCELISTMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

#include <QSet>

class ScriptInstanceListModel :
        public ChaosAbstractTableModel,
        protected ApiHandler {
    Q_OBJECT
protected:
    int getRowCount() const;
    int getColumnCount() const;
    bool isCellSelectable(const QModelIndex &index) const;
    bool isCellEditable(const QModelIndex &index) const;
    QString getHeaderForColumn(int column) const;
    QVariant getCellData(int row, int column) const;
    QVariant getCellUserData(int row, int column) const;
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void onApiError(const QString& tag,
                    QSharedPointer<chaos::CException> api_exception);
    void onApiTimeout(const QString& tag);
    bool setCellData(const QModelIndex &index, const QVariant &value);
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row,
                      int column,
                      const QModelIndex &parent);
    bool canDropMimeData(const QMimeData *data,
                         Qt::DropAction action,
                         int row,
                         int column,
                         const QModelIndex &parent);
    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDropActions() const;
    QStringList mimeTypes() const;
public:
    ScriptInstanceListModel(const chaos::service_common::data::script::ScriptBaseDescription& _script_description,
                            QObject *parent=0);
    void updateInstanceListForSearchString(const QString& _search_string);
    void commitChange();
    void rollbackChange();
    const chaos::service_common::data::script::ScriptBaseDescription& getScriptDescription();

signals:
    void bindTypeChanged(chaos::service_common::data::script::ScriptInstance& script_instance);
    void commitDone(bool commit_result);
private:
    ApiSubmitter api_submitter;
    QString search_string;
    void startSearch();
    //description of the script parent of the found instance
    chaos::service_common::data::script::ScriptBaseDescription script_description;
    //list of the current pag contained the found instances
    QSet<unsigned int> changed_instance;
    std::vector<chaos::service_common::data::script::ScriptInstance> ni_list_wrapper;
    std::vector<chaos::service_common::data::script::ScriptInstance> original_ni_list_wrapper;
};

#endif // SCRIPTINSTANCELISTMODEL_H
