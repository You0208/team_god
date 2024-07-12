#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "misc.h"
#include "Sprite.h"
#include "SpriteBatch.h"
#include "high_resolution_timer.h"
#include "GeometricPrimitive.h"
#include "./Lemur/Model/Model.h"
#include "Framebuffer.h"
#include "FullscreenQuad.h"
#include "..\Lemur.h"
#include "Graphics.h"
#include "..\Input/Input.h"



#include <d3d11.h>
#include <wrl.h>

#include "./ImGuiCtrl.h"

CONST LPCWSTR APPLICATION_NAME{ L"Lemur" };

class Framework
{
private:
	Lemur::Graphics::Graphics graphics;
	Input input;

public:
	CONST HWND hwnd;

	float timer{ 0.0f };
	Framework(HWND hwnd);
	~Framework();

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	void on_size_changed(UINT64 width, UINT height);

	int run()
	{
		MSG msg{};

		if (!Initialize())
		{
			return 0;
		}

		// ImGuièâä˙âª(DirectX11ÇÃèâä˙âªÇÃâ∫Ç…íuÇ≠Ç±Ç∆)
		IMGUI_CTRL_INITIALIZE(hwnd, graphics.GetDevice(), graphics.GetDeviceContext());

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				high_resolution_timer::Instance().tick();
				CalculateFrameStats();
				Update(high_resolution_timer::Instance().time_interval());
				Render(high_resolution_timer::Instance().time_interval());
			}
		}

		// ImGuièIóπâª
		IMGUI_CTRL_UNINITIALIZE();


#if 0
		BOOL fullscreen = 0;
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return Uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		// ImGui(êÊì™Ç…íuÇ≠)
		IMGUI_CTRL_WND_PRC_HANDLER(hwnd, msg, wparam, lparam);

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_MOUSEWHEEL:
			Input::Instance().GetMouse().SetWheel(GET_WHEEL_DELTA_WPARAM(wparam));
			break;
		case WM_ENTERSIZEMOVE:
			high_resolution_timer::Instance().stop();
			break;
		case WM_EXITSIZEMOVE:
			high_resolution_timer::Instance().start();
			break;
		case WM_SIZE:
		{
#if 1
			RECT client_rect{};
			GetClientRect(hwnd, &client_rect);
			on_size_changed(static_cast<UINT64>(client_rect.right - client_rect.left), client_rect.bottom - client_rect.top);
#endif
			break;
		}
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool Initialize();
	void Update(float elapsed_time/*Elapsed seconds from last frame*/);
	void Render(float elapsed_time/*Elapsed seconds from last frame*/);
	bool Uninitialize();

private:
	//high_resolution_timer tictoc;
	uint32_t frames{ 0 };
	float elapsed_time{ 0.0f };
	void CalculateFrameStats()
	{
		if (++frames, (high_resolution_timer::Instance().time_stamp() - elapsed_time) >= 1.0f)
		{
			float fps = static_cast<float>(frames);
			std::wostringstream outs;
			outs.precision(6);
			outs <<  L" Ç’ÇÁÇÒÇ∆Ç’ÇÁÇÒ";
			SetWindowTextW(hwnd, outs.str().c_str());

			frames = 0;
			elapsed_time += 1.0f;
		}
	}
};

