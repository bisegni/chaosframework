#include "CDatasetAttributeImage.h"

#include <QImageReader>
#include <QPainter>

//#include <chaos/common/data/CDataBuffer.h>

CDatasetAttributeImage::CDatasetAttributeImage(QWidget *parent):
    ChaosBaseDatasetAttributeUI(parent),
    image_loaded(false),
    image_placeholder(QIcon(":/images/image_placeholder.png")),
    image_ptr(new QImage()){}

QSize CDatasetAttributeImage::sizeHint() const
{
    return QSize(100, 100);
}

QSize CDatasetAttributeImage::minimumSizeHint() const
{
    return QSize(100, 100);
}

void CDatasetAttributeImage::paintEvent(QPaintEvent */*event*/) {
    QPainter painter(this);
    if(image_loaded && !image_ptr->isNull()) {

        int deltaX = size().width() - image_ptr->size().width();
        int deltaY = size().height() - image_ptr->size().height();

        // Just apply coordinates transformation to draw where we need.
        painter.translate(deltaX / 2, deltaY / 2);
        painter.drawImage(0,0,*image_ptr);
    } else {
        //draw icon for icon identification
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
