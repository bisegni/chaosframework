#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <QWidget>

#include "../data/ScriptListModel.h"
#include "../presenter/PresenterWidget.h"

namespace Ui {
class ScriptManager;
}

class ScriptManager :
        public PresenterWidget {
    Q_OBJECT

public:
    explicit ScriptManager(QWidget *parent = 0);
    ~ScriptManager();

protected:
    bool isClosing();
    void initUI();
private slots:
    void on_pushButtonCreateNewScript_clicked();

    void on_pushButtonDeleteScript_clicked();

    void on_pushButtonStartSearch_clicked();

private:
    ScriptListModel script_list_model;
    Ui::ScriptManager *ui;
};

#endif // SCRIPTMANAGER_H
