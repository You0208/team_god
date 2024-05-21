#include "ClearScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"

#include "Game/Scene/FormationScene.h"
#include "Game/Scene/SelectScene.h"
#include "Game/Scene/GameScene.h"
#include "Game/Scene/LoadingScene.h"
#include "Game/Stage/StageManager.h"

void ClearScene::Initialize()
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
        clear_start_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\start_back.png");
        clear_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\result_back.png");
        result_cover = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\Result_scene.png");
        star1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\Star_1.png");
        star2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\Star_2.png");
        star3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\Star_3.png");
        star_frame1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\frame_1.png");
        star_frame2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\frame_2.png");
        star_frame3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\frame_3.png");
        next_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\Next.png");
        select_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\Stageselect.png");
        again_text = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\GameOver\\Again.png");
    }
    // ゲーム
    {
        select.value = 1.0f;
        again.value = 1.0f;
        next.value = 1.0f;
        star1_scale.value = 0.0f;
        star2_scale.value = 0.0f;
        star3_scale.value = 0.0f;
        interval_1 = 0.0f;
        interval_2 = 0.3f;
        interval_3 = 0.6f;

        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::CLEAR, true);

        CallTransition(false);
    }
}

void ClearScene::Finalize()
{
    // BGM終了
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void ClearScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    // マスクの更新
    TransitionMask(elapsed_time);


    // イージングの更新
    star1_scale.EasingValue(elapsed_time);
    star2_scale.EasingValue(elapsed_time);
    star3_scale.EasingValue(elapsed_time);

    // アイリスインの最中操作を受け付けない
    if (start_transition && is_in)return;

    // スターの演出
    if (is_star_1 || is_star_2 || is_star_3)
    {
        if (!start_transition)
        {
            timer += elapsed_time;
        }

        // 演出用ラムダ式
        auto star_direction = [&](EasingFunction& scale, int& direction_num, bool& is_star) {

            switch (direction_num)
            {
            case 0:
                // アイリスアウトが終わってから一度のみ実行
                if (!start_transition)
                {
                    // 大きくする
                    scale.CallValueEasing(1.4f, scale.value, EasingFunction::EasingType::OutSine, 0.4f);
                    direction_num++;
                }
                break;
            case 1:
                // 大きくなるイージングが終わったら
                if (!scale.is_easing)
                {
                    // 小さくする
                    scale.CallValueEasing(0.7f, scale.value, EasingFunction::EasingType::InSine, 0.2f);
                    direction_num++;
                }
                break;
            case 2:
                // 小さくなったら
                if (!scale.is_easing)
                {
                    // 大きさを戻す
                    scale.CallValueEasing(1.0f, scale.value, EasingFunction::EasingType::OutSine, 0.1f);
                    // 初期化してフラグをOFF
                    direction_num = 0;
                    is_star = false;
                }
            }
            };

        if (is_star_1 && timer >= interval_1)
        {
            star_direction(star1_scale, direction_num_1, is_star_1);
        }
        if (is_star_2 && timer >= interval_2)
        {
            star_direction(star2_scale, direction_num_2, is_star_2);
        }
        if (is_star_3 && timer >= interval_3)
        {
            star_direction(star3_scale, direction_num_3, is_star_3);
        }
        return;
    }

    if (once_only)
    {
        // つぎへボタンを動かし始める
        next.CallValueContinue(1.0f, 1.1f, next.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
        once_only = false;
    }


    // どのボタンを選択しているか
    switch (select_num)
    {
    case Button::Next:// 次へ
        if (!is_continue)// 選択されていない時
        {
            // 下を選んだとき
            if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
            {
                next.CallValueEasing(1.0f, next.value, EasingFunction::EasingType::InSine);
                select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Select;
                break;
            }

            // イージングを更新
            select.EasingValue(elapsed_time);
            again.EasingValue(elapsed_time);
            next.ContinueEasing(elapsed_time);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // アイリスインを呼ぶ
        if (clear_start_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // ステージのレベルを加算
            StageManager::Instance().AddStageLevel();
            // 次のシーンへ
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new FormationScene));
        }
        break;

    case Button::Select:
        if (!is_continue)// 選択されていない時
        {
            // 下を選んだとき
            if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
            {
                select.CallValueEasing(1.0f, select.value, EasingFunction::EasingType::InSine);
                again.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Again;
                break;
            }
            // 上を選んだとき
            else if (gamePad.GetButtonDown() & gamePad.BTN_UP)
            {
                select.CallValueEasing(1.0f, select.value, EasingFunction::EasingType::InSine);
                next.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Next;
                break;
            }

            // イージングを更新
            next.EasingValue(elapsed_time);
            again.EasingValue(elapsed_time);
            select.ContinueEasing(elapsed_time);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // アイリスインを呼ぶ
        if (clear_start_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // 次のシーンへ
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
        }
        break;
    case Button::Again:

        if (!is_continue)// 選択されていない時
        {
            if (gamePad.GetButtonDown() & gamePad.BTN_UP)// 上選択すると
            {
                again.CallValueEasing(1.0f, again.value, EasingFunction::EasingType::InSine);
                select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Select;
                break;
            }

            // イージングを更新
            next.EasingValue(elapsed_time);
            select.EasingValue(elapsed_time);
            again.ContinueEasing(elapsed_time);

            // 選択されると
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // アイリスインを呼ぶ
        if (clear_start_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // 次のシーンへ
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
        }
        break;
    }


}

void ClearScene::Render(float elapsedTime)
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
        if(!clear_start_back->is_anime_end)clear_start_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, false);
        else clear_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 1);

        result_cover->RenderRightUp(immediate_context, SCREEN_WIDTH, 0, 1200, SCREEN_HEIGHT, 0.0f);

        if (is_star_1)star_frame1->RenderCenter(immediate_context, 135 + 1077, 270 + 30, 190, 200, 0.0f);
        if (is_star_2)star_frame2->RenderCenter(immediate_context, 350 + 1077, 175 + 30, 325, 315, 0.0f);
        if (is_star_3)star_frame3->RenderCenter(immediate_context, 560 + 1077, 270 + 30, 190, 200, 0.0f);
        star1->RenderCenter(immediate_context, 135 + 1077, 270 + 30, 190 * star1_scale.value, 190 * star1_scale.value, 0.0f);
        star2->RenderCenter(immediate_context, 350 + 1077, 175 + 30, 260 * star2_scale.value, 250 * star2_scale.value, 0.0f);
        star3->RenderCenter(immediate_context, 560 + 1077, 270 + 30, 190 * star3_scale.value, 190 * star3_scale.value, 0.0f);
        if (!is_star_1)star_frame1->RenderCenter(immediate_context, 135 + 1077, 270 + 30, 190, 200, 0.0f);
        if (!is_star_2)star_frame2->RenderCenter(immediate_context, 350 + 1077, 175 + 30, 325, 315, 0.0f);
        if (!is_star_3)star_frame3->RenderCenter(immediate_context, 560 + 1077, 270 + 30, 190, 200, 0.0f);
    
        // ボタン
        next_text->RenderCenter(immediate_context, 1616, 573, 500 * next.value, 200 * next.value);
        select_text->RenderCenter(immediate_context, 1616, 756, 500 * select.value, 200 * select.value);
        again_text->RenderCenter(immediate_context, 1616, 940, 500 * again.value, 200 * again.value);
    }
    // マスク
    RenderTransitionMask(elapsedTime);
}

void ClearScene::DebugImgui()
{
}
