#include "TreeGroupManager.h"
#include "ui_TreeGroupManager.h"
#include "AddNewDomain.h"

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
    QMap<QString, QVariant> cm_tree_map;
    cm_tree_map.insert(tr("Add New Root"), tr("new"));

    QMap<QString, QVariant> cm_list_map;
    cm_list_map.insert(tr("Select Domain"), tr("select_domain"));

    registerWidgetForContextualMenu(ui->treeViewDomainsTree,
                                    &cm_tree_map,
                                    false);

    ui->listViewDomains->setModel(&domain_list_model);
    connect(ui->listViewDomains->selectionModel(),
            SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
            SLOT(handleListSelectionChanged(QModelIndex,QModelIndex)));

    registerWidgetForContextualMenu(ui->listViewDomains,
                                    &cm_list_map,
                                    false);

    enableWidgetAction(ui->treeViewDomainsTree,
                       tr("Add New Root"),
                       true);

    ui->treeViewDomainsTree->setModel(&tree_model);
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
    } else if(tag.compare("add_domain") == 0) {
        updateDomains();
    }

}

void TreeGroupManager::updateDomains() {
    submitApiResult("get_domains",
                    GET_CHAOS_API_PTR(groups::GetDomains)->execute());
}

void TreeGroupManager::contextualMenuActionTrigger(const QString& cm_title,
                                                   const QVariant& cm_data){
    if(cm_title.compare("Select Domain")==0) {
        ui->labelDomainSelected->setText(cm_data.toString());
        tree_model.loadRootsForDomain(cm_data.toString());
    }
}

void TreeGroupManager::handleListSelectionChanged(const QModelIndex &current_row,
                                                  const QModelIndex &previous_row) {
    if(!current_row.isValid()) {
        enableWidgetAction(ui->listViewDomains,
                           tr("Select Domain"),
                           false);
    } else {
        enableWidgetAction(ui->listViewDomains,
                           tr("Select Domain"),
                           true);
        setWidgetActionData(ui->listViewDomains,
                            tr("Select Domain"),
                            current_row.data());
    }
}
void TreeGroupManager::on_pushButtonUpdateDomainsList_clicked() {
    updateDomains();
}

void TreeGroupManager::on_pushButton_clicked() {
    AddNewDomain add_new_domain_dialog;
    int result = add_new_domain_dialog.exec();
    if(result == 1) {
        //we can create the new domain with one root
        submitApiResult("add_domain",
                        GET_CHAOS_API_PTR(groups::AddNode)->execute(add_new_domain_dialog.getDomainName().toStdString(),
                                                                    add_new_domain_dialog.getRootName().toStdString()));
    }
}
