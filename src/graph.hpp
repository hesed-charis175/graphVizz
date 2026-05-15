#pragma once
#include "logger.hpp"
#include "matvec.hpp"
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

enum GraphType { City, Island, Maze };
enum class NodeType { None, Start, Intermediate, End };

struct Node {
  static size_t global_id_counter;
  size_t node_id;
  Vec2 position;
  bool isIntermediate;
  bool isStart;
  bool isEnd;
  std::vector<Node *> neighbors;
  std::string debugNodeTypeString;

  Node()
      : node_id(global_id_counter++), position(0, 0), isIntermediate(false),
        isStart(false), isEnd(false) {}
  Node(float x, float y)
      : node_id(global_id_counter++), position(x, y), isIntermediate(false),
        isStart(false), isEnd(false) {}
  Node(const Node &other)
      : node_id(global_id_counter++), position(other.position),
        isIntermediate(other.isIntermediate), isStart(other.isStart),
        isEnd(other.isEnd), neighbors(),
        debugNodeTypeString(other.debugNodeTypeString) {}
  NodeType getNodeType() const {
    if (isEnd) {
      return NodeType::End;
    }
    if (isIntermediate) {
      return NodeType::Intermediate;
    }
    if (isStart) {
      return NodeType::Start;
    }
    return NodeType::None;
  }

  void print() {
    std::cout << "Node [" << debugNodeTypeString << "] (" << position.x << ", "
              << position.y << ")" << std::endl;
  }

  void setNodeType(NodeType type) {
    if (type == NodeType::Start) {
      debugNodeTypeString = "START";
      isStart = true;
      isIntermediate = false;
      isEnd = false;
    } else if (type == NodeType::Intermediate) {
      debugNodeTypeString = "INTERMEDIATE";
      isStart = false;
      isIntermediate = true;
      isEnd = false;
    } else if (type == NodeType::End) {
      debugNodeTypeString = "END";
      isStart = false;
      isIntermediate = false;
      isEnd = true;
    } else if (type == NodeType::None) {
      debugNodeTypeString = "DEFAULT";
      isStart = false;
      isIntermediate = false;
      isEnd = false;
    }
  }
  ~Node() { neighbors.clear(); }
};

size_t Node::global_id_counter = 0;

struct Graph {
public:
  std::vector<std::unique_ptr<Node>> nodes;
  std::vector<std::pair<Node *, Node *>> edges;
  Node *start;
  Node *end;
  std::vector<Node *> intermediateNodes;

  Graph() {}
  ~Graph() { clearGraph(); }
  void clearGraph() {
    intermediateNodes.clear();
    nodes.clear();
    edges.clear();
    start = nullptr;
    end = nullptr;
  }

  void setNodeType(Node *node, NodeType type) {

    if (!node)
      return;

    for (auto &n : nodes) {
      n->setNodeType(NodeType::None);
    }

    switch (type) {
    case NodeType::Start:
      if (node == end)
        end = nullptr;
      start = node;
      break;
    case NodeType::End:
      if (node == start)
        start = nullptr;
      end = node;
      break;
    case NodeType::Intermediate:
      if (node == start)
        start = nullptr;
      if (node == end)
        end = nullptr;
      break;
    case NodeType::None:
      if (node == start)
        start = nullptr;
      if (node == end)
        end = nullptr;
      break;
    }

    if (type == NodeType::Intermediate && node != start && node != end) {
      auto it =
          std::find(intermediateNodes.begin(), intermediateNodes.end(), node);
      if (it == intermediateNodes.end()) {
        intermediateNodes.push_back(node);
      }
    }

    if (type != NodeType::Intermediate) {
      auto it =
          std::find(intermediateNodes.begin(), intermediateNodes.end(), node);
      if (it != intermediateNodes.end()) {
        intermediateNodes.erase(it);
      }
    }
    std::vector<Node *> updatedIntermediateNodes;

    for (auto &iNode : intermediateNodes) {
      if (!iNode)
        continue;

      Node *updated = getNodeAt(iNode->position, 5.0f, 5.0f);
      if (updated) {
        updated->setNodeType(NodeType::Intermediate);
        updatedIntermediateNodes.push_back(updated);
      }
    }

    intermediateNodes = std::move(updatedIntermediateNodes);

    if (start) {
      start = getNodeAt(start->position, 1, 1);
      start->setNodeType(NodeType::Start);
    }
    if (end) {
      end = getNodeAt(end->position, 1, 1);
      end->setNodeType(NodeType::End);
    }
  }

  void addNode(const Node &copyNode) {
    nodes.push_back(std::make_unique<Node>(copyNode));
    Logger::log("[DEBUG] addNode: Added node " +
                std::to_string(reinterpret_cast<uintptr_t>(&nodes.back())) +
                " [id=" + std::to_string(nodes.back()->node_id) +
                "] from copyNode " +
                std::to_string(reinterpret_cast<uintptr_t>(&copyNode)) +
                " [id=" + std::to_string(copyNode.node_id) +
                "] (type: " + copyNode.debugNodeTypeString + ")\n");
    setNodeType(nodes.back().get(), copyNode.getNodeType());
    debugPrintState();
  }

