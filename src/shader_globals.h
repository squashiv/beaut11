#pragma once

#include <glm/mat4x4.hpp>

struct ShaderGlobals {
  glm::mat4 view_projection = glm::mat4(1.0f);

  glm::vec3 ambient_light_color = glm::vec3(1.0f, 1.0f, 1.0f);
  const float padding_1 = 0.0f;

  glm::vec3 directional_light_direction = glm::vec3(0.0f, 1.0f, 0.0f);
  const float padding_2 = 0.0f;
  glm::vec3 directional_light_color = glm::vec3(1.0f, 1.0f, 0.75f);
  float directional_light_intensity = 0.0f;

  glm::vec3 point_light_position = glm::vec3(0.0f, 3.0f, 0.0f);
  float point_light_radius = 10.0f;
  glm::vec3 point_light_color = glm::vec3(1.0f, 0.5f, 1.0f);
  float point_light_intensity = 10.0f;

  float time = 0.0f;
  int frame = 0;
  const float padding[2] = {};
};

extern ShaderGlobals shader_globals;