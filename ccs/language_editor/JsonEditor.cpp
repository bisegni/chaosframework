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
    ui->tableViewJsonEditor->setModel(&json_tb);
    QHeaderView *headerView = ui->tableViewJsonEditor->horizontalHeader();
    headerView->setSectionResizeMode(0, QHeaderView::Interactive);
    headerView->setSectionResizeMode(1, QHeaderView::Stretch);

    connect(ui->tableViewJsonEditor->selectionModel(),
            SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            SLOT(selectionChanged(QItemSelection,QItemSelection)));

    QVector< QPair<QString, QVariant> > cm;
    cm.push_back(QPair<QString, QVariant>(ADD_KEY, QVariant()));
    registerWidgetForContextualMenu(ui->tableViewJsonEditor,
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

    }
}

void JsonEditor::selectionChanged(const QItemSelection& selected,
                                  const QItemSelection& deselected) {
    bool can_add_attribute = (selected.size() == 1);
    contextualMenuActionSetEnable(ui->tableViewJsonEditor,
                                  ADD_KEY,
                                  can_add_attribute);
}
