#include "TitleScene.h"
#include "TutorialScene.h"
#include "LoadingScene.h"
#include "SelectScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"

void TitleScene::Initialize()
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

    // テクスチャ&PS
    {
        title_left = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_left.png");
        title_right = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_right.png");
        title_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_scene.png");
        title_back2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_scene.png");
        title_logo = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Titlelogo.png");
        title_credit = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Credit.png");
        title_menu = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Credit_menu.png");
        title_start = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Start.png");
    }

    // ゲーム
    {
        credit_y.value = -1080.0f;
        title_x_2 = 3840.0f;
        start_scale.value = 1.0f;
        left_scale.CallValueEasing(1.2f, left_scale.value, EasingFunction::EasingType::OutSine, 0.8f);
        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::TITLE, true);
        //CallTransition(false);
    }
}

void TitleScene::Finalize()
{
    // BGM終了
    // BGM
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void TitleScene::Update(HWND hwnd, float elapsedTime)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsedTime);

    TransitionMask(elapsedTime);

    if (start_transition)return;

    // イージング更新
    {
        left_scale.EasingValue(elapsedTime);
        right_scale.EasingValue(elapsedTime);
        logo_scale.EasingValue(elapsedTime);
        credit_y.EasingValue(elapsedTime);
        start_scale.ContinueEasing(elapsedTime);
    }

    // タイトル流し
    title_x_1 -= elapsedTime*100;
    title_x_2 -= elapsedTime*100;
    if (title_x_1 < -3840.0f)title_x_1 = 3840.0f;
    if (title_x_2 < -3840.0f)title_x_2 = 3840.0f;

    if (is_direction)
    {
        switch (direction_num)
        {
        case LEFT:
            if (!left_scale.is_easing)
            {
                left_scale.CallValueEasing(1.0f, left_scale.value, EasingFunction::EasingType::InSine, 0.3f);
                right_scale.CallValueEasing(1.2f, right_scale.value, EasingFunction::EasingType::OutSine, 0.8f);
                direction_num = RIGHT;
            }
            break;
        case RIGHT:

            if (!right_scale.is_easing)
            {
                right_scale.CallValueEasing(1.0f, right_scale.value, EasingFunction::EasingType::InSine, 0.3f);
                direction_num = RIGHT_2;
            }
            break;
        case RIGHT_2:

            if (!right_scale.is_easing)
            {
                logo_scale.CallValueEasing(1.2f, logo_scale.value, EasingFunction::EasingType::InSine, 0.5f);
                direction_num = LOGO;
            }
            break;
        case LOGO:
            if (!logo_scale.is_easing)
            {
                logo_scale.CallValueEasing(1.0f, logo_scale.value, EasingFunction::EasingType::OutBounce, 0.5f);
                direction_num = LOGO_2;
            }
            break;
        case LOGO_2:
            if (!logo_scale.is_easing)
            {
                start_scale.CallValueContinue(1.0f, 1.2f, start_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.6f);
                is_direction = false;
            }
            break;
        }
        return;
    }

    if (gamePad.GetButtonDown() & gamePad.BTN_B&&!is_credit)
    {
        Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
        if (!is_in)
        {
            CallTransition(true);
        }
    }
    if (gamePad.GetButtonUp() & gamePad.BTN_START)
    {
        if (is_credit)
        {
            is_credit = false;
            credit_y.CallValueEasing(-1920.0f, credit_y.value, EasingFunction::EasingType::InSine, 1.0f);
        }
        else
        {
            is_credit = true;
            credit_y.CallValueEasing(0.0f, credit_y.value, EasingFunction::EasingType::OutBounce, 1.0f);
        }
    }

    // アイリスインを呼ぶ
    if (!start_transition && is_in)
    {
        if (Lemur::Scene::SceneManager::Instance().once_tutorial)
        {
            Lemur::Scene::SceneManager::Instance().once_tutorial = false;
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new TutorialScene));
        }
        else if (!Lemur::Scene::SceneManager::Instance().once_tutorial)
        {
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
        }
    }
   // DebugImgui();
}

void TitleScene::Render(float elapsedTime)
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
    SetUpConstantBuffer();

    // ステートの設定
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    title_back->Render(immediate_context, title_x_1, 0, SCREEN_WIDTH*2+5, SCREEN_HEIGHT);
    title_back2->Render(immediate_context, title_x_2, 0, SCREEN_WIDTH * 2+5, SCREEN_HEIGHT);
    //title_left->RenderCenter(immediate_context, left_x.value,left_y.value, 1010 * left_scale.value, SCREEN_HEIGHT * left_scale.value);
    title_left->RenderLeftDown(immediate_context, 0, 1080.0f, 1010 * left_scale.value, SCREEN_HEIGHT * left_scale.value, 0.0f);
    title_right->RenderRightDown(immediate_context, 1920.0f, 1080.0f, 955 * right_scale.value, SCREEN_HEIGHT * right_scale.value, 0.0f);
    title_logo->RenderCenter(immediate_context, 1000, 190, 1100 * logo_scale.value, 300 * logo_scale.value);

    title_menu->RenderRightDown(immediate_context, 1920, 1080, 400, 100, 0.0f);
    title_start->RenderCenter(immediate_context, 905, 975, 400* start_scale.value, 120* start_scale.value);
    title_credit->Render(immediate_context, 0, credit_y.value, SCREEN_WIDTH, SCREEN_HEIGHT);

    // マスク
    RenderTransitionMask(elapsedTime);
}

void TitleScene::DebugImgui()
{
}
