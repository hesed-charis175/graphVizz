#pragma once
#include <chrono>
#include <cmath>
#include <functional>
#include <limits>
#include <queue>
#include <random>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "render_core.hpp"

static std::vector<Node *> s_pathResult;
static std::vector<Node *> s_exploredResult;
static double s_lastPathTime = 0.0;
static bool s_pathFound = false;
static std::string s_pathStatusMsg = "";
static float s_pathTotalDist = 0.0f;

enum class HeuristicType {
  Euclidean = 0,
  Manhattan = 1,
  Chebyshev = 2,
  Unweighted = 3,
  Diagonal = 4
};
enum class AlgorithmType {
  BFS = 0,
  DFS = 1,
  AStar = 2,
  UCS = 3,
  Greedy = 4,
  RRT = 5
};

static HeuristicType s_heuristic = HeuristicType::Euclidean;
static AlgorithmType s_algorithm = AlgorithmType::AStar;

static float heuristic(const Node *a, const Node *goal, HeuristicType h) {
  float dx = std::fabs(a->position.x - goal->position.x);
  float dy = std::fabs(a->position.y - goal->position.y);
  switch (h) {
  case HeuristicType::Euclidean:
    return sqrtf(dx * dx + dy * dy);
  case HeuristicType::Manhattan:
    return dx + dy;
  case HeuristicType::Chebyshev:
    return std::max(dx, dy);
  case HeuristicType::Unweighted:
    return 1.0f;
  case HeuristicType::Diagonal:
    return (dx + dy) + (sqrtf(2.0f) - 2.0f) * std::min(dx, dy);
  }
  return sqrtf(dx * dx + dy * dy);
}

static float edgeW(const Node *a, const Node *b) {
  return a->position.distance(b->position);
}
static std::vector<Node *>
reconstructPath(std::unordered_map<Node *, Node *> &parent, Node *start,
                Node *end) {
  std::vector<Node *> path;
  for (Node *cur = end; cur != nullptr;) {
    path.push_back(cur);
    if (cur == start)
      break;
    auto it = parent.find(cur);
    if (it == parent.end())
      break;
    cur = it->second;
  }
  std::reverse(path.begin(), path.end());
  if (path.empty() || path.front() != start)
    return {};
  return path;
}

static std::vector<Node *> segBFS(Node *from, Node *to,
                                  std::vector<Node *> &explored) {
  std::queue<Node *> q;
  std::unordered_map<Node *, Node *> parent;
  std::unordered_set<Node *> visited;
  q.push(from);
  visited.insert(from);
  parent[from] = nullptr;

  while (!q.empty()) {
    Node *cur = q.front();
    q.pop();
    explored.push_back(cur);
    if (cur == to)
      return reconstructPath(parent, from, to);
    for (Node *nb : cur->neighbors)
      if (!visited.count(nb)) {
        visited.insert(nb);
        parent[nb] = cur;
        q.push(nb);
      }
  }
  return {};
}

static std::vector<Node *> segDFS(Node *from, Node *to,
                                  std::vector<Node *> &explored) {
  std::stack<Node *> stk;
  std::unordered_map<Node *, Node *> parent;
  std::unordered_set<Node *> visited;
  stk.push(from);
  parent[from] = nullptr;

  while (!stk.empty()) {
    Node *cur = stk.top();
    stk.pop();
    if (visited.count(cur))
      continue;
    visited.insert(cur);
    explored.push_back(cur);
    if (cur == to)
      return reconstructPath(parent, from, to);
    for (Node *nb : cur->neighbors)
      if (!visited.count(nb)) {
        parent[nb] = cur;
        stk.push(nb);
      }
  }
  return {};
}

