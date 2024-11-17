#include "shader.h"

#include <iostream>
#include "vertex.h"

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

Shader::Shader() {
  std::cout << "Shader::create()" << std::endl;
}

Shader::~Shader() {
  std::cout << "Shader::destroy()" << std::endl;
}

bool Shader::compile(const std::wstring& file_path) {
  std::cout << "Shader::compile()" << std::endl;

  if (!compile_stage(vsb, file_path, "vs_main", "vs_5_0")) {
    return false;
  }

  if (!compile_stage(psb, file_path, "ps_main", "ps_5_0")) {
    return false;
  }

  return true;
}

bool Shader::create_vs_ps(const ComPtr<ID3D11Device>& device) {
  std::cout << "Shader::create_vs_ps()" << std::endl;

  HRESULT hr;

  hr = device->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, &vs);
  if (FAILED(hr)) {
    std::cout << "Shader::CreateVertexShader() failed." << std::endl;
    return false;
  }

  hr = device->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, &ps);
  if (FAILED(hr)) {
    std::cout << "Shader::CreatePixelShader() failed." << std::endl;
    return false;
  }

  return true;
}

bool Shader::create_input_layout(const ComPtr<ID3D11Device>& device) {
  std::cout << "Shader::create_input_layout()" << std::endl;

  constexpr D3D11_INPUT_ELEMENT_DESC input_layout_desc[] = {
      {"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, position),
       D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, normal),
       D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"UV", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, uv),
       D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
      {"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color),
       D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0},
  };

  HRESULT hr = device->CreateInputLayout(input_layout_desc, _countof(input_layout_desc), vsb->GetBufferPointer(),
                                         vsb->GetBufferSize(), &input_layout);
  if (FAILED(hr)) {
    std::cout << "Shader::CreateInputLayout() failed." << std::endl;
    return false;
  }

  return true;
}

void Shader::use(const ComPtr<ID3D11DeviceContext>& context) {
  context->IASetInputLayout(input_layout.Get());
  context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  context->VSSetShader(vs.Get(), nullptr, 0);
  context->PSSetShader(ps.Get(), nullptr, 0);
}

bool Shader::compile_stage(ComPtr<ID3DBlob>& blob, const std::wstring& file_path, const std::string& entry_point,
                           const std::string& profile) {
  std::cout << "Shader::compile_stage()" << std::endl;

  constexpr UINT compile_flags = D3DCOMPILE_ENABLE_STRICTNESS;

  ComPtr<ID3DBlob> error_blob = nullptr;

  HRESULT hr = D3DCompileFromFile(file_path.data(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entry_point.data(),
                                  profile.data(), compile_flags, 0, &blob, &error_blob);
  if (FAILED(hr)) {
    std::cout << "Renderer::D3DCompileFromFile() failed." << std::endl;
    if (error_blob) {
      std::cout << "Renderer::D3DCompileFromFile() failed." << static_cast<const char*>(error_blob->GetBufferPointer())
                << std::endl;
      return false;
    }
    return false;
  }

  return true;
}