#pragma once
#include <bits/stdc++.h>

static void HelpMarker(const char *desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::BeginItemTooltip()) {
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}
inline float computeDistanceToSegment(const ImVec2 &point,
                                      const ImVec2 &lineStart,
                                      const ImVec2 &lineStop) {
  ImVec2 line = ImVec2(lineStop.x - lineStart.x, lineStop.y - lineStart.y);
  ImVec2 lineStartToPoint =
      ImVec2(point.x - lineStart.x, point.y - lineStart.y);

  float lineLength = line.x * line.x + line.y * line.y;
  float normalized =
      lineLength > 0.0f
          ? (lineStartToPoint.x * line.x + lineStartToPoint.y * line.y) /
                lineLength
          : 0.0f;
  normalized = std::clamp(normalized, 0.0f, 1.0f);

  ImVec2 closest = ImVec2(lineStart.x + line.x * normalized,
                          lineStart.y + line.y * normalized);
  ImVec2 delta = ImVec2(point.x - closest.x, point.y - closest.y);
  return std::sqrt(delta.x * delta.x + delta.y * delta.y);
}
