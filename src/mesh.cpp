#include "mesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <iostream>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

Mesh::Mesh(const ComPtr<ID3D11Device>& device, const std::vector<Vertex>& p_vertices,
           const std::vector<VertexIndex>& p_indices) {
  std::cout << "Mesh::create()" << std::endl;

  vertices = p_vertices;
  indices = p_indices;

  create_vertex_buffer(device);
  create_index_buffer(device);
  create_instance_data_cuffer(device);
}

Mesh::~Mesh() {
  std::cout << "Mesh::destroy()" << std::endl;
}

void Mesh::use(const ComPtr<ID3D11DeviceContext>& context) {
  constexpr UINT stride = sizeof(Vertex);
  constexpr UINT offset = 0;

  context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
  context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

  update_instance_data_cuffer(context);

  context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}

bool Mesh::update_instance_data_cuffer(const ComPtr<ID3D11DeviceContext>& context) {
  D3D11_MAPPED_SUBRESOURCE mapped_subresource;

  HRESULT hr = context->Map(instance_data_cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
  if (FAILED(hr)) {
    std::cout << "Mesh::Map() failed." << std::endl;
    return false;
  }

  memcpy(mapped_subresource.pData, &instance_data, sizeof(instance_data));

  context->Unmap(instance_data_cbuffer.Get(), 0);

  context->VSSetConstantBuffers(1, 1, instance_data_cbuffer.GetAddressOf());
  context->PSSetConstantBuffers(1, 1, instance_data_cbuffer.GetAddressOf());

  return true;
}

bool Mesh::create_vertex_buffer(const ComPtr<ID3D11Device>& device) {
  std::cout << "Mesh::create_vertex_buffer()" << std::endl;

  D3D11_BUFFER_DESC buffer_desc = {};
  buffer_desc.ByteWidth = sizeof(Vertex) * vertices.size();
  buffer_desc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
  buffer_desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;

  HRESULT hr;
  D3D11_SUBRESOURCE_DATA resource_data = {};
  resource_data.pSysMem = vertices.data();

  hr = device->CreateBuffer(&buffer_desc, &resource_data, &vertex_buffer);
  if (FAILED(hr)) {
    std::cout << "Mesh::CreateBuffer() failed." << std::endl;
    return false;
  }

  return true;
}

bool Mesh::create_index_buffer(const ComPtr<ID3D11Device>& device) {
  std::cout << "Mesh::create_index_buffer()" << std::endl;

  D3D11_BUFFER_DESC buffer_desc = {};
  buffer_desc.ByteWidth = static_cast<UINT>(sizeof(VertexIndex)) * static_cast<UINT>(indices.size());
  buffer_desc.Usage = D3D11_USAGE::D3D11_USAGE_IMMUTABLE;
  buffer_desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;

  HRESULT hr;
  D3D11_SUBRESOURCE_DATA resource_data = {};
  resource_data.pSysMem = indices.data();

  hr = device->CreateBuffer(&buffer_desc, &resource_data, &index_buffer);
  if (FAILED(hr)) {
    std::cout << "Mesh::CreateBuffer() failed." << std::endl;
    return false;
  }

  return true;
}

bool Mesh::create_instance_data_cuffer(const ComPtr<ID3D11Device>& device) {
  std::cout << "Mesh::create_instance_data_cuffer()" << std::endl;

  D3D11_BUFFER_DESC buffer_desc = {};
  buffer_desc.ByteWidth = sizeof(InstanceData);
  buffer_desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
  buffer_desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

  D3D11_SUBRESOURCE_DATA subresource_data = {};
  subresource_data.pSysMem = &instance_data;

  HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, &instance_data_cbuffer);
  if (FAILED(hr)) {
    std::cout << "Mesh::CreateBuffer() failed." << std::endl;
    return false;
  }

  return true;
}