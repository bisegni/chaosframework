#ifndef ControlUnitCommandTemplateEditor_H
#define ControlUnitCommandTemplateEditor_H

#include "../data/CommandParameterTableModel.h"

#include <QDialog>

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
private slots:
    void on_buttonBox_accepted();

private:
    CommandParameterTableModel parameter_table_model;
    Ui::ControlUnitCommandTemplateEditor *ui;
};

#endif // ControlUnitCommandTemplateEditor_H
