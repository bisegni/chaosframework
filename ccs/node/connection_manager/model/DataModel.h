#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QObject>

#include "../../../external_lib/QtNodeEditor/NodeData.hpp"
#include "../../../external_lib/QtNodeEditor/NodeDataModel.hpp"
#include "../../../external_lib/QtNodeEditor/DataModelRegistry.hpp"

using QtNodes::NodePortType;
using QtNodes::NodeModel;
using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::PortIndex;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:

    NodePortType
    type() const override
    {
        return NodePortType {"MyNodeData",
            "My Node Data"};
    }
};

class SimpleNodeData : public NodeData
{
public:

    NodePortType
    type() const override
    {
        return NodePortType {"SimpleData",
            "Simple Data"};
    }
};


//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class DataModel :
        public NodeModel {
    Q_OBJECT
public:

    virtual
    ~DataModel() {}

public:

    QString caption() const override {
        return QString("Control Unit Node");
    }

    QString name() const override {
        return QString("ControlUnitNode");
    }

    std::unique_ptr<NodeModel> clone() const override {
        return cpp11::make_unique<DataModel>();
    }

public:
    unsigned int nPorts(PortType portType) const override {
        unsigned int result = 1;
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

    NodePortType dataType(PortType portType,
                          PortIndex portIndex) const override {
        switch (portType) {
        case PortType::In:
            switch (portIndex)
            {
            case 0:
                return MyNodeData().type();
                break;

            case 1:
                return SimpleNodeData().type();
                break;
            }
            break;

        case PortType::Out:
            switch (portIndex)
            {
            case 0:
                return MyNodeData().type();
                break;

            case 1:
                return SimpleNodeData().type();
                break;
            }
            break;

        default:
            return SimpleNodeData().type();
            break;
        }
        return SimpleNodeData().type();
    }

    std::shared_ptr<NodeData> outData(PortIndex port) {
        if (port < 1)
            return std::make_shared<MyNodeData>();

        return std::make_shared<SimpleNodeData>();
    }

    void  setInData(std::shared_ptr<NodeData>, int)
    {
        //
    }

    QWidget * embeddedWidget() override { return nullptr; }
};


#endif // DATAMODEL_H
