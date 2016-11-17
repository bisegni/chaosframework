#include "TwoLineInformationListItemDelegate.h"

#include <QPainter>
#include <QFontMetrics>
#include <QAbstractItemView>
#include <QSharedPointer>
#include <QMargins>
#include <QPen>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

using namespace chaos::common::data;

TwoLineInformationListItemDelegate::TwoLineInformationListItemDelegate(QWidget *parent):
    QItemDelegate(parent) {

}

QPainterPath TwoLineInformationListItemDelegate::roundRectangle(const QRectF& rect,
                                                                qreal leftTopRadius,
                                                                qreal leftBottomRadius,
                                                                qreal rightTopRadius,
                                                                qreal rightBottomRadius) const {
        QPainterPath path(QPoint(rect.left(), rect.top() + leftTopRadius));
        path.quadTo(rect.left(), rect.top(), rect.left() + leftTopRadius, rect.top());
        path.lineTo(rect.right() - rightTopRadius, rect.top());
        path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + rightTopRadius);
        path.lineTo(rect.right(), rect.bottom() - rightBottomRadius);
        path.quadTo(rect.right(), rect.bottom(), rect.right() - rightBottomRadius, rect.bottom());
        path.lineTo(rect.left() + leftBottomRadius, rect.bottom());
        path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - leftBottomRadius);
        path.closeSubpath();
        return path;
    }
void TwoLineInformationListItemDelegate::paint(QPainter* painter,
                                               const QStyleOptionViewItem& option,
                                               const QModelIndex& index) const {
    if (index.data().canConvert< QSharedPointer<TwoLineInformationItem> >()) {
        QSharedPointer<TwoLineInformationItem> item_to_show = index.data().value< QSharedPointer<TwoLineInformationItem> >();

        painter->save();
        //painter->setRenderHint(QPainter::Antialiasing);
        //painter->setRenderHint(QPainter::HighQualityAntialiasing);
        drawBackground(painter, option, index);

        QStyleOptionViewItem alias_option = option;
        alias_option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        alias_option.font.setPointSize(12);
        alias_option.font.setBold(true);
        QRect alias_rect(alias_option.rect.x(), alias_option.rect.y(), alias_option.rect.width(), alias_option.rect.height()/2);

        painter->fillPath(roundRectangle(alias_rect.marginsRemoved(QMargins(2,2,2,0)),0,0,6,6), QColor(200,200,200, 50));
        //painter->drawRoundedRect(alias_rect.marginsRemoved(QMargins(2,2,2,0)),3,3);
        drawDisplay(painter, alias_option, alias_rect, item_to_show->title);

        QStyleOptionViewItem description_option = option;
        description_option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        description_option.font.setPointSize(10);
        description_option.font.setBold(false);
        description_option.font.setItalic(true);
        QRect description_rect(description_option.rect.x(), description_option.rect.y()+(description_option.rect.height()/2), description_option.rect.width(), description_option.rect.height()/2);
        drawDisplay(painter, description_option, description_rect, item_to_show->description);

        drawFocus(painter, option, option.rect);
        painter->restore();
    } else {
        QItemDelegate::paint(painter, option, index);
    }
}

QWidget *TwoLineInformationListItemDelegate::createEditor(QWidget *parent,
                                                          const QStyleOptionViewItem &option,
                                                          const QModelIndex &index) const {
    return QItemDelegate::createEditor(parent, option, index);
}

void TwoLineInformationListItemDelegate::setEditorData(QWidget *editor,
                                                       const QModelIndex &index) const {
    QItemDelegate::setEditorData(editor, index);
}

void TwoLineInformationListItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                                      const QModelIndex &index) const {
    QItemDelegate::setModelData(editor, model, index);
}

QSize TwoLineInformationListItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                                   const QModelIndex &index) const {
    return QItemDelegate::sizeHint(option, index)*2;
}
