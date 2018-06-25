#include "ControlUnitNodeDataModel.h"
#include <QtMath>
#include <QDebug>

using namespace chaos::common::data::structured;

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

ControlUnitNodeDataModel::ControlUnitNodeDataModel(const QString& _cu_name,
                                                   std::unique_ptr<DatasetAttributeList>& _attr_list):
    cu_name(_cu_name),
    attr_list(std::move(_attr_list)){

    for(DatasetAttributeListIterator it = attr_list->begin(),
        end = attr_list->end();
        it != end;
        it++){
        if(it->direction == chaos::DataType::Input ||
                it->direction ==  chaos::DataType::Bidirectional) {
            in_attr_list.push_back(*it);
        }
        if(it->direction == chaos::DataType::Output ||
                it->direction ==  chaos::DataType::Bidirectional) {
            out_attr_list.push_back(*it);
        }
    }
}

ControlUnitNodeDataModel::~ControlUnitNodeDataModel(){}

QString ControlUnitNodeDataModel::caption() const {
    return cu_name;
}

QString ControlUnitNodeDataModel::name() const {
    return cu_name;
}

std::unique_ptr<NodeModel> ControlUnitNodeDataModel::clone() const {
    return cpp11::make_unique<ControlUnitNodeDataModel>();
}

unsigned int ControlUnitNodeDataModel::nPorts(PortType portType) const {
    unsigned int result = 0;
    switch (portType) {
    case PortType::In:
        result = in_attr_list.size();
        break;

    case PortType::Out:
        result = out_attr_list.size();

    default:
        break;
    }
    return result;
}

NodePortType ControlUnitNodeDataModel::dataType(PortType portType,
                                                PortIndex portIndex) const {
    switch (portType) {
    case PortType::In:
        return ControlUnitChannelData(in_attr_list[portIndex].type, QString::fromStdString(in_attr_list[portIndex].name)).type();
        break;

    case PortType::Out:
        return ControlUnitChannelData(out_attr_list[portIndex].type, QString::fromStdString(out_attr_list[portIndex].name)).type();
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
