// BLOOM
#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl.h>

#include "Framebuffer.h"
#include "FullscreenQuad.h"

class GaussianBlur
{
public:
	GaussianBlur(ID3D11Device* device, uint32_t width, uint32_t height);
	~GaussianBlur() = default;
	GaussianBlur(const GaussianBlur&) = delete;
	GaussianBlur& operator =(const GaussianBlur&) = delete;
	GaussianBlur(GaussianBlur&&) noexcept = delete;
	GaussianBlur& operator =(GaussianBlur&&) noexcept = delete;

	void GaussianBlur::Make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map);
	ID3D11ShaderResourceView* ShaderResourceView() const
	{
		return frame_buffer->shader_resource_views[0].Get();
	}

public:
	float bloom_extraction_threshold = 1.00f;
	float bloom_intensity = 0.574f;

private:
	std::unique_ptr<FullscreenQuad> bit_block_transfer;
	std::unique_ptr<Framebuffer> frame_buffer;

	static const size_t downsampled_count = 8;
	std::unique_ptr<Framebuffer> gaussian_blur[downsampled_count][2];

	Microsoft::WRL::ComPtr<ID3D11PixelShader> return_normal_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> final_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_downsampling_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_horizontal_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_vertical_ps;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> gaussian_blur_upsampling_ps;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

};
