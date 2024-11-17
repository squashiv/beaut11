#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl.h>
#include <string>

class Shader {
  template <typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

 public:
  Shader();
  ~Shader();

 public:
  bool compile(const std::wstring& file_path);
  bool create_vs_ps(const ComPtr<ID3D11Device>& device);
  bool create_input_layout(const ComPtr<ID3D11Device>& device);
  void use(const ComPtr<ID3D11DeviceContext>& context);

  // private:
  ComPtr<ID3DBlob> vsb = nullptr;
  ComPtr<ID3DBlob> psb = nullptr;
  ComPtr<ID3D11VertexShader> vs = nullptr;
  ComPtr<ID3D11PixelShader> ps = nullptr;
  ComPtr<ID3D11InputLayout> input_layout = nullptr;

 private:
  bool compile_stage(ComPtr<ID3DBlob>& blob, const std::wstring& file_path, const std::string& entry_point,
                     const std::string& profile);
};