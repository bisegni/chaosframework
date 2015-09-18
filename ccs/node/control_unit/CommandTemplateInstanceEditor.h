#ifndef COMMANDTEMPLATEINSTANCEEDITOR_H
#define COMMANDTEMPLATEINSTANCEEDITOR_H

#include "../../data/CommandReader.h"
#include "../../widget/CDSAttrQLineEdit.h"
#include "../../presenter/PresenterWidget.h"

#include <QMap>
#include <QEvent>

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
public slots:
    void submitInstance();
protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::TemplateSubmission> getTempalteSubmissionTask();

    bool eventFilter(QObject *object, QEvent *event);
private:
    //create the user interface for set the requested parameter
    void configureForTemplate(QSharedPointer<chaos::common::data::CDataWrapper> template_description,
                              QSharedPointer<chaos::common::data::CDataWrapper>  command_description);
    const QString node_uid;
    const QString template_name;
    const QString command_uid;

    QMap<QString, CDSAttrQLineEdit*> map_attr_name_value_editor;
    Ui::CommandTemplateInstanceEditor *ui;
    bool close_after_submition;
};

#endif // COMMANDTEMPLATEINSTANCEEDITOR_H
