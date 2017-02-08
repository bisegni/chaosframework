#ifndef AGENTEDITOR_H
#define AGENTEDITOR_H

#include <QWidget>

namespace Ui {
class AgentEditor;
}

class AgentEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AgentEditor(QWidget *parent = 0);
    ~AgentEditor();

private:
    Ui::AgentEditor *ui;
};

#endif // AGENTEDITOR_H
