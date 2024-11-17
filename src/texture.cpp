#include <texture.h>

#define STB_IMAGE_IMPLEMENTATION

#include <assert.h>
#include <stb_image.h>
#include <iostream>

template <typename T>
using ComPtr = Microsoft::WRL::ComPtr<T>;

Texture::Texture(const ComPtr<ID3D11Device>& device, const std::string& file_path, const bool srgb) {
  std::cout << "Texture::create()" << std::endl;

  valid = true;

  format = srgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;

  stbi_set_flip_vertically_on_load(true);

  load_from_file(file_path);
  if (!valid)
    return;
  create_texture_2d(device);
  if (!valid)
    return;
  create_sampler_state(device);
  if (!valid)
    return;
  create_shader_resource_view(device);
  if (!valid)
    return;
}

Texture::~Texture() {
  std::cout << "Texture::destroy()" << std::endl;

  if (data != nullptr)
    stbi_image_free(data);
}

void Texture::load_from_file(const std::string& file_path) {
  data = stbi_load(file_path.c_str(), &width, &height, &channels, 4);
  if (data == nullptr) {
    std::cout << "Texture::stbi_load() failed." << std::endl;
    valid = false;
    return;
  }
}

void Texture::create_texture_2d(const ComPtr<ID3D11Device>& device) {
  DXGI_SAMPLE_DESC sample_desc = {};
  sample_desc.Count = 1;
  sample_desc.Quality = 0;

  D3D11_TEXTURE2D_DESC texture_2d_desc = {};
  texture_2d_desc.Width = width;
  texture_2d_desc.Height = height;
  texture_2d_desc.MipLevels = 1;
  texture_2d_desc.ArraySize = 1;
  texture_2d_desc.Format = format;
  texture_2d_desc.Usage = D3D11_USAGE_DEFAULT;
  texture_2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
  texture_2d_desc.CPUAccessFlags = 0;
  texture_2d_desc.SampleDesc = sample_desc;

  D3D11_SUBRESOURCE_DATA subresource_data = {};
  subresource_data.pSysMem = data;
  subresource_data.SysMemPitch = width * 4;
  subresource_data.SysMemSlicePitch = 0;

  auto hr = device->CreateTexture2D(&texture_2d_desc, &subresource_data, &texture);
  if (FAILED(hr)) {
    std::cout << "Texture::CreateTexture2D() failed." << std::endl;
    valid = false;
  }
}

void Texture::create_sampler_state(const ComPtr<ID3D11Device>& device) {
  D3D11_SAMPLER_DESC sampler_desc = {};
  sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sampler_desc.MinLOD = 0;
  sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

  auto hr = device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf());
  if (FAILED(hr)) {
    std::cout << "Texture::CreateSamplerState() failed." << std::endl;
    valid = false;
  }
}

void Texture::create_shader_resource_view(const ComPtr<ID3D11Device>& device) {
  D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc = {};
  shader_resource_view_desc.Format = format;
  shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
  shader_resource_view_desc.Texture2D.MipLevels = 1;

  auto hr = device->CreateShaderResourceView(texture.Get(), &shader_resource_view_desc, &shader_resource_view);
  if (FAILED(hr)) {
    std::cout << "Failed to create ShaderResourceView." << std::endl;
    valid = false;
  }
}

void Texture::use(const ComPtr<ID3D11DeviceContext>& context, const int slot) const {
  assert(valid);

  context->PSSetShaderResources(slot, 1, shader_resource_view.GetAddressOf());
  context->PSSetSamplers(slot, 1, sampler_state.GetAddressOf());
}