#ifndef SCRIPTINSTANCELISTMODEL_H
#define SCRIPTINSTANCELISTMODEL_H

#include "ChaosAbstractTableModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

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
    void updateInstanceListForSearchString(const QString& search_string);

    const chaos::service_common::data::script::ScriptBaseDescription& getScriptDescription();
private:
    ApiSubmitter api_submitter;
    //description of the script parent of the found instance
    chaos::service_common::data::script::ScriptBaseDescription script_description;
    //list of the current pag contained the found instances
    std::vector<chaos::service_common::data::script::ScriptInstance> ni_list_wrapper;
};

#endif // SCRIPTINSTANCELISTMODEL_H
