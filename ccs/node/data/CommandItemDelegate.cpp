#include "CommandItemDelegate.h"

CommandItemDelegate::CommandItemDelegate(QWidget *parent):
    QStyledItemDelegate(parent) {

}

void CommandItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                const QModelIndex &index) const {
    //    if (index.data().canConvert<StarRating>()) {
    //        StarRating starRating = qvariant_cast<StarRating>(index.data());

    //        if (option.state & QStyle::State_Selected)
    //            painter->fillRect(option.rect, option.palette.highlight());

    //        starRating.paint(painter, option.rect, option.palette,
    //                         StarRating::ReadOnly);
    //    } else {
    //        QStyledItemDelegate::paint(painter, option, index);
    //    }
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
    //    if (index.data().canConvert<StarRating>()) {
    //        StarRating starRating = qvariant_cast<StarRating>(index.data());
    //        return starRating.sizeHint();
    //    } else {
    //        return QStyledItemDelegate::sizeHint(option, index);
    //    }
}
