#pragma once
static void ShowMenuFile() {
  if (ImGui::MenuItem("(File Management)", NULL, false, false)) {
  }
  if (ImGui::MenuItem("New")) {
  }
  if (ImGui::MenuItem("Open", "Ctrl+O")) {
  }
  if (ImGui::BeginMenu("Open Recent")) {

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // TODO: Add stack structure to handle recent files based on json saved node
    // configurations, parameters, edges... etc
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ImGui::EndMenu();
  }
  if (ImGui::MenuItem("Save", "Ctrl+S")) {
  }
  if (ImGui::MenuItem("Save As..")) {
  }

  ImGui::Separator();
  if (ImGui::MenuItem("Quit", "Alt+F4")) {
  }
}

static void showAppMainMenuBar(WindowData *winData) {
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ShowMenuFile();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Edit")) {
      if (ImGui::MenuItem("Undo", "CTRL+Z")) {
      }
      if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Cut", "CTRL+X")) {
      }
      if (ImGui::MenuItem("Copy", "CTRL+C")) {
      }
      if (ImGui::MenuItem("Paste", "CTRL+V")) {
      }
      ImGui::EndMenu();
    }
  }
  ImGui::EndMainMenuBar();
}
