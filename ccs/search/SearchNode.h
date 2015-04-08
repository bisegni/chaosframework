#ifndef SEARCHNODE_H
#define SEARCHNODE_H

#include "../presenter/CommandPresenter.h"

#include <QFrame>

namespace Ui {
class SearchNode;
}

class SearchNode : public QFrame
{
    Q_OBJECT
    CommandPresenter *presenter;
public:
    explicit SearchNode(QWidget *parent = 0, CommandPresenter *_presenter = NULL);
    ~SearchNode();

private slots:

    void on_pushButtonStartSearch_clicked();

private:
    Ui::SearchNode *ui;
};

#endif // SEARCHNODE_H
