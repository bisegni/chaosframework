#ifndef SCRIPTDESCRIPTIONWIDGET_H
#define SCRIPTDESCRIPTIONWIDGET_H

#include <QWidget>

namespace Ui {
class ScriptDescriptionWidget;
}

class ScriptDescriptionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScriptDescriptionWidget(QWidget *parent = 0);
    ~ScriptDescriptionWidget();

private slots:
    void on_lineEditScriptName_editingFinished();

private:
    Ui::ScriptDescriptionWidget *ui;
};

#endif // SCRIPTDESCRIPTIONWIDGET_H
