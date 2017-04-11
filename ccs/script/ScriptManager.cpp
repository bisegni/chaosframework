#include "ScriptManager.h"
#include "ui_ScriptManager.h"
#include "CreateNewScriptDialog.h"
#include "ScriptInstanceManagerWidget.h"
#include "../error/ErrorManager.h"

#include <QMap>
#include <QDir>
#include <QDebug>
#include <QFileDialog>

const QString CM_EDIT_INSTANCE = "Instances List";

using namespace chaos::common::data;
using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;

Q_DECLARE_METATYPE(chaos::service_common::data::script::ScriptBaseDescription)

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
    QMap<QString, QVariant> cm_list_map;
    cm_list_map.insert(CM_EDIT_INSTANCE, tr("Edit Instances"));

    registerWidgetForContextualMenu(ui->listViewScriptList,
                                    &cm_list_map,
                                    false);

    ui->listViewScriptList->setModel(&script_list_model);
    ui->pushButtonExport->setEnabled(false);
    connect(ui->listViewScriptList->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(on_scriptListSelectionchanged(QItemSelection,QItemSelection)));

    connect(ui->tabWidgetSourcesDocument,
            SIGNAL(tabCloseRequested(int)),
            SLOT(closeScriptEditorTab(int)));
}

bool ScriptManager::isClosing() {
    return true;
}

void ScriptManager::onApiDone(const QString& tag,
                              QSharedPointer<CDataWrapper> api_result) {
    if(tag.startsWith("sm::export")) {
        int raw_size = 0;
        const QDir destination_path = QDir(tag.right(tag.size()-tag.indexOf("|")-1));
        if(destination_path.exists()){
            chaos::service_common::data::script::ScriptSDWrapper sdw;
            sdw.deserialize(api_result.data());
            QFile export_file(destination_path.filePath(QString("%1.json").arg(QString::fromStdString(sdw().script_description.name))));
            if (export_file.open(QFile::WriteOnly | QFile::Truncate)) {
                QTextStream out(&export_file);
                out << QString::fromStdString(api_result->getJSONString());
            }else{
                ErrorManager::getInstance()->submiteError(-1,
                                                          QString("Error opening file '%1' for write").arg(export_file.fileName()),
                                                          __PRETTY_FUNCTION__);
            }
        } else {
            ErrorManager::getInstance()->submiteError(-2,
                                                      QString("The directory '%1' doesn't exists").arg(destination_path.path()),
                                                      __PRETTY_FUNCTION__);
        }
    } else {
        script_list_model.updateSearch();
    }
}

void ScriptManager::on_pushButtonCreateNewScript_clicked() {
    CreateNewScriptDialog new_script_dialog;

    if (new_script_dialog.exec() == QDialog::Accepted) {
        Script new_script;
        new_script_dialog.fillScript(new_script);
        submitApiResult("sm::saveScript",
                        GET_CHAOS_API_PTR(script::SaveScript)->execute(new_script));
    }
}

void ScriptManager::on_pushButtonDeleteScript_clicked() {
    QModelIndexList selected_script = ui->listViewScriptList->selectionModel()->selectedRows();
    foreach(QModelIndex index, selected_script) {
        QVariant script_sequence_id = index.data(Qt::UserRole);
        QVariant script_name = index.data(Qt::DisplayRole);
        submitApiResult(QString("sm::delete"),
                        GET_CHAOS_API_PTR(script::ManageScriptInstance)->execute(script_sequence_id.toULongLong(),
                                                                                 script_name.toString().toStdString(),
                                                                                 false));
    }
}


void ScriptManager::on_pushButtonStartSearch_clicked() {
    script_list_model.updateSearchString(ui->lineEditSearchString->text());
}

void ScriptManager::on_listViewScriptList_doubleClicked(const QModelIndex &index) {
    //load script editor on tab panel
    QVariant script_sequence_id = index.data(Qt::UserRole);
    QVariant script_name = index.data(Qt::DisplayRole);
    Script script_to_edit;
    script_to_edit.script_description.unique_id = script_sequence_id.toULongLong();
    script_to_edit.script_description.name = script_name.toString().toStdString();

    ScriptDescriptionWidget *new_editor = new ScriptDescriptionWidget(script_to_edit);
    new_editor->setProperty("chaos_widget_type", 0);
    ui->tabWidgetSourcesDocument->addTab(new_editor, script_name.toString());
}

void ScriptManager::on_scriptListSelectionchanged(const QItemSelection &selected,
                                                  const QItemSelection &unselected) {
    bool can_edit_instance = selected.indexes().size() == 1;
    contextualMenuActionSetVisible(ui->listViewScriptList,
                                   CM_EDIT_INSTANCE,
                                   can_edit_instance);
    if(can_edit_instance) {
        ScriptBaseDescription scrpt_desc;
        scrpt_desc.name = selected.indexes().first().data(Qt::DisplayRole).toString().toStdString();
        scrpt_desc.unique_id = selected.indexes().first().data(Qt::UserRole).toULongLong();
        contextualMenuActionSetData(ui->listViewScriptList,
                                    CM_EDIT_INSTANCE,
                                    QVariant::fromValue<chaos::service_common::data::script::ScriptBaseDescription>(scrpt_desc));
    }

    ui->pushButtonExport->setEnabled(selected.indexes().size()>0);
}

void ScriptManager::closeScriptEditorTab(int idx) {
    QWidget *wdg = ui->tabWidgetSourcesDocument->widget(idx);
    if(!wdg) return;

    //remove editor tab
    ui->tabWidgetSourcesDocument->removeTab(idx);

    //delete editor tab
    delete(wdg);
}

void ScriptManager::contextualMenuActionTrigger(const QString& cm_title,
                                                const QVariant& cm_data) {
    if(cm_title.compare(CM_EDIT_INSTANCE) == 0) {
        ScriptBaseDescription scrpt_desc = cm_data.value<ScriptBaseDescription>();
        ScriptInstanceManagerWidget *new_editor = new ScriptInstanceManagerWidget(scrpt_desc);
        new_editor->setProperty("chaos_widget_type", 1);
        ui->tabWidgetSourcesDocument->addTab(new_editor, QString("Instance Editor for %1").arg(QString::fromStdString(scrpt_desc.name)));
    }
}

void ScriptManager::on_pushButtonExport_clicked() {
    QString selected_path = QFileDialog::getExistingDirectory ( this,
                                                                "Select export destination folder");
    if(selected_path.size()) {
        QModelIndexList selected_script = ui->listViewScriptList->selectionModel()->selectedRows();
        foreach(QModelIndex index, selected_script) {
            QVariant script_sequence_id = index.data(Qt::UserRole);
            QVariant script_name = index.data(Qt::DisplayRole);
            ScriptBaseDescription script_description;
            script_description.unique_id = script_sequence_id.toULongLong();
            script_description.name = script_name.toString().toStdString();
            submitApiResult(QString("sm::export|%1").arg(selected_path),
                            GET_CHAOS_API_PTR(script::LoadFullScript)->execute(script_description));
        }
    }
}

void ScriptManager::on_pushButtonImport_clicked() {
//    api_submitter.submitApiResult("ScriptDescriptionWidget::updateScript",
//                                  GET_CHAOS_API_PTR(script::SaveScript)->execute(script_wrapper.dataWrapped()));
    QStringList fileNames;
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setNameFilter(tr("Script JSON Export (*.json)"));
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        qDebug() << fileNames;
    }
}