static std::vector<Node *> segAStar(Node *from, Node *to, HeuristicType h,
                                    std::vector<Node *> &explored) {
  using P = std::pair<float, Node *>;
  std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
  std::unordered_map<Node *, float> g;
  std::unordered_map<Node *, Node *> parent;
  std::unordered_set<Node *> closed;

  g[from] = 0.0f;
  parent[from] = nullptr;
  pq.push({heuristic(from, to, h), from});

  while (!pq.empty()) {
    auto [f, cur] = pq.top();
    pq.pop();
    if (closed.count(cur))
      continue;
    closed.insert(cur);
    explored.push_back(cur);
    if (cur == to)
      return reconstructPath(parent, from, to);
    for (Node *nb : cur->neighbors) {
      if (closed.count(nb))
        continue;
      float ng = g[cur] + edgeW(cur, nb);
      if (!g.count(nb) || ng < g[nb]) {
        g[nb] = ng;
        parent[nb] = cur;
        pq.push({ng + heuristic(nb, to, h), nb});
      }
    }
  }
  return {};
}

static std::vector<Node *> segUCS(Node *from, Node *to,
                                  std::vector<Node *> &explored) {
  using P = std::pair<float, Node *>;
  std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
  std::unordered_map<Node *, float> cost;
  std::unordered_map<Node *, Node *> parent;
  std::unordered_set<Node *> visited;

  cost[from] = 0.0f;
  parent[from] = nullptr;
  pq.push({0.0f, from});

  while (!pq.empty()) {
    auto [c, cur] = pq.top();
    pq.pop();
    if (visited.count(cur))
      continue;
    visited.insert(cur);
    explored.push_back(cur);
    if (cur == to)
      return reconstructPath(parent, from, to);
    for (Node *nb : cur->neighbors) {
      float nc = cost[cur] + edgeW(cur, nb);
      if (!cost.count(nb) || nc < cost[nb]) {
        cost[nb] = nc;
        parent[nb] = cur;
        pq.push({nc, nb});
      }
    }
  }
  return {};
}

static std::vector<Node *> segGreedy(Node *from, Node *to, HeuristicType h,
                                     std::vector<Node *> &explored) {
  using P = std::pair<float, Node *>;
  std::priority_queue<P, std::vector<P>, std::greater<P>> pq;
  std::unordered_map<Node *, Node *> parent;
  std::unordered_set<Node *> visited;

  parent[from] = nullptr;
  pq.push({heuristic(from, to, h), from});

  while (!pq.empty()) {
    auto [hv, cur] = pq.top();
    pq.pop();
    if (visited.count(cur))
      continue;
    visited.insert(cur);
    explored.push_back(cur);
    if (cur == to)
      return reconstructPath(parent, from, to);
    for (Node *nb : cur->neighbors)
      if (!visited.count(nb)) {
        parent[nb] = cur;
        pq.push({heuristic(nb, to, h), nb});
      }
  }
  return {};
}

static std::vector<Node *> segRRT(Node *from, Node *to,
                                  std::vector<Node *> &explored) {
  if (s_Graph.nodes.empty())
    return {};

  std::mt19937 rng(std::random_device{}());
  std::uniform_int_distribution<int> nodeDist(0, (int)s_Graph.nodes.size() - 1);

  std::unordered_map<Node *, Node *> parent;
  std::unordered_set<Node *> tree;

  parent[from] = nullptr;
  tree.insert(from);
  explored.push_back(from);

  const int maxIter = 2000;
  for (int i = 0; i < maxIter; i++) {
    Node *target = (rng() % 100 < 15) ? to : s_Graph.nodes[nodeDist(rng)].get();

    Node *nearest = nullptr;
    float best = std::numeric_limits<float>::max();
    for (Node *t : tree) {
      float d = t->position.distance(target->position);
      if (d < best) {
        best = d;
        nearest = t;
      }
    }
    if (!nearest)
      continue;

    Node *extend = nullptr;
    float extBest = std::numeric_limits<float>::max();
    for (Node *nb : nearest->neighbors) {
      if (tree.count(nb))
        continue;
      float d = nb->position.distance(target->position);
      if (d < extBest) {
        extBest = d;
        extend = nb;
      }
    }
    if (!extend)
      continue;

    parent[extend] = nearest;
    tree.insert(extend);
    explored.push_back(extend);
    if (extend == to)
      return reconstructPath(parent, from, to);
  }
  return {};
}

