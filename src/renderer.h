#pragma once

#include <d3d11.h>
#include <dxgi1_3.h>
#include <wrl.h>
#include <memory>
#include "mesh.h"
#include "shader.h"
#include "texture.h"

class Renderer {
  template <typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

 public:
  Renderer();
  ~Renderer();

  ID3D11Device* get_device() { return device.Get(); };
  ID3D11DeviceContext* get_context() { return context.Get(); };

  bool init(const HWND hwnd);
  void on_resize();
  void render_begin();
  void render();
  void render_end();

 private:
  HWND hwnd;

  ComPtr<IDXGIFactory2> factory = nullptr;
  ComPtr<ID3D11Device> device = nullptr;
  ComPtr<ID3D11DeviceContext> context = nullptr;
  ComPtr<ID3D11Debug> debug = nullptr;
  ComPtr<IDXGISwapChain1> swap_chain = nullptr;
  ComPtr<ID3D11RenderTargetView> render_target_view = nullptr;
  ComPtr<ID3D11RasterizerState> rasterizer_state = nullptr;
  ComPtr<ID3D11Texture2D> depth_stencil_buffer = nullptr;
  ComPtr<ID3D11DepthStencilView> depth_stencil_view = nullptr;
  ComPtr<ID3D11DepthStencilState> depth_stencil_state = nullptr;

  ComPtr<ID3D11Buffer> shader_globals_cbuffer = nullptr;

 public:
  Shader* the_shader;
  Mesh* the_mesh;
  Texture* the_texture;

 private:
  bool create_factory();
  bool create_device_and_context();
  bool create_debug();
  bool create_swap_chain();
  bool create_swap_chain_resources();
  bool create_rasterizer_state(const bool wireframe);
  bool create_depth_stencil_state();
  void destroy_swap_chain_resources();
  bool create_shader_globals_cbuffer();
  bool update_shader_globals_cbuffer();
  bool load();
};