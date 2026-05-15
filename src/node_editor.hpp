#pragma once
void nodePositionSection(Node *editableNode) {
  HelpMarker(Strings::nodeHelpMarker);

  int _x = editableNode->position.x, _y = editableNode->position.y;

  ImGui::SeparatorText("Node Position");
  ImGui::InputInt("X coordinate", &_x, 10);

  ImGui::InputInt("Y coordinate", &_y, 10);

  editableNode->position.x = static_cast<float>(_x);
  editableNode->position.y = static_cast<float>(_y);
}

void showAddEditButton(WindowData *winData, bool editable, Node *editableNode,
                       NodeType type) {
  if (!editable && ImGui::Button("Add")) {
    editableNode->setNodeType(NodeType::None);
    s_Graph.addNode(*editableNode);
    Logger::log(s_Graph.debugPrintState());

    s_Graph.setNodeType(s_Graph.nodes.back().get(), type);

    winData->editableNode = false;
    ImGui::CloseCurrentPopup();
    editableNode = nullptr;
  } else if (editable && ImGui::Button("Edit")) {
    Logger::log(s_Graph.debugPrintState());
    s_Graph.setNodeType(editableNode, type);
    s_targetEditable = nullptr;
    ImGui::CloseCurrentPopup();
    winData->editableNode = false;
    s_CopiedOnceFlag = false;
    editableNode = nullptr;
  }
}

void showDeleteButton(WindowData *winData, Node *editableNode) {
  ImGui::PushStyleColor(ImGuiCol_Button, Config::red);
  if (ImGui::Button(("Delete"))) {
    s_Graph.removeNode(s_targetEditable);
    s_targetEditable = nullptr;
    ImGui::CloseCurrentPopup();
    winData->editableNode = false;
    s_CopiedOnceFlag = false;
    editableNode = nullptr;
  }

  ImGui::PopStyleColor();
  ImGui::SameLine();
  ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 80);
  ImGui::SameLine();
}

static void showNodeEditor(WindowData *winData, Node *editableNode,
                           bool editable) {
  if (editable && !s_CopiedOnceFlag) {
    s_copiedNode.position = editableNode->position;
    s_copiedNode.setNodeType(editableNode->getNodeType());
    if (s_Graph.end)
      s_localGrEnd = *s_Graph.end;
    if (s_Graph.start)
      s_localGrStart = *s_Graph.start;
    s_CopiedOnceFlag = true;
  }
  if (winData->editableNode && Config::allowNodeDragging) {
    // if(ImGui::IsMouseDragging(0, 0.1f)){
    //     editableNode->position.x = (editableNode->position.x +
    //     ImGui::GetIO().MouseDelta.x )/ Config::zoom; editableNode->position.y
    //     = (editableNode->position.y + ImGui::GetIO().MouseDelta.y) /
    //     Config::zoom;
    // }

    ImGuiIO &io = ImGui::GetIO();
    static bool isDraggingNode = false;
    static ImVec2 dragStartMouse;
    static ImVec2 dragStartNodePos;

    ImVec2 nodeScreenPos = {
        editableNode->position.x * Config::zoom + Config::panOffset.x +
            ImGui::GetMainViewport()->Pos.x,
        editableNode->position.y * Config::zoom + Config::panOffset.y +
            ImGui::GetMainViewport()->Pos.y};
    float radius = Config::nodeSize;

    ImVec2 delta = {io.MousePos.x - nodeScreenPos.x,
                    io.MousePos.y - nodeScreenPos.y};
    bool hoveringNode =
        (delta.x * delta.x + delta.y * delta.y) <= (radius * radius);

    if (ImGui::IsMouseClicked(0) && hoveringNode) {
      isDraggingNode = true;
      dragStartMouse = io.MousePos;
      dragStartNodePos = {editableNode->position.x, editableNode->position.y};
    }
    if (isDraggingNode && ImGui::IsMouseDragging(0, 0.1f)) {
      ImVec2 mouseDeltaScreen = {io.MousePos.x - dragStartMouse.x,
                                 io.MousePos.y - dragStartMouse.y};
      ImVec2 deltaWorld = {mouseDeltaScreen.x / Config::zoom,
                           mouseDeltaScreen.y / Config::zoom};
      editableNode->position = {dragStartNodePos.x + deltaWorld.x,
                                dragStartNodePos.y + deltaWorld.y};
    }

    if (ImGui::IsMouseReleased(0)) {
      isDraggingNode = false;
    }
  }

  ImGui::SeparatorText("Node Type");

  const char *nodeTypes[] = {"- Default", "Start", "Intermediate", "End"};
  int currentType = static_cast<int>(editableNode->getNodeType());

  ImGui::ListBox("Node Type", &currentType, nodeTypes, IM_ARRAYSIZE(nodeTypes),
                 4);
  ImGui::SameLine();

  nodePositionSection(editableNode);
  NodeType type;
  ImVec4 tmpNodeColor;
  switch (currentType) {
  case 0:
    tmpNodeColor = Config::s_nodeColor;
    editableNode->setNodeType(NodeType::None);
    type = NodeType::None;
    break;
  case 1:
    tmpNodeColor = Config::s_startColor;
    editableNode->setNodeType(NodeType::Start);
    type = NodeType::Start;
    break;
  case 2:
    tmpNodeColor = Config::s_intermediateColor;
    editableNode->setNodeType(NodeType::Intermediate);
    type = NodeType::Intermediate;
    break;
  case 3:
    tmpNodeColor = Config::s_endColor;
    editableNode->setNodeType(NodeType::End);
    type = NodeType::End;
    break;
  }

  drawNode(*editableNode, tmpNodeColor);
  if (editable)
    drawNode(*editableNode, Config::s_selectionColor, true);

  Logger::log("[DEBUG/UI] Adding node from editableNode " +
              std::to_string(reinterpret_cast<uintptr_t>(editableNode)) +
              "[id=" + std::to_string(editableNode->node_id) +
              "] type: " + std::to_string(static_cast<int>(type)) + " | " +
              editableNode->debugNodeTypeString + "\n");

  if (editable)
    showDeleteButton(winData, editableNode);
  if (ImGui::Button("Cancel")) {
    if (editable) {
      s_targetEditable = nullptr;
      *editableNode = s_copiedNode;
      winData->editableNode = false;
      s_CopiedOnceFlag = false;
      editableNode = nullptr;
    }
    ImGui::CloseCurrentPopup();
    if (!editable)
      *editableNode = Node();
  }
  ImGui::SameLine();
  showAddEditButton(winData, editable, editableNode, type);
}

