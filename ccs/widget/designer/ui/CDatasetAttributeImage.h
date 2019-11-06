#ifndef CDATASETATTRIBUTEIMAGE_H
#define CDATASETATTRIBUTEIMAGE_H

#include <QIcon>
#include <QImage>
#include <QSharedPointer>
#include "../ChaosBaseDatasetAttributeUI.h"

class CDatasetAttributeImage:
        public ChaosBaseDatasetAttributeUI {
    Q_OBJECT
public:
    CDatasetAttributeImage(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent */*event*/);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
protected:
    void updateOnline(ChaosBaseDatasetUI::OnlineState state);
    void updateValue(QVariant variant_value);
private:
    bool image_loaded;
    QIcon image_placeholder;
    QSharedPointer<QImage> image_ptr;
};

#endif // CDATASETATTRIBUTEIMAGE_H
