#pragma once

#include "imgui.h"

namespace Config {

const int defaultWindowWidth = 1280;
const int defaultWindowHeight = 720;

int windowWidth;
int windowHeight;

bool showGrids = true;
float minGridSpacing = 2.0f;
float maxGridSpacing = 80.0f;
float gridSpacing = 40.0f;

float joystickRadius = 60.0f;

float zoom = 1.0f;

ImVec2 panOffset = ImVec2(0, 0);
float maxPan = 200.0f;

int nodeSize = 10;
int minNodeSize = 5;
int maxNodeSize = 30;

float minNodeGlowFactor = 0.0f;
float glowNodeFactor = 1.0f;
float maxNodeGlowFactor = 2.0f;

int glowNodeIterations = 5;
int maxNodeGlowIteration = 8;
int minNodeGlowIteration = 0;

bool hasOutline = true;
bool allowNodeDragging = true;
bool displayPosition = true;

float selectionRadius = 10.0f;

float edgeSnapRadius = 18.0f;
float edgeDeletionThreshold = 5.0f;

int edgeThickness = 3;

int maxEdgeThickness = 10;
int minEdgeThickness = 1;

float minEdgeGlowFactor = 0.0f;
float glowEdgeFactor = 1.0f;
float maxEdgeGlowFactor = 2.0f;
int glowEdgeIterations = 5;
int maxEdgeGlowIteration = 8;
int minEdgeGlowIteration = 0;

const ImVec4 red = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
const ImVec4 blue = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
const ImVec4 green = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);

const ImVec4 black = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
const ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

const ImVec4 orange = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
const ImVec4 yellow = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

const ImVec4 darkTheme = ImVec4(0.0901961f, 0.101961f, 0.109804f, 1.00f);
const ImVec4 blueDarkTheme = ImVec4(0.187212f, 0.219003f, 0.240196f, 1.0f);
const ImVec4 redDarkTheme = ImVec4(0.112745f, 0.011053f, 0.011053f, 1.0f);

ImVec4 s_viewportColor = darkTheme;
ImVec4 s_edgeColor = white;
ImVec4 s_nodeColor = blue;
ImVec4 s_startColor = green;
ImVec4 s_endColor = red;
ImVec4 s_intermediateColor = orange;
ImVec4 s_pathColor = yellow;
ImVec4 s_auxPathColor = ImVec4(0.4f, 0.9f, 1.0f, 1.0f);
ImVec4 s_selectionColor = yellow;
ImVec4 s_exploredColor = ImVec4(0.55f, 0.55f, 0.55f, 0.45f);

float s_textOffsetX = 15.0f, s_textOffsetY = 0.0f;
float maxTextOffset = 30.0f, minTextOffset = 0.0f;

bool showMinimap = true;
float minimapWidth = 220.0f;
float minimapHeight = 150.0f;

int mapGenGraphType = 0; // 0=City  1=Maze  2=Island
int mapGenNodeCount = 40;
float mapGenConnectionRadius = 160.0f;
int mapGenMaxNeighbors = 4;
int mapGenSeed = 42;
int mapGenMazeRows = 10;
int mapGenMazeCols = 14;
float mapGenMapWidth = 900.0f;
float mapGenMapHeight = 550.0f;

bool showExplored = true;
bool showPathAnimation = false;
int pathAlgorithm = 2;
int pathHeuristic = 0;

bool showPerformanceMetrics = true;
bool enableDebugTools = true;
} // namespace Config

namespace Strings {
const char *version = "1.0.0 unique";
const char *colorHelpMarker =
    "Click on the color square to open a color picker\n"
    "Click and hold to use drag and drop.\n"
    "Right-click on the color square to show options.\n"
    "CTRL+click on individual component to input value";
const char *nodeHelpMarker =
    "Select a node type.\n\n"
    "Start is the start point of the search - It holds only one node\n\n"
    "Intermediate represents nodes that must be visited by the algorithm.\n\n"
    "End is the goal of the search - It holds only one node\n\n";
} // namespace Strings