static void appendSegment(std::vector<Node *> &full,
                          const std::vector<Node *> &seg) {
  if (seg.empty())
    return;
  if (!full.empty() && full.back() == seg.front())
    full.insert(full.end(), seg.begin() + 1, seg.end());
  else
    full.insert(full.end(), seg.begin(), seg.end());
}

static std::vector<Node *> buildWaypointSequence() {
  std::vector<Node *> seq;
  seq.push_back(s_Graph.start);
  for (Node *im : s_Graph.intermediateNodes)
    if (im)
      seq.push_back(im);
  seq.push_back(s_Graph.end);
  return seq;
}

static void runPathfinding() {
  if (!s_Graph.start || !s_Graph.end) {
    s_pathStatusMsg = "Set start and end nodes first!";
    s_pathFound = false;
    return;
  }

  s_pathResult.clear();
  s_exploredResult.clear();
  s_pathTotalDist = 0.0f;

  std::vector<Node *> waypoints = buildWaypointSequence();

  auto t0 = std::chrono::high_resolution_clock::now();
  bool allFound = true;

  for (size_t i = 0; i + 1 < waypoints.size(); i++) {
    Node *from = waypoints[i];
    Node *to = waypoints[i + 1];
    std::vector<Node *> seg;

    switch (s_algorithm) {
    case AlgorithmType::BFS:
      seg = segBFS(from, to, s_exploredResult);
      break;
    case AlgorithmType::DFS:
      seg = segDFS(from, to, s_exploredResult);
      break;
    case AlgorithmType::AStar:
      seg = segAStar(from, to, s_heuristic, s_exploredResult);
      break;
    case AlgorithmType::UCS:
      seg = segUCS(from, to, s_exploredResult);
      break;
    case AlgorithmType::Greedy:
      seg = segGreedy(from, to, s_heuristic, s_exploredResult);
      break;
    case AlgorithmType::RRT:
      seg = segRRT(from, to, s_exploredResult);
      break;
    }

    if (seg.empty()) {
      allFound = false;
      break;
    }
    appendSegment(s_pathResult, seg);
  }

  auto t1 = std::chrono::high_resolution_clock::now();
  s_lastPathTime = std::chrono::duration<double, std::milli>(t1 - t0).count();
  s_pathFound = allFound;

  for (size_t i = 0; i + 1 < s_pathResult.size(); i++)
    s_pathTotalDist +=
        s_pathResult[i]->position.distance(s_pathResult[i + 1]->position);

  if (allFound) {
    s_pathStatusMsg = "Path found!  " + std::to_string(s_pathResult.size()) +
                      " nodes  |  " + std::to_string(s_exploredResult.size()) +
                      " explored  |  " + std::to_string((int)s_lastPathTime) +
                      " ms";
  } else {
    s_pathStatusMsg = "No path found.  Explored " +
                      std::to_string(s_exploredResult.size()) + " nodes.";
  }
  Logger::log("[Pathfinding] " + s_pathStatusMsg);
}

static void drawPathOverlay() {
  if (Config::showExplored && !s_exploredResult.empty()) {
    for (Node *n : s_exploredResult) {
      if (!n)
        continue;
      drawNode(*n, Config::s_exploredColor, true);
    }
  }

  if (s_pathResult.size() < 2)
    return;

  ImDrawList *dl = ImGui::GetBackgroundDrawList(ImGui::GetMainViewport());
  ImVec2 origin = {ImGui::GetMainViewport()->Pos.x + Config::panOffset.x,
                   ImGui::GetMainViewport()->Pos.y + Config::panOffset.y};
  ImU32 pathCol = ImGui::ColorConvertFloat4ToU32(Config::s_pathColor);

  for (size_t i = 0; i + 1 < s_pathResult.size(); i++) {
    Node *a = s_pathResult[i];
    Node *b = s_pathResult[i + 1];
    if (!a || !b)
      continue;

    for (int g = 0; g < Config::glowEdgeIterations; g++) {
      float alpha =
          (Config::glowEdgeFactor - g * 0.1f) * 0.25f * Config::glowEdgeFactor;
      float thick = Config::edgeThickness + g * Config::glowEdgeFactor;
      ImVec4 gc = ImVec4(Config::s_pathColor.x, Config::s_pathColor.y,
                         Config::s_pathColor.z, alpha);
      dl->AddLine({a->position.x * Config::zoom + origin.x,
                   a->position.y * Config::zoom + origin.y},
                  {b->position.x * Config::zoom + origin.x,
                   b->position.y * Config::zoom + origin.y},
                  ImGui::ColorConvertFloat4ToU32(gc), thick);
    }
    dl->AddLine({a->position.x * Config::zoom + origin.x,
                 a->position.y * Config::zoom + origin.y},
                {b->position.x * Config::zoom + origin.x,
                 b->position.y * Config::zoom + origin.y},
                pathCol, (float)Config::edgeThickness);
  }
}

