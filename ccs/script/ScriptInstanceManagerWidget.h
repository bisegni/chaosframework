#ifndef SCRIPTINSTANCEMANAGERWIDGET_H
#define SCRIPTINSTANCEMANAGERWIDGET_H

#include "../utility/WidgetUtility.h"
#include "../data/ScriptInstanceListModel.h"
#include "../api_async_processor/ApiSubmitter.h"

#include <QWidget>
#include <QItemSelection>

namespace Ui {
class ScriptInstanceManagerWidget;
}

class ScriptInstanceManagerWidget :
        public QWidget,
        public ApiHandler,
        protected WidgetUtilityhandler {
    Q_OBJECT

public:
    explicit ScriptInstanceManagerWidget(chaos::service_common::data::script::ScriptBaseDescription& script_description,
                                         QWidget *parent = 0);
    ~ScriptInstanceManagerWidget();
protected:
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void cmActionTrigger(const QString& cm_title,
                         const QVariant& cm_data);
private slots:
    void on_pushButtonSearchInstances_clicked();

    void on_pushButtonAddNew_clicked();

    void on_pushButtonremoveInstance_clicked();

    void selectionChanged(const QItemSelection& selected,
                          const QItemSelection& unselected);

    void bindTypeChanged(chaos::service_common::data::script::ScriptInstance& script_instance);
    void instanceCommitResult(bool achived);
    void on_pushButtonResetChange_clicked();

    void on_pushButtonApplyChange_clicked();

private:
    ApiSubmitter api_submitter;
    WidgetUtility widget_utility;
    Ui::ScriptInstanceManagerWidget *ui;
    ScriptInstanceListModel instance_list_model;
};

#endif // SCRIPTINSTANCEMANAGERWIDGET_H
