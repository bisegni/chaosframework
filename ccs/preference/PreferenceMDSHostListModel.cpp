#include "PreferenceMDSHostListModel.h"


PreferenceMDSHostListModel::PreferenceMDSHostListModel(QObject *parent):
    QStringListModel(parent) {

}


bool PreferenceMDSHostListModel::setData(const QModelIndex & index,
                                         const QVariant & value,
                                         int role) {
    bool result = false;

    return  QStringListModel::setData(index,
                                      value,
                                      role);
}
