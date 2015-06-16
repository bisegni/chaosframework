#ifndef COMMANDTEMPLATEINSTANCEEDITOR_H
#define COMMANDTEMPLATEINSTANCEEDITOR_H

#include "../../presenter/PresenterWidget.h"

namespace Ui {
class CommandTemplateInstanceEditor;
}

class CommandTemplateInstanceEditor :
        public PresenterWidget{
    Q_OBJECT

public:
    explicit CommandTemplateInstanceEditor(const QString& _node_uid,
                                           const QString& _template_name,
                                           const QString& _command_uid);
    ~CommandTemplateInstanceEditor();
protected:
    void initUI();
    bool canClose();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private slots:
    void on_pushButtonSubmitInstance_clicked();
    void on_pushButtonClose_clicked();

private:
    //create the user interface for set the requested parameter
    void configureForTemplate(QSharedPointer<chaos::common::data::CDataWrapper> template_description);
    const QString node_uid;
    const QString template_name;
    const QString command_uid;
    Ui::CommandTemplateInstanceEditor *ui;
};

#endif // COMMANDTEMPLATEINSTANCEEDITOR_H
