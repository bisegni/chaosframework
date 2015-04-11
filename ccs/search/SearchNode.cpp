#include "SearchNode.h"
#include "ui_searchnode.h"

#include "SearchNodeResult.h"

#include <QDebug>

SearchNode::SearchNode(QWidget *parent, CommandPresenter *_presenter):
    QFrame(parent),
    presenter(_presenter),
    ui(new Ui::SearchNode)
{
    ui->setupUi(this);
    QStringList searchTypes;
    searchTypes << "All types" << "Unit server" << "Control unit";
    ui->comboBoxSearchType->addItems(searchTypes);
}

SearchNode::~SearchNode()
{
    delete ui;
}

void SearchNode::on_pushButtonStartSearch_clicked()
{
    int idx =  ui->comboBoxSearchType->currentIndex();
    qDebug() << "Start search with type:" << idx << " and search criteria:" << ui->lineEditSearchCriteria->text();
    presenter->showCommandPresenter(new SearchNodeResult());
}
