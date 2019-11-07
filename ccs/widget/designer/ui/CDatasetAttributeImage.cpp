#include "CDatasetAttributeImage.h"

#include <QImageReader>
#include <QPainter>
#include <QDebug>

CDatasetAttributeImage::CDatasetAttributeImage(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    image_loaded(false),
    image_placeholder(QIcon(":/images/image_placeholder.png")),
    image_ptr(new QImage()){}

QSize CDatasetAttributeImage::sizeHint() const {
    return QSize(100, 100);
}

QSize CDatasetAttributeImage::minimumSizeHint() const {
    return QSize(100, 100);
}

void CDatasetAttributeImage::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    if(image_loaded && !image_ptr->isNull()) {
        QImage scaled = image_ptr->scaled(size(), Qt::KeepAspectRatio);
        int deltaX = size().width() - scaled.size().width();
        int deltaY = size().height() - scaled.size().height();
        painter.translate(deltaX / 2, deltaY / 2);
        painter.drawImage(0,0, scaled);
    } else {
        image_placeholder.paint(&painter, rect());
    }
}

void CDatasetAttributeImage::updateOnline(ChaosBaseDatasetUI::OnlineState /*state*/) {}

void CDatasetAttributeImage::updateValue(QVariant variant_value) {
    if(!variant_value.canConvert<QByteArray>()) return;
    QByteArray ba = variant_value.toByteArray();
    image_loaded = image_ptr->loadFromData(ba);
    repaint();
}
