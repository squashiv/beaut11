#pragma once

#include <functional>
#include <vector>

struct Settings {
  const char* name = "Beaut11";
  int width = 1600;
  int height = 900;
  int fps = 144;
  float clear_color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  float time = 0.0f;
  int frame = 0;
  float delta = 1.0f / static_cast<float>(fps);
  bool wireframe = false;

  int input_ws = 0;
  int input_ad = 0;
  int input_mouse_x = 0;
  int input_mouse_y = 0;
  int input_mouse_wheel = 0;

  std::vector<std::function<void()>> wireframe_toggled;
  std::vector<std::function<void()>> window_resized;
};

extern Settings settings;