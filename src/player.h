#pragma once

#include <glm/gtx/color_space.hpp>
#include <glm/gtx/euler_angles.hpp>
#include "camera.h"
#include "settings.h"
#include "shader_globals.h"

struct Player {
  void update() {
    glm::vec3 forward = glm::vec3(glm::normalize(camera->transform[2]));
    glm::vec3 right = glm::vec3(glm::normalize(camera->transform[0]));

    camera->position +=
        (forward * static_cast<float>(-settings.input_ws) + (right * static_cast<float>(settings.input_ad))) *
        settings.delta * 25.0f;

    camera->rotation +=
        glm::vec3(static_cast<float>(-settings.input_mouse_y), static_cast<float>(-settings.input_mouse_x), 0.0f) *
        settings.delta * 50.0f;

    camera->transform = glm::mat4(1.0f);

    camera->transform = glm::translate(camera->transform, camera->position);

    camera->rotation.x = glm::clamp(camera->rotation.x, -89.0f, 89.0f);

    camera->transform = glm::rotate(camera->transform, glm::radians(camera->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    camera->transform = glm::rotate(camera->transform, glm::radians(camera->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));

    // camera->transform = glm::rotate(camera->transform, glm::radians(camera->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    camera->fov -= settings.input_mouse_wheel;

    shader_globals.view_projection =
        camera->get_projection(static_cast<float>(settings.width) / static_cast<float>(settings.height)) *
        camera->get_view();
  }
};