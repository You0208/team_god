#include "Framework.h"
#include "Shader.h"
#include "Texture.h"
#include "Graphics.h"
#include "..\Scene\SceneManager.h"
#include "Lemur/Audio/AudioManager.h"

#include "../Scene/BaseScene.h"
#include "../Scene/DemoScene.h"
#include "./Game/Scene/TitleScene.h"
#include "./Game/Scene/GameScene.h"
#include "./Game/Scene/LoadingScene.h"
#include "./Game/Scene/OverScene.h"
#include "./Game/Scene/SelectScene.h"
#include "./Game/Scene/FormationScene.h"
#include "./Game/Scene/ClearScene.h"


// Effect
#include "../Effekseer/EffekseerManager.h"

Framework::Framework(HWND hwnd) 
	: hwnd(hwnd),
	graphics(hwnd, FALSE),
	input(hwnd)
{
	input.GetMouse().SetScreenWidth(SCREEN_WIDTH);
	input.GetMouse().SetScreenHeight(SCREEN_HEIGHT);
}

bool Framework::Initialize()
{
	// エフェクトマネージャー初期化
	EffectManager::Instance().Initialize();

	// XAUDIO2
	{
		HRESULT hr{ S_OK };

		Lemur::Audio::AudioManager& audio = Lemur::Audio::AudioManager::Instance();
		hr = XAudio2Create(audio.xAudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = audio.xAudio2->CreateMasteringVoice(&audio.masterVoice);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Lemur::Audio::AudioManager::Instance().LoadAudio();
	}

	// シーン初期化
	Lemur::Scene::SceneManager::Instance().ChangeScene(new GameScene);

	return true;
}

void Framework::Update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#if 1
	if (GetAsyncKeyState(VK_RETURN) & 1 && GetAsyncKeyState(VK_MENU) & 1)
	{
		graphics.StylizeWindow(!graphics.fullscreen_mode);
	}
#endif
	// ImGui更新
	ImGuiCtrl::ClearFrame(hwnd);

	// 入力更新処理
	input.Update();

	// シーン更新処理
	Lemur::Scene::SceneManager::Instance().Update(hwnd,elapsed_time);

	// 時間経過更新
	timer += elapsed_time;
}

void Framework::Render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	using namespace Lemur::Graphics;
	// 別スレッド中にデバイスコンテキストが使われていた場合に
	// 同時にアクセスしないように排他制御をする
	std::lock_guard<std::mutex> lock(graphics.GetMutex());
	// シーン描画処理
	Lemur::Scene::SceneManager::Instance().Render(elapsed_time);

	// ImGui表示（この下にも何も描画しない）
	IMGUI_CTRL_DISPLAY();
	graphics.Render(elapsed_time);
	// ↓この下は何も出ない
}

bool Framework::Uninitialize()
{
	return true;
}

Framework::~Framework()
{
	Lemur::Scene::SceneManager::Instance().Clear();

	// エフェクトマネージャー終了化
	EffectManager::Instance().Finalize();
}

void Framework::on_size_changed(UINT64 width, UINT height)
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	HRESULT hr{ S_OK };
	if (width != graphics.framebuffer_dimensions.cx || height != graphics.framebuffer_dimensions.cy)
	{
		graphics.framebuffer_dimensions.cx = static_cast<LONG>(width);
		graphics.framebuffer_dimensions.cy = height;

		// Release all objects that hold shader resource views here.
#ifdef ENABLE_DIRECT2D
		graphics.d2d1_device_context.Reset();
#endif

		Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
		hr = graphics.GetSwap()->GetParent(IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		graphics.CreateSwapChain(dxgi_factory6.Get());

		// Recreate all objects that hold shader resource views here.
#ifdef ENABLE_DIRECT2D
		graphics.create_direct2d_objects();
#endif
	}
}
