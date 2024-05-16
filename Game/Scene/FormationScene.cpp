#include "FormationScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"
#include "./Lemur/Effekseer/EffekseerManager.h"


#include "GameScene.h"
#include "LoadingScene.h"

void FormationScene::Initialize()
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
        // マスクの初期化
        InitializeMask();

        create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", Try.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", chara_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps.cso", stage_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_chara_ps.cso", gltf_ps.GetAddressOf());

        create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
    }

    // モデル、テクスチャ読み込み
    {
        // 2D
        back             = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene.png");
        line_1           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_1.png");
        line_2           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_2.png");
        unit_1           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_1.png");
        unit_2           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_2.png");
        unit_3           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_3.png");
        unit_4           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_4.png");
        unit_5           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_5.png");
        unit_6           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_6.png");
        Button           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Button.png");
        base             = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Base.png");
        Controller_UI[0] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_A.png");
        Controller_UI[1] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_B.png");
        Controller_UI[2] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_X.png");
        Controller_UI[3] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_Y.png");
        mark_1           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_1.png");
        mark_1_1         = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_1_1.png");
        mark_2           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_2.png");
        mark_2_2         = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_2_2.png");

        // 3D
        gltf_unit[0]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb", true);
        gltf_unit[1]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Shishito.glb", true);
        gltf_unit[2]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\OrangePumpkin.glb", true);
        gltf_unit[3]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\GreenPumpkin.glb", true);
        gltf_unit[4]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Broccoli.glb", true);
        gltf_unit[5]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Cauliflower.glb", true);
   
        // エフェクト
        effect           = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    }

    // ゲーム関連
    {
        // カメラ
        Camera& camera = Camera::Instance();
        camera_angle = { DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(180),DirectX::XMConvertToRadians(0) };

        // 並行ライト
        directional_light_direction = { 0.0f,-0.3f,-1.0f,1.0f };

        for (int i = 0; i < 6; i++)
        {
            // アニメーションの再生
            gltf_unit[i]->PlayAnimation(0, true);
            // 透明度のクリア
            gltf_unit[i]->ClearThreshold();
        }

        // ブルームの調整
        bloomer->bloom_extraction_threshold = 0.05f;
        bloomer->bloom_intensity = 0.15f;

        enable_post_effect = true;

        line_y.value = -1080.0f;
        button.value = 0.9f;

        // マスクを呼ぶ
        CallTransition(false);
        // ポストエフェクト最終
        create_ps_from_cso(graphics.GetDevice(), "Shader/formation_final_pass.cso", pixel_shaders[static_cast<size_t>(PS::FINAL)].GetAddressOf());
    }

    // ポイントライト・スポットライトの初期位置設定
    InitializeLight();
}

void FormationScene::Finalize()
{
    // エフェクト終了
    if (effect != nullptr)
    {
        delete effect;
        effect = nullptr;
    }

}

void FormationScene::Update(HWND hwnd, float elapsedTime)
{
    using namespace DirectX;
    Camera& camera = Camera::Instance();
    // エフェクト更新処理
    EffectManager::Instance().Update(elapsedTime);
    // カメラ
    {
        camera.Update(elapsedTime);
        camera.SetTarget(camera_target);
        camera.SetRange(camera_range);
        camera.SetEyeYOffset(0);
        camera.SetAngle(camera_angle);
    }
    // ライトの更新
    LightUpdate();

    // マスクの更新
    TransitionMask(elapsedTime);

    // アイリスインの最中操作を受け付けない
    if (start_transition && is_in)return;

    // イージングの更新
    line_y.EasingValue(elapsedTime);

    if (once_only && !start_transition)
    {
        line_y.CallValueEasing(0, line_y.value, EasingFunction::EasingType::OutBounce, 1.5f);
        once_only = false;
    }
    if (line_y.is_easing)return;

    // ユニットの更新
    UpdateUnit(elapsedTime);

    // 操作
    UpdateOperate(elapsedTime);

    DebugImgui();
}

