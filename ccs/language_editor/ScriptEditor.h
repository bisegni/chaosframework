#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H


#include "../presenter/PresenterWidget.h"

#include <QSyntaxHighlighter>

namespace Ui {
class ScriptEditor;
}

class ScriptEditor :
        public PresenterWidget {
    Q_OBJECT

public:
    struct Script {
        QString script_content;
        QString script_language;

        Script();
        Script(const QString& script_json_description);
        Script(const QString& _script_content,
               const QString& _script_language);
        Script(const Script& _script);

        QString getJSONDescription();
    };

    explicit ScriptEditor(QWidget *parent = 0);
    ~ScriptEditor();
protected:
    void initUI();
    bool isClosing();

signals:
    void saveScript(ScriptEditor::Script& script_info);
    void cancel();

public slots:
    void setScript(ScriptEditor::Script& script_info);

private slots:

    void on_pushButtonSaveScript_clicked();

    void on_pushButtonCancelEditing_clicked();

    void on_comboBoxLanguageType_currentIndexChanged(int index);

    void on_pushButtonResetScript_clicked();

private:
    Script current_script;
    QSyntaxHighlighter *current_highlighter;
    Ui::ScriptEditor *ui;
};

#endif // SCRIPTEDITOR_H
