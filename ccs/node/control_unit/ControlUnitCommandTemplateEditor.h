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
    explicit ControlUnitCommandTemplateEditor();
    explicit ControlUnitCommandTemplateEditor(const QString& _template_name,
                                              const QString& _command_uid);
    ~ControlUnitCommandTemplateEditor();
    //used for editing and new creation
    void setCommandDescription(QSharedPointer<chaos::common::data::CDataWrapper> _command_description);
    //used for editing
    void setTemplateDescription(QSharedPointer<chaos::common::data::CDataWrapper> _template_description);
    void reset();

protected:
    void initUI();
    bool canClose();
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
    //!Api has been give an error
    void onApiError(const QString& tag,
                            QSharedPointer<chaos::CException> api_exception);

    //! api has gone in timeout
    void onApiTimeout(const QString& tag);
signals:
    void templateSaved(const QString& tempalte_name,
                       const QString& command_uid);

private slots:
    void on_pushButtonReset_clicked();
    void on_pushButtonSave_clicked();

    void on_pushButtonCancel_clicked();

private:
    bool in_editing;
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::node::CommandTemplate> getTemplateDescription();
    QSharedPointer<chaos::common::data::CDataWrapper> command_description;
    CommandParameterTableModel parameter_table_model;
    Ui::ControlUnitCommandTemplateEditor *ui;
};

#endif // ControlUnitCommandTemplateEditor_H
