#ifndef TREEGROUPMANAGER_H
#define TREEGROUPMANAGER_H

#include <QWidget>

#include "DomainListModel.h"
#include "../presenter/PresenterWidget.h"

namespace Ui {
class TreeGroupManager;
}

class TreeGroupManager :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit TreeGroupManager();
    ~TreeGroupManager();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    void updateDomains();
private slots:
    void on_pushButtonUpdateDomainsList_clicked();

private:
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);
    Ui::TreeGroupManager *ui;
    DomainListModel domain_list_model;
};

#endif // TREEGROUPMANAGER_H
