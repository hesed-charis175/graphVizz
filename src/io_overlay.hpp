#pragma once
#include "render_core.hpp"

// An IO debugging overlay

struct KeyBufferEntry {
  ImGuiKey key;
  std::string name;
  std::chrono::steady_clock::time_point timestamp;
};

static std::deque<KeyBufferEntry> s_KeyBuffer;
static constexpr float s_multiKeyDuration = 0.3f;
static constexpr float s_uniqueKeyDuration = 1.0f;

static void updateKeyBuffer() {
  auto now = std::chrono::steady_clock::now();

  while (!s_KeyBuffer.empty()) {
    float elapsed =
        std::chrono::duration<float>(now - s_KeyBuffer.front().timestamp)
            .count();
    if (s_KeyBuffer.size() == 1 && elapsed >= s_uniqueKeyDuration) {
      s_KeyBuffer.pop_front();
    } else if (s_KeyBuffer.size() > 1 && elapsed >= s_multiKeyDuration) {
      s_KeyBuffer.pop_front();
    } else {
      break;
    }
  }
}

enum class DebugPosition { TopLeft, TopRight, BottomLeft, BottomRight };

static void showDebugLogs(WindowData *winData) {
  static int location = static_cast<int>(DebugPosition::BottomLeft);
  static std::unordered_set<std::string> uniqueLogs;
  static std::vector<std::string> logHistory;

  ImGuiWindowFlags winFlags = ImGuiWindowFlags_None;

  const float PAD = 10.0f;

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 work_size = viewport->WorkSize;
  ImVec2 window_pos, window_pos_pivot;

  window_pos.x =
      (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
  window_pos.y =
      (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
  window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
  window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;

  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowBgAlpha(0.35f);

  if (ImGui::Begin("Graph Memory Logs", &winData->showAppSimpleOverlay,
                   winFlags)) {
    ImGui::Text("Graph Memory Logs");
    ImGui::Separator();

    ImGui::BeginChild("LogScrollRegion", ImVec2(0, 0), false,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);

    const auto &logs = Logger::getBufferedLogs();
    for (const auto &log : logs) {
      ImGui::TextWrapped("%s", log.c_str());
    }

    ImGui::SetScrollHereY(1.0f);

    ImGui::EndChild();
  }

  ImGui::End();
}

static void
showDebugNodeList(WindowData *winData,
                  const std::function<void()> &popupContentCallback) {
  static int location = static_cast<int>(DebugPosition::BottomRight);

  ImGuiWindowFlags winFlags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
  ImVec2 origin = ImGui::GetMainViewport()->Pos;

  const float PAD = 10.0f;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 work_size = viewport->WorkSize;
  ImVec2 window_pos, window_pos_pivot;
  window_pos.x =
      (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
  window_pos.y =
      (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
  window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
  window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowViewport(viewport->ID);
  winFlags |= ImGuiWindowFlags_NoMove;

  ImGui::SetNextWindowBgAlpha(0.35f);

  ImVec2 minSize(0, 0);
  ImVec2 maxSize(400, 300);
  // ImGui::SetNextWindowSizeConstraints(minSize, maxSize);

  if (ImGui::Begin("Node Debug List", &winData->showAppSimpleOverlay,
                   winFlags)) {
    ImGui::Text("Node Debug List");
    ImGui::Separator();

    if (s_Graph.start) {
      ImGui::Text("Start : NODE_ID %d | Position(%.2f, %.2f) | Type(%s)",
                  static_cast<int>(s_Graph.start->node_id),
                  s_Graph.start->position.x, s_Graph.start->position.y,
                  s_Graph.start->debugNodeTypeString.c_str());
    }
    if (s_Graph.end) {
      ImGui::Text("End   : NODE_ID %d | Position(%.2f, %.2f) | Type(%s)",
                  static_cast<int>(s_Graph.end->node_id),
                  s_Graph.end->position.x, s_Graph.end->position.y,
                  s_Graph.end->debugNodeTypeString.c_str());
    }

    ImGui::Separator();

    for (auto &node : s_Graph.nodes) {
      bool isBeingEdited = false;
      if (winData->editableNode && node.get() == s_targetEditable)
        isBeingEdited = true;
      ImGui::Text("[NODE_ID %d] : Position(%.2f, %.2f) | Type(%s)",
                  static_cast<int>(node->node_id), node->position.x,
                  node->position.y, node->debugNodeTypeString.c_str());

      ImGui::SameLine();
      std::string nodeLabel = isBeingEdited
                                  ? "##" + std::to_string(node->node_id)
                                  : "Select##" + std::to_string(node->node_id);

      if (isBeingEdited)
        ImGui::PushStyleColor(ImGuiCol_Button, Config::red);
      if (ImGui::SmallButton(nodeLabel.c_str())) {
        s_targetEditable = node.get();
        winData->editableNode = node.get();

        ImVec2 nodeEditorPosition =
            ImVec2(node->position.x + origin.x + 30.0f + Config::panOffset.x,
                   node->position.y + origin.y + 30.0f + Config::panOffset.y);
        ImGui::SetNextWindowPos(nodeEditorPosition, ImGuiCond_Always);
        ImGui::OpenPopup("Node Editor");
        if (winData->editableNode)
          HelpMarker("<Drag to Edit Node Position>");
      }
      if (isBeingEdited)
        ImGui::PopStyleColor();

      ImGui::Separator();
    }
    if (ImGui::BeginPopup("Node Editor")) {
      if (popupContentCallback)
        popupContentCallback();
      ImGui::EndPopup();
    }

    if (winData->editableNode) {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Text("<Edit Mode>");
    }
  }
  ImGui::End();
}

static void showAppSimpleOverlay(WindowData *winData) {
  static int location = 0;

  ImGuiIO &io = ImGui::GetIO();
  ImGuiWindowFlags winFlags =
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

  const float PAD = 10.0f;
  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 work_size = viewport->WorkSize;
  ImVec2 window_pos, window_pos_pivot;
  window_pos.x =
      (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
  window_pos.y =
      (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
  window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
  window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowViewport(viewport->ID);
  winFlags |= ImGuiWindowFlags_NoMove;

  ImGui::SetNextWindowBgAlpha(0.35f);
  if (ImGui::Begin("IO overlay", &winData->showAppSimpleOverlay, winFlags)) {
    ImGui::Text("IO overlay");
    ImGui::Separator();
    if (ImGui::IsMousePosValid())
      ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
    else
      ImGui::Text("Mouse Position: <invalid>");

    updateKeyBuffer();

    for (int key = ImGuiKey_NamedKey_BEGIN; key < ImGuiKey_COUNT; key++) {
      if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(key))) {
        s_KeyBuffer.push_back({static_cast<ImGuiKey>(key),
                               ImGui::GetKeyName(static_cast<ImGuiKey>(key)),
                               std::chrono::steady_clock::now()});
        break;
      }
    }

    for (const auto &entry : s_KeyBuffer) {
      ImGui::Separator();
      ImGui::Text("Key Pressed : <%s>", entry.name.c_str());
    }

    if (winData->editableNode) {
      ImGui::Spacing();
      ImGui::Separator();
      ImGui::Text("<Edit Mode>");
    }
  }
  ImGui::End();
}
