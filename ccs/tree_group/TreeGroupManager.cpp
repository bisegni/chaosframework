#include "TreeGroupManager.h"
#include "ui_TreeGroupManager.h"

#include <QDebug>
#include <QMap>
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
}

bool TreeGroupManager::isClosing() {
    true;
}

void TreeGroupManager::onApiDone(const QString& tag,
                                 QSharedPointer<chaos::common::data::CDataWrapper> api_result) {

}

void TreeGroupManager::contextualMenuActionTrigger(const QString& cm_title,
                                                  const QVariant& cm_data){
    qDebug() << "test";
}
