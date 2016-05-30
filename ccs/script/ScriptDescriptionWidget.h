#ifndef SCRIPTDESCRIPTIONWIDGET_H
#define SCRIPTDESCRIPTIONWIDGET_H

#include <QWidget>
#include <QTextEdit>

#include "../language_editor/LuaHighlighter.h"
#include "../api_async_processor/ApiSubmitter.h"

namespace Ui {
class ScriptDescriptionWidget;
}

class ScriptDescriptionWidget :
        public QWidget,
        public ApiHandler {
    Q_OBJECT

public:
    explicit ScriptDescriptionWidget(QWidget *parent = 0);
    explicit ScriptDescriptionWidget(const chaos::service_common::data::script::Script& _script, QWidget *parent = 0);
    ~ScriptDescriptionWidget();
    QString getScriptName();
protected:

    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void updateScripUI();
private:
    Ui::ScriptDescriptionWidget *ui;
    QSyntaxHighlighter *current_highlighter;
    ApiSubmitter api_submitter;
    //declre the script serialization wrapper
    chaos::service_common::data::script::ScriptSDWrapper script_wrapper;
};

#endif // SCRIPTDESCRIPTIONWIDGET_H
