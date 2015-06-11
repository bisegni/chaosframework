#ifndef COMMANDTEMPLATELISTMODEL_H
#define COMMANDTEMPLATELISTMODEL_H


#include "ChaosAbstractListModel.h"

#include <QVector>
#include <QSharedPointer>

#include <chaos/common/data/CDataWrapper.h>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

class CommandTemplateListModel:
        public ChaosAbstractListModel {
    Q_OBJECT

public:
    explicit CommandTemplateListModel(QObject *parent = 0);

    void updateSearchPage(const QSharedPointer<chaos::common::data::CDataWrapper>& template_result_page);
protected:
    int getRowCount() const;
    QVariant getRowData(int row) const;
    QVariant getUserData(int row) const;
private:
    QVector< QSharedPointer<chaos::common::data::CDataWrapper> > page_element;
};

#endif // COMMANDTEMPLATELISTMODEL_H