void FormationScene::UpdateUnit(float elapsedTime)
{
    // ユニットの更新
    {
        for (int i = 0; i < 6; i++)
        {
            gltf_unit[i]->UpdateAnimation(elapsedTime);

            gltf_unit[i]->GetTransform()->SetPositionX(units_position[i].x);
            gltf_unit[i]->GetTransform()->SetPositionY(units_position[i].y);
            gltf_unit[i]->GetTransform()->SetPositionZ(units_position[i].z);
            gltf_unit[i]->GetTransform()->SetRotationY(units_rotation[i]);
            gltf_unit[i]->GetTransform()->SetScaleFactor(2.0f);

            gltf_unit[i]->InDissolve(elapsedTime);
        }
    }
}

void FormationScene::UpdateOperate(float elapsedTime)
{
    GamePad& gamePad = Input::Instance().GetGamePad();

    button.EasingValue(elapsedTime);

    if (!start_transition && is_in)
    {
        // 次のシーンへ
        Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
    }
    // ボタンを選択してる時
    if (select_button)
    {
        // イージングを更新
        button.ContinueEasing(elapsedTime);

        // 上に行ったら
        if (gamePad.GetAxisLY() >= 1.0f || gamePad.GetButtonDown() & gamePad.BTN_UP)
        {
            select_button = false;
            interval_timer = 0.0f;
            button.CallValueEasing(0.9f, button.value, EasingFunction::EasingType::InSine);
        }

        // Aボタンを押したら
        if (gamePad.GetButtonDown() & gamePad.BTN_A && all_unit_num >= 4)
        {
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A] = cont_num[gamePad.A];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B] = cont_num[gamePad.B];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X] = cont_num[gamePad.X];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y] = cont_num[gamePad.Y];

            CallTransition(true);
        }
    }


    // ボタンを選択していない時
    if (!select_button)
    {
        // イージングを更新
        button.EasingValue(elapsedTime);

        // 全部初期化
        if (gamePad.GetButtonDown() & gamePad.BTN_LEFT_TRIGGER)
        {
            bool is = false;
            for (int n = 0; n < 6; n++)
            {
                if (gltf_unit[n]->GetIsDissolve())
                {
                    is = true;
                }
            }
            if (is)return;
            for (int i = 0; i < 6; i++)
            {
                enable_units[i] = {};
                units_rotation[i] = {};
                units_position[i] = {};
            }
            for (int j = 0; j < 4; j++)
            {
                enable_controllers[j] = {};
                cont_num[j] = {};
            }
            choose_num = 0;
            all_unit_num = 0;
            select_button = false;

            for (int i = 0; i < 6; i++)
            {
                // 透明度のクリア
                gltf_unit[i]->ClearThreshold();
            }
        }

        // 下を選択する時
        if (gamePad.GetAxisLY() <= -1.0f || gamePad.GetButtonDown() & gamePad.BTN_DOWN)
        {
            select_button = true;
            interval_timer = 0.0f;
            // ボタンコンティニューを呼ぶ
            button.CallValueContinue(0.9f, 1.1f, button.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
        }


        // ユニットの選択
        {
            // 右
            if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
            {
                if (choose_num < 6)choose_num++;
            }
            // 左
            else if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
            {
                if (choose_num > 0)choose_num--;
            }
            // 右
            else if (gamePad.GetAxisLX() >= 0.5f)
            {
                // インターバルを考慮してカウントを動かす
                interval_timer += elapsedTime;
                if (choose_num < 5 && interval_timer_max <= interval_timer)
                {
                    choose_num++;
                    interval_timer = 0.0f;
                }
            }
            // 左
            else if (gamePad.GetAxisLX() <= -0.5f)
            {
                // インターバルを考慮してカウントを動かす
                interval_timer += elapsedTime;
                if (choose_num > 0 && interval_timer_max <= interval_timer)
                {
                    choose_num--;
                    interval_timer = 0.0f;
                }
            }
        }

        // ユニットが全員選ばれていない時
        if (all_unit_num < 4)
        {
            // ユニットが選べる状態の時
            if (!enable_units[choose_num])
            {
                SelectUnit(gamePad.GetButtonDown()& gamePad.BTN_A, gamePad.A);
                SelectUnit(gamePad.GetButtonDown()& gamePad.BTN_B, gamePad.B);
                SelectUnit(gamePad.GetButtonDown()& gamePad.BTN_X, gamePad.X);
                SelectUnit(gamePad.GetButtonDown()& gamePad.BTN_Y, gamePad.Y);
            }
        }
    }
}

