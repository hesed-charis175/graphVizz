#pragma once
#include "render_core.hpp"

static void drawNode(Node node, ImVec4 &color, bool outline = false) {
  ImDrawList *drawList = ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());

  if (!drawList) {
    Logger::log("[DEBUG] DrawList is null!");
    return;
  }

  ImVec2 origin = {ImGui::GetMainViewport()->Pos.x + Config::panOffset.x,
                   ImGui::GetMainViewport()->Pos.y + Config::panOffset.y};

  ImVec2 pos = ImVec2(node.position.x * Config::zoom + origin.x,
                      node.position.y * Config::zoom + origin.y);
  ImU32 colorU32 = ImGui::ColorConvertFloat4ToU32(color);

  if (outline) {
    drawList->AddCircle(pos, Config::nodeSize + Config::edgeThickness, colorU32,
                        0, 2.0f);
  } else {
    ImVec2 textOffset = {Config::s_textOffsetX, Config::s_textOffsetY};
    ImVec2 textPosition = {pos.x + textOffset.x, pos.y + textOffset.y};

    if (Config::displayPosition) {
      ImGui::GetBackgroundDrawList(ImGui::GetMainViewport())
          ->AddText(textPosition, IM_COL32(255, 255, 255, 255),
                    ("(" + std::to_string((int)node.position.x) + ", " +
                     std::to_string((int)node.position.y) + ")")
                        .c_str());
    }

    if (Config::hasOutline) {
      drawList->AddCircle(pos, Config::nodeSize + Config::edgeThickness,
                          ImGui::ColorConvertFloat4ToU32(Config::s_edgeColor));
    }

    for (int i = 0; i < Config::glowNodeIterations; i++) {
      float glowRadius = Config::nodeSize + i * Config::glowNodeFactor;
      float alpha =
          (Config::glowNodeFactor - i * 0.1f) * 0.2f * Config::glowNodeFactor;
      ImVec4 glowColor = ImVec4(color.x, color.y, color.z, alpha);
      ImU32 glowU32 = ImGui::ColorConvertFloat4ToU32(glowColor);
      drawList->AddCircleFilled(pos, glowRadius, glowU32);
    }

    drawList->AddCircleFilled(pos, Config::nodeSize, colorU32);
  }
}

static void updateNodes(WindowData *winData) {
  for (auto &node : s_Graph.nodes) {
    ImVec4 tmpNodeColor;
    switch (node->getNodeType()) {
    case NodeType::Start:
      tmpNodeColor = Config::s_startColor;
      break;
    case NodeType::Intermediate:
      tmpNodeColor = Config::s_intermediateColor;
      break;
    case NodeType::End:
      tmpNodeColor = Config::s_endColor;
      break;
    case NodeType::None:
      tmpNodeColor = Config::s_nodeColor;
      break;
    }
    drawNode(*node.get(), tmpNodeColor);
  }
}

static void drawEdge(Vec2 _positionNode1, Vec2 _positionNode2, ImVec4 color) {
  ImDrawList *drawList = ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());
  ImVec2 origin = {ImGui::GetMainViewport()->Pos.x + Config::panOffset.x,
                   ImGui::GetMainViewport()->Pos.y + Config::panOffset.y};

  ImVec2 p1 = {_positionNode1.x * Config::zoom + origin.x,
               _positionNode1.y * Config::zoom + origin.y};
  ImVec2 p2 = {_positionNode2.x * Config::zoom + origin.x,
               _positionNode2.y * Config::zoom + origin.y};

  for (int i = 0; i < Config::glowEdgeIterations; ++i) {
    float thickness = Config::edgeThickness + i * Config::glowEdgeFactor;
    float alpha =
        (Config::glowEdgeFactor - i * 0.1f) * 0.2f * Config::glowEdgeFactor;
    ImVec4 glowColor = ImVec4(color.x, color.y, color.z, alpha);
    ImU32 glowU32 = ImGui::ColorConvertFloat4ToU32(glowColor);
    drawList->AddLine(p1, p2, glowU32, thickness);
  }
  drawList->AddLine(p1, p2, ImGui::ColorConvertFloat4ToU32(color),
                    (float)Config::edgeThickness);
}

static void updateEdges(WindowData *winData) {

  for (const auto &edge : s_Graph.edges) {
    Node *a = edge.first;
    Node *b = edge.second;
    a = s_Graph.getNodeAt(a->position, Config::selectionRadius,
                          Config::nodeSize);
    b = s_Graph.getNodeAt(b->position, Config::selectionRadius,
                          Config::nodeSize);
    if (!a || !b || a == b) {
      Logger::log("Edge did abracadabra!(" +
                  std::to_string(edge.first->position.x) + ", " +
                  std::to_string(edge.first->position.y) + ")" + "(" +
                  std::to_string(edge.second->position.x) + ", " +
                  std::to_string(edge.second->position.y) + ")");
      continue;
    }
    drawEdge(a->position, b->position, Config::s_edgeColor);
  }
}
