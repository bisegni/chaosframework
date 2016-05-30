#include "ScriptManager.h"
#include "ui_ScriptManager.h"
#include "CreateNewScriptDialog.h"

#include <QDebug>

using namespace chaos::metadata_service_client::api_proxy;

ScriptManager::ScriptManager(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::ScriptManager) {
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitterMain->setSizes(sizes);
}

ScriptManager::~ScriptManager() {
    delete ui;
}


void ScriptManager::initUI() {
    ui->listViewScriptList->setModel(&script_list_model);
    connect(ui->tabWidgetSourcesDocument,
            SIGNAL(tabCloseRequested(int)),
            SLOT(closeScriptEditorTab(int)));
}

bool ScriptManager::isClosing() {
    return true;
}

void ScriptManager::onApiDone(const QString& tag,
                              QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    script_list_model.updateSearch();
}

void ScriptManager::on_pushButtonCreateNewScript_clicked() {
    CreateNewScriptDialog new_script_dialog;

    if (new_script_dialog.exec() == QDialog::Accepted) {
        chaos::service_common::data::script::Script new_script;
        new_script_dialog.fillScript(new_script);
        submitApiResult("ScriptManager::saveScript",
                        GET_CHAOS_API_PTR(script::SaveScript)->execute(new_script));
    }
}

void ScriptManager::on_pushButtonDeleteScript_clicked() {

}


void ScriptManager::on_pushButtonStartSearch_clicked() {
    script_list_model.updateSearchString(ui->lineEditSearchString->text());
}

void ScriptManager::on_listViewScriptList_doubleClicked(const QModelIndex &index) {
    //load script editor on tab panel
    QVariant script_sequence_id = index.data(Qt::UserRole);
    QVariant script_name = index.data(Qt::DisplayRole);
    chaos::service_common::data::script::Script script_to_edit;
    script_to_edit.script_description.unique_id = script_sequence_id.toULongLong();
    script_to_edit.script_description.name = script_name.toString().toStdString();

    ScriptDescriptionWidget *new_editor = new ScriptDescriptionWidget(script_to_edit);
    ui->tabWidgetSourcesDocument->addTab(new_editor, script_name.toString());
}

void ScriptManager::closeScriptEditorTab(int idx) {
    ScriptDescriptionWidget *editor_to_remove = static_cast<ScriptDescriptionWidget*>(ui->tabWidgetSourcesDocument->widget(idx));
    //remove editor tab
    ui->tabWidgetSourcesDocument->removeTab(idx);

    //delete editor tab
    delete(editor_to_remove);
}
