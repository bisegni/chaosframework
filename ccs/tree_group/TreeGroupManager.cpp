#include "TreeGroupManager.h"
#include "ui_TreeGroupManager.h"

#include <QDebug>
#include <QMap>

using namespace chaos::common::data;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;
using namespace chaos::metadata_service_client::api_proxy::node;

TreeGroupManager::TreeGroupManager() :
    PresenterWidget(NULL),
    ui(new Ui::TreeGroupManager) {
    ui->setupUi(this);
}

TreeGroupManager::~TreeGroupManager() {
    delete ui;
}

void TreeGroupManager::initUI() {
    QMap<QString, QVariant> cm_map;
    cm_map.insert(tr("Add New Root"), tr("new"));

    registerWidgetForContextualMenu(ui->treeViewDomainsTree,
                                    &cm_map,
                                    false);

    enableWidgetAction(ui->treeViewDomainsTree,
                       tr("Add New Root"),
                       true);

    ui->listViewDomains->setModel(&domain_list_model);

    updateDomains();
}

bool TreeGroupManager::isClosing() {
    true;
}

void TreeGroupManager::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    qDebug() << "onApiDone";
    PresenterWidget::onApiDone(tag,
                               api_result);

    if(tag.compare("get_domains") == 0) {
       domain_list_model.update(groups::GetDomains::getHelper(api_result.data()));
    }

}

void TreeGroupManager::updateDomains() {
    submitApiResult("get_domains",
                    GET_CHAOS_API_PTR(groups::GetDomains)->execute());
}

void TreeGroupManager::contextualMenuActionTrigger(const QString& cm_title,
                                                  const QVariant& cm_data){
    qDebug() << "test";
}

void TreeGroupManager::on_pushButtonUpdateDomainsList_clicked() {
    updateDomains();
}
