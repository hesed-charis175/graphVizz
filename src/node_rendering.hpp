#pragma once
static void handleEdgeAddition(WindowData *winData, ImVec2 cursorPosition,
                               ImVec2 origin) {
  static bool dragging = false;
  static Vec2 dragStart, dragEnd;

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
    if (!ImGui::GetIO().WantCaptureMouse) {
      dragging = true;
      dragStart = {cursorPosition.x - origin.x, cursorPosition.y - origin.y};
    }
  }

  Node *dragStartNode =
      s_Graph.getNodeAt(dragStart, Config::selectionRadius, Config::nodeSize);
  Node *dragEndNode = s_Graph.getNodeAt(
      {cursorPosition.x - origin.x, cursorPosition.y - origin.y},
      Config::edgeSnapRadius, Config::nodeSize);
  if (dragging && ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    if (dragStartNode) {
      dragStart.x = dragStartNode->position.x;
      dragStart.y = dragStartNode->position.y;
      Logger::log("Dragging from Node" +
                  std::to_string(reinterpret_cast<uintptr_t>(dragStartNode)));
    }

    if (!dragEndNode) {
      dragEnd = {cursorPosition.x - origin.x, cursorPosition.y - origin.y};
    } else {
      dragEnd = {dragEndNode->position.x, dragEndNode->position.y};
    }

    drawEdge(dragStart, dragEnd, Config::s_edgeColor);
  }
  if (dragging && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
    dragging = false;
    if (dragEndNode && dragStartNode) {
      dragStartNode->print();
      dragEndNode->print();
      s_Graph.addEdge(dragStartNode, dragEndNode);
    }
  }
}

void handleMultiSelection(WindowData *winData, ImVec2 cursorPosition,
                          ImVec2 origin) {
  Vec2 relativePosition(cursorPosition.x - origin.x,
                        cursorPosition.y - origin.y);
  Node *selected = s_Graph.getNodeAt(relativePosition, Config::selectionRadius,
                                     Config::nodeSize);

  if (selected) {
    auto it =
        std::find(s_selectedNodes.begin(), s_selectedNodes.end(), selected);
    if (it == s_selectedNodes.end()) {
      s_selectedNodes.push_back(selected);
    } else {
      s_selectedNodes.erase(it);
    }
  }

  if (winData->keepMultiSelectionOpened) {
    ImVec2 multiNodeEditorPosition =
        ImVec2(cursorPosition.x + 30, cursorPosition.y + 30);
    ImGui::SetNextWindowPos(multiNodeEditorPosition, ImGuiCond_Always);
    ImGui::OpenPopup("Node Editor");
    if (winData->editableNode)
      HelpMarker("<Multi Selection Mode is Enabled!>");
  }
}

void handleAllNodeSelected(WindowData *winData) {
  for (const auto &node : s_Graph.nodes) {
    s_selectedNodes.push_back(node.get());
  }
  winData->allNodesSelected = true;
  ImVec2 multiNodeEditorPosition = ImVec2(ImGui::GetMainViewport()->Size.x / 2,
                                          ImGui::GetMainViewport()->Size.y / 2);
  ImGui::SetNextWindowPos(multiNodeEditorPosition, ImGuiCond_Always);
  ImGui::OpenPopup("Node Editor");
  winData->multiSelectionEnabled = true;
}

void handleSingleSelection(WindowData *winData, ImVec2 cursorPosition,
                           ImVec2 origin) {
  s_targetEditable = nullptr;
  winData->editableNode = false;
  s_tempNode.position.x = (cursorPosition.x - origin.x) / Config::zoom;
  s_tempNode.position.y = (cursorPosition.y - origin.y) / Config::zoom;

  if (!s_targetEditable) {
    s_targetEditable = s_Graph.getNodeAt(
        s_tempNode.position, Config::selectionRadius, Config::nodeSize);
    s_tempNode.setNodeType(NodeType::None);
  }
  if (s_targetEditable)
    winData->editableNode = true;
  ImVec2 nodeEditorPosition =
      ImVec2(cursorPosition.x + 30, cursorPosition.y + 30);
  ImGui::SetNextWindowPos(nodeEditorPosition, ImGuiCond_Always);
  ImGui::OpenPopup("Node Editor");
  if (winData->editableNode)
    HelpMarker("<Drag to Edit Node Position>");
}

void handleEdgeDeletion(WindowData *winData, ImVec2 cursorPosition,
                        ImVec2 origin) {
  for (auto &edge : s_Graph.edges) {
    Node *node_A = edge.first;
    Node *node_B = edge.second;

    float distanceCursorToLine = computeDistanceToSegment(
        ImVec2(cursorPosition.x - origin.x, cursorPosition.y - origin.y),
        ImVec2(node_A->position.x, node_A->position.y),
        ImVec2(node_B->position.x, node_B->position.y));

    if (distanceCursorToLine <= Config::edgeDeletionThreshold &&
        ImGui::IsMouseClicked(ImGuiMouseButton_Right) &&
        s_Graph.getNodeAt(
            Vec2(cursorPosition.x - origin.x, cursorPosition.y - origin.y),
            Config::selectionRadius, Config::nodeSize) == nullptr) {
      s_Graph.removeEdge(node_A, node_B);
      break;
    }
  }
}

static void handleClickInViewport(WindowData *winData) {

  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 cursorPosition = ImGui::GetMousePos();
  ImVec2 origin = {ImGui::GetMainViewport()->Pos.x + Config::panOffset.x,
                   ImGui::GetMainViewport()->Pos.y + Config::panOffset.y};

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) &&
      !ImGui::GetIO().WantCaptureMouse && cursorPosition.x >= viewport->Pos.x &&
      cursorPosition.x <= viewport->Pos.x + viewport->Size.x &&
      cursorPosition.y >= viewport->Pos.y &&
      cursorPosition.y <= viewport->Pos.y + viewport->Size.y &&
      !winData->hoveringJoystick) {
    if (winData->multiSelectionEnabled)
      handleMultiSelection(winData, cursorPosition, origin);
    else {
      handleSingleSelection(winData, cursorPosition, origin);
    }
  }
  if (ImGui::BeginPopup("Node Editor")) {
    if (winData->multiSelectionEnabled) {
      showMultiNodeEditor(winData);
    } else {
      winData->editableNode ? showNodeEditor(winData, s_targetEditable, true)
                            : showNodeEditor(winData, &s_tempNode, false);
    }
    ImGui::EndPopup();
  }
  if (ImGui::IsKeyPressed(ImGuiKey_A, false))
    handleAllNodeSelected(winData);
  if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
    winData->multiSelectionEnabled = false;
    s_selectedNodes.clear();
    ImGui::CloseCurrentPopup();
  }
  handleEdgeAddition(winData, cursorPosition, origin);
  handleEdgeDeletion(winData, cursorPosition, origin);
  bool isPopupOpenNow = ImGui::IsPopupOpen("Node Editor");

  if ((s_WasPopupOpen && !isPopupOpenNow && winData->editableNode)) {
    *s_targetEditable = s_copiedNode;
    winData->editableNode = false;
    s_targetEditable = nullptr;
    s_CopiedOnceFlag = false;
    s_selectedNodes.clear();
    s_winData.keepMultiSelectionOpened = false;
    s_winData.multiSelectionEnabled = false;
    s_tempPosition = {0, 0};
    s_tempPositionCaptured = false;
  }

  s_WasPopupOpen = isPopupOpenNow;
}