static void showMultiNodeEditor(WindowData *winData) {

  if (winData->allNodesSelected) {
    ImGui::Text("Reset All Nodes Types");
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
      s_Graph.resetNodeRoles();
      ImGui::CloseCurrentPopup();
      winData->multiSelectionEnabled = false;
      winData->allNodesSelected = false;
    }
  }

  int _x = 0, _y = 0;

  ImGui::SeparatorText("Node Position");
  ImGui::SameLine();

  HelpMarker(Strings::nodeHelpMarker);
  ImGui::InputInt("X coordinate", &_x, 10);

  if (_x > Config::windowWidth - Config::nodeSize)
    _x = Config::windowWidth - Config::nodeSize;
  if (_x < -Config::windowWidth + Config::nodeSize)
    _x = -Config::windowWidth + Config::nodeSize;

  ImGui::InputInt("Y coordinate", &_y, 10);

  if (_y > Config::windowHeight - Config::nodeSize)
    _y = Config::windowHeight - Config::nodeSize;
  if (_y < -Config::windowHeight + Config::nodeSize)
    _y = -Config::windowHeight + Config::nodeSize;

  if (Config::allowNodeDragging) {
    if (ImGui::IsMouseDragging(0, 0.1f)) {
      _x += ImGui::GetIO().MouseDelta.x;
      _y += ImGui::GetIO().MouseDelta.y;
    }
  }
  for (auto &n : s_selectedNodes) {
    n->position = {n->position.x + _x, n->position.y + _y};
  }

  Logger::log("[DEBUG] MultiSelection Enabled!\n");

  if (ImGui::Button("Cancel")) {
    Vec2 position = {s_selectedNodes[0]->position.x,
                     s_selectedNodes[0]->position.y};
    for (auto &n : s_selectedNodes) {
      n->position = {n->position.x - (position.x - s_tempPosition.x),
                     n->position.y - (position.y - s_tempPosition.y)};
    }
    s_selectedNodes.clear();
    s_winData.keepMultiSelectionOpened = false;
    s_winData.multiSelectionEnabled = false;
    s_tempPosition = {0, 0};
    s_tempPositionCaptured = false;
    ImGui::CloseCurrentPopup();
  }
  ImGui::SameLine();
  if (ImGui::Button("Edit")) {
    s_selectedNodes.clear();
    s_winData.keepMultiSelectionOpened = false;
    s_winData.multiSelectionEnabled = false;
    s_tempPosition = {0, 0};
    s_tempPositionCaptured = false;
    ImGui::CloseCurrentPopup();
  }
}

static void keyBindMovement() {}