void FormationScene::Render(float elapsedTime)
{
    HRESULT hr{ S_OK };

    Camera& camera = Camera::Instance();
    GamePad& gamePad = Input::Instance().GetGamePad();
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

    camera.SetPerspectiveFov(immediate_context);
    // 描画の設定
    SetUpRendering();
    SetUpConstantBuffer();

    // テクスチャをセット
    {
        // ノイズ
        immediate_context->PSSetShaderResources(9/*slot(1番にセットします)*/, 1, mask_texture.GetAddressOf());//TODO
        // シャドウ
        //immediate_context->PSSetShaderResources(8, 1, shadow_map->shader_resource_view.GetAddressOf());
        //　深度値
        immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
    }

    //ステートの設定
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].Get(), nullptr, 0xFFFFFFFF);
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());



    // ポストエフェクトの開始
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->Clear(immediate_context);
        framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->Activate(immediate_context);
    }

    // 2D描画（3Dの後面）
    {
        // ステートの設定
        immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
        immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

        back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        for (int i = 0; i < 6; i++)
        {
            line_1->Render(immediate_context, (660 + 210 * i), line_y.value, 210, SCREEN_HEIGHT);
        }
        line_2->Render(immediate_context, (660 + 210 * choose_num), line_y.value, 210, SCREEN_HEIGHT);

        unit_1->Render(immediate_context,(660 + 210 * 0), line_y.value, 210, SCREEN_HEIGHT);
        unit_2->Render(immediate_context,(660 + 210 * 1), line_y.value, 210, SCREEN_HEIGHT);
        unit_3->Render(immediate_context,(660 + 210 * 2), line_y.value, 210, SCREEN_HEIGHT);
        unit_4->Render(immediate_context,(660 + 210 * 3), line_y.value, 210, SCREEN_HEIGHT);
        unit_5->Render(immediate_context,(660 + 210 * 4), line_y.value, 210, SCREEN_HEIGHT);
        unit_6->Render(immediate_context,(660 + 210 * 5), line_y.value, 210, SCREEN_HEIGHT);

        for (int j = 0; j < 4; j++)
        {
            base->Render(immediate_context, (691.5f + 300 * j), 680, 300, 300);
        }
    }

    // ポストエフェクトの実行
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->Deactivate(immediate_context);
    }

    // ポストエフェクトの開始
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context, 0, 0, 0, 0);
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
    }

    // 3D描画
    {
        immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

        for (int i = 0; i < 6; i++)
        {
            if (enable_units[i])           gltf_unit[i]->Render(1.0f, gltf_ps.Get());
        }
    }

    // ポストエフェクトの実行
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
        ExePostEffct();
    }

    // 2D描画（3Dの前面）
    {
        // ボタンのアルファベット
        for (int i = 0; i < 4; i++)
        {
            Controller_UI[i]->Render(immediate_context, float(850 + i * 300), 800, 150, 150);
        }
        // ボタン
        Button->RenderCenter(immediate_context, 1300.0f, 980.0f, 500* button.value, 200* button.value);

        {
            DirectX::XMFLOAT4X4 view;
            DirectX::XMFLOAT4X4 projection;
            DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
            DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
            graphics.GetDebugRenderer()->Render(immediate_context, view, projection);

            // エフェクト再生
            EffectManager::Instance().Render(view, projection);
        }

        // マスク
        RenderTransitionMask(elapsedTime);
    }
}


void FormationScene::DebugImgui()
{
    BaseScene::DebugImgui();
    Camera::Instance().DrawDebug();
    gltf_unit[0]->DrawDebug();
}

void FormationScene::InitializeLight()
{
    ZeroMemory(&point_light[8], sizeof(pointLights) * 8);
    ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);

    BaseScene::InitializeLight();

}

void FormationScene::SelectUnit(bool BTN, int button_num)
{
    if (BTN && !enable_controllers[button_num])
    {
        // ユニットの設定
        units_position[choose_num] = position[button_num];
        units_rotation[choose_num] = rotation[button_num];
        enable_units[choose_num] = true;
        all_unit_num++;

        // コントローラー
        enable_controllers[button_num] = true;
        // 何番目のユニットがAボタンに選ばれたか
        cont_num[button_num] = choose_num;

        // エフェクトの再生
        effect->Play(position[button_num], effect_scale);
        gltf_unit[choose_num]->SetIsDissolve(true);
    }
}
