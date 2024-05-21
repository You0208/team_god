#include "OverScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"
#include "Lemur/Math/Easing.h"

#include "TitleScene.h"
#include "SelectScene.h"
#include "LoadingScene.h"
#include "FormationScene.h"

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
        // テクスチャの初期化
        InitializeMask();
    }

    // テクスチャ
    {
        game_over_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\GameOver.png");
        ui_text[0] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_g.png");
        ui_text[1] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_a.png");
        ui_text[2] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_m.png");
        ui_text[3] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_e.png");
        ui_text[4] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_o.png");
        ui_text[5] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_v.png");
        ui_text[6] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_e_2.png");
        ui_text[7] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\UI_r.png");
        continue_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\continue.png");
        again_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\Again.png");
        stageselect_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\Stageselect.png");
    }

    // ゲーム
    {
        timer = 0.0f;

        interval = 0.1f;

        select_num = 0;

        select.value = 1.0f;
        again.value = 1.0f;

        anim_stop = true;

        for (int i = 0; i < 8; i++)
        {
            text_y[i].value = -100.0f;
        }


        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::OVER, true);

        CallTransition(false);
    }
}

void OverScene::Finalize()
{
    // BGM終了
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void OverScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    TransitionMask(elapsed_time);

    // アイリスアウトが終わってから一度のみ実行
    if (!start_transition && once_only)
    {
        switch (direction_num)// 現在の演出が何番か
        {
        case 0:// 上から徐々に文字が落ちてくる
            timer += elapsed_time;// タイマー

            // 現在のカウントのイージングを動かす
            text_y[count_num].CallValueEasing(160.0f, text_y[count_num].value, EasingFunction::EasingType::OutSine, 0.15f);

            // インターバル分時間がたてば次へ
            if (timer >= interval)
            {
                count_num++;// カウントを進める
                timer = 0.0f;// タイマーを初期化
                // カウントが規定値を超えていたら
                if (count_num >= 8)
                {
                    timer = 0;// タイマーの初期化
                    count_num = 0;// カウントの初期化
                    interval = 0.05f;// 次回演出のためインターバルを再設定
                    direction_num++;// 演出を次の段階
                }
            }
            anim_stop = false;
            break;
        case 1:
            timer += elapsed_time;// タイマー

            // 現在のカウントのイージングを動かす
            text_angle[random_num[count_num]].CallValueContinue(-10.0f, 10.0f, text_angle[random_num[count_num]].value, EasingFunction::EasingType::InOutSine, EasingFunction::EasingType::InOutSine);

            // インターバル分時間がたてば次へ
            if (timer >= interval)
            {
                count_num++;// カウントを進める
                timer = 0.0f;// タイマーを初期化                
                if (count_num >= 8)// カウントが規定値を超えていたら
                {
                    timer = 0;// タイマーの初期化
                    count_num = 0;// カウントの初期化
                    // セレクトボタンを動かし始める
                    select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                    once_only = false;// フラグをOFF
                }

            }
            break;
        }

        // テキストのイージング更新
        for (int i = 0; i < 8; i++)
        {
            text_y[i].EasingValue(elapsed_time);
            text_angle[i].ContinueEasing(elapsed_time);
        }

        return;
    }
    else if (start_transition && once_only) return;// 演出中はボタンを推せないように
    
    // どのボタンを選択しているか
    switch (select_num)
    {
    case Button::Select :
        if (!is_continue)// 選択されていない時
        {
            // 右を選んだとき
            if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
            {
                again.CallValueEasing(1.1f, again.value, EasingFunction::EasingType::InSine);
                select.CallValueEasing(1.0f, select.value, EasingFunction::EasingType::InSine);
                again.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Again;
                break;
            }
            // イージングを更新
            select.EasingValue(elapsed_time);
            again.EasingValue(elapsed_time);
            select.ContinueEasing(elapsed_time);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // アイリスインを呼ぶ
        if (continue_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // 次のシーンへ
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
        }
        break;

    case Button::Again:

        if (!is_continue)// 選択されていない時
        {
            if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)// 左選択すると
            {
                select.CallValueEasing(1.1f, select.value, EasingFunction::EasingType::InSine);
                again.CallValueEasing(1.0f, again.value, EasingFunction::EasingType::InSine);
                select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Select;
                break;
            }

            // イージングを更新
            again.EasingValue(elapsed_time);
            select.EasingValue(elapsed_time);
            again.ContinueEasing(elapsed_time);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // アイリスインを呼ぶ
        if (continue_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // 次のシーンへ
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new FormationScene));
        }
        break;
    }

    // 演出外でイージングを更新
    for (int i = 0; i < 8; i++)
    {
        text_y[i].EasingValue(elapsed_time);
        text_angle[i].ContinueEasing(elapsed_time);
    }
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
    SetUpConstantBuffer();

    // ステートの設定
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    // 2D基本
    {
        // 基本背景
        if (!is_continue)game_over_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, false, anim_stop);
        // コンティニュー背景
        else if (is_continue)continue_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, false);

        // UIの文字
        for (int i = 0; i < 8; i++)
        {
            ui_text[i]->RenderCenter(immediate_context, 530 + float(130 * i), text_y[i].value, 150, 150, text_angle[i].value);
        }

        // ボタン
        stageselect_text->RenderCenter(immediate_context, 550, 950, 500 * select.value, 200 * select.value);
        again_text->RenderCenter(immediate_context, 1430, 950, 500 * again.value, 200 * again.value);
    }
    // マスク
    RenderTransitionMask(elapsedTime);
}

void OverScene::DebugImgui()
{
    ImGui::Begin("ImGUI");
    ImGui::End();
}
