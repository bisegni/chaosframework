#ifndef CONTROLUNITEDITOR_H
#define CONTROLUNITEDITOR_H

#include <QWidget>

namespace Ui {
class ControlUnitEditor;
}

class ControlUnitEditor : public QWidget
{
    Q_OBJECT

public:
    explicit ControlUnitEditor(QWidget *parent = 0);
    ~ControlUnitEditor();

private:
    Ui::ControlUnitEditor *ui;
};

#endif // CONTROLUNITEDITOR_H
