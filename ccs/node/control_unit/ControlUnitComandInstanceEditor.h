#ifndef CONTROLUNITCOMANDINSTANCEEDITOR_H
#define CONTROLUNITCOMANDINSTANCEEDITOR_H

#include <QDialog>

namespace Ui {
class ControlUnitComandInstanceEditor;
}

class ControlUnitComandInstanceEditor : public QDialog
{
    Q_OBJECT

public:
    explicit ControlUnitComandInstanceEditor(QWidget *parent = 0);
    ~ControlUnitComandInstanceEditor();

private:
    Ui::ControlUnitComandInstanceEditor *ui;
};

#endif // CONTROLUNITCOMANDINSTANCEEDITOR_H
