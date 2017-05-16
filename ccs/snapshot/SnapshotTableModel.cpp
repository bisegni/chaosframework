#include "SnapshotTableModel.h"

#include <QColor>
#include <QDateTime>

using namespace chaos::metadata_service_client::api_proxy::service;

SnapshotTableModel::SnapshotTableModel(QObject *parent):
    ChaosAbstractTableModel(parent) {

}

SnapshotTableModel::~SnapshotTableModel() {

}

void SnapshotTableModel::updateSnapshotList(std::unique_ptr<chaos::metadata_service_client::api_proxy::service::GetAllSnapshotHelper> _snapshot_info_list) {
 beginResetModel();
 snapshot_info_list = std::move(_snapshot_info_list);
 endResetModel();
}

int SnapshotTableModel::getRowCount() const {
    return (snapshot_info_list.get()==NULL)?0:snapshot_info_list->getSnapshotListSize();
}

int SnapshotTableModel::getColumnCount() const {
    return 3;
}

QString SnapshotTableModel::getHeaderForColumn(int column) const {
    QString result;
    switch(column) {
    case 0:
        result = QString("Name");
        break;
    case 1:
        result = QString("Timestamp");
        break;
    case 2:
        result = QString("Work Concurrency");
        break;
    }
    return result;
}

QVariant SnapshotTableModel::getCellData(int row, int column) const {
    QVariant result;
    const SnapshotInfoList& info_list = snapshot_info_list->getSnapshotInfoList();
    switch(column) {
    case 0:
        result = QString::fromStdString(info_list[row]->name);
        break;
    case 1:
        result = QDateTime::fromMSecsSinceEpoch(info_list[row]->timestamp, Qt::LocalTime).toString();
        break;
    case 2:
        result = QString::number(info_list[row]->work_concurency);
    default:
        break;
    }
    return result;
}

QVariant SnapshotTableModel::getTextAlignForData(int row, int column) const {
    QVariant result;
    switch(column) {
    case 0:
        result =  Qt::AlignLeft+Qt::AlignVCenter;
        break;
    case 1:
        result =  Qt::AlignHCenter+Qt::AlignVCenter;
        break;
    case 2:
        result =  Qt::AlignHCenter+Qt::AlignVCenter;
        break;

    default:
        result =  Qt::AlignLeft;
    }
    return result;
}

QVariant SnapshotTableModel::getBackgroundForData(int row, int column) const {
    QVariant result;
    const SnapshotInfoList& info_list = snapshot_info_list->getSnapshotInfoList();
    if(info_list[row]->work_concurency) {
           return QColor(94,170,255);
    }
    return result;
}
bool SnapshotTableModel::isCellSelectable(const QModelIndex &index) const {
    return true;
}
