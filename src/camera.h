#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/functions.hpp>
#include <iostream>

struct Camera {
  glm::mat4 transform = glm::mat4(1.0f);
  glm::vec3 position = glm::vec3(0.0f, 0.25f, 2.0f);
  glm::vec3 rotation = glm::vec3(0.0f);
  float near_plane = 0.1f;
  float far_plane = 100.0f;
  float fov = 75.0f;

  Camera() { std::cout << "Camera::create()" << std::endl; }
  ~Camera() { std::cout << "Camera::destroy()" << std::endl; }

  glm::mat4 get_view() const { return glm::inverse(transform); }

  glm::mat4 get_projection(const float aspect_ratio) const {
    return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
  }
};

extern Camera* camera;