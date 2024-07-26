#include "SelectScene.h"
#include "FormationScene.h"
#include "TitleScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Scene/SceneManager.h"
#include "Game/Stage/StageManager.h"
#include "Game/Scene/LoadingScene.h"

void SelectScene::Initialize()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // シェーダー関連
    BaseScene::Initialize();

    // テクスチャ&PS
    {  
        if (!Lemur::Scene::SceneManager::Instance().cont_type)
        {
            ui = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\UI_button_backandclick.png");
            ui_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_select_UI_2.png");
            ui_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_select_UI_3.png.png");
        }
        else
        {
            ui = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_select_UI_AB.png");
            ui_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_select_UI_LB.png");
            ui_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_select_UI_RB.png");
        }
        kakashi_1          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\kakashi1.png");
        kakashi_2          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\kakashi2.png");
        kakashi_3          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\kakashi3.png");
        highlight_1        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage1.png");
        highlight_2        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage2.png");
        highlight_3        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage3.png");
        select_back        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_back.png");
        joro               = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\jouro_UI_highlight.png");

        // 遷移
        transition         = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition.png");
        transition_line1   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line1.png");
        transition_line2   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line2.png");
        transition_line3   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line3.png");
        transition_line1_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line1_2.png");
        transition_line2_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line2_2.png");
        transition_line3_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line3_2.png");

        window = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window.png");
        window_title = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_title.png");
        window_yes = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_yes.png");
        window_no = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_no.png");

        create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_select_ps.cso", select_ps.GetAddressOf());
    }

    // ゲーム
    {
        world_num = StageManager::Instance().current_world_level;
        stage_num = StageManager::Instance().current_stage_level;

        // 初期位置設定
        transition_angle.value         = 140.0f;
        transition_line1_angle.value   = 90.0f;
        transition_line2_angle.value   = 110.0f;
        transition_line3_angle.value   = 180.0f;
        transition_angle.value         = 90.0f;
        transition_line1_angle_2.value = 110.0f;
        transition_line2_angle_2.value = 180.0f;
        transition_line3_angle_2.value =90.0f;

        switch_direction = false;

        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::SELECT, true);

        CallTransition(false);
    }
}

void SelectScene::Finalize()
{
    // BGM終了
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void SelectScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // ゲームパッド

    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    TransitionMask(elapsed_time);

    if (start_transition)return;

    // イージングを更新
    transition_angle.EasingValue(elapsed_time);
    transition_line1_angle.EasingValue(elapsed_time);
    transition_line2_angle.EasingValue(elapsed_time);
    transition_line3_angle.EasingValue(elapsed_time);
    transition_line1_angle_2.EasingValue(elapsed_time);
    transition_line2_angle_2.EasingValue(elapsed_time);
    transition_line3_angle_2.EasingValue(elapsed_time);

    // 演出
    Direction();

    // 操作
    if (!start_transition && is_in)
    {
        pause_window_scale.EasingValue(elapsed_time);
        StageManager::Instance().SetStageLevel(set_level[world_num][stage_num]);
        // 次のシーンへ
        if(next_scene==0)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new FormationScene));
        if(next_scene==1)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new TitleScene));
        return;
    }
    HandleInput(elapsed_time);
}

