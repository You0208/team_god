#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <cstdint>
#include <functional>

#include "FullscreenQuad.h"
#include "Framebuffer.h"

class ShadowMap
{
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

public:
	ShadowMap(ID3D11Device* device, uint32_t width, uint32_t height);
	virtual ~ShadowMap() = default;
	ShadowMap(const ShadowMap&) = delete;
	ShadowMap& operator =(const ShadowMap&) = delete;
	ShadowMap(ShadowMap&&) noexcept = delete;
	ShadowMap& operator =(ShadowMap&&) noexcept = delete;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_VIEWPORT viewport;

	void Clear(ID3D11DeviceContext* immediate_context, float depth = 1);
	void Activate(ID3D11DeviceContext* immediate_context);
	void Deactivate(ID3D11DeviceContext* immediate_context);

private:
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
};

#pragma once
