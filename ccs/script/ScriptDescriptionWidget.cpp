#include "ScriptDescriptionWidget.h"
#include "ui_ScriptDescriptionWidget.h"
#include "../GlobalServices.h"
#include "../language_editor/LuaLanguageSupport.h"
#include "../language_editor/CLINGLanguageSupport.h"
#include "../tree_group/TreeGroupManager.h"

#include <QMap>
#include <QDebug>
#include <QVariant>
#include <QStatusBar>

using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;

#define CM_ADD_CHAOS_WRAPPER    "Add chaos wrapper"
#define CM_ADD_LAUNCH_PHASE     "Add launch handler"
#define CM_ADD_START_PHASE      "Add start handler"
#define CM_ADD_STEP_PHASE       "Add step handler"
#define CM_ADD_STOP_PHASE       "Add stop handler"
#define CM_ADD_DEINIT_PHASE     "Add deinit handler"
#define CM_ADD_IA_CHANGED_PHASE "Add input attribute changed handler"

#define SHOW_WIDGET(x)\
{if(x){x->show();}}

ScriptDescriptionWidget::ScriptDescriptionWidget(QWidget *parent) :
    QWidget(parent),
    widget_utility(this),
    ui(new Ui::ScriptDescriptionWidget),
    api_submitter(this){
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitter->setSizes(sizes);

    languages.insert("Lua", QSharedPointer<LanguageEditorSupport>(new LuaLanguageSupport()));
    languages.insert("C++ [CLING]", QSharedPointer<LanguageEditorSupport>(new CLINGLanguageSupport()));
    updateLanguages();

    //set the model for the dataset managment
    ui->tableViewDataset->setModel(&editable_dataset_table_model);
    ui->tableViewDataset->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->tableViewDataset->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SIGNAL(handleModelSelectionChanged(QItemSelection,QItemSelection)));

    editable_dataset_table_model.setDatasetAttributeList(&script_wrapper.dataWrapped().dataset_attribute_list);

    ui->listViewClassifications->setModel(&classification_model);
    connect(ui->listViewClassifications->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleModelSelectionChanged(QItemSelection,QItemSelection)));

    ui->listViewExecutionPools->setModel(&execution_pool_model);
    connect(ui->listViewExecutionPools->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleModelSelectionChanged(QItemSelection,QItemSelection)));

    QVector< QPair<QString, QVariant> > cm_vec;
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_CHAOS_WRAPPER, QVariant(LanguageEditorSupport::ChaosWrapper)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_LAUNCH_PHASE, QVariant(LanguageEditorSupport::LaunchHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_START_PHASE, QVariant(LanguageEditorSupport::StartHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_STEP_PHASE, QVariant(LanguageEditorSupport::StepHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_STOP_PHASE, QVariant(LanguageEditorSupport::StopHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_DEINIT_PHASE, QVariant(LanguageEditorSupport::TerminateHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_IA_CHANGED_PHASE, QVariant(LanguageEditorSupport::AttributeChangedHHandler)));
    ui->textEditSourceCode->setContextMenuPolicy(Qt::CustomContextMenu);
    widget_utility.cmRegisterActions(ui->textEditSourceCode,
                                     cm_vec);

    //update script
    api_submitter.submitApiResult("ScriptDescriptionWidget::loadFullScript",
                                  GET_CHAOS_API_PTR(script::LoadFullScript)->execute(script_wrapper.dataWrapped().script_description));
}

ScriptDescriptionWidget::ScriptDescriptionWidget(const Script &_script,
                                                 QWidget *parent):
    QWidget(parent),
    widget_utility(this),
    ui(new Ui::ScriptDescriptionWidget),
    api_submitter(this),
    script_wrapper(_script){
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitter->setSizes(sizes);

    languages.insert("Lua", QSharedPointer<LanguageEditorSupport>(new LuaLanguageSupport()));
    languages.insert("C++ [CLING]", QSharedPointer<LanguageEditorSupport>(new CLINGLanguageSupport()));
    updateLanguages();

    //set the model for the dataset managment
    ui->tableViewDataset->setModel(&editable_dataset_table_model);
    ui->tableViewDataset->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->tableViewDataset->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleModelSelectionChanged(QItemSelection,QItemSelection)));

    ui->listViewClassifications->setModel(&classification_model);
    connect(ui->listViewClassifications->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleModelSelectionChanged(QItemSelection,QItemSelection)));

    ui->listViewExecutionPools->setModel(&execution_pool_model);
    connect(ui->listViewExecutionPools->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(handleModelSelectionChanged(QItemSelection,QItemSelection)));
    QVector< QPair<QString, QVariant> > cm_vec;
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_CHAOS_WRAPPER, QVariant(LanguageEditorSupport::ChaosWrapper)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_LAUNCH_PHASE, QVariant(LanguageEditorSupport::LaunchHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_START_PHASE, QVariant(LanguageEditorSupport::StartHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_STEP_PHASE, QVariant(LanguageEditorSupport::StepHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_STOP_PHASE, QVariant(LanguageEditorSupport::StopHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_DEINIT_PHASE, QVariant(LanguageEditorSupport::TerminateHandler)));
    cm_vec.push_back(QPair<QString, QVariant>(CM_ADD_IA_CHANGED_PHASE, QVariant(LanguageEditorSupport::AttributeChangedHHandler)));
    ui->textEditSourceCode->setContextMenuPolicy(Qt::CustomContextMenu);
    widget_utility.cmRegisterActions(ui->textEditSourceCode,
                                     cm_vec);
    //update script
    api_submitter.submitApiResult("ScriptDescriptionWidget::loadFullScript",
                                  GET_CHAOS_API_PTR(script::LoadFullScript)->execute(script_wrapper.dataWrapped().script_description));
}

ScriptDescriptionWidget::~ScriptDescriptionWidget() {
    delete ui;
}

void ScriptDescriptionWidget::updateLanguages(){
    ui->comboBoxsScirptLanguage->clear();
    for(QMap< QString, QSharedPointer<LanguageEditorSupport> >::iterator liter = languages.begin(),
        lend = languages.end();
        liter != lend;
        liter++) {
            ui->comboBoxsScirptLanguage->addItem(liter.key());
    }
}

QString ScriptDescriptionWidget::getScriptName() {
    return QString::fromStdString(script_wrapper.dataWrapped().script_description.name);
}

void ScriptDescriptionWidget::onApiDone(const QString& tag,
                                        QSharedPointer<chaos::common::data::CDataWrapper> api_result) {
    //deserialize the script information
    if(tag.compare("ScriptDescriptionWidget::loadFullScript") == 0) {
        script_wrapper.deserialize(api_result.data());
        QMetaObject::invokeMethod(this,
                                  "updateScripUI",
                                  Qt::QueuedConnection);
    }else if(tag.compare("ScriptDescriptionWidget::updateScript") == 0) {
        //set on statu bar that the save operation hase been achieved
        //((MainWindow*)window())->statusBar()->showMessage(QString("%1 has been saved").arg(QString::fromStdString(script_wrapper.dataWrapped().script_description.name)), 5000);
    }
}

void ScriptDescriptionWidget::cmActionTrigger(const QString& cm_title,
                                              const QVariant& cm_data) {
    if(!language_support.data()) return;
    QTextCursor text_cursor = QTextCursor(ui->textEditSourceCode->document());
    int current_position = text_cursor.position();

    LanguageEditorSupport::LanguageHandler handler_type = static_cast<LanguageEditorSupport::LanguageHandler>(cm_data.toUInt());
    ui->textEditSourceCode->insertPlainText(language_support->getCodeForHandler(handler_type));
    if(handler_type == LanguageEditorSupport::ChaosWrapper) {
        text_cursor.movePosition(QTextCursor::Start);
    } else  {
        text_cursor.movePosition(QTextCursor::End);
    }
    text_cursor.setPosition(current_position);
}

void ScriptDescriptionWidget::updateScripUI() {
    QStringList string_list;
    language_support.clear();

    ui->toolBox->setCurrentIndex(0);
    ui->comboBoxsScirptLanguage->setCurrentIndex(ui->comboBoxsScirptLanguage->findText(QString::fromStdString(script_wrapper.dataWrapped().script_description.language)));
    ui->lineEditScriptName->setText(QString::fromStdString(script_wrapper.dataWrapped().script_description.name));
    ui->plainTextEditScriptDescirption->setPlainText(QString::fromStdString(script_wrapper.dataWrapped().script_description.description));
    //ashow source code
    ui->textEditSourceCode->setPlainText(QString::fromStdString(script_wrapper.dataWrapped().script_content));
    qDebug() << QString::fromStdString(script_wrapper.dataWrapped().script_content);

    updateTextEditorFeatures();

    //update table dataset model
    editable_dataset_table_model.setDatasetAttributeList(&script_wrapper.dataWrapped().dataset_attribute_list);
    //update classification
    for(ChaosStringVectorIterator c_it = script_wrapper.dataWrapped().classification_list.begin(),
        c_end = script_wrapper.dataWrapped().classification_list.end();
        c_it != c_end;
        c_it++) {
        string_list << QString::fromStdString(*c_it);
    }
    classification_model.setStringList(string_list);

    //update execution pools
    string_list.clear();
    for(ChaosStringVectorIterator c_it = script_wrapper.dataWrapped().execution_pool_list.begin(),
        c_end = script_wrapper.dataWrapped().execution_pool_list.end();
        c_it != c_end;
        c_it++) {
        string_list << QString::fromStdString(*c_it);
    }
    execution_pool_model.setStringList(string_list);
}

void ScriptDescriptionWidget::fillScriptWithGUIValues() {
    script_wrapper.dataWrapped().script_description.name = ui->lineEditScriptName->text().toStdString();
    script_wrapper.dataWrapped().script_description.description = ui->plainTextEditScriptDescirption->document()->toPlainText().toStdString();
    script_wrapper.dataWrapped().script_description.language = ui->comboBoxsScirptLanguage->currentText().toStdString();
    script_wrapper.dataWrapped().script_content = ui->textEditSourceCode->document()->toPlainText().toStdString();
    script_wrapper.dataWrapped().classification_list.clear();
    foreach (QString classification, classification_model.stringList()) {
        script_wrapper.dataWrapped().classification_list.push_back(classification.toStdString());
    }

    script_wrapper.dataWrapped().execution_pool_list.clear();
    foreach (QString ep, execution_pool_model.stringList()) {
        script_wrapper.dataWrapped().execution_pool_list.push_back(ep.toStdString());
    }
}

void ScriptDescriptionWidget::updateTextEditorFeatures() {
    const QString language = ui->comboBoxsScirptLanguage->currentText();
    QMap< QString, QSharedPointer<LanguageEditorSupport> >::iterator liter = languages.find(language);
    if(liter == languages.end()) return;
    language_support = liter.value();
    language_support->getHiglighter(ui->textEditSourceCode->document());

}

void ScriptDescriptionWidget::on_comboBoxsScirptLanguage_currentIndexChanged(int index) {
    updateTextEditorFeatures();
}

void ScriptDescriptionWidget::on_pushButtonSaveScript_clicked() {
    //update script class description
    fillScriptWithGUIValues();

    //save script
    api_submitter.submitApiResult("ScriptDescriptionWidget::updateScript",
                                  GET_CHAOS_API_PTR(script::SaveScript)->execute(script_wrapper.dataWrapped()));
}

void ScriptDescriptionWidget::on_pushButtonUpdateAll_clicked() {
    api_submitter.submitApiResult("ScriptDescriptionWidget::loadFullScript",
                                  GET_CHAOS_API_PTR(script::LoadFullScript)->execute(script_wrapper.dataWrapped().script_description));
}

void ScriptDescriptionWidget::on_pushButtonAddAttributeToDataset_clicked() {
    editable_dataset_table_model.addNewDatasetAttribute();
}

void ScriptDescriptionWidget::on_pushButtonremoveAttributeToDataset_clicked() {
    foreach (QModelIndex index, ui->tableViewDataset->selectionModel()->selectedRows()) {
        editable_dataset_table_model.removeElementFromDatasetAtIndex(index.row());
    }
}

void ScriptDescriptionWidget::on_tableViewDataset_doubleClicked(const QModelIndex &index) {
    editable_dataset_table_model.editDatasetAttributeAtIndex(index.row());
}

void ScriptDescriptionWidget::handleModelSelectionChanged(const QItemSelection& selected,const QItemSelection& deselected) {
    QObject* sndr = sender();
    if(sndr == ui->tableViewDataset->selectionModel()) {
        ui->pushButtonremoveAttributeToDataset->setEnabled(selected.indexes().size());
    } else if(sndr == ui->listViewClassifications->selectionModel()) {
        ui->pushButtonRemoveSelectedClass->setEnabled(selected.indexes().size());
    }else if(sndr == ui->listViewExecutionPools->selectionModel()) {
        ui->pushButtonRemoveSelectedExecutionPools->setEnabled(selected.indexes().size());
    }
}

void ScriptDescriptionWidget::on_pushButtonSelectClass_clicked() {
    TreeGroupManager *selection_group_presenter = new TreeGroupManager(true, "class_selection");
    if(selection_group_presenter) {
        connect(selection_group_presenter,
                SIGNAL(selectedPath(QString,QStringList)),
                SLOT(selectedGroupPath(QString,QStringList)));
        SHOW_WIDGET(selection_group_presenter)
    }
}

void ScriptDescriptionWidget::selectedGroupPath(const QString &selection_tag,
                                                const QStringList& selected_groups) {
    //se seletced class
    if(selection_tag.compare("class_selection") == 0) {
        classification_model.setStringList(selected_groups);
    } else if(selection_tag.compare("pool_selection") == 0) {
        execution_pool_model.setStringList(selected_groups);
    }
}

void ScriptDescriptionWidget::on_pushButtonRemoveSelectedClass_clicked() {
    ui->listViewClassifications->setUpdatesEnabled(false);
    QModelIndexList selected_list = ui->listViewClassifications->selectionModel()->selectedRows();
    for(int idx  = selected_list.count()-1;
        idx >= 0;
        idx--) {
        classification_model.removeRow(selected_list.at(idx).row());
    }
    ui->listViewClassifications->setUpdatesEnabled(true);
}

void ScriptDescriptionWidget::on_pushButtonSelectExecutionPools_clicked() {
    TreeGroupManager *selection_group_presenter = new TreeGroupManager(true, "pool_selection");
    if(selection_group_presenter) {
        connect(selection_group_presenter,
                SIGNAL(selectedPath(QString,QStringList)),
                SLOT(selectedGroupPath(QString,QStringList)));
        SHOW_WIDGET(selection_group_presenter)
    }
}

void ScriptDescriptionWidget::on_pushButtonRemoveSelectedExecutionPools_clicked() {
    ui->listViewExecutionPools->setUpdatesEnabled(false);
    QModelIndexList selected_list = ui->listViewExecutionPools->selectionModel()->selectedRows();
    for(int idx  = selected_list.count()-1;
        idx >= 0;
        idx--) {
        execution_pool_model.removeRow(selected_list.at(idx).row());
    }
    ui->listViewExecutionPools->setUpdatesEnabled(true);
}
