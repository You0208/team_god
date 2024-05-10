#include "OverScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"

#include "TitleScene.h"
#include "GameScene.h"

void OverScene::Initialize()
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
    }

    // テクスチャ
    {
        game_over_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\GameOver.png");
        game_over_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\gameover_text.png");
        continue_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\continue.png");
        again_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\Again.png");
        stageselect_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\Stageselect.png");
    }

    // ゲーム
    {
        select_num = 0;

        select.scale = 1.0f;
        again.scale = 1.0f;

        select.CallScaleEasing(1.1f, select.scale,false);
        select.CallScaleContinue(1.0f, 1.1f, 1.1f, 0.4f);
    }
}

void OverScene::Finalize()
{
}

void OverScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    switch (select_num)
    {
    case 0:
        if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
        {
            again.CallScaleEasing(1.1f, again.scale);
            select.CallScaleEasing(1.0f, select.scale);
            again.CallScaleContinue(1.0f, 1.1f, 1.1f,0.4f);
            select_num++;
            break;
        }
        select.EasingScale(elapsed_time);
        again.EasingScale(elapsed_time);
        select.ContinueEasing(elapsed_time);
        break;
    case 1:
        if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
        {
            select.CallScaleEasing(1.1f, select.scale);
            again.CallScaleEasing(1.0f, again.scale);
            select.CallScaleContinue(1.0f, 1.1f, 1.1f, 0.4f);
            select_num--;
            break;
        }
        again.EasingScale(elapsed_time);
        select.EasingScale(elapsed_time);
        again.ContinueEasing(elapsed_time);
        break;
    }


    if (gamePad.GetButton() & gamePad.BTN_B)
    {
        Lemur::Scene::SceneManager::Instance().ChangeScene(new TitleScene);
    }
    DebugImgui();
}

void OverScene::Render(float elapsedTime)
{
    HRESULT hr{ S_OK };

    Camera& camera = Camera::Instance();
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

    camera.SetPerspectiveFov(immediate_context);

    // 描画の設定
    SetUpRendering();

    // ステートの設定
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
    game_over_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, true);
    //continue_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, true);
    game_over_text->Render(immediate_context, 475, 85, 960, 150);

    stageselect_text->RenderCenter(immediate_context, 550, 950, 500 * select.scale, 200 * select.scale);
    again_text->RenderCenter(immediate_context, 1430, 950, 500 * again.scale, 200 * again.scale);
}

void OverScene::DebugImgui()
{
    ImGui::Begin("ImGUI");

    if (ImGui::Button(u8"ゲームに戻る"))
    {
        Lemur::Scene::SceneManager::Instance().ChangeScene(new GameScene);
    }
    ImGui::End();
}
