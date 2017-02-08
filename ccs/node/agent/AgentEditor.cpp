#include "AgentEditor.h"
#include "ui_AgentEditor.h"

AgentEditor::AgentEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AgentEditor)
{
    ui->setupUi(this);
}

AgentEditor::~AgentEditor()
{
    delete ui;
}
