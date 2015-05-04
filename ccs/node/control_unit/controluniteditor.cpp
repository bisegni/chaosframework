#include "controluniteditor.h"
#include "ui_controluniteditor.h"

ControlUnitEditor::ControlUnitEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ControlUnitEditor)
{
    ui->setupUi(this);
}

ControlUnitEditor::~ControlUnitEditor()
{
    delete ui;
}
