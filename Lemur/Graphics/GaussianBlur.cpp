// BLOOM
#include "GaussianBlur.h"

#include<vector>

#include "Shader.h"
#include "misc.h"

GaussianBlur::GaussianBlur(ID3D11Device* device, uint32_t width, uint32_t height)
{
	bit_block_transfer = std::make_unique<FullscreenQuad>(device);

	frame_buffer = std::make_unique<Framebuffer>(device, width, height, false);
	for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
	{
		gaussian_blur[downsampled_index][0] = std::make_unique<Framebuffer>(device, width >> downsampled_index, height >> downsampled_index, false);
		gaussian_blur[downsampled_index][1] = std::make_unique<Framebuffer>(device, width >> downsampled_index, height >> downsampled_index, false);
	}
	create_ps_from_cso(device, "./Shader/gaussian_blur_final_ps.cso", return_normal_ps.GetAddressOf());
	create_ps_from_cso(device, "./Shader/gaussian_bler_return_color.cso", final_ps.GetAddressOf());
	create_ps_from_cso(device, "./Shader/gaussian_blur_downsampling_ps.cso", gaussian_blur_downsampling_ps.GetAddressOf());
	create_ps_from_cso(device, "./Shader/gaussian_blur_horizontal_ps.cso", gaussian_blur_horizontal_ps.GetAddressOf());
	create_ps_from_cso(device, "./Shader/gaussian_blur_vertical_ps.cso", gaussian_blur_vertical_ps.GetAddressOf());
	create_ps_from_cso(device, "./Shader/gaussian_blur_upsampling_ps.cso", gaussian_blur_upsampling_ps.GetAddressOf());

	HRESULT hr{ S_OK };

	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{};
	depth_stencil_desc.DepthEnable = FALSE;
	depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_BLEND_DESC blend_desc{};
	blend_desc.AlphaToCoverageEnable = FALSE;
	blend_desc.IndependentBlendEnable = FALSE;
	blend_desc.RenderTarget[0].BlendEnable = FALSE;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blend_desc, blend_state.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void GaussianBlur::Make(ID3D11DeviceContext* immediate_context, ID3D11ShaderResourceView* color_map)
{
	// Store current states
	ID3D11ShaderResourceView* null_shader_resource_view{};
	ID3D11ShaderResourceView* cached_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	immediate_context->PSGetShaderResources(0, downsampled_count, cached_shader_resource_views);

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>  cached_depth_stencil_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>  cached_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11BlendState>  cached_blend_state;
	FLOAT blend_factor[4];
	UINT sample_mask;
	immediate_context->OMGetDepthStencilState(cached_depth_stencil_state.GetAddressOf(), 0);
	immediate_context->RSGetState(cached_rasterizer_state.GetAddressOf());
	immediate_context->OMGetBlendState(cached_blend_state.GetAddressOf(), blend_factor, &sample_mask);

	Microsoft::WRL::ComPtr<ID3D11Buffer>  cached_constant_buffer;
	immediate_context->PSGetConstantBuffers(8, 1, cached_constant_buffer.GetAddressOf());

	// Bind states
	immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	immediate_context->RSSetState(rasterizer_state.Get());
	immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);

	//TODO �K�E�V�A���u���[���V�F�[�_�[�ɒʂ��ƂȂ����@���}�b�v���O�ʂɏo�ĕ\�����ꂽ�̂ŁA��������Ă��艟���Ă���
	//	���Ǔ�Ԗڂ̃e�N�X�`���ɕ\������Ă���s��������Ă��Ȃ��̂Œ����Ă���
	// Extracting bright color
	frame_buffer->Clear(immediate_context, 0, 0, 0, 1);
	frame_buffer->Activate(immediate_context);
	ID3D11ShaderResourceView* final_pass_shader_resource_views[] =
	{
		color_map,
		nullptr,
		nullptr,
	};
	bit_block_transfer->Blit(immediate_context, final_pass_shader_resource_views, 0, _countof(final_pass_shader_resource_views), return_normal_ps.Get());
	frame_buffer->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	// Gaussian blur
	// Efficient Gaussian blur with linear sampling
	// http://rastergrid.com/blog/2010/09/efficient-gaussian-blur-with-linear-sampling/
	// Downsampling
	gaussian_blur[0][0]->Clear(immediate_context, 0, 0, 0, 1);
	gaussian_blur[0][0]->Activate(immediate_context);
	bit_block_transfer->Blit(immediate_context, frame_buffer->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_downsampling_ps.Get());
	gaussian_blur[0][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	// Ping-pong gaussian blur
	gaussian_blur[0][1]->Clear(immediate_context, 0, 0, 0, 1);
	gaussian_blur[0][1]->Activate(immediate_context);
	bit_block_transfer->Blit(immediate_context, gaussian_blur[0][0]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_horizontal_ps.Get());
	gaussian_blur[0][1]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	gaussian_blur[0][0]->Clear(immediate_context, 0, 0, 0, 1);
	gaussian_blur[0][0]->Activate(immediate_context);
	bit_block_transfer->Blit(immediate_context, gaussian_blur[0][1]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_vertical_ps.Get());
	gaussian_blur[0][0]->Deactivate(immediate_context);
	immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	for (size_t downsampled_index = 1; downsampled_index < downsampled_count; ++downsampled_index)
	{
		// Downsampling
		gaussian_blur[downsampled_index][0]->Clear(immediate_context, 0, 0, 0, 1);
		gaussian_blur[downsampled_index][0]->Activate(immediate_context);
		bit_block_transfer->Blit(immediate_context, gaussian_blur[downsampled_index - 1][0]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_downsampling_ps.Get());
		gaussian_blur[downsampled_index][0]->Deactivate(immediate_context);
		immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

		// Ping-pong gaussian blur
		gaussian_blur[downsampled_index][1]->Clear(immediate_context, 0, 0, 0, 1);
		gaussian_blur[downsampled_index][1]->Activate(immediate_context);
		bit_block_transfer->Blit(immediate_context, gaussian_blur[downsampled_index][0]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_horizontal_ps.Get());
		gaussian_blur[downsampled_index][1]->Deactivate(immediate_context);
		immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

		gaussian_blur[downsampled_index][0]->Clear(immediate_context, 0, 0, 0, 1);
		gaussian_blur[downsampled_index][0]->Activate(immediate_context);
		bit_block_transfer->Blit(immediate_context, gaussian_blur[downsampled_index][1]->shader_resource_views[0].GetAddressOf(), 0, 1, gaussian_blur_vertical_ps.Get());
		gaussian_blur[downsampled_index][0]->Deactivate(immediate_context);
		immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);
	}

	// Downsampling
	frame_buffer->Clear(immediate_context, 0, 0, 0, 1);
	frame_buffer->Activate(immediate_context);
	std::vector<ID3D11ShaderResourceView*> shader_resource_views;
	for (size_t downsampled_index = 0; downsampled_index < downsampled_count; ++downsampled_index)
	{
		shader_resource_views.push_back(gaussian_blur[downsampled_index][0]->shader_resource_views[0].Get());
	}
	bit_block_transfer->Blit(immediate_context, shader_resource_views.data(), 0, downsampled_count, gaussian_blur_upsampling_ps.Get());
	frame_buffer->Deactivate(immediate_context);

	ID3D11ShaderResourceView* final_pass_shader_resource_views_2[] =
	{
		frame_buffer->shader_resource_views[0].Get()
	};
	bit_block_transfer->Blit(immediate_context, final_pass_shader_resource_views_2, 0, _countof(final_pass_shader_resource_views_2), final_ps.Get());

	//immediate_context->PSSetShaderResources(0, 1, &null_shader_resource_view);

	//// Restore states
	//immediate_context->PSSetConstantBuffers(8, 1, cached_constant_buffer.GetAddressOf());

	//immediate_context->OMSetDepthStencilState(cached_depth_stencil_state.Get(), 0);
	//immediate_context->RSSetState(cached_rasterizer_state.Get());
	//immediate_context->OMSetBlendState(cached_blend_state.Get(), blend_factor, sample_mask);

	//immediate_context->PSSetShaderResources(0, downsampled_count, cached_shader_resource_views);
	//for (ID3D11ShaderResourceView* cached_shader_resource_view : cached_shader_resource_views)
	//{
	//	if (cached_shader_resource_view) cached_shader_resource_view->Release();
	
}
