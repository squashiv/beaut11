#pragma once

#include <SDL.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl2.h>
#include <d3d11.h>
#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "imfilebrowser.h"
// #include "mesh_loader.h"
// #include "renderer.h"
#include "settings.h"
#include "shader_globals.h"

inline bool ends_with(std::string const& value, std::string const& ending) {
  if (ending.size() > value.size())
    return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

struct GUI {
  bool valid;
  ImGuiContext* context;
  ImGui::FileBrowser* file_browser;
  // Renderer* renderer;

  GUI(SDL_Window* sdl_window, ID3D11Device* d3d11_device, ID3D11DeviceContext* d3d11_device_context) {
    std::cout << "GUI::create()" << std::endl;

    valid = true;

    IMGUI_CHECKVERSION();

    context = ImGui::CreateContext();
    if (context == nullptr) {
      std::cerr << "GUI::CreateContext() failed." << std::endl;
      valid = false;
      return;
    }

    ImGui::StyleColorsDark();

    valid = ImGui_ImplSDL2_InitForD3D(sdl_window);
    if (!valid) {
      std::cerr << "GUI::ImGui_ImplSDL2_InitForD3D() failed." << std::endl;
      valid = false;
      ImGui::DestroyContext();
      return;
    }

    valid = ImGui_ImplDX11_Init(d3d11_device, d3d11_device_context);
    if (!valid) {
      std::cerr << "GUI::ImGui_ImplSDL2_InitForD3D() failed." << std::endl;
      valid = false;
      ImGui_ImplSDL2_Shutdown();
      ImGui::DestroyContext();
      return;
    }

    file_browser = new ImGui::FileBrowser();
  }

  ~GUI() {
    std::cout << "GUI::destroy()" << std::endl;
    if (valid) {
      ImGui_ImplDX11_Shutdown();
      ImGui_ImplSDL2_Shutdown();
      ImGui::DestroyContext();
    }
  }

  // void load_selected(const std::string& file_path) {
  //   if (ends_with(file_path, ".obj")) {
  //     std::vector<Vertex> vertices;
  //     std::vector<VertexIndex> indices;
  //     MeshLoader::load_obj("assets/sponza.obj", vertices, indices);
  //     renderer->the_mesh = new Mesh(renderer->get_device(), vertices, indices);
  //   } else {
  //     auto the_texture = new Texture(renderer->get_device(), file_path, true);
  //     if (the_texture->is_valid())
  //       renderer->the_texture = the_texture;
  //   }
  // }

  void update() {
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::Text("Beaut11");

    ImGui::ColorEdit4("Clear Color", settings.clear_color);
    ImGui::ColorEdit3("Ambient Light Color", glm::value_ptr(shader_globals.ambient_light_color));

    ImGui::Text("Directional Light");
    ImGui::ColorEdit3("DL Color", glm::value_ptr(shader_globals.directional_light_color));
    ImGui::DragFloat("DL Intensity", &shader_globals.directional_light_intensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat3("DL Direction", glm::value_ptr(shader_globals.directional_light_direction), 0.1f, 0.0f, 1.0f);

    ImGui::Text("Point Light");
    ImGui::ColorEdit3("PL Color", glm::value_ptr(shader_globals.point_light_color));
    ImGui::DragFloat("PL Intensity", &shader_globals.point_light_intensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat3("PL Position", glm::value_ptr(shader_globals.point_light_position), 0.1f);
    ImGui::DragFloat("PL Radius", &shader_globals.point_light_radius, 0.1f, 0.0f, 100.0f);

    if (ImGui::Checkbox("Wireframe", &settings.wireframe)) {
      for (auto& func : settings.wireframe_toggled) {
        func();
      }
    }

    if (ImGui::Button("File")) {
      file_browser->Open();
    }

    file_browser->Display();

    if (file_browser->HasSelected()) {
      auto file_path = std::filesystem::relative(file_browser->GetSelected()).string();
      std::cout << "GUI::GetSelected() " << file_path << std::endl;
      file_browser->ClearSelected();
      // load_selected(file_path);
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
  }
};