static void showPathfindingPanel(WindowData *winData) {

  ImGui::SeparatorText("Algorithm");
  const char *algNames[] = {
      "BFS", "DFS", "A*", "UCS (Dijkstra)", "Greedy Best-First", "RRT"};
  int alg = static_cast<int>(s_algorithm);
  if (ImGui::ListBox("##alg", &alg, algNames, IM_ARRAYSIZE(algNames), 6))
    s_algorithm = static_cast<AlgorithmType>(alg);

  bool needsH = (s_algorithm == AlgorithmType::AStar ||
                 s_algorithm == AlgorithmType::Greedy);
  if (needsH) {
    ImGui::Spacing();
    ImGui::SeparatorText("Heuristic");
    const char *hNames[] = {"Euclidean", "Manhattan", "Chebyshev", "Unweighted",
                            "Diagonal"};
    int h = static_cast<int>(s_heuristic);
    if (ImGui::ListBox("##heur", &h, hNames, IM_ARRAYSIZE(hNames), 5))
      s_heuristic = static_cast<HeuristicType>(h);
  }

  ImGui::Spacing();
  ImGui::SeparatorText("Options");
  ImGui::Checkbox("Show Explored Nodes", &Config::showExplored);

  ImGui::ColorEdit4("Path Color", (float *)&Config::s_pathColor);
  ImGui::ColorEdit4("Explored Color", (float *)&Config::s_exploredColor);

  ImGui::Spacing();
  ImGui::Separator();

  float bw =
      (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) *
      0.5f;
  if (ImGui::Button("Run##pf", ImVec2(bw, 0))) {
    runPathfinding();
  }
  ImGui::SameLine();
  if (ImGui::Button("Clear##pf", ImVec2(bw, 0))) {
    s_pathResult.clear();
    s_exploredResult.clear();
    s_pathStatusMsg.clear();
    s_pathFound = false;
    s_pathTotalDist = 0.0f;
  }

  if (!s_pathStatusMsg.empty()) {
    ImGui::Spacing();
    ImGui::Separator();
    ImVec4 statusCol = s_pathFound ? ImVec4(0.25f, 0.95f, 0.45f, 1.0f)
                                   : ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
    ImGui::TextColored(statusCol, "%s", s_pathStatusMsg.c_str());

    if (s_pathFound) {
      ImGui::Text("Total distance : %.2f units", s_pathTotalDist);
      ImGui::Text("Execution time : %.4f ms", s_lastPathTime);
      if (!s_Graph.intermediateNodes.empty())
        ImGui::Text("Waypoints used : %zu intermediate(s)",
                    s_Graph.intermediateNodes.size());
    }
  }

  ImGui::Spacing();
  ImGui::SeparatorText("Graph Info");
  ImGui::Text("Nodes   : %zu", s_Graph.nodes.size());
  ImGui::Text("Edges   : %zu", s_Graph.edges.size());
  if (s_Graph.start)
    ImGui::Text("Start   : NODE_ID %zu", s_Graph.start->node_id);
  else
    ImGui::TextDisabled("Start   : (not set)");
  if (s_Graph.end)
    ImGui::Text("End     : NODE_ID %zu", s_Graph.end->node_id);
  else
    ImGui::TextDisabled("End     : (not set)");
  ImGui::Text("Interm. : %zu node(s)", s_Graph.intermediateNodes.size());
}
