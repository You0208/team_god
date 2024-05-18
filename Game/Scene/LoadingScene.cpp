#include "LoadingScene.h"
#include "Lemur/Scene/SceneManager.h"
#include "Lemur/Graphics/Camera.h"
#include "high_resolution_timer.h"
#include "imgui.h"

void LoadingScene::Initialize()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // シェーダー関連
    {
        // ステートの初期化
        InitializeState();
        // ディファードレンダリングの初期化
        InitializeDeffered(SCREEN_WIDTH, SCREEN_HEIGHT);
        // フレームバッファーの初期化
        InitializeFramebuffer();
        // ピクセルシェーダーの初期化
        InitializePS();
        // ポイントライト・スポットライトの初期位置設定
        InitializeLight();
        // テクスチャの初期化
        InitializeMask();
    }

    // シェーダー関連
    // スレッド
    {
        thread = new std::thread(LoadingThread, this);
    }
    // ゲーム部分
    {
        spr_back = std::make_unique<Sprite>(graphics.GetDevice(), L".\\resources\\Image\\Load\\Loading.png");
        loading_text = std::make_unique<Sprite>(graphics.GetDevice(), L".\\resources\\Image\\Load\\loadion_text.png");
        point_1_text = std::make_unique<Sprite>(graphics.GetDevice(), L".\\resources\\Image\\Load\\loading_1.png");
        point_2_text = std::make_unique<Sprite>(graphics.GetDevice(), L".\\resources\\Image\\Load\\loading_2.png");
        point_3_text = std::make_unique<Sprite>(graphics.GetDevice(), L".\\resources\\Image\\Load\\loading_3.png");

        CallTransition(false);

        time_interval = 0.4f;
    }
}

void LoadingScene::Finalize()
{
}

void LoadingScene::Update(HWND hwnd, float elapsedTime)
{
    Camera& camera = Camera::Instance();
    camera.Update(elapsedTime);

    // 次のシーンの準備が完了したらシーンを切り替える
    if (!start_transition && !once_only)
    {
        Lemur::Scene::SceneManager::Instance().ChangeScene(nextScene);
        nextScene = nullptr;
    }
    else if (nextScene->IsReady()&& once_only)
    {   
        once_only = false;
        CallTransition(true);
    }
    TransitionMask(elapsedTime);

    time += elapsedTime;

    enable_posint[0] = (time >= time_interval) ? true : false;
    enable_posint[1] = (time >= time_interval * 2.0f) ? true : false;
    enable_posint[2] = (time >= time_interval * 3.0f) ? true : false;
    if (time >= time_interval*4.0f)
    {
        time = 0.0f;
    }
}

void LoadingScene::Render(float elapsedTime)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    Camera& camera = Camera::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

    camera.SetPerspectiveFov(immediate_context);

    SetUpRendering();
    SetUpConstantBuffer();
    // 2D描画
    {
        // sprite描画
        {
            immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
            immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
            immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
            spr_back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0);
            loading_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0);
            if(enable_posint[0])point_1_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0);
            if (enable_posint[1])point_2_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0);
            if (enable_posint[2])point_3_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0);

            RenderTransitionMask(elapsedTime);
        }
    }
}

void LoadingScene::LoadingThread(LoadingScene* scene)
{
    // COM関連の初期化でスレッド毎に呼ぶ必要がある
    CoInitialize(nullptr);

    // 次のシーンの初期を行う
    scene->nextScene->Initialize();

    // スレッドが終わる前にCOM関連の終了化
    CoUninitialize();

    // 次のシーンの準備完了設定
    scene->nextScene->SetReady();
}
