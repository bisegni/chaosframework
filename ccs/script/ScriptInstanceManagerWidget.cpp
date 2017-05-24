#include "ScriptInstanceManagerWidget.h"
#include "ui_ScriptInstanceManagerWidget.h"
#include "../node/control_unit/ControUnitInstanceEditor.h"
#include "../GlobalServices.h"

#include <QString>
#include <QInputDialog>
#include <QModelIndexList>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy::script;

const QString CM_EDIT_INSTANCE = "Edit Instance";
const QString CM_UPDATE_SOURCE = "Update Source";

ScriptInstanceManagerWidget::ScriptInstanceManagerWidget(ScriptBaseDescription &script_description, QWidget *parent) :
    QWidget(parent),
    widget_utility(this),
    api_submitter(this),
    instance_list_model(script_description),
    ui(new Ui::ScriptInstanceManagerWidget) {
    ui->setupUi(this);
    QHeaderView *headerView = ui->tableViewInstance->horizontalHeader();
    headerView->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableViewInstance->setModel(&instance_list_model);
    connect(ui->tableViewInstance->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));

    QVector< QPair<QString, QVariant> > cm_vec;
    cm_vec.push_back(QPair<QString, QVariant>(CM_EDIT_INSTANCE, QVariant()));
    cm_vec.push_back(QPair<QString, QVariant>(CM_UPDATE_SOURCE, QVariant()));
    ui->tableViewInstance->setContextMenuPolicy(Qt::CustomContextMenu);
    widget_utility.cmRegisterActions(ui->tableViewInstance,
                                     cm_vec);

    //start first search
    on_pushButtonSearchInstances_clicked();
}

ScriptInstanceManagerWidget::~ScriptInstanceManagerWidget() {
    delete ui;
}

void ScriptInstanceManagerWidget::onApiDone(const QString& tag,
                                            QSharedPointer<CDataWrapper> api_result) {
    on_pushButtonSearchInstances_clicked();
}

void ScriptInstanceManagerWidget::cmActionTrigger(const QString& cm_title,
                                                  const QVariant& cm_data) {
    if(cm_title.compare(CM_EDIT_INSTANCE) == 0) {
        QModelIndexList selected_index = cm_data.value<QModelIndexList>();
        foreach (QModelIndex index, selected_index) {
            ControUnitInstanceEditor *w=new ControUnitInstanceEditor(QString(),
                                                                     index.data().toString(),
                                                                     true);
            if(w){w->show();}
        }

    } else if(cm_title.compare(CM_UPDATE_SOURCE) == 0) {
        ChaosStringVector node_vec;
        QModelIndexList selected_index = cm_data.value<QModelIndexList>();
        foreach (QModelIndex index, selected_index) {
            node_vec.push_back(index.data().toString().toStdString());
        }
        api_submitter.submitApiResult("update_source_on_node",
                                      GET_CHAOS_API_PTR(UpdateScriptOnNode)->execute(node_vec,
                                                                                     instance_list_model.getScriptDescription()));
    }
}

void ScriptInstanceManagerWidget::selectionChanged(const QItemSelection& selected,
                                                   const QItemSelection& unselected) {
    bool selection = selected.indexes().size();

    widget_utility.cmActionSetEnable(ui->tableViewInstance,
                                      CM_EDIT_INSTANCE,
                                      selection);
    widget_utility.cmActionSetEnable(ui->tableViewInstance,
                                      CM_UPDATE_SOURCE,
                                      selection);
    ui->pushButtonremoveInstance->setEnabled(selection);
    if(selection) {
        widget_utility.cmActionSetData(ui->tableViewInstance,
                                       CM_EDIT_INSTANCE,
                                       QVariant::fromValue<QModelIndexList>(selected.indexes()));
        widget_utility.cmActionSetData(ui->tableViewInstance,
                                       CM_UPDATE_SOURCE,
                                       QVariant::fromValue<QModelIndexList>(selected.indexes()));
    }
}

void ScriptInstanceManagerWidget::on_pushButtonSearchInstances_clicked() {
    instance_list_model.updateInstanceListForSearchString(ui->lineEditSearchText->text());
}

void ScriptInstanceManagerWidget::on_pushButtonAddNew_clicked() {
    bool ok;
    QString instance_name = QInputDialog::getText(this,
                                                  tr("Script Instance Creation"),
                                                  tr("Name:"),
                                                  QLineEdit::Normal,
                                                  tr("New Intance"),
                                                  &ok);
    if (ok && !instance_name.isEmpty()) {
        api_submitter.submitApiResult("create_instance",
                                      GET_CHAOS_API_PTR(ManageScriptInstance)->execute(instance_list_model.getScriptDescription().unique_id,
                                                                                       instance_list_model.getScriptDescription().name,
                                                                                       instance_name.toStdString(),
                                                                                       true));
    }
}

void ScriptInstanceManagerWidget::on_pushButtonremoveInstance_clicked() {
    ChaosStringVector str_list;
    foreach (QModelIndex index, ui->tableViewInstance->selectionModel()->selectedRows()) {
        str_list.push_back(index.data().toString().toStdString());
    }
    api_submitter.submitApiResult("delete_instance",
                                  GET_CHAOS_API_PTR(ManageScriptInstance)->execute(instance_list_model.getScriptDescription().unique_id,
                                                                                   instance_list_model.getScriptDescription().name,
                                                                                   str_list,
                                                                                   false));
}
