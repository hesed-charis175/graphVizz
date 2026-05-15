#pragma once
static void showGrid(WindowData *winData) {
  ImDrawList *draw_list =
      ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());
  ImVec2 viewportPos = ImGui::GetMainViewport()->Pos;
  ImVec2 canvasSize = ImGui::GetMainViewport()->Size;

  float spacing = Config::gridSpacing * Config::zoom;

  if (spacing < 4.0f || spacing > 400.0f)
    return;

  float xOffset = fmodf(Config::panOffset.x, spacing);
  float yOffset = fmodf(Config::panOffset.y, spacing);
  if (xOffset < 0)
    xOffset += spacing;
  if (yOffset < 0)
    yOffset += spacing;

  float x0 = viewportPos.x;
  float y0 = viewportPos.y;
  float x1 = viewportPos.x + canvasSize.x;
  float y1 = viewportPos.y + canvasSize.y;

  for (float x = x0 + xOffset; x < x1; x += spacing)
    draw_list->AddLine(ImVec2(x, y0), ImVec2(x, y1),
                       IM_COL32(200, 200, 200, 40));

  for (float y = y0 + yOffset; y < y1; y += spacing)
    draw_list->AddLine(ImVec2(x0, y), ImVec2(x1, y),
                       IM_COL32(200, 200, 200, 40));
}

static ImVec2 joystickCenter;
static bool dragging = false;
static ImVec2 joystickOffset = ImVec2(0, 0);

ImVec2 ImLerp(const ImVec2 &a, const ImVec2 &b, float t) {
  return ImVec2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
}
float ImLengthSqr(const ImVec2 &v) { return v.x * v.x + v.y * v.y; }

ImVec2 ImVec2Normalize(const ImVec2 &v) {
  float len = sqrtf(v.x * v.x + v.y * v.y);
  return (len == 0.0f) ? ImVec2(0, 0) : ImVec2(v.x / len, v.y / len);
}

float ImClamp(float *v, float mn, float mx) {
  if (*v > mx)
    *v = mx;
  else if (*v < mn)
    *v = mn;
  return *v;
}

void showCircularJoystick(WindowData *winData) {
  ImGuiViewport *viewport = ImGui::GetMainViewport();
  ImDrawList *drawList = ImGui::GetBackgroundDrawList(viewport);

  const float PAD = 90.0f;
  const float radius = Config::joystickRadius;
  ImVec2 basePos = ImVec2(viewport->Pos.x + viewport->Size.x - PAD,
                          viewport->Pos.y + viewport->Size.y - PAD);
  joystickCenter = basePos;

  ImVec2 mousePos = ImGui::GetIO().MousePos;
  bool mouseDown = ImGui::IsMouseDown(0);

  float dSqr =
      (mousePos.x - joystickCenter.x) * (mousePos.x - joystickCenter.x) +
      (mousePos.y - joystickCenter.y) * (mousePos.y - joystickCenter.y);
  winData->hoveringJoystick = (dSqr <= radius * radius);

  if (!dragging && dSqr < radius * radius && mouseDown)
    dragging = true;
  if (!mouseDown)
    dragging = false;

  const float speed = 3.0f;
  const float maxOff = radius;
  ImVec2 normOff = {joystickOffset.x / maxOff, joystickOffset.y / maxOff};
  ImClamp(&normOff.x, -1.0f, 1.0f);
  ImClamp(&normOff.y, -1.0f, 1.0f);

  if (dragging) {
    joystickOffset = {mousePos.x - joystickCenter.x,
                      mousePos.y - joystickCenter.y};
    if (ImLengthSqr(joystickOffset) > radius * radius) {
      ImVec2 n = ImVec2Normalize(joystickOffset);
      joystickOffset = {n.x * radius, n.y * radius};
    }
    Config::panOffset.x += normOff.x * speed;
    Config::panOffset.y += normOff.y * speed;
    ImClamp(&Config::panOffset.x, -Config::maxPan, Config::maxPan);
    ImClamp(&Config::panOffset.y, -Config::maxPan, Config::maxPan);
  } else {
    joystickOffset = ImLerp(joystickOffset, ImVec2(0, 0), 0.2f);
  }

  drawList->AddCircleFilled(joystickCenter, radius,
                            IM_COL32(100, 100, 100, 100));
  drawList->AddCircleFilled({joystickCenter.x + joystickOffset.x,
                             joystickCenter.y + joystickOffset.y},
                            radius * 0.4f, IM_COL32(200, 200, 255, 180));
}

void scrollToZoom() {
  ImGuiIO &io = ImGui::GetIO();
  float scroll = io.MouseWheel;
  if (scroll == 0.0f)
    return;

  lastZoomFocus = io.MousePos;
  lastZoom = Config::zoom;

  float prevZoom = Config::zoom;
  Config::zoom *= (1.0f + scroll * 0.1f);
  ImClamp(&Config::zoom, 0.1f, 3.0f);

  float scale = Config::zoom / prevZoom;
  Config::panOffset.x =
      io.MousePos.x - (io.MousePos.x - Config::panOffset.x) * scale;
  Config::panOffset.y =
      io.MousePos.y - (io.MousePos.y - Config::panOffset.y) * scale;
}
