#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <glm/mat4x4.hpp>
#include <iostream>
#include <vector>
#include "vertex.h"

class Mesh {
  template <typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

 public:
  __declspec(align(16)) struct InstanceData {
    glm::mat4 transform = glm::mat4(1);
    glm::vec4 color = glm::vec4(1);
  };

 public:
  InstanceData instance_data;

 public:
  Mesh(const ComPtr<ID3D11Device>& device, const std::vector<Vertex>& vertices,
       const std::vector<VertexIndex>& indices);
  ~Mesh();

 public:
  void use(const ComPtr<ID3D11DeviceContext>& context);

 private:
  std::vector<Vertex> vertices;
  std::vector<VertexIndex> indices;
  ComPtr<ID3D11Buffer> vertex_buffer = nullptr;
  ComPtr<ID3D11Buffer> index_buffer = nullptr;
  ComPtr<ID3D11Buffer> instance_data_cbuffer = nullptr;

 private:
  bool create_vertex_buffer(const ComPtr<ID3D11Device>& device);
  bool create_index_buffer(const ComPtr<ID3D11Device>& device);
  bool create_instance_data_cuffer(const ComPtr<ID3D11Device>& device);
  bool update_instance_data_cuffer(const ComPtr<ID3D11DeviceContext>& context);
};
