#include "CQTableView.h"
#include <QVector>
#include <QHeaderView>
CQTableView::CQTableView(QWidget *parent):
    QTableView(parent){}

void CQTableView::setColumnToContents(quint32 column,
                                      bool  force_to_content) {
    map_col_contents[column] = force_to_content;
}

void CQTableView::resizeEvent(QResizeEvent *event) {
    QTableView::resizeEvent(event);
    for(int col=0;
        col < horizontalHeader()->count();
        col++) {
        if(map_col_contents.count(col) &&
                map_col_contents[col]) {
            resizeColumnToContents(col);
        } else {
            horizontalHeader()->setSectionResizeMode(col, QHeaderView::Stretch);
        }
    }
}
