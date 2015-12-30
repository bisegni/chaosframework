#ifndef ControlUnitCommandTemplateEditor_H
#define ControlUnitCommandTemplateEditor_H

#include "../data/CommandParameterTableModel.h"
#include "../../presenter/PresenterWidget.h"

#include <QDialog>
#include <QAbstractButton>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class ControlUnitCommandTemplateEditor;
}

class ControlUnitCommandTemplateEditor :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit ControlUnitCommandTemplateEditor(const QString& _command_uid);
    explicit ControlUnitCommandTemplateEditor(const QString& _template_name,
                                              const QString& _command_uid);
    ~ControlUnitCommandTemplateEditor();
    //used for editing and new creation
    void setCommandDescription(QSharedPointer<chaos::common::data::CDataWrapper> _command_description);
    //used for editing
    void setTemplateDescription(QSharedPointer<chaos::common::data::CDataWrapper> _template_description);

public slots:
    void reset();

protected:
    void initUI();
    bool isClosing();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
signals:
    void templateSaved(const QString& tempalte_name,
                       const QString& command_uid);

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButtonCancel_clicked();

    void on_pushButtonReset_clicked();

private:
    bool in_editing;
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::CommandTemplate> getTemplateDescription();
    QSharedPointer<chaos::common::data::CDataWrapper> command_description;
    CommandParameterTableModel parameter_table_model;
    Ui::ControlUnitCommandTemplateEditor *ui;
};

#endif // ControlUnitCommandTemplateEditor_H
