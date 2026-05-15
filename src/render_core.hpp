#pragma once
#include "config.hpp"
#include "graph_inst.hpp"
#include "utilities.hpp"
#include <chrono>
#include <cstdio>
#include <deque>
#include <functional>
#include <iomanip>
#include <sstream>
#include <unordered_set>

#define IM_MIN(A, B) (((A) < (B)) ? (A) : (B))

struct WindowData {
  bool showMainMenuBar = true;
  bool showAppSimpleOverlay = true;
  bool showMetrics = false;
  bool showAbout = false;
  bool showStyleEditor = false;
  bool showNodeEditor = false;
  bool showNodeDebugList = true;
  bool showDebugLogs = true;

  bool allNodesSelected = false;
  bool evaluateClick = true;
  bool updateNodes = true;
  bool editableNode = false;
  bool multiSelectionEnabled = false;
  bool keepMultiSelectionOpened = false;
  bool hoveringJoystick = false;
};

static WindowData s_winData;

static bool s_renderingFlag = true;

static bool s_CopiedOnceFlag = false;

static Node s_copiedNode;

static Node s_localGrEnd, s_localGrStart;

static Node *s_targetEditable;

static std::vector<Node *> s_selectedNodes;

static bool s_WasPopupOpen = false;

static Vec2 s_tempPosition;
static bool s_tempPositionCaptured = false;

static std::vector<NodeType> s_nodeTypes;

static ImVec2 lastZoomFocus;
static float lastZoom = Config::zoom;
