#pragma once
void showAbout(WindowData *winData) {
  if (!ImGui::Begin("About Me", &winData->showAbout)) {
    ImGui::End();
    return;
  }

  ImGui::Text(
      "Graph Visualizer is a real-time visualization tool built in C++ "
      "with ImGui,"
      "\ndesigned to help users interactively explore graph "
      "structures, modify parameters,"
      "\nand test pathfinding algorithms. It provides a map-based "
      "interface where users can "
      "\nadjust the nature of node connectivity, set start and end "
      "points, and analyze algorithm performance. I hope it will, one day");

  static ImGuiTreeNodeFlags baseFlags =
      ImGuiTreeNodeFlags_DrawLinesFull | ImGuiTreeNodeFlags_DefaultOpen;

  if (ImGui::TreeNodeEx("Features", baseFlags)) {
    if (ImGui::TreeNodeEx("Map-based Graph Representation", baseFlags)) {

      ImGui::BulletText("city-like graph structures");
      ImGui::BulletText("Configurable node connectivity settings");
      ImGui::BulletText(
          "Click to set start, end, or intermediate nodes dynamically");
      ImGui::BulletText("If a new end point is assigned, the previous end "
                        "becomes an intermediate node");

      ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Customizable Appearance", baseFlags)) {

      ImGui::BulletText(
          "Adjust colors for nodes, edges, solution paths and backgrounds");
      ImGui::BulletText(
          "Highlight keypoints such as start, intermediate and end points");

      ImGui::TreePop();
    }

    if (ImGui::TreeNodeEx("Algorithm Testing and Logs", baseFlags)) {

      ImGui::BulletText("Supports BFS, DFS, A*, RRT, UCS, Greedy (eulerian "
                        "heuristics) algorithms");
      ImGui::BulletText(
          "Execution time tracking and graphs to analyse algorithm efficiency");
      ImGui::TreePop();
    }
    ImGui::TreePop();
  }
  ImGui::Spacing();
  ImGui::Text("Author: Bayiha Hesed Charis.");

  ImGui::End();
}
