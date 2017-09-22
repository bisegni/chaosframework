#ifndef SEARCHNODELISTMODEL_H
#define SEARCHNODELISTMODEL_H

#include "ChaosAbstractListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

class SearchNodeListModel:
        public ChaosAbstractListModel,
        protected ApiHandler {

public:
    SearchNodeListModel();
    void searchNode(const QString& search_string,
                    unsigned int search_type,
                    bool is_alive);
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
    bool isRowCheckable(int row) const;
    Qt::CheckState getCheckableState(int row)const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    Qt::DropActions supportedDragActions() const;
    QMimeData *mimeData(const QModelIndexList &indexes) const;
    QStringList mimeTypes() const;
    bool setRowCheckState(const int row, const QVariant& value);
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private:
    ApiSubmitter api_submitter;
    QVector< QSharedPointer<chaos::common::data::CDataWrapper> > result_found;
};

#endif // SEARCHNODELISTMODEL_H
