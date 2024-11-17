#pragma once

#include <SDL.h>
#include <SDL_syswm.h>

class Window {
 public:
  Window();
  ~Window();

  bool init();
  bool poll();
  HWND get_hwnd() const { return hwnd; }
  SDL_Window* get_sdl_window() const { return window; }

 private:
  HWND hwnd;
  SDL_Window* window;
  bool fps_mode;

 private:
  bool poll_event(const SDL_Event& event);
};