void SelectScene::Render(float elapsedTime)
{
    DebugImgui();
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

    // 2D
    {
        // 背景
        select_back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * world_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        switch (world_num)
        {
        case 0:
            highlight_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            joro->Render(immediate_context, select_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            kakashi_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case 1:
            highlight_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            joro->Render(immediate_context, select_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            kakashi_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        case 2:
            highlight_3->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            joro->Render(immediate_context, select_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            kakashi_3->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, SCREEN_WIDTH * stage_num, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
            break;
        }

        ui->Render(immediate_context, select_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
        if ( world_num != 0) ui_1->Render(immediate_context, select_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);
        if ( world_num != 2 )ui_2->Render(immediate_context, select_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

        if (switch_direction)
        {
            switch (direction_num)
            {
            case 0:
                transition->RenderRightDown(immediate_context, select_ps.Get(), SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_angle.value);
                transition_line1->RenderRightDown(immediate_context,select_ps.Get(), SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line1_angle.value);
                transition_line2->RenderRightDown(immediate_context,select_ps.Get(), SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line2_angle.value);
                transition_line2->RenderRightDown(immediate_context,select_ps.Get(), SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line3_angle.value);
                break;

            case 1:
                transition->RenderRightUp(immediate_context, select_ps.Get(), SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 2.0, transition_angle.value);
                transition_line1_2->RenderRightUp(immediate_context, select_ps.Get(), SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line1_angle_2.value);
                transition_line2_2->RenderRightUp(immediate_context, select_ps.Get(), SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line2_angle_2.value);
                transition_line2_2->RenderRightUp(immediate_context, select_ps.Get(), SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line3_angle_2.value);
                break;

            }
        }
        if (is_title_window)
        {
            window->RenderCenter(immediate_context, 960, 540, 1400 * pause_window_scale.value, 700 * pause_window_scale.value);
            window_title->RenderCenter(immediate_context, 960, 420, 1100 * pause_window_scale.value, 200 * pause_window_scale.value);
            window_yes->RenderCenter(immediate_context, 700, 645, 200 * pause_window_scale.value * yes.value, 100 * pause_window_scale.value * yes.value);
            window_no->RenderCenter(immediate_context, 1200, 645, 200 * pause_window_scale.value * no.value, 100 * pause_window_scale.value * no.value);
        }

    }

    // マスク
    RenderTransitionMask(elapsedTime);
}

void SelectScene::DebugImgui()
{
#ifdef DEBUG_IMGUI
    ImGui::Begin("Select");
    ImGui::Checkbox("add", &add);
    ImGui::Checkbox("switch", &switch_direction);
    ImGui::DragInt("world_num", &world_num);
    ImGui::End();
#endif
}

void SelectScene::Direction()
{
    if (switch_direction)
    {
        switch (direction_num)
        {
        case 0:
            // 演出が終わったら
            if (!transition_angle.is_easing
                && !transition_line1_angle.is_easing
                && !transition_line2_angle.is_easing
                && !transition_line3_angle.is_easing)
            {
                // 角度を初期化
                transition_angle.value = 90.0f;
                transition_line1_angle.value = 90.0f;
                transition_line2_angle.value = 110.0f;
                transition_line3_angle.value = 180.0f;

                // イージングを呼ぶ
                transition_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.7f);
                transition_line1_angle_2.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.5f);
                transition_line2_angle_2.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.3f);
                transition_line3_angle_2.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.1f);

                // つぎのセレクト画面を初期化
                if (add)world_num++;
                else if (minus)world_num--;
                stage_num = 0;

                // 次の演出へ
                direction_num++;
            }
            break;
        case 1:
            // 演出が終わったら
            if (!transition_angle.is_easing
                && !transition_line1_angle_2.is_easing
                && !transition_line2_angle_2.is_easing
                && !transition_line3_angle_2.is_easing)
            {
                // 角度を初期化
                transition_angle.value = 140.0f;
                transition_line1_angle_2.value = 110.0f;
                transition_line2_angle_2.value = 180.0f;
                transition_line3_angle_2.value = 90.0f;

                // 初期化
                direction_num = 0;
                switch_direction = false;
                add = false;
                minus = false;
            }
            break;
        }

        return;
    }
}

void SelectScene::HandleInput(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    Mouse& mouse = Input::Instance().GetMouse();

    // 確認ウィンドウ
    if (is_title_window)
    {
        pause_window_scale.EasingValue(elapsedTime);
        if (pause_window_scale.is_easing)return;

        switch (select_num)
        {
        case Button::YES:
            // 右を選んだとき
            if (gamePad.GetAxisLX() >= 0.5f || gamePad.GetAxisRX() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_RIGHT || GetKeyState(VK_RIGHT) & 0x8000 || GetKeyState('D') & 0x8000)
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
            if (gamePad.GetButtonDown() & gamePad.BTN_B || GetKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
                is_title_window = false;
                next_scene = 1;
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
                is_title_window = false;
                pause_window_scale.CallValueEasing(0.0f, pause_window_scale.value, pause_window_scale.OutSine);
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
            }
            break;
        }
        return;
    }


    if (!first_touch)touch_interval += elapsedTime;
    else touch_interval = 0.0f;
    if (touch_interval >= 0.3f)
    {
        first_touch = true;
        touch_interval = 0.0f;
    }

    // ステージ選択
    if (gamePad.GetAxisLX() >= 0.5f || gamePad.GetAxisRX() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_RIGHT || GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
    {
        if (first_touch)
        {
            if (stage_num < 2)
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                stage_num++;
            }
            first_touch = false;
        }
    }
    else if (gamePad.GetAxisLX() <= -0.5f || gamePad.GetAxisRX() <= -0.5f || gamePad.GetButtonDown() & gamePad.BTN_LEFT || GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)
    {
        if (first_touch)
        {
            if (stage_num > 0)
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                stage_num--;
            }
            first_touch = false;
        }
    }
    else
    {
        first_touch = true;
    }

    // ワールド選択
    if (!switch_direction)
    {
        if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT_SHOULDER || GetAsyncKeyState('3') & 0x8000)
        {
            if (world_num < 2)
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::WORLD_CHANGE, false);
                switch_direction = true;
                add = true;
                transition_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.3f);
                transition_line1_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.1f);
                transition_line2_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.5f);
                transition_line3_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.7f);
                direction_num = 0;
            }
        }
        else if (gamePad.GetButtonDown() & gamePad.BTN_LEFT_SHOULDER || GetAsyncKeyState('2') & 0x8000)
        {
            if (world_num > 0)
            {
                Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::WORLD_CHANGE, false);
                switch_direction = true;
                minus = true;
                transition_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.3f);
                transition_line1_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.1f);
                transition_line2_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.5f);
                transition_line3_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.7f);
                direction_num = 0;
            }
        }
    }
    // ステージ決定
    if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
    {
        StageManager::Instance().current_world_level = world_num;
        StageManager::Instance().current_stage_level = stage_num;

        Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
        next_scene = 0;
        CallTransition(true, stage_mask_pos[stage_num]);
    }
    if (gamePad.GetButtonDown() & gamePad.BTN_START || GetAsyncKeyState(VK_BACK) & 0x8000)
    {
        is_title_window = true;
        pause_window_scale.CallValueEasing(1.0f, pause_window_scale.value, pause_window_scale.OutSine);
        no.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::OutSine);
        yes.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::OutSine);
        no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
    }

}

