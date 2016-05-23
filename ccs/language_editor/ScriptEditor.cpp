#include "ScriptEditor.h"
#include "ui_ScriptEditor.h"
#include "LuaHighlighter.h"

#include <QJsonObject>
#include <QJsonDocument>

ScriptEditor::Script::Script() {}

ScriptEditor::Script::Script(const QString& script_json_description) {
    QJsonObject json_result = QJsonDocument::fromVariant(script_json_description).object();
    script_language = json_result["script_language"].toString();
    script_content = json_result["script_content"].toString();
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
}

bool ScriptEditor::isClosing() {
    true;
}

void ScriptEditor::setScript(ScriptEditor::Script& script_info) {
    current_script = script_info;
    ui->scriptEditor->setText(current_script.script_content);
    if(ui->comboBoxLanguageType->currentText().compare(current_script.script_language) != 0) {
        //set new language
        ui->comboBoxLanguageType->setCurrentIndex(ui->comboBoxLanguageType->findText(current_script.script_language));
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
        current_highlighter = new LuaHighlighter(ui->scriptEditor->document());
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
