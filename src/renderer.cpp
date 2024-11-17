#include "renderer.h"

#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include "mesh_loader.h"
#include "settings.h"
#include "shader_globals.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "dxguid.lib")

// op
Renderer::Renderer() {
  std::cout << "Renderer::create()" << std::endl;
}

Renderer::~Renderer() {
  std::cout << "Renderer::destroy()" << std::endl;

  delete the_shader;
  delete the_mesh;
  delete the_texture;

  rasterizer_state.Reset();

  depth_stencil_state.Reset();
  depth_stencil_view.Reset();
  depth_stencil_buffer.Reset();

  context->Flush();
  destroy_swap_chain_resources();

  swap_chain.Reset();
  factory.Reset();
  context.Reset();

  HRESULT hr = debug->ReportLiveDeviceObjects(D3D11_RLDO_FLAGS::D3D11_RLDO_DETAIL);
  if (FAILED(hr)) {
    std::cerr << "Renderer::ReportLiveDeviceObjects() failed." << std::endl;
    return;
  }
  debug.Reset();
  device.Reset();
}

// public
bool Renderer::init(const HWND hwnd) {
  std::cout << "Renderer::init()" << std::endl;

  this->hwnd = hwnd;

  if (!create_factory())
    return false;
  if (!create_device_and_context())
    return false;
  if (!create_debug())
    return false;
  if (!create_swap_chain())
    return false;
  if (!create_swap_chain_resources())
    return false;
  if (!create_rasterizer_state(settings.wireframe))
    return false;
  if (!create_depth_stencil_state())
    return false;
  if (!create_shader_globals_cbuffer())
    return false;

  if (!load())
    return false;

  settings.wireframe_toggled.push_back([&]() { this->create_rasterizer_state(settings.wireframe); });
  settings.window_resized.push_back([&]() { this->on_resize(); });

  return true;
}

void Renderer::on_resize() {
  std::cout << "Renderer::on_resize()" << std::endl;

  context->Flush();

  destroy_swap_chain_resources();

  HRESULT hr =
      swap_chain->ResizeBuffers(0, settings.width, settings.height, DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM, 0);
  if (FAILED(hr)) {
    std::cerr << "Renderer::ResizeBuffers() failed." << std::endl;
    return;
  }

  create_swap_chain_resources();

  create_depth_stencil_state();
}

void Renderer::render_begin() {
  D3D11_VIEWPORT viewport = {};
  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width = static_cast<float>(settings.width);
  viewport.Height = static_cast<float>(settings.height);
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;

  context->ClearRenderTargetView(render_target_view.Get(), settings.clear_color);
  context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  context->RSSetViewports(1, &viewport);
  context->RSSetState(rasterizer_state.Get());

  context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());
  context->OMSetDepthStencilState(depth_stencil_state.Get(), 1);

  update_shader_globals_cbuffer();
}

void Renderer::render() {
  the_shader->use(context);
  the_texture->use(context, 0);
  the_mesh->instance_data.transform = glm::mat4(1.0f);
  the_mesh->use(context);
}

void Renderer::render_end() {
  HRESULT hr = swap_chain->Present(1, 0);
  if (FAILED(hr)) {
    std::cerr << "Renderer::Present() failed." << std::endl;
    return;
  }
}

// private
bool Renderer::create_factory() {
  HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateDXGIFactory1() failed." << std::endl;
    return false;
  }

  return true;
}

bool Renderer::create_device_and_context() {
  constexpr D3D_FEATURE_LEVEL expected_feature_level = D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0;
  constexpr UINT device_flags =
      D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG;

  HRESULT hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE::D3D_DRIVER_TYPE_HARDWARE, nullptr,
                                 D3D11_CREATE_DEVICE_FLAG::D3D11_CREATE_DEVICE_DEBUG, &expected_feature_level, 1,
                                 D3D11_SDK_VERSION, &device, nullptr, &context);
  if (FAILED(hr)) {
    std::cerr << "Renderer::D3D11CreateDevice() failed." << std::endl;
    return false;
  }

  return true;
}

bool Renderer::create_debug() {
  if (FAILED(device.As(&debug))) {
    std::cerr << "Renderer::D3D11CreateDevice() debug failed." << std::endl;
    return false;
  }

  return true;
}

bool Renderer::create_swap_chain() {
  DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
  swap_chain_desc.Width = settings.width;
  swap_chain_desc.Height = settings.height;
  swap_chain_desc.Format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
  swap_chain_desc.SampleDesc.Count = 1;
  swap_chain_desc.SampleDesc.Quality = 0;
  swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swap_chain_desc.BufferCount = 2;
  swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD;
  swap_chain_desc.Scaling = DXGI_SCALING::DXGI_SCALING_STRETCH;
  swap_chain_desc.Flags = {};

  DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc = {};
  swap_chain_fullscreen_desc.Windowed = true;

  HRESULT hr = factory->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc, &swap_chain_fullscreen_desc,
                                               nullptr, &swap_chain);
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateSwapChainForHwnd() failed." << std::endl;
    return false;
  }

  return true;
}

