#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_sdl2.h>
#include <imgui.h>
#include "camera.h"
#include "gui.h"
#include "player.h"
#include "renderer.h"
#include "settings.h"
#include "shader_globals.h"
#include "window.h"

Window* window;
Renderer* renderer;
GUI* gui;
Player* player;
// Camera* camera;

bool init() {
  std::cout << "Engine::init()" << std::endl;

  window = new Window();
  if (!window->init())
    return false;

  renderer = new Renderer();
  if (!renderer->init(window->get_hwnd()))
    return false;

  gui = new GUI(window->get_sdl_window(), renderer->get_device(), renderer->get_context());
  if (!gui->valid)
    return false;

  // gui->renderer = renderer;

  player = new Player();

  camera = new Camera();

  return true;
}

void cleanup() {
  std::cout << "Engine::cleanup()" << std::endl;

  if (camera)
    delete camera;
  if (player)
    delete player;
  if (gui)
    delete gui;
  if (renderer)
    delete renderer;
  if (window)
    delete window;
}

int main(int argc, char** argv) {
  if (!init()) {
    cleanup();
    return -1;
  }

  while (true) {
    if (!window->poll())
      break;

    shader_globals.time = settings.time;
    shader_globals.frame = settings.frame;

    renderer->render_begin();

    player->update();

    renderer->render();

    gui->update();

    renderer->render_end();
  }

  cleanup();

  return 0;
}