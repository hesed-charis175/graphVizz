#pragma once
float ImClamp(float *v, float mn, float mx);

static void showMinimap(WindowData *winData) {
  if (!Config::showMinimap)
    return;

  const ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImDrawList *dl =
      ImGui::GetForegroundDrawList(const_cast<ImGuiViewport *>(viewport));

  const float mmW = Config::minimapWidth;
  const float mmH = Config::minimapHeight;
  const float PAD = 10.0f;
  const float IPAD = 8.0f;

  const float JOYSTICK_SPACE = 100.0f;
  ImVec2 mmBR =
      ImVec2(viewport->WorkPos.x + viewport->WorkSize.x - PAD,
             viewport->WorkPos.y + viewport->WorkSize.y - PAD - JOYSTICK_SPACE);
  ImVec2 mmTL = ImVec2(mmBR.x - mmW, mmBR.y - mmH);

  dl->AddRectFilled(mmTL, mmBR, IM_COL32(10, 12, 20, 210), 6.0f);
  dl->AddRect(mmTL, mmBR, IM_COL32(70, 85, 130, 240), 6.0f, 0, 1.8f);

  dl->AddRectFilled(mmTL, ImVec2(mmBR.x, mmTL.y + 18.0f),
                    IM_COL32(30, 35, 60, 230), 6.0f);
  dl->AddLine(ImVec2(mmTL.x, mmTL.y + 18.0f), ImVec2(mmBR.x, mmTL.y + 18.0f),
              IM_COL32(70, 85, 130, 200), 1.0f);
  dl->AddText(ImVec2(mmTL.x + 8, mmTL.y + 3), IM_COL32(200, 210, 255, 230),
              "Minimap");

  const float titleH = 18.0f;
  ImVec2 innerTL = ImVec2(mmTL.x + IPAD, mmTL.y + titleH + IPAD);
  ImVec2 innerBR = ImVec2(mmBR.x - IPAD, mmBR.y - IPAD);
  float innerW = innerBR.x - innerTL.x;
  float innerH = innerBR.y - innerTL.y;

  if (s_Graph.nodes.empty()) {
    dl->AddText(ImVec2(innerTL.x + 4, innerTL.y + innerH * 0.5f - 7),
                IM_COL32(120, 120, 120, 200), "No nodes in graph");
    return;
  }

  float mnX = FLT_MAX, mnY = FLT_MAX;
  float mxX = -FLT_MAX, mxY = -FLT_MAX;
  for (auto &n : s_Graph.nodes) {
    mnX = std::min(mnX, n->position.x);
    mxX = std::max(mxX, n->position.x);
    mnY = std::min(mnY, n->position.y);
    mxY = std::max(mxY, n->position.y);
  }
  float rngX = mxX - mnX;
  if (rngX < 1.0f)
    rngX = 1.0f;
  float rngY = mxY - mnY;
  if (rngY < 1.0f)
    rngY = 1.0f;

  float scl = std::min(innerW / rngX, innerH / rngY);

  float offX = innerTL.x + (innerW - rngX * scl) * 0.5f;
  float offY = innerTL.y + (innerH - rngY * scl) * 0.5f;

  auto w2mm = [&](Vec2 p) -> ImVec2 {
    return ImVec2(offX + (p.x - mnX) * scl, offY + (p.y - mnY) * scl);
  };

  for (auto &edge : s_Graph.edges) {
    if (!edge.first || !edge.second)
      continue;
    ImVec2 a = w2mm(edge.first->position);
    ImVec2 b = w2mm(edge.second->position);
    dl->AddLine(a, b, IM_COL32(130, 130, 140, 100), 1.0f);
  }

  if (s_pathResult.size() >= 2) {
    ImU32 pathCol = ImGui::ColorConvertFloat4ToU32(Config::s_pathColor);
    for (size_t i = 0; i + 1 < s_pathResult.size(); i++) {
      if (!s_pathResult[i] || !s_pathResult[i + 1])
        continue;
      ImVec2 a = w2mm(s_pathResult[i]->position);
      ImVec2 b = w2mm(s_pathResult[i + 1]->position);
      dl->AddLine(a, b, pathCol, 2.0f);
    }
  }

  for (auto &n : s_Graph.nodes) {
    ImVec2 pos = w2mm(n->position);
    ImVec4 col;
    switch (n->getNodeType()) {
    case NodeType::Start:
      col = Config::s_startColor;
      break;
    case NodeType::End:
      col = Config::s_endColor;
      break;
    case NodeType::Intermediate:
      col = Config::s_intermediateColor;
      break;
    default:
      col = Config::s_nodeColor;
      break;
    }
    dl->AddCircleFilled(pos, 3.0f, ImGui::ColorConvertFloat4ToU32(col));
  }
  float viewW = viewport->Size.x / Config::zoom;
  float viewH = viewport->Size.y / Config::zoom;
  float vpWX = -Config::panOffset.x / Config::zoom;
  float vpWY = -Config::panOffset.y / Config::zoom;

  ImVec2 vpMM_TL = w2mm(Vec2(vpWX, vpWY));
  ImVec2 vpMM_BR = w2mm(Vec2(vpWX + viewW, vpWY + viewH));
  dl->AddRect(vpMM_TL, vpMM_BR, IM_COL32(255, 255, 255, 160), 0.0f, 0, 1.2f);

  ImVec2 mouse = ImGui::GetIO().MousePos;
  bool inMM = mouse.x >= mmTL.x && mouse.x <= mmBR.x && mouse.y >= mmTL.y &&
              mouse.y <= mmBR.y;

  if (inMM && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    float wx = (mouse.x - offX) / scl + mnX;
    float wy = (mouse.y - offY) / scl + mnY;
    Config::panOffset.x =
        -(wx - viewport->Size.x * 0.5f / Config::zoom) * Config::zoom;
    Config::panOffset.y =
        -(wy - viewport->Size.y * 0.5f / Config::zoom) * Config::zoom;
    ImClamp(&Config::panOffset.x, -Config::maxPan, Config::maxPan);
    ImClamp(&Config::panOffset.y, -Config::maxPan, Config::maxPan);
  }

  if (inMM) {
    ImGui::SetNextWindowBgAlpha(0.65f);
    ImGui::BeginTooltip();
    ImGui::Text("Zoom: %.2fx  |  Click to navigate", Config::zoom);
    ImGui::EndTooltip();
  }
}
