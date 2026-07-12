# Graph Viewer

A real-time graph and pathfinding visualizer built in C++ with [Dear ImGui](https://github.com/ocornut/imgui), GLFW, and OpenGL. It provides a map-based interface for generating graph structures, wiring them up by hand, and watching search algorithms explore them step by step.

## Features

### Graph generation
- Procedurally generate three kinds of graphs: **City** (radius-connected point cloud), **Maze**, and **Island**
- Configurable node count, connection radius, max neighbors per node, and RNG seed for reproducible layouts
- Click a node to mark it **Start**, **Intermediate**, or **End**

### Pathfinding
- Algorithms: **BFS, DFS, A\*, UCS, Greedy, and RRT**
- Heuristics: **Euclidean, Manhattan, Chebyshev, Unweighted, Diagonal**
- Visualizes the explored set and the final path
- Tracks execution time and other metrics per run

### Graph editing
- Add, edit, and delete nodes through a popup editor (position, type)
- Right-click drag from one node to another to create an edge, with snap to nearest node while dragging
- Multi-select nodes (hold + click, or Ctrl) to batch-edit node types at once
- Live node debug list with click-to-select

### Appearance & navigation
- Full color customization for nodes, edges, path, explored cells, and background, with adjustable glow effects
- Adjustable node and edge size and grid spacing, with a toggleable grid overlay
- Scroll to zoom, drag to pan, plus an on-screen circular joystick for panning
- Minimap showing the current viewport relative to the whole graph 

### Debugging tools
- In-app overlay showing recent keypresses (IO debug overlay)
- Threaded logger (logs are queued and processed on a background thread and viewable in an in-app log overlay)
- "About" panel documenting the app's own feature set

### Not yet implemented
- File menu items for Open,Save,Save As and Undo,Redo are present in the UI but are stubs (no-ops) — noted directly in the code as planned work.

## Project structure

```
graphViz/
├── backends/            # Dear ImGui renderer backends (GLFW + OpenGL3)
├── imgui/                # Dear ImGui library
├── screenshots
├── src/
│   ├── main.cpp          # App entry point: GLFW window and ImGui and OpenGL3 init and main loop
│   ├── config.hpp         # Colors, sizes, grid, minimap, map-gen & pathfinding defaults
│   ├── graph.hpp           # Node nad graph data model (Node, NodeType, GraphType)
│   ├── graph_inst.hpp      # Global graph instance
│   ├── pathfinding.hpp     # Search algorithms and heuristics
│   ├── map_generator.hpp   # Procedural City,Maze,Island graph generation
│   ├── node_editor.hpp     # Add,edit,delete node popup UI
│   ├── node_rendering.hpp  # Node interaction (click,drag,select) on the canvas
│   ├── draw_node.hpp       # Node and edge drawing primitives
│   ├── render.hpp          # Render orchestration
│   ├── render_core.hpp     # Render state and helpers
│   ├── io_overlay.hpp      # On-screen recent keypress debug overlay
│   ├── minimap.hpp         # Minimap and circular pan joystick
│   ├── menu_bar.hpp        # Main menu bar (File and Edit)
│   ├── style_editor.hpp    # Appearance and settings side panel
│   ├── show_about.hpp      # About panel
│   ├── logger.hpp/.cpp     # Background threaded logger
│   ├── matvec.hpp          # Vector, math helpers
│   ├── utilities.hpp       # Other helpers
│   └── Makefile            # Build file (see below)
└── LICENSE               
```

## Building

### Prerequisites

- A C++17 compiler (`g++` or `clang++`)
- [GLFW](https://www.glfw.org/)
  - Linux: `sudo apt-get install libglfw3-dev`
  - macOS: `brew install glfw`
  - MSYS2: `pacman -S --noconfirm --needed mingw-w64-x86_64-toolchain mingw-w64-x86_64-glfw`
- `pkg-config` (used to locate GLFW on Linux)
- An OpenGL driver available on your system

### Build & run

```bash
git clone https://github.com/hesed-charis175/graphViz.git
cd graphViz/src
make
./graph_viewer
```

Run `make clean` to remove build artifacts.

## Controls quick reference

| Action | Input |
|---|---|
| Pan | Drag canvas, or use the on-screen joystick |
| Zoom | Mouse scroll |
| Create edge | Right-click drag from one node to another |
| Multi-select nodes | Hold Ctrl and click nodes |
| Add,  edit , delete node | Click a node, or use the node editor popup |

## License

Licensed under the [MIT License](LICENSE).

## Author

Bayiha Hesed Charis
