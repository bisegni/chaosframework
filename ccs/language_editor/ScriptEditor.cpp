#include "ScriptEditor.h"
#include "ui_ScriptEditor.h"

#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

ScriptEditor::Script::Script() {}

ScriptEditor::Script::Script(const QString& script_json_description) {
    QJsonObject json_result;
    QJsonDocument json_doc = QJsonDocument::fromJson(script_json_description.toUtf8());
    if(!json_doc.isNull()) {
        if(json_doc.isObject()) {
            json_result = json_doc.object();
        } else {
            qDebug() << "Document is not an object" << endl;
        }
    } else {
        qDebug() << "Invalid JSON...\n" << endl;
    }
    if(json_result.contains("script_language")) {
        script_language = json_result["script_language"].toString();
    }
    if(json_result.contains("script_content")) {
        script_content = json_result["script_content"].toString();
    }
}

ScriptEditor::Script::Script(const QString& _script_content,
                             const QString& _script_language):
    script_content(_script_content),
    script_language(_script_language){}

ScriptEditor::Script::Script(const Script& _script):
    script_content(_script.script_content),
    script_language(_script.script_language){}

QString ScriptEditor::Script::getJSONDescription() {
    QJsonObject json_result;
    json_result["script_language"] = script_language;
    json_result["script_content"] = script_content;

    QJsonDocument doc(json_result);
    return QString(doc.toJson(QJsonDocument::Compact));
}

ScriptEditor::ScriptEditor(QWidget *parent) :
    PresenterWidget(parent),
    current_highlighter(NULL),
    ui(new Ui::ScriptEditor) {
    ui->setupUi(this);

}

ScriptEditor::~ScriptEditor() {
    delete ui;
}

void ScriptEditor::initUI() {
    //initilize all
    on_comboBoxLanguageType_currentIndexChanged(ui->comboBoxLanguageType->currentIndex());
}

bool ScriptEditor::isClosing() {
    return true;
}

void ScriptEditor::setScript(ScriptEditor::Script& script_info) {
    current_script = script_info;
    ui->scriptEditor->setText(current_script.script_content);
    if(ui->comboBoxLanguageType->currentText().compare(current_script.script_language) != 0) {
        //set new language
        ui->comboBoxLanguageType->setCurrentIndex(ui->comboBoxLanguageType->findText(current_script.script_language));
        on_comboBoxLanguageType_currentIndexChanged(ui->comboBoxLanguageType->currentIndex());
    }
}

void ScriptEditor::on_pushButtonSaveScript_clicked() {
    current_script.script_content = ui->scriptEditor->toPlainText();
    emit(saveScript(current_script));
}

void ScriptEditor::on_pushButtonCancelEditing_clicked() {
    emit(cancel());
}

void ScriptEditor::on_comboBoxLanguageType_currentIndexChanged(int index) {
    switch(index) {
    case 0:

        break;

    default:
        break;
    }
    current_script.script_language = ui->comboBoxLanguageType->currentText();
}

void ScriptEditor::on_pushButtonResetScript_clicked() {
    switch(ui->comboBoxLanguageType->currentIndex()) {
    case 0:
        //lua

        break;

    default:
        break;
    }
}
