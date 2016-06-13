#include "ScriptDescriptionWidget.h"
#include "ui_ScriptDescriptionWidget.h"
#include "../MainWindow.h"
#include "../GlobalServices.h"
#include "../language_editor/LuaHighlighter.h"
#include "../tree_group/TreeGroupManager.h"

#include <QDebug>
#include <QStatusBar>

using namespace chaos::service_common::data::script;
using namespace chaos::metadata_service_client::api_proxy;

ScriptDescriptionWidget::ScriptDescriptionWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptDescriptionWidget),
    current_highlighter(NULL),
    api_submitter(this){
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitter->setSizes(sizes);

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

    //update script
    api_submitter.submitApiResult("ScriptDescriptionWidget::loadFullScript",
                                  GET_CHAOS_API_PTR(script::LoadFullScript)->execute(script_wrapper.dataWrapped().script_description));
}

ScriptDescriptionWidget::ScriptDescriptionWidget(const Script &_script,
                                                 QWidget *parent):
    QWidget(parent),
    ui(new Ui::ScriptDescriptionWidget),
    current_highlighter(NULL),
    api_submitter(this),
    script_wrapper(_script){
    ui->setupUi(this);
    QList<int> sizes;
    sizes << (size().width()*1/5) << (size().width()*4/5);
    ui->splitter->setSizes(sizes);

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
    //update script
    api_submitter.submitApiResult("ScriptDescriptionWidget::loadFullScript",
                                  GET_CHAOS_API_PTR(script::LoadFullScript)->execute(script_wrapper.dataWrapped().script_description));
}

ScriptDescriptionWidget::~ScriptDescriptionWidget() {
    if(current_highlighter) delete(current_highlighter);
    delete ui;
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
    }
}

void ScriptDescriptionWidget::updateScripUI() {
    QStringList string_list;
    //set on statu bar that the save operation hase been achieved
    ((MainWindow*)window())->statusBar()->showMessage(QString("%1 has been saved").arg(QString::fromStdString(script_wrapper.dataWrapped().script_description.name)), 5000);

    if(current_highlighter) {
        delete(current_highlighter);
        current_highlighter = NULL;
    }
    ui->lineEditScriptName->setText(QString::fromStdString(script_wrapper.dataWrapped().script_description.name));
    ui->plainTextEditScriptDescirption->setPlainText(QString::fromStdString(script_wrapper.dataWrapped().script_description.description));
    ui->comboBoxsScirptLanguage->setCurrentIndex(ui->comboBoxsScirptLanguage->findText(QString::fromStdString(script_wrapper.dataWrapped().script_description.language)));
    //ashow source code
    ui->textEditSourceCode->setPlainText(QString::fromStdString(script_wrapper.dataWrapped().script_content));

    updateTextEditorFeatures();

    //update table dataset model
    editable_dataset_table_model.setDatasetAttributeList(&script_wrapper.dataWrapped().dataset_attribute_list);
    //update classification
    for(ChaosStringListIterator c_it = script_wrapper.dataWrapped().classification_list.begin(),
        c_end = script_wrapper.dataWrapped().classification_list.end();
        c_it != c_end;
        c_it++) {
        string_list << QString::fromStdString(*c_it);
    }
    classification_model.setStringList(string_list);

    //update execution pools
    for(ChaosStringListIterator c_it = script_wrapper.dataWrapped().execution_pool_lis.begin(),
        c_end = script_wrapper.dataWrapped().execution_pool_lis.end();
        c_it != c_end;
        c_it++) {
        string_list << QString::fromStdString(*c_it);
    }
    classification_model.setStringList(string_list);
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

    foreach (QString ep, execution_pool_model.stringList()) {
        script_wrapper.dataWrapped().classification_list.push_back(ep.toStdString());
    }
}

void ScriptDescriptionWidget::updateTextEditorFeatures() {
    int index = ui->comboBoxsScirptLanguage->currentIndex();
    if ( index != -1 ) { // -1 for not found
        switch(index){
        case 0:
            current_highlighter = new LuaHighlighter(ui->textEditSourceCode->document());
            break;
        }
    }
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
        ui->pushButtonremoveAttributeToDataset->setEnabled(selected.size());
    } else if(sndr == ui->listViewClassifications->selectionModel()) {
        ui->pushButtonRemoveSelectedClass->setEnabled(selected.size());
    }
}

void ScriptDescriptionWidget::on_pushButtonSelectClass_clicked() {
    TreeGroupManager *selection_group_presenter = new TreeGroupManager(true, "class_selection");
    if(selection_group_presenter) {
        connect(selection_group_presenter,
                SIGNAL(selectedPath(QString,QStringList)),
                SLOT(selectedGroupPath(QString,QStringList)));
        GlobalServices::getInstance()->presenter()->showCommandPresenter(selection_group_presenter);
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
    foreach (QModelIndex index, ui->listViewClassifications->selectionModel()->selectedRows()) {
        classification_model.removeRow(index.row());
    }
}

void ScriptDescriptionWidget::on_pushButtonSelectExecutionPools_clicked() {
    TreeGroupManager *selection_group_presenter = new TreeGroupManager(true, "pool_selection");
    if(selection_group_presenter) {
        connect(selection_group_presenter,
                SIGNAL(selectedPath(QString,QStringList)),
                SLOT(selectedGroupPath(QString,QStringList)));
        GlobalServices::getInstance()->presenter()->showCommandPresenter(selection_group_presenter);
    }
}

void ScriptDescriptionWidget::on_pushButtonRemoveSelectedExecutionPools_clicked() {
    foreach (QModelIndex index, ui->listViewExecutionPools->selectionModel()->selectedRows()) {
        execution_pool_model.removeRow(index.row());
    }
}
