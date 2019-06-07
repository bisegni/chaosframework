#include "CDatasetAttributeLabel.h"

CDatasetAttributeLabel::CDatasetAttributeLabel(QWidget *parent) :
    QLabel(parent) {

}

CDatasetAttributeLabel::~CDatasetAttributeLabel() {

}

void CDatasetAttributeLabel::updateData(QSharedPointer<chaos::common::data::CDataVariant> /*variantValue*/) {

}

void CDatasetAttributeLabel::setDeviceID(const QString &newDeviceID) {
    myDeviceID = newDeviceID;
}

QString CDatasetAttributeLabel::deviceID() const {
    return myDeviceID;
}

void CDatasetAttributeLabel::setDatasetType(const CDatasetAttributeLabel::DatasetType &newDatasetType) {
    myDatasetType = newDatasetType;
}

CDatasetAttributeLabel::DatasetType CDatasetAttributeLabel::datasetType() const {
    return myDatasetType;
}

//slots hiding
void	CDatasetAttributeLabel::clear(){QLabel::clear();}
void	CDatasetAttributeLabel::setMovie(QMovie * movie){QLabel::setMovie(movie);}
void	CDatasetAttributeLabel::setNum(int num){QLabel::setNum(num);}
void	CDatasetAttributeLabel::setNum(double num){QLabel::setNum(num);}
void	CDatasetAttributeLabel::setPicture(const QPicture & picture){QLabel::setPicture(picture);}
void	CDatasetAttributeLabel::setPixmap(const QPixmap &pixmap){QLabel::setPixmap(pixmap);}
