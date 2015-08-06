#ifndef TwoLineInformationListItemDelegate_H
#define TwoLineInformationListItemDelegate_H

#include "TwoLineInformationItem.h"

#include <QItemDelegate>

class TwoLineInformationListItemDelegate :
        public QItemDelegate {
    Q_OBJECT

public:
    TwoLineInformationListItemDelegate(QWidget *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const Q_DECL_OVERRIDE;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const Q_DECL_OVERRIDE;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                          const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    void setModelData(QWidget *editor, QAbstractItemModel *model,
                      const QModelIndex &index) const Q_DECL_OVERRIDE;

private slots:
   // void commitAndCloseEditor();
private:
    QPainterPath roundRectangle(const QRectF& rect, qreal leftTopRadius, qreal leftBottomRadius, qreal rightTopRadius, qreal rightBottomRadius) const;
};
#endif // TwoLineInformationListItemDelegate_H
