#pragma once

#include <memory>

#include <QtWidgets/QWidget>

#include "PortType.hpp"
#include "NodeData.hpp"
#include "Serializable.hpp"

namespace QtNodes
{

enum class NodeValidationState
{
  Valid,
  Warning,
  Error
};

class NodeModel
  : public QObject
  , public Serializable
{
  Q_OBJECT

public:

  virtual
  ~NodeModel() {}

  /// Caption is used in GUI
  virtual QString
  caption() const = 0;

  /// It is possible to hide caption in GUI
  virtual bool
  captionVisible() const { return true; }

  /// Port caption is used in GUI to label individual ports
  virtual QString
  portCaption(PortType portType, PortIndex portIndex) const { return QString(""); }

  /// It is possible to hide port caption in GUI
  virtual bool
  portCaptionVisible(PortType portType, PortIndex portIndex) const { return false; }

  /// Name makes this model unique
  virtual QString
  name() const = 0;

  /// Function creates instances of a model stored in DataModelRegistry
  virtual std::unique_ptr<NodeModel>
  clone() const = 0;

public:

  QJsonObject
  save() const override
  {
    QJsonObject modelJson;

    modelJson["name"] = name();

    return modelJson;
  }

public:

  virtual
  unsigned int
  nPorts(PortType portType) const = 0;

  virtual
  int maxConnectionsPerPort(PortType, PortIndex ) const { return -1; }

  virtual
  NodePortType
  dataType(PortType portType, PortIndex portIndex) const = 0;

public:

  virtual
  QWidget *
  embeddedWidget() = 0;

  virtual
  bool
  resizable() const { return false; }

  virtual
  NodeValidationState
  validationState() const { return NodeValidationState::Valid; }

  virtual
  QString
  validationMessage() const { return QString(""); }

signals:

  void
  dataUpdated(PortIndex index);

  void
  dataInvalidated(PortIndex index);

  void
  computingStarted();

  void
  computingFinished();
};
}
