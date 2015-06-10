#ifndef ControlUnitCommandTemplateEditor_H
#define ControlUnitCommandTemplateEditor_H

#include "../data/CommandParameterTableModel.h"

#include <QDialog>
#include <QAbstractButton>
#include <ChaosMetadataServiceClient/ChaosMetadataServiceClient.h>

namespace Ui {
class ControlUnitCommandTemplateEditor;
}

class ControlUnitCommandTemplateEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ControlUnitCommandTemplateEditor(QWidget *parent = 0);
    ~ControlUnitCommandTemplateEditor();
    void setCommandDescription(QSharedPointer<chaos::common::data::CDataWrapper> _command_description);

signals:
    void saveTemplate(boost::shared_ptr<chaos::metadata_service_client::api_proxy::control_unit::CommandTemplate> command_template);

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    boost::shared_ptr<chaos::metadata_service_client::api_proxy::control_unit::CommandTemplate> getTemplateDescription();
    QSharedPointer<chaos::common::data::CDataWrapper> command_description;
    CommandParameterTableModel parameter_table_model;
    Ui::ControlUnitCommandTemplateEditor *ui;
};

#endif // ControlUnitCommandTemplateEditor_H
