#include "PreferenceMDSHostListModel.h"

#include <chaos/common/utility/InetUtility.h>

#include <qmessagebox.h>

using namespace chaos::common::utility;

PreferenceMDSHostListModel::PreferenceMDSHostListModel(QObject *parent):
    QStringListModel(parent) {}


bool PreferenceMDSHostListModel::setData(const QModelIndex & index,
                                         const QVariant & value,
                                         int role) {
    std::string host = value.toString().toStdString();
    if(!InetUtility::checkWellFormedHostIpPort(host) &&
            !InetUtility::checkWellFormedHostNamePort(host)) {
        QMessageBox::information(NULL,
                                 tr("Hostname Description"),
                                 QString("The %1 is no well formed").arg(value.toString()));
        return false;
    }
    return  QStringListModel::setData(index,
                                      value,
                                      role);
}
