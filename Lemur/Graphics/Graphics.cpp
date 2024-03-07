#include "Graphics.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"

namespace Lemur::Graphics
{
	void acquire_high_performance_adapter(IDXGIFactory6* dxgi_factory6, IDXGIAdapter3** dxgi_adapter3)
	{
		HRESULT hr{ S_OK };

		Microsoft::WRL::ComPtr<IDXGIAdapter3> enumerated_adapter;
		for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != dxgi_factory6->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumerated_adapter.ReleaseAndGetAddressOf())); ++adapter_index)
		{
			DXGI_ADAPTER_DESC1 adapter_desc;
			hr = enumerated_adapter->GetDesc1(&adapter_desc);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
			{
				OutputDebugStringW((std::wstring(adapter_desc.Description) + L" has been selected.\n").c_str());
				OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapter_desc.VendorId) + '\n').c_str());
				OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapter_desc.DeviceId) + '\n').c_str());
				OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapter_desc.SubSysId) + '\n').c_str());
				OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapter_desc.Revision) + '\n').c_str());
				OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapter_desc.DedicatedVideoMemory) + '\n').c_str());
				OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapter_desc.DedicatedSystemMemory) + '\n').c_str());
				OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapter_desc.SharedSystemMemory) + '\n').c_str());
				OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapter_desc.AdapterLuid.HighPart) + '\n').c_str());
				OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapter_desc.AdapterLuid.LowPart) + '\n').c_str());
				OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapter_desc.Flags) + '\n').c_str());
				break;
			}
		}
		*dxgi_adapter3 = enumerated_adapter.Detach();
	}

	Graphics* Graphics::instance = nullptr;

	Graphics::Graphics(HWND hwnd, bool fullscreen) : hwnd(hwnd), fullscreen_mode(fullscreen), windowed_style(static_cast<DWORD>(GetWindowLongPtrW(hwnd, GWL_STYLE)))
	{
		// インスタンス設定
		instance = this;

		// ウィンドウサイズの調整
		if (fullscreen)
		{
			StylizeWindow(TRUE);
		}

		RECT client_rect;
		GetClientRect(hwnd, &client_rect);

#if 0
		framebuffer_dimensions.cx = client_rect.right - client_rect.left;
		framebuffer_dimensions.cy = client_rect.bottom - client_rect.top;
#else
		framebuffer_dimensions.cx = SCREEN_WIDTH;
		framebuffer_dimensions.cy = SCREEN_HEIGHT;
#endif

		HRESULT hr{ S_OK };


		// ADAPTER
		//IDXGIFactory* factory;
		//CreateDXGIFactory(IID_PPV_ARGS(&factory));
		//IDXGIAdapter* adapter;
		//for (UINT adapter_index = 0; S_OK == factory->EnumAdapters(adapter_index, &adapter); ++adapter_index) {
		//	DXGI_ADAPTER_DESC adapter_desc;
		//	adapter->GetDesc(&adapter_desc);
		//	if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
		//	{
		//		break;
		//	}
		//	adapter->Release();
		//}
		//factory->Release();

	// 改善後のフルスクリーン
		UINT create_factory_flags{};
#ifdef _DEBUG
		create_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
		hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		acquire_high_performance_adapter(dxgi_factory6.Get(), adapter_screen.GetAddressOf());


		UINT create_device_flags{ 0 };
#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#ifdef ENABLE_DIRECT2D
		create_device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif
		D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_1 };
		hr = D3D11CreateDevice(adapter_screen.Get(), D3D_DRIVER_TYPE_UNKNOWN, 0, create_device_flags, &feature_levels, 1, D3D11_SDK_VERSION, &device, NULL, &immediate_context);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		CreateSwapChain(dxgi_factory6.Get());
#ifdef ENABLE_DIRECT2D
		create_direct2d_objects();
