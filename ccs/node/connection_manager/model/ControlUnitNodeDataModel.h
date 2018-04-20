#ifndef CONTROLUNITNODEDATAMODEL_H
#define CONTROLUNITNODEDATAMODEL_H


#include <QObject>
#include <QLineEdit>

#include "../../../external_lib/QtNodeEditor/NodeData.hpp"
#include "../../../external_lib/QtNodeEditor/NodeDataModel.hpp"
#include "../../../external_lib/QtNodeEditor/DataModelRegistry.hpp"

using QtNodes::NodePortType;
using QtNodes::NodeModel;
using QtNodes::NodeData;
using QtNodes::PortType;
using QtNodes::PortIndex;

class NullNodeData : public NodeData
{
public:

    NodePortType
    type() const override
    {
        return NodePortType {"null",
            "void"};
    }
};

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class ControlUnitChannelData :
        public NodeData {
public:
    ControlUnitChannelData();
    ControlUnitChannelData(chaos::DataType::DataType _type,
                           const QString& _name);
    NodePortType type() const override;
private:
    chaos::DataType::DataType ch_type;
    const QString ch_name;
};

//------------------------------------------------------------------------------

class ControlUnitNodeDataModel:
        public NodeModel {
    Q_OBJECT
public:
    ControlUnitNodeDataModel();
    ControlUnitNodeDataModel(const QString& _cu_name,
                             std::unique_ptr<chaos::common::data::structured::DatasetAttributeList>& _attr_lis);
    virtual ~ControlUnitNodeDataModel();

public:

    QString caption() const override;

    QString name() const override;

    std::unique_ptr<NodeModel> clone() const override ;

public:
    unsigned int nPorts(PortType portType) const override;

    NodePortType dataType(PortType portType,
                          PortIndex portIndex) const override ;

    std::shared_ptr<NodeData> outData(PortIndex port);

    void  setInData(std::shared_ptr<NodeData>, int);

    QWidget * embeddedWidget() override;
private slots:

private:
    const QString cu_name;
    std::unique_ptr<chaos::common::data::structured::DatasetAttributeList> attr_list;
    chaos::common::data::structured::DatasetAttributeList  in_attr_list;
    chaos::common::data::structured::DatasetAttributeList  out_attr_list;
};

#endif // CONTROLUNITNODEDATAMODEL_H
