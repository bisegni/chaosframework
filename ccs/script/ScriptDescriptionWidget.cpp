#include "ScriptDescriptionWidget.h"
#include "ui_ScriptDescriptionWidget.h"

#include "../language_editor/LuaHighlighter.h"


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
    script_wrapper.deserialize(api_result.data());
    QMetaObject::invokeMethod(this,
                              "updateScripUI",
                              Qt::QueuedConnection);
}

void ScriptDescriptionWidget::updateScripUI() {
    if(current_highlighter) delete(current_highlighter);
    ui->lineEditScriptName->setText(QString::fromStdString(script_wrapper.dataWrapped().script_description.name));
    ui->plainTextEditScriptDescirption->setPlainText(QString::fromStdString(script_wrapper.dataWrapped().script_description.description));
    int index = ui->comboBoxsScirptLanguage->findData(QString::fromStdString(script_wrapper.dataWrapped().script_description.language));
    if ( index != -1 ) { // -1 for not found
        ui->comboBoxsScirptLanguage->setCurrentIndex(index);
        switch(index){
        case 0:
            current_highlighter = new LuaHighlighter(ui->textEditSourceCode->document());
            break;
        }
    }

    //ashow source code
    ui->textEditSourceCode->setText(QString::fromStdString(script_wrapper.dataWrapped().script_content));
}