#endif

		//// ADAPTER
		//adapter->Release();

		//ID3D11Texture2D* back_buffer{};
		//hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer));
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//hr = device->CreateRenderTargetView(back_buffer, NULL, &render_target_view);
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//back_buffer->Release();


		debugRenderer = std::make_unique<DebugRenderer>(device.Get());
	}

	void Graphics::CreateSwapChain(IDXGIFactory6* dxgi_factory6)
	{
		HRESULT hr{ S_OK };

		if (swap_chain)
		{
			ID3D11RenderTargetView* null_render_target_view{};
			immediate_context->OMSetRenderTargets(1, &null_render_target_view, NULL);
			render_target_view.Reset();
#if 0
			immediate_context->Flush();
			immediate_context->ClearState();
#endif
			DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
			swap_chain->GetDesc(&swap_chain_desc);
			hr = swap_chain->ResizeBuffers(swap_chain_desc.BufferCount, framebuffer_dimensions.cx, framebuffer_dimensions.cy, swap_chain_desc.BufferDesc.Format, swap_chain_desc.Flags);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
			hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			D3D11_TEXTURE2D_DESC texture2d_desc;
			render_target_buffer->GetDesc(&texture2d_desc);

			hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, render_target_view.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}
		else
		{
			BOOL allow_tearing = FALSE;
			if (SUCCEEDED(hr))
			{
				hr = dxgi_factory6->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allow_tearing, sizeof(allow_tearing));
			}
			tearing_supported = SUCCEEDED(hr) && allow_tearing;

			DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};
			swap_chain_desc1.Width = framebuffer_dimensions.cx;
			swap_chain_desc1.Height = framebuffer_dimensions.cy;
			swap_chain_desc1.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			swap_chain_desc1.Stereo = FALSE;
			swap_chain_desc1.SampleDesc.Count = 1;
			swap_chain_desc1.SampleDesc.Quality = 0;
			swap_chain_desc1.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swap_chain_desc1.BufferCount = 2;
			swap_chain_desc1.Scaling = DXGI_SCALING_STRETCH;
			swap_chain_desc1.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			swap_chain_desc1.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
			swap_chain_desc1.Flags = tearing_supported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;
			hr = dxgi_factory6->CreateSwapChainForHwnd(device.Get(), hwnd, &swap_chain_desc1, NULL, NULL, swap_chain.ReleaseAndGetAddressOf());
#if 0
			swap_chain_desc1.Flags |= DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
#endif
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			hr = dxgi_factory6->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

			Microsoft::WRL::ComPtr<ID3D11Texture2D> render_target_buffer;
			hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(render_target_buffer.GetAddressOf()));
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			hr = device->CreateRenderTargetView(render_target_buffer.Get(), NULL, render_target_view.ReleaseAndGetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = framebuffer_dimensions.cx;
		texture2d_desc.Height = framebuffer_dimensions.cy;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		depth_stencil_view_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_VIEWPORT viewport{};
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = static_cast<float>(framebuffer_dimensions.cx);
		viewport.Height = static_cast<float>(framebuffer_dimensions.cy);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		immediate_context->RSSetViewports(1, &viewport);
	}

