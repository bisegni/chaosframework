#include "ControlUnitNodeDataModel.h"
#include <QtMath>
#include <QDebug>
ControlUnitChannelData::ControlUnitChannelData():
    ch_type(chaos::DataType::TYPE_UNDEFINED),
    ch_name("empty"){}

ControlUnitChannelData::ControlUnitChannelData(chaos::DataType::DataType _type,
                                               const QString& _name):
ch_type(_type),
ch_name(_name){}

NodePortType ControlUnitChannelData::type() const {
    return NodePortType {QString::number(ch_type), ch_name};
}
ControlUnitNodeDataModel::ControlUnitNodeDataModel():
    attr_list(){}

ControlUnitNodeDataModel::ControlUnitNodeDataModel(std::unique_ptr<chaos::common::data::structured::DatasetAttributeList>& _attr_list):
    attr_list(std::move(_attr_list)){}

ControlUnitNodeDataModel::~ControlUnitNodeDataModel(){}

QString ControlUnitNodeDataModel::caption() const {
    return QString("quatb");
}

QString ControlUnitNodeDataModel::name() const {
    return QString("Control Unit Node");
}

std::unique_ptr<NodeModel> ControlUnitNodeDataModel::clone() const {
    return cpp11::make_unique<ControlUnitNodeDataModel>();
}

unsigned int ControlUnitNodeDataModel::nPorts(PortType portType) const {
    unsigned int result = 0;
    switch (portType) {
    case PortType::In:
        result = 2;
        break;

    case PortType::Out:
        result = 2;

    default:
        break;
    }
    return result;
}

NodePortType ControlUnitNodeDataModel::dataType(PortType portType,
                                                PortIndex portIndex) const {
    switch (portType) {
    case PortType::In:
        switch (portIndex) {
        case 0:
            return ControlUnitChannelData(chaos::DataType::TYPE_BOOLEAN, "InCh1").type();
            break;

        case 1:
            return ControlUnitChannelData(chaos::DataType::TYPE_INT32, "InCh2").type();
            break;
        }
        break;

    case PortType::Out:
        switch (portIndex) {
        case 0:
            return ControlUnitChannelData(chaos::DataType::TYPE_INT32, "OutCh1").type();
            break;

        case 1:
            return ControlUnitChannelData(chaos::DataType::TYPE_BOOLEAN, "OutCh2").type();
            break;
        }
        break;

    default:
        return ControlUnitChannelData().type();
        break;
    }
    return ControlUnitChannelData().type();
}

std::shared_ptr<NodeData> ControlUnitNodeDataModel::outData(PortIndex port) {
    std::shared_ptr<ControlUnitChannelData> result;
    switch(port) {
    case 1: result = std::make_shared<ControlUnitChannelData>(); break;
    case 2: result = std::make_shared<ControlUnitChannelData>(); break;
    }
    return result;
}

void  ControlUnitNodeDataModel::setInData(std::shared_ptr<NodeData>, int) {}

QWidget * ControlUnitNodeDataModel::embeddedWidget() {
    return nullptr;
}