bool Renderer::create_swap_chain_resources() {
  std::cout << "Renderer::create_swap_chain_resources()" << std::endl;

  ComPtr<ID3D11Texture2D> back_buffer = nullptr;
  HRESULT hr;

  hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
  if (FAILED(hr)) {
    std::cerr << "Renderer::GetBuffer() failed." << std::endl;
    return false;
  }

  hr = device->CreateRenderTargetView(back_buffer.Get(), nullptr, &render_target_view);
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateRenderTargetView() failed." << std::endl;
    return false;
  }

  return true;
}

bool Renderer::create_rasterizer_state(const bool wireframe) {
  std::cout << "Renderer::create_rasterizer_state()" << std::endl;

  D3D11_RASTERIZER_DESC rasterizer_desc = {};
  rasterizer_desc.FillMode = wireframe ? D3D11_FILL_MODE::D3D11_FILL_WIREFRAME : D3D11_FILL_MODE::D3D11_FILL_SOLID;
  rasterizer_desc.CullMode = D3D11_CULL_BACK;
  rasterizer_desc.FrontCounterClockwise = true;  // ??
  rasterizer_desc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
  rasterizer_desc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
  rasterizer_desc.DepthClipEnable = true;
  rasterizer_desc.ScissorEnable = false;
  rasterizer_desc.MultisampleEnable = false;
  rasterizer_desc.AntialiasedLineEnable = false;

  HRESULT hr = device->CreateRasterizerState(&rasterizer_desc, &rasterizer_state);
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateRasterizerState() failed." << std::endl;
    return false;
  }

  return true;
}

bool Renderer::create_depth_stencil_state() {
  std::cout << "Renderer::create_depth_stencil_state()" << std::endl;

  HRESULT hr;

  D3D11_DEPTH_STENCIL_DESC depth_stencil_state_desc = {};
  depth_stencil_state_desc.DepthEnable = true;
  depth_stencil_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depth_stencil_state_desc.DepthFunc = D3D11_COMPARISON_LESS;
  depth_stencil_state_desc.StencilEnable = false;

  hr = device->CreateDepthStencilState(&depth_stencil_state_desc, depth_stencil_state.GetAddressOf());
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateDepthStencilState() failed." << std::endl;
    return false;
  }

  D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc = {};
  depth_stencil_buffer_desc.Width = settings.width;
  depth_stencil_buffer_desc.Height = settings.height;
  depth_stencil_buffer_desc.MipLevels = 1;
  depth_stencil_buffer_desc.ArraySize = 1;
  depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depth_stencil_buffer_desc.SampleDesc.Count = 1;
  depth_stencil_buffer_desc.SampleDesc.Quality = 0;
  depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
  depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  depth_stencil_buffer_desc.CPUAccessFlags = 0;

  hr = device->CreateTexture2D(&depth_stencil_buffer_desc, nullptr, depth_stencil_buffer.GetAddressOf());
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateTexture2D() failed." << std::endl;
    return false;
  }

  hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), nullptr, depth_stencil_view.GetAddressOf());
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateDepthStencilView() failed." << std::endl;
    return false;
  }

  return true;
}

void Renderer::destroy_swap_chain_resources() {
  std::cout << "Renderer::destroy_swap_chain_resources()" << std::endl;

  render_target_view.Reset();
}

bool Renderer::create_shader_globals_cbuffer() {
  std::cout << "Renderer::create_global_cbuffer()" << std::endl;

  D3D11_BUFFER_DESC buffer_desc = {};
  buffer_desc.ByteWidth = sizeof(ShaderGlobals);
  buffer_desc.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
  buffer_desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
  buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;

  D3D11_SUBRESOURCE_DATA subresource_data = {};
  subresource_data.pSysMem = &shader_globals;

  HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, &shader_globals_cbuffer);
  if (FAILED(hr)) {
    std::cerr << "Renderer::CreateBuffer() failed." << std::endl;
    return false;
  }

  context->VSSetConstantBuffers(0, 1, shader_globals_cbuffer.GetAddressOf());
  context->PSSetConstantBuffers(0, 1, shader_globals_cbuffer.GetAddressOf());

  return true;
}

bool Renderer::update_shader_globals_cbuffer() {
  D3D11_MAPPED_SUBRESOURCE mapped_subresource;

  HRESULT hr = context->Map(shader_globals_cbuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
  if (FAILED(hr)) {
    std::cerr << "Renderer::Map() failed." << std::endl;
    return false;
  }

  memcpy(mapped_subresource.pData, &shader_globals, sizeof(ShaderGlobals));

  context->Unmap(shader_globals_cbuffer.Get(), 0);

  context->VSSetConstantBuffers(0, 1, shader_globals_cbuffer.GetAddressOf());
  context->PSSetConstantBuffers(0, 1, shader_globals_cbuffer.GetAddressOf());

  return true;
}

bool Renderer::load() {
  the_shader = new Shader();
  if (!the_shader->compile(L"assets/the_shader.hlsl"))
    return false;
  if (!the_shader->create_vs_ps(device))
    return false;
  if (!the_shader->create_input_layout(device))
    return false;

  std::vector<Vertex> vertices;
  std::vector<VertexIndex> indices;
  MeshLoader::load_obj("assets/the_mesh.obj", vertices, indices);
  the_mesh = new Mesh(device, vertices, indices);

  the_texture = new Texture(device, "assets/the_texture.png", true);
  if (!the_texture->is_valid())
    return false;

  return true;
}