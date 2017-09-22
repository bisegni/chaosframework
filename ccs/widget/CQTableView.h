#ifndef CQTABLEVIEW_H
#define CQTABLEVIEW_H

#include <QTableView>
#include <QMap>

class CQTableView :
        public QTableView {
public:
    CQTableView(QWidget *parent = Q_NULLPTR);
    void setColumnToContents(quint32 column,
                              bool  force_to_content);
protected:
    void resizeEvent(QResizeEvent *event);
private:
    QMap<quint32, bool> map_col_contents;
};

#endif // CQTABLEVIEW_H
