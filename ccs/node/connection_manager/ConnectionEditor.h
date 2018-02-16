#ifndef CONNECTIONEDITOR_H
#define CONNECTIONEDITOR_H

#include <QMainWindow>
#include "../../api_async_processor/ApiSubmitter.h"
#include "../../external_lib/QtNodeEditor/FlowScene.hpp"
#include "../../external_lib/QtNodeEditor/DataModelRegistry.hpp"
namespace Ui {
class ConnectionEditor;
}

class ConnectionEditor :
        public QMainWindow,
        protected ApiHandler {
    Q_OBJECT

public:
    explicit ConnectionEditor(QWidget *parent = 0);
    ~ConnectionEditor();
protected:
    //!Api has ben called successfully
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dragLeaveEvent(QDragLeaveEvent* e);
private slots:
    void on_actionControl_Unit_triggered();
    void on_actionSave_triggered();

private:
    ApiSubmitter api_submitter;
    QtNodes::FlowScene scene;
    Ui::ConnectionEditor *ui;
    static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels();
};

#endif // CONNECTIONEDITOR_H
