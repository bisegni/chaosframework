#ifndef SCRIPTDESCRIPTIONWIDGET_H
#define SCRIPTDESCRIPTIONWIDGET_H

#include <QWidget>
#include <QVector>
#include <QTextEdit>
#include <QItemSelection>
#include <QStringListModel>
#include "../language_editor/LuaHighlighter.h"
#include "../api_async_processor/ApiSubmitter.h"
#include "../data/EditableDatasetTableModel.h"
#include "../data/EditableScriptVariableTableModel.h"

namespace Ui {
class ScriptDescriptionWidget;
}

class ScriptDescriptionWidget :
        public QWidget,
        public ApiHandler {
    Q_OBJECT

    void updateTextEditorFeatures();
    void fillScriptWithGUIValues();
public:
    explicit ScriptDescriptionWidget(QWidget *parent = 0);
    explicit ScriptDescriptionWidget(const chaos::service_common::data::script::Script& _script, QWidget *parent = 0);
    ~ScriptDescriptionWidget();
    QString getScriptName();

protected:

    void onApiDone(const QString& tag,
                   QSharedPointer<chaos::common::data::CDataWrapper> api_result);
private slots:
    void updateScripUI();
    void on_comboBoxsScirptLanguage_currentIndexChanged(int index);

    void on_pushButtonSaveScript_clicked();

    void on_pushButtonUpdateAll_clicked();

    void on_pushButtonAddAttributeToDataset_clicked();

    void on_tableViewDataset_doubleClicked(const QModelIndex &index);

    void datasetSelectionChanged(const QItemSelection& selected,const QItemSelection& deselected);

    void on_pushButtonAddVariable_clicked();

    void on_pushButtonremoveAttributeToDataset_clicked();

    void on_pushButtonSelectClass_clicked();

    void selectedGroupPath(const QStringList& selected_groups);
private:
    Ui::ScriptDescriptionWidget *ui;
    QStringListModel classification_model;
    EditableDatasetTableModel editable_dataset_table_model;
    EditableScriptVariableTableModel editable_variable_table_model;
    QSyntaxHighlighter *current_highlighter;
    ApiSubmitter api_submitter;
    //declre the script serialization wrapper
    chaos::service_common::data::script::ScriptSDWrapper script_wrapper;
};

#endif // SCRIPTDESCRIPTIONWIDGET_H