  void addNode(float x, float y, NodeType type = NodeType::None) {
    nodes.push_back(std::make_unique<Node>(x, y));
    setNodeType(nodes.back().get(), type);
  }

  void addEdge(Node *node_A, Node *node_B) {

    if (!node_A || !node_B)
      return;

    edges.emplace_back(node_A, node_B);
    node_A->neighbors.push_back(node_B);
    node_B->neighbors.push_back(node_A);
  }

  void removeNode(Node *nodeToRemove) {

    if (!nodeToRemove)
      return;

    if (nodeToRemove == start)
      start = nullptr;
    if (nodeToRemove == end)
      end = nullptr;
    intermediateNodes.erase(std::remove(intermediateNodes.begin(),
                                        intermediateNodes.end(), nodeToRemove),
                            intermediateNodes.end());

    for (auto *neighbor : nodeToRemove->neighbors) {
      neighbor->neighbors.erase(std::remove(neighbor->neighbors.begin(),
                                            neighbor->neighbors.end(),
                                            nodeToRemove),
                                neighbor->neighbors.end());
    }

    edges.erase(
        std::remove_if(edges.begin(), edges.end(),
                       [nodeToRemove](const std::pair<Node *, Node *> edge) {
                         return edge.first == nodeToRemove ||
                                edge.second == nodeToRemove;
                       }),
        edges.end());

    auto _nodeIt =
        std::remove_if(nodes.begin(), nodes.end(),
                       [nodeToRemove](const std::unique_ptr<Node> &n_arg) {
                         return n_arg.get() == nodeToRemove;
                       });
    if (_nodeIt != nodes.end()) {
      nodes.erase(_nodeIt, nodes.end());
    }
  }

  void resetNodeRoles() {
    for (auto &node : nodes) {
      setNodeType(node.get(), NodeType::None);
    }
  }

  Node *getNodeAt(Vec2 _position, float selectionRadius, float nodeSize) {
    auto _node = std::find_if(
        nodes.begin(), nodes.end(),
        [_position, selectionRadius, nodeSize](const std::unique_ptr<Node> &n) {
          return (n->position - _position).length() <=
                 nodeSize + selectionRadius;
        });

    return (_node != nodes.end()) ? _node->get() : nullptr;
  }

  void removeEdge(Node *node_A, Node *node_B) {
    if (!node_A || !node_B)
      return;

    node_A->neighbors.erase(
        std::find(node_A->neighbors.begin(), node_A->neighbors.end(), node_B),
        node_A->neighbors.end());
    node_B->neighbors.erase(
        std::find(node_B->neighbors.begin(), node_B->neighbors.end(), node_A),
        node_B->neighbors.end());

    auto it = std::remove_if(
        edges.begin(), edges.end(),
        [node_A, node_B](const std::pair<Node *, Node *> &edge) {
          return (edge.first == node_A && edge.second == node_B) ||
                 (edge.first == node_B && edge.second == node_A);
        });
    if (it != edges.end()) {
      Logger::log("Deleted edge between Node [NODE_ID " +
                  std::to_string(node_A->node_id) + "] and Node NODE_ID " +
                  std::to_string(node_B->node_id) + "]");
      edges.erase(it, edges.end());
    }
  }

  bool isNodePtrValid(Node *ptr) const {
    for (const auto &node : nodes) {
      if (node.get() == ptr)
        return true;
    }
    return false;
  }

  std::string debugPrintState() const {
    std::string s_toLog;
    s_toLog = "----- GRAPH STATE -----\n";
    s_toLog += "Start: " + std::to_string(reinterpret_cast<uintptr_t>(start)) +
               " End: " + std::to_string(reinterpret_cast<uintptr_t>(end)) +
               "\n";

    for (const auto &node : nodes) {
      s_toLog += "Node " + std::to_string(reinterpret_cast<uintptr_t>(&node)) +
                 " [id=" + std::to_string(node->node_id) +
                 "] type: " + node->debugNodeTypeString;

      if (node.get() == start)
        s_toLog += " <-- START";
      if (node.get() == end)
        s_toLog += " <-- END";

      for (auto *intnode : intermediateNodes) {
        if (node.get() == intnode)
          s_toLog += " <-- INTERMEDIATE";
      }

      s_toLog += "\n";
    }

    s_toLog += "Intermediate nodes: ";
    for (const auto &node : intermediateNodes) {
      if (node)
        s_toLog += std::to_string(reinterpret_cast<uintptr_t>(node)) +
                   "[id=" + std::to_string(node->node_id) + "] ";
    }
    s_toLog += "\n-----------------------\n";

    return s_toLog;
  }
};
