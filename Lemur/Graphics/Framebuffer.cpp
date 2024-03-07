#include "Framebuffer.h"
#include "misc.h"

Framebuffer::Framebuffer(ID3D11Device* device, uint32_t width, uint32_t height, bool has_depthstencil /*BLOOM*/)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
	D3D11_TEXTURE2D_DESC texture2d_desc{};
	texture2d_desc.Width = width;
	texture2d_desc.Height = height;
	texture2d_desc.MipLevels = 1;
	texture2d_desc.ArraySize = 1;
	texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // DXGI_FORMAT_R8G8B8A8_UNORM
	texture2d_desc.SampleDesc.Count = 1;
	texture2d_desc.SampleDesc.Quality = 0;
	texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
	texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2d_desc.CPUAccessFlags = 0;
	texture2d_desc.MiscFlags = 0;
	hr = device->CreateTexture2D(&texture2d_desc, 0, render_target_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc{};
	render_target_view_desc.Format = texture2d_desc.Format;
	render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = device->CreateRenderTargetView(render_target_buffer.Get(), &render_target_view_desc, render_target_view.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	shader_resource_view_desc.Format = texture2d_desc.Format;
	shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shader_resource_view_desc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(render_target_buffer.Get(), &shader_resource_view_desc, shader_resource_views[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// BLOOM
	if (has_depthstencil)
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
		texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		hr = device->CreateTexture2D(&texture2d_desc, 0, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		shader_resource_view_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		hr = device->CreateShaderResourceView(depth_stencil_buffer.Get(), &shader_resource_view_desc, shader_resource_views[1].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void Framebuffer::Clear(ID3D11DeviceContext* immediate_context, float r, float g, float b, float a, float depth)
{
	float color[4]{ r, g, b, a };
	// レンダーターゲット内のすべての要素を一つの値に設定
	immediate_context->ClearRenderTargetView(
		render_target_view.Get(),	//　レンダーターゲットへのポインタ
		color						// レンダーターゲットを塗りつぶす色を表す４要素の配列
	);
	if (depth_stencil_view) // BLOOM
	{
		// 深度ステンシルリソースをクリア
		immediate_context->ClearDepthStencilView(
			depth_stencil_view.Get(),	// クリアする深度ステンシルへのポインター
			D3D11_CLEAR_DEPTH,			// 消去するデータの種類を特定
			depth,						// この値を使用して深度バッファーをクリア この値は 0 から 1 の間でクランプ
			0							// この値を使用してステンシル バッファーをクリア
		);
	}
}

void Framebuffer::Activate(ID3D11DeviceContext* immediate_context)
{
	viewport_count = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;

	// ラスタライザステージに現在渡されているビューポートの数を変数に代入
	immediate_context->RSGetViewports(
		&viewport_count,	// ビューポートの数を指定する変数へのポインター
		cached_viewports	// ラスタライザステージにバインドされているビューポートの構造体の配列
	);

	// 出力マージャーステージに渡されたリソースへのポインターを取得
	// （画面に書かれるものがどこにあるかを見つける手順）
	immediate_context->OMGetRenderTargets(
		1,													// 取得するレンダーターゲットの数
		cached_render_target_view.ReleaseAndGetAddressOf(),	// レンダーターゲットビューを表す配列へのポインター
		cached_depth_stencil_view.ReleaseAndGetAddressOf()	// 深度ステンシルビューへのポインター
	);

	// ビューポートの配列をパイプラインのラスタライザーステージに渡す
	// （何処に描画をしてほしいかを指示）
	immediate_context->RSSetViewports(
		1,					// バインドするビューポートの数
		&viewport			// ビューポート構造体の配列
	);

	// 全てのバッファの設定が同時に行い出力マージャーステージに深度ステンシルバッファを渡す
	immediate_context->OMSetRenderTargets(
		1,									// バインドするレンダー ターゲットの数
		render_target_view.GetAddressOf(),	// レンダーターゲットの配列へのポインター
		depth_stencil_view.Get()			// 深度ステンシル ビューの配列へのポインター
	);
}

void Framebuffer::Deactivate(ID3D11DeviceContext* immediate_context)
{
	// ビューポートの配列をパイプラインのラスタライザーステージに渡す
	// （何処に描画をしてほしいかを指示）
	immediate_context->RSSetViewports(
		viewport_count,				// バインドするビューポートの数
		cached_viewports			// ビューポート構造体の配列
	);

	// 全てのバッファの設定が同時に行い出力マージャーステージに深度ステンシルバッファを渡す
	immediate_context->OMSetRenderTargets(
		1,											// バインドするレンダー ターゲットの数
		cached_render_target_view.GetAddressOf(),	// レンダーターゲットの配列へのポインター
		cached_depth_stencil_view.Get()				// 深度ステンシル ビューの配列へのポインター
	);
}
