#ifndef CONTROUNITINSTANCEEDITOR_H
#define CONTROUNITINSTANCEEDITOR_H

#include "../../presenter/PresenterWidget.h"

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class ControUnitInstanceEditor;
}

class ControUnitInstanceEditor :
        public PresenterWidget
{
    Q_OBJECT
    QString unit_server_uid;
    QString control_unit_type;
    QString control_unit_uid;

    bool is_in_editing;

    //table model
    QStandardItemModel *table_model_driver_spec;
    QStandardItemModel *table_model_dataset_attribute_setup;
public:
    //!Create instance without specifing the type
    explicit ControUnitInstanceEditor(const QString& unit_server_uid);
    //!Create instance with unit server and cu type
    explicit ControUnitInstanceEditor(const QString& unit_server_uid,
                                      const QString& control_unit_type);
    //!edit the control unit
    explicit ControUnitInstanceEditor(const QString& unit_server_uid,
                                      const QString& control_unit_type,
                                      const QString& control_unit_uid);
    ~ControUnitInstanceEditor();

protected:
    void initUI();
    bool canClose();
    void onApiDone(QString tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private:
    Ui::ControUnitInstanceEditor *ui;
};

#endif // CONTROUNITINSTANCEEDITOR_H
