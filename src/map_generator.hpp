#pragma once
#include <algorithm>
#include <cmath>
#include <random>
#include <stack>

#include "pathfinding.hpp"

static bool edgeExists(Node *a, Node *b) {
  for (auto &e : s_Graph.edges)
    if ((e.first == a && e.second == b) || (e.first == b && e.second == a))
      return true;
  return false;
}
static void generateCityGraph(int nodeCount, float mapW, float mapH,
                              float radius, int maxNeigh, int seed) {
  s_Graph.clearGraph();

  std::mt19937 rng(seed);
  std::uniform_real_distribution<float> rx(30.0f, mapW - 30.0f);
  std::uniform_real_distribution<float> ry(30.0f, mapH - 30.0f);

  float minSep = radius * 0.36f;

  int placed = 0, tries = 0;
  const int maxTries = nodeCount * 120;
  while (placed < nodeCount && tries < maxTries) {
    float x = rx(rng), y = ry(rng);
    bool ok = true;
    for (auto &n : s_Graph.nodes)
      if (n->position.distance(Vec2(x, y)) < minSep) {
        ok = false;
        break;
      }
    if (ok) {
      s_Graph.addNode(x, y);
      placed++;
    }
    tries++;
  }

  size_t N = s_Graph.nodes.size();
  for (size_t i = 0; i < N; i++) {
    std::vector<std::pair<float, size_t>> cands;
    for (size_t j = 0; j < N; j++) {
      if (i == j)
        continue;
      float d = s_Graph.nodes[i]->position.distance(s_Graph.nodes[j]->position);
      if (d <= radius)
        cands.push_back({d, j});
    }
    std::sort(cands.begin(), cands.end());
    int conn = 0;
    for (auto &[d, j] : cands) {
      if (conn >= maxNeigh)
        break;
      if (!edgeExists(s_Graph.nodes[i].get(), s_Graph.nodes[j].get())) {
        s_Graph.addEdge(s_Graph.nodes[i].get(), s_Graph.nodes[j].get());
        conn++;
      }
    }
  }
  Logger::log("[MapGen/City] " + std::to_string(s_Graph.nodes.size()) +
              " nodes, " + std::to_string(s_Graph.edges.size()) + " edges.");
}

static void generateMazeGraph(int rows, int cols, float mapW, float mapH,
                              int seed) {
  s_Graph.clearGraph();

  float cellW = mapW / cols;
  float cellH = mapH / rows;

  std::vector<std::vector<Node *>> grid(rows,
                                        std::vector<Node *>(cols, nullptr));
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      float x = cellW * c + cellW * 0.5f;
      float y = cellH * r + cellH * 0.5f;
      s_Graph.addNode(x, y);
      grid[r][c] = s_Graph.nodes.back().get();
    }
  }

  std::mt19937 rng(seed);
  std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
  std::stack<std::pair<int, int>> stk;

  std::uniform_int_distribution<int> rr(0, rows - 1), rc(0, cols - 1);
  int sr = rr(rng), sc = rc(rng);
  stk.push({sr, sc});
  visited[sr][sc] = true;

  const int DR[] = {-1, 1, 0, 0};
  const int DC[] = {0, 0, -1, 1};

  while (!stk.empty()) {
    auto [r, c] = stk.top();
    std::vector<int> unvis;
    for (int d = 0; d < 4; d++) {
      int nr = r + DR[d], nc = c + DC[d];
      if (nr >= 0 && nr < rows && nc >= 0 && nc < cols && !visited[nr][nc])
        unvis.push_back(d);
    }
    if (unvis.empty()) {
      stk.pop();
    } else {
      std::uniform_int_distribution<int> pick(0, (int)unvis.size() - 1);
      int dir = unvis[pick(rng)];
      int nr = r + DR[dir], nc = c + DC[dir];
      s_Graph.addEdge(grid[r][c], grid[nr][nc]);
      visited[nr][nc] = true;
      stk.push({nr, nc});
    }
  }
  Logger::log("[MapGen/Maze] " + std::to_string(rows) + "x" +
              std::to_string(cols) + " = " +
              std::to_string(s_Graph.nodes.size()) + " nodes.");
}
static void generateIslandGraph(int nodeCount, float mapW, float mapH,
                                float connRadius, int maxNeigh, int seed) {
  s_Graph.clearGraph();

  std::mt19937 rng(seed);
  float minSep = std::min(mapW, mapH) / (sqrtf((float)nodeCount) * 1.4f);

  std::uniform_real_distribution<float> rx(30.0f, mapW - 30.0f);
  std::uniform_real_distribution<float> ry(30.0f, mapH - 30.0f);

  int placed = 0, tries = 0;
  const int maxTries = nodeCount * 200;
  while (placed < nodeCount && tries < maxTries) {
    float x = rx(rng), y = ry(rng);
    bool ok = true;
    for (auto &n : s_Graph.nodes)
      if (n->position.distance(Vec2(x, y)) < minSep) {
        ok = false;
        break;
      }
    if (ok) {
      s_Graph.addNode(x, y);
      placed++;
    }
    tries++;
  }

  size_t N = s_Graph.nodes.size();
  for (size_t i = 0; i < N; i++) {
    std::vector<std::pair<float, size_t>> dists;
    for (size_t j = 0; j < N; j++) {
      if (i == j)
        continue;
      float d = s_Graph.nodes[i]->position.distance(s_Graph.nodes[j]->position);
      if (d <= connRadius)
        dists.push_back({d, j});
    }
    std::sort(dists.begin(), dists.end());
    int conn = 0;
    for (auto &[d, j] : dists) {
      if (conn >= maxNeigh)
        break;
      if (!edgeExists(s_Graph.nodes[i].get(), s_Graph.nodes[j].get())) {
        s_Graph.addEdge(s_Graph.nodes[i].get(), s_Graph.nodes[j].get());
        conn++;
      }
    }
  }
  Logger::log("[MapGen/Island] " + std::to_string(s_Graph.nodes.size()) +
              " nodes, " + std::to_string(s_Graph.edges.size()) + " edges.");
}

