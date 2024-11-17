#include "window.h"

#include <imgui_impl_sdl2.h>
#include <iostream>
#include "settings.h"

Window::Window() {
  std::cout << "Window::create()" << std::endl;
}

Window::~Window() {
  if (window) {
    SDL_DestroyWindow(window);
  }
  SDL_Quit();

  std::cout << "Window::destroy()" << std::endl;
}

bool Window::init() {
  std::cout << "Window::init()" << std::endl;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
    return false;
  }

  window = SDL_CreateWindow(settings.name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, settings.width,
                            settings.height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!window) {
    std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
    return false;
  }

  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  if (SDL_GetWindowWMInfo(window, &info)) {
    hwnd = info.info.win.window;
  } else {
    std::cerr << "SDL_GetWindowWMInfo() failed: " << SDL_GetError() << std::endl;
    return false;
  }

  fps_mode = true;
  SDL_SetRelativeMouseMode(SDL_TRUE);
  SDL_ShowCursor(SDL_FALSE);

  return true;
}

bool Window::poll() {
  Uint32 delay = static_cast<Uint32>(1.0 / settings.fps * 1000);
  SDL_Delay(delay);

  settings.time = static_cast<float>(SDL_GetTicks()) / 1000.0f;
  settings.frame++;

  settings.input_ws = 0;
  settings.input_ad = 0;
  settings.input_mouse_x = 0;
  settings.input_mouse_y = 0;
  settings.input_mouse_wheel = 0;

  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    ImGui_ImplSDL2_ProcessEvent(&event);
    if (!poll_event(event))
      return false;
  }

  return true;
}

bool Window::poll_event(const SDL_Event& event) {
  switch (event.type) {
    case SDL_QUIT:
      return false;
    case SDL_KEYDOWN:
      switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_F5:
          return false;
        case SDL_SCANCODE_ESCAPE:
          if (!fps_mode) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            SDL_ShowCursor(SDL_FALSE);
          } else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            SDL_ShowCursor(SDL_TRUE);
          }
          fps_mode = !fps_mode;
          break;
        case SDL_SCANCODE_W:
          settings.input_ws += 1;
          break;
        case SDL_SCANCODE_S:
          settings.input_ws -= 1;
          break;
        case SDL_SCANCODE_A:
          settings.input_ad -= 1;
          break;
        case SDL_SCANCODE_D:
          settings.input_ad += 1;
          break;
        default:
          break;
      }
    case SDL_WINDOWEVENT:
      if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        settings.width = event.window.data1;
        settings.height = event.window.data2;
        for (auto& func : settings.window_resized) {
          func();
        }
      }
      break;
    case SDL_MOUSEMOTION:
      if (fps_mode) {
        settings.input_mouse_x = event.motion.xrel;
        settings.input_mouse_y = event.motion.yrel;
      }
      break;
    case SDL_MOUSEWHEEL:
      settings.input_mouse_wheel = event.wheel.y;
      break;
  }

  return true;
}