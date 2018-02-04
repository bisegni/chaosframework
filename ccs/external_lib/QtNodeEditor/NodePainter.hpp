#pragma once

#include <memory>

#include <QtGui/QPainter>

namespace QtNodes
{

class Node;
class NodeState;
class NodeGeometry;
class NodeGraphicsObject;
class NodeModel;
class FlowItemEntry;
class FlowScene;

class NodePainter
{
public:

  NodePainter();

public:

  static
  void paint(QPainter* painter,
             Node& node, 
             FlowScene const& scene);

  static
  void drawNodeRect(QPainter* painter, NodeGeometry const& geom,
                    NodeGraphicsObject const & graphicsObject);

  static
  void drawModelName(QPainter* painter,
                     NodeGeometry const& geom,
                     NodeState const& state,
                     NodeModel* const model);

  static
  void drawEntryLabels(QPainter* painter,
                       NodeGeometry const& geom,
                       NodeState const& state,
                       NodeModel* const model);

  static
  void drawConnectionPoints(QPainter* painter,
                            NodeGeometry const& geom,
                            NodeState const& state,
                            NodeModel* const model,
                            FlowScene const & scene);

  static
  void drawFilledConnectionPoints(QPainter* painter,
                                  NodeGeometry const& geom,
                                  NodeState const& state,
                                  NodeModel* const model);

  static
  void drawResizeRect(QPainter* painter,
                      NodeGeometry const& geom,
                      NodeModel* const model);

  static
  void drawValidationRect(QPainter * painter,
                          NodeGeometry const & geom,
                          NodeModel* const model,
                          NodeGraphicsObject const & graphicsObject);
};
}
