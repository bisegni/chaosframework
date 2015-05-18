#include "ChaosFixedCDataWrapperTableModel.h"

using namespace chaos::common::data;

ChaosFixedCDataWrapperTableModel::ChaosFixedCDataWrapperTableModel(QObject *parent):
    ChaosAbstractTableModel(parent) {
}

void ChaosFixedCDataWrapperTableModel::updateData(const QSharedPointer<chaos::common::data::CDataWrapper>& _data_wrapped) {
    data_wrapped = _data_wrapped;
}
