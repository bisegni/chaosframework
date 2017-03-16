#include "JsonEditor.h"
#include "ui_JsonEditor.h"

#define ADD_KEY "Add attribute"

JsonEditor::JsonEditor(QWidget *parent) :
    PresenterWidget(parent),
    ui(new Ui::JsonEditor) {
    ui->setupUi(this);
}

JsonEditor::~JsonEditor() {
    delete ui;
}

void JsonEditor::initUI() {
    ui->treeViewJsonVIew->setModel(&json_tb);
    QHeaderView *headerView = ui->treeViewJsonVIew->header();
    headerView->setSectionResizeMode(0, QHeaderView::Interactive);
    headerView->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(ui->treeViewJsonVIew->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));

    QVector< QPair<QString, QVariant> > cm;
    cm.push_back(QPair<QString, QVariant>(ADD_KEY, QVariant()));
    registerWidgetForContextualMenu(ui->treeViewJsonVIew,
                                    cm,
                                    false);
}

bool JsonEditor::isClosing() {
    return true;
}

void JsonEditor::loadJSONDocument(const QString& json_document) {
    json_tb.loadJson(QByteArray::fromStdString(json_document.toStdString()));
}

void JsonEditor::contextualMenuActionTrigger(const QString& cm_title,
                                             const QVariant& cm_data) {
    if(cm_title.compare(ADD_KEY) == 0) {
        QModelIndex parent_index = cm_data.value<QModelIndex>();
        json_tb.insertNewAttribute(parent_index,
                                   "test key",
                                   QJsonValue::String);
        contextualMenuActionSetData(ui->treeViewJsonVIew,
                                    ADD_KEY,
                                    QVariant());
    }
}

void JsonEditor::selectionChanged(const QItemSelection& selected,
                                  const QItemSelection& deselected) {
    bool can_add_attribute = (selected.size() == 1) || (selected.size() == 0);
    if(can_add_attribute) {
        contextualMenuActionSetData(ui->treeViewJsonVIew,
                                    ADD_KEY,
                                    selected.indexes().first());
    } else {
        contextualMenuActionSetData(ui->treeViewJsonVIew,
                                    ADD_KEY,
                                    QVariant());
    }
    contextualMenuActionSetEnable(ui->treeViewJsonVIew,
                                  ADD_KEY,
                                  can_add_attribute);
}

void JsonEditor::on_pushButtonSaveAction_clicked() {

}
