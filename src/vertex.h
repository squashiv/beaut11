#pragma once

#include <stdint.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

typedef uint32_t VertexIndex;

__declspec(align(16)) struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
  glm::vec3 color;

  constexpr Vertex() = delete;
  constexpr Vertex(const glm::vec3 position, const glm::vec3 normal, const glm::vec2 uv, const glm::vec3 color)
      : position(position), normal(normal), uv(uv), color(color) {}
};