#ifdef ENABLE_DIRECT2D
	void Graphics::create_direct2d_objects()
	{
		HRESULT hr{ S_OK };

		Microsoft::WRL::ComPtr<IDXGIDevice2> dxgi_device2;
		hr = device.As(&dxgi_device2);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<ID2D1Factory1> d2d_factory1;
		D2D1_FACTORY_OPTIONS factory_options{};
#ifdef _DEBUG
		factory_options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, factory_options, d2d_factory1.GetAddressOf());

		Microsoft::WRL::ComPtr<ID2D1Device> d2d_device;
		hr = d2d_factory1->CreateDevice(dxgi_device2.Get(), d2d_device.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = d2d_device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, d2d1_device_context.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = dxgi_device2->SetMaximumFrameLatency(1);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<IDXGISurface2> dxgi_surface2;
		hr = swap_chain->GetBuffer(0, IID_PPV_ARGS(dxgi_surface2.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<ID2D1Bitmap1> d2d_bitmap1;
		hr = d2d1_device_context->CreateBitmapFromDxgiSurface(dxgi_surface2.Get(),
			D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), d2d_bitmap1.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		d2d1_device_context->SetTarget(d2d_bitmap1.Get());


		Microsoft::WRL::ComPtr<IDWriteFactory> dwrite_factory;
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwrite_factory.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = dwrite_factory->CreateTextFormat(L"Meiryo", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 11, L"", dwrite_text_formats[0].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = dwrite_text_formats[0]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = dwrite_factory->CreateTextFormat(L"Impact", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 24, L"", dwrite_text_formats[1].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = dwrite_text_formats[1]->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = d2d1_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), d2d_solid_color_brushes[0].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = d2d1_device_context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::CornflowerBlue), d2d_solid_color_brushes[1].ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
#endif

	void Graphics::StylizeWindow(BOOL fullscreen)
	{
		fullscreen_mode = fullscreen;
		if (fullscreen)
		{
			// ウィンドウサイズの取得
			GetWindowRect(hwnd, &windowed_rect);
			//TODO ウィンドウの属性を変更(?)
			SetWindowLongPtrA(hwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW & ~(WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME));

			RECT fullscreen_window_rect;

			HRESULT hr{ E_FAIL };
			if (swap_chain)// スワップチェインに画像が入ってるとき
			{
				//　出力インターフェース（アダプターの出力等を表す）
				Microsoft::WRL::ComPtr<IDXGIOutput> dxgi_output;
				hr = swap_chain->GetContainingOutput(&dxgi_output);// ディスプレイ モニターを取得
				if (hr == S_OK)// 成功した場合
				{
					//typedef struct DXGI_OUTPUT_DESC {
					//	WCHAR              DeviceName[32];　		出力デバイスの名前を含む文字列
					//	RECT               DesktopCoordinates;　デスクトップ座標
					//	BOOL               AttachedToDesktop;　	出力がデスクトップに接続される場合は True。それ以外の場合は false
					//	DXGI_MODE_ROTATION Rotation;　			出力によって画像がどのように回転されるか
					//	HMONITOR           Monitor;　			ディスプレイモニターを表すHMONITORハンドル
					//} DXGI_OUTPUT_DESC;
					DXGI_OUTPUT_DESC output_desc;
					hr = dxgi_output->GetDesc(&output_desc);// テクスチャリソースのプロパティを取得

					if (hr == S_OK)// 成功した場合
					{
						// 取得した座標を代入
						fullscreen_window_rect = output_desc.DesktopCoordinates;
					}
				}
			}
			if (hr != S_OK)// 成功していないor画像が入ってない場合
			{
				// DEVMODE：表示装置の初期設定と環境に関する情報
				DEVMODE devmode = {};
				devmode.dmSize = sizeof(DEVMODE);
				EnumDisplaySettings(
					NULL,					// ディスプレイデバイスを指定するため、NULL 
					ENUM_CURRENT_SETTINGS,	// 現在のデスクトップ設定を取得するための指定
					&devmode				// 取得した設定を格納
				);

				fullscreen_window_rect = {
					devmode.dmPosition.x,
					devmode.dmPosition.y,
					devmode.dmPosition.x + static_cast<LONG>(devmode.dmPelsWidth),
					devmode.dmPosition.y + static_cast<LONG>(devmode.dmPelsHeight)
				};
			}

			SetWindowPos(
				hwnd,
				NULL,
				fullscreen_window_rect.left,		
				fullscreen_window_rect.top,			
				fullscreen_window_rect.right,		
				fullscreen_window_rect.bottom,		

				// SWP_FRAMECHANGED：ウィンドウ のサイズ が変更されていない場合でも、WM_NCCALCSIZE メッセージをウィンドウに送信
				// SWP_NOACTIVATE：ウィンドウをアクティブにしない。
				// このフラグが設定されていない場合、アクティブになり、最上位グループまたは最上位以外のグループの先頭に移動
				SWP_FRAMECHANGED | SWP_NOACTIVATE	// サイズ設定と配置フラグ
			);
		
			// 指定したウィンドウの表示状態を設定
			// SW_MAXIMIZE：指定したウィンドウを最小化し、Z オーダーで次の最上位ウィンドウをアクティブ
			ShowWindow(hwnd, SW_MAXIMIZE);
		}
		else// フルスクリーンでない場合
		{
			// ウィンドウの属性を変更
			SetWindowLongPtrA(
				hwnd,			
				GWL_STYLE,		// 新しいウィンドウスタイルを設定
				windowed_style	// 置換値
			);
			SetWindowPos(
				hwnd,
				HWND_NOTOPMOST,
				windowed_rect.left,
				windowed_rect.top,
				windowed_rect.right - windowed_rect.left,
				windowed_rect.bottom - windowed_rect.top,
				SWP_FRAMECHANGED | SWP_NOACTIVATE);

			// SW_NORMAL：ウィンドウをアクティブにして表示、ウィンドウが最小化、最大化、または配置されている場合は、元のサイズと位置に復元
			ShowWindow(hwnd, SW_NORMAL);
		}
	}

	void Graphics::Initialize(HWND hwnd, bool fullscreen)
	{

	}

	void Graphics::Render(float elapsed_time)
	{
		HRESULT hr{ S_OK };

#ifdef ENABLE_DIRECT2D
		static float count{ 0 };
		count += elapsed_time * 50;
		d2d1_device_context->BeginDraw();
		static const wchar_t message[]{ L"PBR, or more commonly known as physically based rendering, is a collection of render techniques that are more or less based on the same underlying theory that more closely matches that of the physical world. As physically based rendering aims to mimic light in a physically plausible way, it generally looks more realistic compared to our original lighting algorithms like Phong and Blinn-Phong. Not only does it look better, as it closely approximates actual physics, we (and especially the artists) can author surface materials based on physical parameters without having to resort to cheap hacks and tweaks to make the lighting look right. One of the bigger advantages of authoring materials based on physical parameters is that these materials will look correct regardless of lighting conditions; something that is not true in non-PBR pipelines." };
		if (count > _countof(message))
		{
			count = 0;
		}
		d2d1_device_context->DrawTextW(message, static_cast<UINT32>(count), dwrite_text_formats[0].Get(),
			D2D1::RectF(50.0f, 10.0f, static_cast<FLOAT>(framebuffer_dimensions.cx - 50), static_cast<FLOAT>(framebuffer_dimensions.cy)),
			d2d_solid_color_brushes[0].Get(),
			D2D1_DRAW_TEXT_OPTIONS_NONE);
		d2d1_device_context->DrawTextW(message, static_cast<UINT32>(count), dwrite_text_formats[1].Get(),
			D2D1::RectF(50.0f, 10.0f + 300.0f, static_cast<FLOAT>(framebuffer_dimensions.cx - 50), static_cast<FLOAT>(framebuffer_dimensions.cy)),
			d2d_solid_color_brushes[1].Get(),
			D2D1_DRAW_TEXT_OPTIONS_NONE);
		hr = d2d1_device_context->EndDraw();
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#endif

		// ビューポートの設定
#if 0
		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		immediate_context->RSGetViewports(&num_viewports, &viewport);

		float aspect_ratio{ viewport.Width / viewport.Height };


		// 視線行列を生成
		DirectX::XMVECTOR eye{ DirectX::XMLoadFloat4(&camera_position) };
		DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };
		DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };

		// 射影行列を生成
		DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) };
		fullscreenQuad->blit(immediate_context.Get(), framebuffers[1]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif
#if 0
		fullscreenQuad->blit(immediate_context.Get(), framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1);
#endif

		// SWAP_CHAIN変更後
		UINT sync_interval{ vsync ? 1U : 0U };
		UINT flags = (tearing_supported && !fullscreen_mode && !vsync) ? DXGI_PRESENT_ALLOW_TEARING : 0;
		hr = swap_chain->Present(sync_interval, flags);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		// FrontバッファとBackバッファを入れ替える
		// ↓この下にspriteしても何も出ない
	}
}