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

    BaseScene::Initialize();

    // テクスチャ
    {
        title_left   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_left.png");
        title_right  = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_right.png");
        title_back   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_scene.png");
        title_back2  = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_scene.png");
        title_logo   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Titlelogo.png");
        title_credit = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Credit.png");
        title_menu   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Credit_menu.png");
        title_start  = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Start.png");

        window = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window.png");
        window_tutorial = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_tutorial.png");
        window_yes = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_yes.png");
        window_no = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_no.png");
    }

    // ゲーム
    {
        credit_y.value    = -1080.0f;
        title_x_2         = 3840.0f;
        start_scale.value = 1.0f;
        left_scale.CallValueEasing(1.2f, left_scale.value, EasingFunction::EasingType::OutSine, 0.8f);
        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::TITLE, true);
    }
}

void TitleScene::Finalize()
{
    // BGM終了
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void TitleScene::Update(HWND hwnd, float elapsedTime)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    Camera& camera = Camera::Instance();
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    camera.Update(elapsedTime);

    // マスクの更新
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

    // タイトルの演出
    Direction();

    // 操作
    HandleInput();

    if (!start_transition && is_in)
    {
        if (Lemur::Scene::SceneManager::Instance().once_tutorial)
        {
            Lemur::Scene::SceneManager::Instance().once_tutorial = false;
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new TutorialScene));
        }
        else if (!Lemur::Scene::SceneManager::Instance().once_tutorial)
        {
            if (!is_tutorial_next)
            {
                Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
            }
            else
            {
                Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new TutorialScene));
            }
        }

    }
    else if (is_tutorial)
    {
        pause_window_scale.EasingValue(elapsedTime);
        if (pause_window_scale.is_easing)return;

        switch (select_num)
        {
        case Button::YES:
            // 右を選んだとき
            if (gamePad.GetAxisLX() >= 0.5f || gamePad.GetAxisRX() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_RIGHT || GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                no.CallValueEasing(1.1f, no.value, EasingFunction::EasingType::InSine);
                yes.CallValueEasing(1.0f, yes.value, EasingFunction::EasingType::InSine);
                no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::NO;
                break;
            }
            // イージングを更新
            yes.EasingValue(elapsedTime);
            no.EasingValue(elapsedTime);
            yes.ContinueEasing(elapsedTime);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                is_tutorial_next = true;
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
                CallTransition(true);
            }
            break;

        case Button::NO:
            if (gamePad.GetAxisLX() <= -0.5f || gamePad.GetAxisRX() <= -0.5f || gamePad.GetButtonDown() & gamePad.BTN_LEFT || GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)// 左選択すると
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                yes.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::InSine);
                no.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::InSine);
                yes.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::YES;
                break;
            }

            // イージングを更新
            no.EasingValue(elapsedTime);
            yes.EasingValue(elapsedTime);
            no.ContinueEasing(elapsedTime);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                is_tutorial_next = false;
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
                CallTransition(true);
            }
            break;
        }
        return;

    }
    DebugImgui();
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

    // 2D描画
    {
        title_back->Render(immediate_context, title_x_1, 0, SCREEN_WIDTH * 2 + 5, SCREEN_HEIGHT);
        title_back2->Render(immediate_context, title_x_2, 0, SCREEN_WIDTH * 2 + 5, SCREEN_HEIGHT);
        title_left->RenderLeftDown(immediate_context, 0, 1080.0f, 1010 * left_scale.value, SCREEN_HEIGHT * left_scale.value, 0.0f);
        title_right->RenderRightDown(immediate_context, 1920.0f, 1080.0f, 955 * right_scale.value, SCREEN_HEIGHT * right_scale.value, 0.0f);
        title_logo->RenderCenter(immediate_context, 1000, 190, 1100 * logo_scale.value, 300 * logo_scale.value);

        title_menu->RenderRightDown(immediate_context, 1920, 1080, 400, 100, 0.0f);
        title_start->RenderCenter(immediate_context, 905, 975, 400 * start_scale.value, 120 * start_scale.value);
        title_credit->Render(immediate_context, 0, credit_y.value, SCREEN_WIDTH, SCREEN_HEIGHT);

        if (is_tutorial)
        {
            window->RenderCenter(immediate_context, 960, 540, 1400 * pause_window_scale.value, 700 * pause_window_scale.value);
            window_tutorial->RenderCenter(immediate_context, 960, 420, 1100 * pause_window_scale.value, 200 * pause_window_scale.value);
            window_yes->RenderCenter(immediate_context, 700, 645, 200 * pause_window_scale.value * yes.value, 100 * pause_window_scale.value * yes.value);
            window_no->RenderCenter(immediate_context, 1200, 645, 200 * pause_window_scale.value * no.value, 100 * pause_window_scale.value * no.value);
        }

        // マスク
        RenderTransitionMask(elapsedTime);
    }
}

void TitleScene::DebugImgui()
{
#ifdef DEBUG_IMGUI
#endif
}

void TitleScene::Direction()
{
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
}

void TitleScene::HandleInput()
{
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();

    if ((gamePad.GetButtonDown() & gamePad.BTN_B) && !is_credit)
    {
        Lemur::Scene::SceneManager::Instance().cont_type = true;
        Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
        if (!is_in && Lemur::Scene::SceneManager::Instance().once_tutorial)
        {
            CallTransition(true);
        }
        else
        {
            is_tutorial = true;
            pause_window_scale.CallValueEasing(1.0f, pause_window_scale.value, pause_window_scale.OutSine);
            no.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::OutSine);
            yes.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::OutSine);
            no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
        }
    }
    else if ((GetAsyncKeyState('B') & 0x8000) && !is_credit)
    {
        Lemur::Scene::SceneManager::Instance().cont_type = false;
        Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
        if (!is_in && Lemur::Scene::SceneManager::Instance().once_tutorial)
        {
            CallTransition(true);
        }
        else 
        {
            is_tutorial = true;
            pause_window_scale.CallValueEasing(1.0f, pause_window_scale.value, pause_window_scale.OutSine);
            no.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::OutSine);
            yes.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::OutSine);
            no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
        }
    }
    if (gamePad.GetButtonDown() & gamePad.BTN_START || GetAsyncKeyState(VK_BACK) & 0x8000)
    {
        if (is_credit&& !credit_y.is_easing)
        {
            is_credit = false;
            credit_y.CallValueEasing(-1920.0f, credit_y.value, EasingFunction::EasingType::InSine, 1.0f);
        }
        else if(!credit_y.is_easing)
        {
            is_credit = true;
            credit_y.CallValueEasing(0.0f, credit_y.value, EasingFunction::EasingType::OutBounce, 1.0f);
        }
    }
}
