#ifndef UNITSERVEREDITOR_H
#define UNITSERVEREDITOR_H

#include "../../presenter/PresenterWidget.h"

#include <QStringListModel>
#include <QStandardItemModel>
namespace Ui {
class UnitServerEditor;
}

class UnitServerEditor :
        public PresenterWidget
{
    Q_OBJECT
    const QString node_unique_id;
    //list model
    QStringListModel *list_model_cu_type;
    //query proxy
    chaos::metadata_service_client::api_proxy::node::GetNodeDescription *gnd_proxy;

public:
    explicit UnitServerEditor(const QString& _node_unique_id);
    ~UnitServerEditor();

private slots:
    void customMenuRequested(QPoint pos);

    void updateUSInfo();

    void on_pushButtonCreateNewInstance_clicked();

protected:
    void initUI();
    bool canClose();
    void onApiDone(QString tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

private:
    QStandardItemModel *table_model;
    Ui::UnitServerEditor *ui;
};

#endif // UNITSERVEREDITOR_H
