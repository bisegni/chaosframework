#ifndef CONNECTIONEDITOR_H
#define CONNECTIONEDITOR_H

#include <QMainWindow>

#include "../../external_lib/QtNodeEditor/FlowScene.hpp"
#include "../../external_lib/QtNodeEditor/DataModelRegistry.hpp"
namespace Ui {
class ConnectionEditor;
}

class ConnectionEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit ConnectionEditor(QWidget *parent = 0);
    ~ConnectionEditor();

private:
    QtNodes::FlowScene scene;
    Ui::ConnectionEditor *ui;

    static std::shared_ptr<QtNodes::DataModelRegistry> registerDataModels();
};

#endif // CONNECTIONEDITOR_H
