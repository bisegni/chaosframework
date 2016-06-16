#ifndef SCRIPTMANAGER_H
#define SCRIPTMANAGER_H

#include <QSet>
#include <QWidget>
#include <QSharedDataPointer>

#include "ScriptDescriptionWidget.h"
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
    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void on_pushButtonCreateNewScript_clicked();

    void on_pushButtonDeleteScript_clicked();

    void on_pushButtonStartSearch_clicked();

    void on_listViewScriptList_doubleClicked(const QModelIndex &index);

    void on_scriptListSelectionchanged(const QItemSelection &selected,
                                       const QItemSelection &unselected);

    void closeScriptEditorTab(int idx);
private:
    void contextualMenuActionTrigger(const QString& cm_title,
                                     const QVariant& cm_data);

    ScriptListModel script_list_model;
    Ui::ScriptManager *ui;
};

#endif // SCRIPTMANAGER_H
