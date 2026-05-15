#pragma once
void showStyleEditor(WindowData *winData) {
  // if(!ImGui::Begin("Appearance Editor", &winData->showStyleEditor)){
  //     ImGui::End();
  //     return;
  // }

  ImGui::Text("Edit appearance");

  if (ImGui::CollapsingHeader("Viewport")) {
    ImGui::ColorEdit4("Background Color", (float *)&Config::s_viewportColor);
    ImGui::SameLine();
    HelpMarker(Strings::colorHelpMarker);
    ImGui::Spacing();

    ImGui::Checkbox("Enable Grid", &Config::showGrids);
    ImGui::Spacing();

    ImGui::SliderFloat("Grid Spacing", &Config::gridSpacing,
                       Config::minGridSpacing, Config::maxGridSpacing);
  }

  if (ImGui::CollapsingHeader("Graph")) {

    static ImGuiTreeNodeFlags baseFlags =
        ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::SeparatorText("Nodes");

    if (ImGui::TreeNodeEx("Node Settings", baseFlags)) {

      if (ImGui::TreeNodeEx("General", baseFlags)) {

        ImGui::SliderInt("Node Size", &Config::nodeSize, Config::minNodeSize,
                         Config::maxNodeSize);

        ImGui::SliderFloat("Glow Factor", &Config::glowNodeFactor,
                           Config::minNodeGlowFactor,
                           Config::maxNodeGlowFactor);

        ImGui::SliderInt("Glow Iteration", &Config::glowNodeIterations,
                         Config::minNodeGlowIteration,
                         Config::maxNodeGlowIteration);

        ImGui::Checkbox("Outline", &Config::hasOutline);

        ImGui::Checkbox("Allow Node Dragging", &Config::allowNodeDragging);

        ImGui::Checkbox("Display Node Coordinates", &Config::displayPosition);

        int x = Config::s_textOffsetX, y = Config::s_textOffsetY;

        ImGui::SeparatorText("Coordinates Text Offset");
        ImGui::InputInt("X axis", &x, 10);

        if (x > Config::maxTextOffset)
          x = Config::maxTextOffset;
        if (x < Config::minTextOffset)
          x = Config::minTextOffset;

        ImGui::InputInt("Y axis", &y, 10);

        if (y > Config::maxTextOffset)
          y = Config::maxTextOffset;
        if (y < Config::minTextOffset)
          y = Config::minTextOffset;

        Config::s_textOffsetX = static_cast<float>(x);
        Config::s_textOffsetY = static_cast<float>(y);

        ImGui::TreePop();
      }

      if (ImGui::TreeNodeEx("Colors", baseFlags)) {
        ImGui::ColorEdit4("Default Color", (float *)&Config::s_nodeColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::ColorEdit4("Start Color", (float *)&Config::s_startColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::ColorEdit4("Intermediate Color",
                          (float *)&Config::s_intermediateColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::ColorEdit4("Goal Color", (float *)&Config::s_endColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TreePop();
      }

      ImGui::TreePop();
    }

    ImGui::SeparatorText("Edges");

    if (ImGui::TreeNodeEx("Edge Settings", baseFlags)) {
      if (ImGui::TreeNodeEx("General", baseFlags)) {
        ImGui::SliderInt("Edge Thickness", &Config::edgeThickness,
                         Config::minEdgeThickness, Config::maxEdgeThickness);

        ImGui::SliderFloat("Glow Factor", &Config::glowEdgeFactor,
                           Config::minEdgeGlowFactor,
                           Config::maxEdgeGlowFactor);

        ImGui::SliderInt("Glow Iteration", &Config::glowEdgeIterations,
                         Config::minEdgeGlowIteration,
                         Config::maxEdgeGlowIteration);

        ImGui::TreePop();
      }

      if (ImGui::TreeNodeEx("Colors", baseFlags)) {
        ImGui::ColorEdit4("Edge Color", (float *)&Config::s_edgeColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::ColorEdit4("Path 1 Color", (float *)&Config::s_pathColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::ColorEdit4("Path 2 Color", (float *)&Config::s_auxPathColor);
        ImGui::SameLine();
        HelpMarker(Strings::colorHelpMarker);
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::TreePop();
      }

      ImGui::TreePop();
    }
  }

  // ImGui::End();
}
