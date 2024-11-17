#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <string>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

struct Texture {
 public:
  explicit Texture(const ComPtr<ID3D11Device>& device, const std::string& file_path, const bool srgb);
  ~Texture();

  void use(const ComPtr<ID3D11DeviceContext>& context, const int slot) const;
  bool is_valid() { return valid; }

 private:
  bool valid;
  unsigned char* data;
  int width;
  int height;
  int channels;
  DXGI_FORMAT format;
  ComPtr<ID3D11Texture2D> texture = nullptr;
  ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
  ComPtr<ID3D11SamplerState> sampler_state = nullptr;

 private:
  Texture() = delete;
  Texture(const Texture&) = delete;
  Texture& operator=(const Texture&) = delete;

  void load_from_file(const std::string& file_path);
  void create_texture_2d(const ComPtr<ID3D11Device>& device);
  void create_sampler_state(const ComPtr<ID3D11Device>& device);
  void create_shader_resource_view(const ComPtr<ID3D11Device>& device);
};