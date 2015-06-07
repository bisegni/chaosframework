#include "CommandItemDelegate.h"
#include "CommandDescription.h"

#include <QPainter>
#include <QFontMetrics>
#include <QAbstractItemView>
#include <QSharedPointer>
#include <QMargins>
#include <QPen>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataWrapper.h>

using namespace chaos::common::data;

CommandItemDelegate::CommandItemDelegate(QWidget *parent):
    QItemDelegate(parent) {

}

void CommandItemDelegate::paint(QPainter *painter,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
    QSharedPointer<CommandDescription> command_description = index.data().value< QSharedPointer<CommandDescription> >();

    painter->save();
    drawBackground(painter, option, index);

    QStyleOptionViewItem alias_option = option;
    alias_option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    alias_option.font.setPointSize(12);
    alias_option.font.setBold(true);
    QRect alias_rect(alias_option.rect.x(), alias_option.rect.y(), alias_option.rect.width(), alias_option.rect.height()/2);

    painter->setBrush(QColor(200,200,200, 50));
    painter->setPen(QPen(QColor(200,200,200, 50), 0));

    painter->drawRoundedRect(alias_rect.marginsRemoved(QMargins(2,2,2,0)), 5,5);
    drawDisplay(painter, alias_option, alias_rect, command_description->alias);

    QStyleOptionViewItem description_option = option;
    description_option.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
    description_option.font.setPointSize(10);
    description_option.font.setBold(false);
    description_option.font.setItalic(true);
    QRect description_rect(description_option.rect.x(), description_option.rect.y()+(description_option.rect.height()/2), description_option.rect.width(), description_option.rect.height()/2);
    drawDisplay(painter, description_option, description_rect, command_description->description);

    drawFocus(painter, option, option.rect);
    painter->restore();
}

QWidget *CommandItemDelegate::createEditor(QWidget *parent,
                                           const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const {
    //    if (index.data().canConvert<StarRating>()) {
    //        StarEditor *editor = new StarEditor(parent);
    //        connect(editor, SIGNAL(editingFinished()),
    //                this, SLOT(commitAndCloseEditor()));
    //        return editor;
    //    } else {
    //        return QStyledItemDelegate::createEditor(parent, option, index);
    //    }
}

void CommandItemDelegate::commitAndCloseEditor() {
    // StarEditor *editor = qobject_cast<StarEditor *>(sender());
    // emit commitData(editor);
    // emit closeEditor(editor);
}

void CommandItemDelegate::setEditorData(QWidget *editor,
                                        const QModelIndex &index) const {
    //    if (index.data().canConvert<StarRating>()) {
    //        StarRating starRating = qvariant_cast<StarRating>(index.data());
    //        StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
    //        starEditor->setStarRating(starRating);
    //    } else {
    //        QStyledItemDelegate::setEditorData(editor, index);
    //    }
}

void CommandItemDelegate::setModelData(QWidget *editor, QAbstractItemModel *model,
                                       const QModelIndex &index) const {
    //    if (index.data().canConvert<StarRating>()) {
    //        StarEditor *starEditor = qobject_cast<StarEditor *>(editor);
    //        model->setData(index, QVariant::fromValue(starEditor->starRating()));
    //    } else {
    //        QStyledItemDelegate::setModelData(editor, model, index);
    //    }
}

QSize CommandItemDelegate::sizeHint(const QStyleOptionViewItem &option,
                                    const QModelIndex &index) const {
    return QItemDelegate::sizeHint(option, index)*2;
}
