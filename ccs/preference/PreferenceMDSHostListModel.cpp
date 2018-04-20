#include "PreferenceMDSHostListModel.h"

#include <qmessagebox.h>
#include <QRegularExpression>

using namespace chaos::common::utility;

PreferenceMDSHostListModel::PreferenceMDSHostListModel(QObject *parent):
    QStringListModel(parent) {}


bool PreferenceMDSHostListModel::addMDSServer(const QString& hostname_port) {
    QStringList host_port_splitted = hostname_port.split(":");
    QRegularExpression ip_regexp("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5]).){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
    QRegularExpression host_regexp("^(([a-zA-Z0-9]|[a-zA-Z0-9][a-zA-Z0-9-]*[a-zA-Z0-9]).)*([A-Za-z0-9]|[A-Za-z0-9][A-Za-z0-9-]*[A-Za-z0-9])$");
    if(!ip_regexp.match(host_port_splitted[0]).hasMatch() &&
            !host_regexp.match(host_port_splitted[0]).hasMatch() ) {
        QMessageBox::information(NULL,
                                 tr("Hostname Description"),
                                 QString("The %1 is no well formed").arg(hostname_port));
        return false;
    }
    beginResetModel();
    server_list << hostname_port;
    setStringList(server_list);
    endResetModel();
    return true;
}

void PreferenceMDSHostListModel::setStringList(const QStringList& host_port_splitted) {
    server_list = host_port_splitted;
    QStringListModel::setStringList(server_list);
}

bool PreferenceMDSHostListModel::setData(const QModelIndex & index,
                                         const QVariant & value,
                                         int role) {
    std::string host = value.toString().toStdString();
    return  QStringListModel::setData(index,
                                      value,
                                      role);
}