static void showMapGeneratorPanel(WindowData *winData) {

  ImGui::SeparatorText("Graph Type");
  const char *types[] = {"City", "Maze", "Island"};
  ImGui::Combo("##gtype", &Config::mapGenGraphType, types, IM_ARRAYSIZE(types));

  ImGui::Spacing();
  ImGui::SeparatorText("Parameters");

  ImGui::SliderInt("Seed", &Config::mapGenSeed, 0, 9999);

  if (Config::mapGenGraphType == 1) { // Maze
    ImGui::SliderInt("Rows", &Config::mapGenMazeRows, 3, 30);
    ImGui::SliderInt("Cols", &Config::mapGenMazeCols, 3, 40);
  } else { // City / Island
    ImGui::SliderInt("Node Count", &Config::mapGenNodeCount, 5, 300);
    ImGui::SliderFloat("Connection Radius", &Config::mapGenConnectionRadius,
                       30.f, 600.f);
    ImGui::SliderInt("Max Connections/Node", &Config::mapGenMaxNeighbors, 1,
                     12);
  }

  ImGui::Spacing();
  ImGui::SeparatorText("Canvas");
  ImGui::SliderFloat("Map Width", &Config::mapGenMapWidth, 200.0f, 1800.0f);
  ImGui::SliderFloat("Map Height", &Config::mapGenMapHeight, 200.0f, 1100.0f);

  ImGui::Spacing();
  ImGui::Separator();
  ImGui::TextColored(ImVec4(1.0f, 0.75f, 0.1f, 1.0f),
                     "! This will replace the current graph.");
  ImGui::Spacing();

  float bw =
      (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) *
      0.5f;

  if (ImGui::Button("Generate", ImVec2(bw, 0))) {
    s_pathResult.clear();
    s_exploredResult.clear();
    s_pathStatusMsg.clear();
    s_pathFound = false;

    switch (Config::mapGenGraphType) {
    case 0:
      generateCityGraph(Config::mapGenNodeCount, Config::mapGenMapWidth,
                        Config::mapGenMapHeight, Config::mapGenConnectionRadius,
                        Config::mapGenMaxNeighbors, Config::mapGenSeed);
      break;
    case 1:
      generateMazeGraph(Config::mapGenMazeRows, Config::mapGenMazeCols,
                        Config::mapGenMapWidth, Config::mapGenMapHeight,
                        Config::mapGenSeed);
      break;
    case 2:
      generateIslandGraph(Config::mapGenNodeCount, Config::mapGenMapWidth,
                          Config::mapGenMapHeight,
                          Config::mapGenConnectionRadius,
                          Config::mapGenMaxNeighbors, Config::mapGenSeed);
      break;
    }
    Config::zoom = 1.0f;
    Config::panOffset = ImVec2(0, 0);
  }

  ImGui::SameLine();
  if (ImGui::Button("Clear All", ImVec2(bw, 0))) {
    s_Graph.clearGraph();
    s_pathResult.clear();
    s_exploredResult.clear();
    s_pathStatusMsg.clear();
    s_pathFound = false;
  }

  ImGui::Spacing();
  ImGui::SeparatorText("Current Graph");
  ImGui::Text("Nodes  : %zu", s_Graph.nodes.size());
  ImGui::Text("Edges  : %zu", s_Graph.edges.size());

  if (!s_Graph.nodes.empty()) {
    float totalDeg = 0.0f;
    for (auto &n : s_Graph.nodes)
      totalDeg += (float)n->neighbors.size();
    ImGui::Text("Avg degree : %.2f", totalDeg / s_Graph.nodes.size());
  }
}
