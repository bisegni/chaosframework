#include "ScriptInstanceManagerWidget.h"
#include "ui_ScriptInstanceManagerWidget.h"

#include <QInputDialog>

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy::script;

ScriptInstanceManagerWidget::ScriptInstanceManagerWidget(ScriptBaseDescription &script_description, QWidget *parent) :
    QWidget(parent),
    api_submitter(this),
    instance_list_model(script_description),
    ui(new Ui::ScriptInstanceManagerWidget) {
    ui->setupUi(this);

    ui->listView->setModel(&instance_list_model);

    connect(ui->listView->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));

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

void ScriptInstanceManagerWidget::selectionChanged(const QItemSelection& selected,
                                                   const QItemSelection& unselected) {
    bool can_remove = selected.indexes().size();

    ui->pushButtonremoveInstance->setEnabled(can_remove);
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
                                      GET_CHAOS_API_PTR(ManageScriptInstance)->execute(instance_list_model.getScriptDescription().name,
                                                                                       instance_name.toStdString(),
                                                                                       true));
    }
}

void ScriptInstanceManagerWidget::on_pushButtonremoveInstance_clicked() {
    ChaosStringList str_list;
    foreach (QModelIndex index, ui->listView->selectionModel()->selectedRows()) {
        str_list.push_back(index.data().toString().toStdString());
    }
    api_submitter.submitApiResult("delete_instance",
                                  GET_CHAOS_API_PTR(ManageScriptInstance)->execute(instance_list_model.getScriptDescription().name,
                                                                                   str_list,
                                                                                   false));
}
