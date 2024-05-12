#include "FormationScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"

#include "GameScene.h"

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

        // SHADOW
        double_speed_z = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);

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
        back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene.png");
        line_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_1.png");
        line_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_2.png");
        unit_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_1.png");
        unit_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_2.png");
        unit_3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_3.png");
        unit_4 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_4.png");
        unit_5 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_5.png");
        unit_6 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_6.png");
        Button = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Button.png");
        base = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Base.png");
        Controller_UI_A = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_A.png");
        Controller_UI_B = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_B.png");
        Controller_UI_X = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_X.png");
        Controller_UI_Y = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Controller_UI_Y.png");
        mark_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_1.png");
        mark_1_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_1_1.png");
        mark_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_2.png");
        mark_2_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\mark_2_2.png");

        // 3D
        gltf_unit_1 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb", true);
        gltf_unit_2 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Shishito.glb", true);
        gltf_unit_3 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\GreenPumpkin.glb", true);
        gltf_unit_4 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\OrangePumpkin.glb", true);
        gltf_unit_5 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Broccoli.glb", true);
        gltf_unit_6 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Cauliflower.glb", true);
    }

    // ゲーム関連
    {
        // カメラ
        Camera& camera = Camera::Instance();
        camera_angle = { DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(180),DirectX::XMConvertToRadians(0) };

        // 並行ライト
        directional_light_direction = { 0.0f,-0.3f,-1.0f,1.0f };

        // アニメーションの再生
        gltf_unit_1->PlayAnimation(0, true);
        gltf_unit_2->PlayAnimation(0, true);
        gltf_unit_3->PlayAnimation(0, true);
        gltf_unit_4->PlayAnimation(0, true);
        gltf_unit_5->PlayAnimation(0, true);
        gltf_unit_6->PlayAnimation(0, true);

        // ブルームの調整
        bloomer->bloom_extraction_threshold = 0.05f;
        bloomer->bloom_intensity = 0.15f;

        enable_post_effect = true;

        //enable_controllers[0] = true;
        //enable_controllers[1] = true;
        //enable_controllers[2] = true;
        //enable_controllers[3] = true;

        for (int i = 0; i < 4; i++)
        {
            controllers_position[i] = { float(850 + i * 300),800 };
        }

        button.value = 0.9f;
    }

    // ポイントライト・スポットライトの初期位置設定
    InitializeLight();
}

void FormationScene::Finalize()
{
}

void FormationScene::Update(HWND hwnd, float elapsedTime)
{
    using namespace DirectX;
    Camera& camera = Camera::Instance();
    GamePad& gamePad = Input::Instance().GetGamePad();

    {
        gltf_unit_1->UpdateAnimation(elapsedTime);
        gltf_unit_2->UpdateAnimation(elapsedTime);
        gltf_unit_3->UpdateAnimation(elapsedTime);
        gltf_unit_4->UpdateAnimation(elapsedTime);
        gltf_unit_5->UpdateAnimation(elapsedTime);
        gltf_unit_6->UpdateAnimation(elapsedTime);
    }

    // カメラ
    {
        camera.Update(elapsedTime);
        camera.SetTarget(camera_target);
        camera.SetRange(camera_range);
        camera.SetEyeYOffset(0);
        camera.SetAngle(camera_angle);
    }

    // ユニットの更新
    {
        gltf_unit_1->GetTransform()->SetPositionX(units_position[0].x);
        gltf_unit_1->GetTransform()->SetPositionY(units_position[0].y);
        gltf_unit_1->GetTransform()->SetPositionZ(units_position[0].z);
        gltf_unit_1->GetTransform()->SetRotationY(units_rotation[0]);
        gltf_unit_1->GetTransform()->SetScaleFactor(2.0f);

        gltf_unit_2->GetTransform()->SetPositionX(units_position[1].x);
        gltf_unit_2->GetTransform()->SetPositionY(units_position[1].y);
        gltf_unit_2->GetTransform()->SetPositionZ(units_position[1].z);
        gltf_unit_2->GetTransform()->SetRotationY(units_rotation[1]);
        gltf_unit_2->GetTransform()->SetScaleFactor(2.0f);

        gltf_unit_3->GetTransform()->SetPositionX(units_position[2].x);
        gltf_unit_3->GetTransform()->SetPositionY(units_position[2].y);
        gltf_unit_3->GetTransform()->SetPositionZ(units_position[2].z);
        gltf_unit_3->GetTransform()->SetRotationY(units_rotation[2]);
        gltf_unit_3->GetTransform()->SetScaleFactor(2.0f);

        gltf_unit_4->GetTransform()->SetPositionX(units_position[3].x);
        gltf_unit_4->GetTransform()->SetPositionY(units_position[3].y);
        gltf_unit_4->GetTransform()->SetPositionZ(units_position[3].z);
        gltf_unit_4->GetTransform()->SetRotationY(units_rotation[3]);
        gltf_unit_4->GetTransform()->SetScaleFactor(2.0f);

        gltf_unit_5->GetTransform()->SetPositionX(units_position[4].x);
        gltf_unit_5->GetTransform()->SetPositionY(units_position[4].y);
        gltf_unit_5->GetTransform()->SetPositionZ(units_position[4].z);
        gltf_unit_5->GetTransform()->SetRotationY(units_rotation[4]);
        gltf_unit_5->GetTransform()->SetScaleFactor(2.0f);

        gltf_unit_6->GetTransform()->SetPositionX(units_position[5].x);
        gltf_unit_6->GetTransform()->SetPositionY(units_position[5].y);
        gltf_unit_6->GetTransform()->SetPositionZ(units_position[5].z);
        gltf_unit_6->GetTransform()->SetRotationY(units_rotation[5]);
        gltf_unit_6->GetTransform()->SetScaleFactor(2.0f);
    }

    button.EasingValue(elapsedTime);

    // 操作
    {
        if (!select_button && gamePad.GetAxisLY() <= -1.0f || gamePad.GetButtonDown() & gamePad.BTN_DOWN)
        {
            select_button = true;
            interval_timer = 0.0f;
            button.CallValueEasing(1.1f, button.value, EasingFunction::EasingType::InSine);
        }
        else if (select_button && gamePad.GetAxisLY() >= 1.0f || gamePad.GetButtonDown() & gamePad.BTN_UP)
        {
            select_button = false;
            interval_timer = 0.0f;
            button.CallValueEasing(0.9f, button.value, EasingFunction::EasingType::InSine);
        }

        if (!select_button)
        {
            // ユニットの選択
            if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
            {
                if (choose_num < 6)choose_num++;
            }
            if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
            {
                if (choose_num > 0)choose_num--;
            }
            if (gamePad.GetAxisLX() >= 0.5f)
            {
                interval_timer += elapsedTime;
                if (choose_num < 5 && interval_timer_max <= interval_timer)
                {
                    choose_num++;
                    interval_timer = 0.0f;
                }
            }
            else if (gamePad.GetAxisLX() <= -0.5f)
            {
                interval_timer += elapsedTime;
                if (choose_num > 0 && interval_timer_max <= interval_timer)
                {
                    choose_num--;
                    interval_timer = 0.0f;
                }
            }

            if (all_unit_num < 4)
            {
                if (!enable_units[choose_num])
                {
                    if (gamePad.GetButtonDown() & gamePad.BTN_A && !enable_controllers[0])
                    {
                        // ユニット
                        units_position[choose_num] = position[all_unit_num];
                        units_rotation[choose_num] = rotation[all_unit_num];
                        enable_units[choose_num] = true;
                        all_unit_num++;

                        // コントローラー
                        controllers_num[gamePad.A] = all_controllers_num;
                        enable_controllers[gamePad.A] = true;
                        cont_num[gamePad.A] = choose_num;
                        all_controllers_num++;
                    }
                    else if (gamePad.GetButtonDown() & gamePad.BTN_B && !enable_controllers[1])
                    {
                        units_position[choose_num] = position[all_unit_num];
                        units_rotation[choose_num] = rotation[all_unit_num];
                        enable_units[choose_num] = true;
                        all_unit_num++;

                        // コントローラー
                        controllers_num[gamePad.B] = all_controllers_num;
                        enable_controllers[gamePad.B] = true;
                        cont_num[gamePad.B] = choose_num;
                        all_controllers_num++;
                    }
                    else if (gamePad.GetButtonDown() & gamePad.BTN_X && !enable_controllers[2])
                    {
                        units_position[choose_num] = position[all_unit_num];
                        units_rotation[choose_num] = rotation[all_unit_num];
                        enable_units[choose_num] = true;
                        all_unit_num++;

                        // コントローラー
                        controllers_num[gamePad.X] = all_controllers_num;
                        enable_controllers[gamePad.X] = true;
                        cont_num[gamePad.X] = choose_num;
                        all_controllers_num++;
                    }
                    else if (gamePad.GetButtonDown() & gamePad.BTN_Y && !enable_controllers[3])
                    {
                        units_position[choose_num] = position[all_unit_num];
                        units_rotation[choose_num] = rotation[all_unit_num];
                        enable_units[choose_num] = true;
                        all_unit_num++;

                        // コントローラー
                        controllers_num[gamePad.Y] = all_controllers_num;
                        enable_controllers[gamePad.Y] = true;
                        cont_num[gamePad.Y] = choose_num;
                        all_controllers_num++;
                    }
                }
            }
        }
        else
        {
            if (gamePad.GetButtonDown() & gamePad.BTN_A && all_controllers_num == 4)
            {
                Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A] = cont_num[gamePad.A];
                Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B] = cont_num[gamePad.B];
                Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X] = cont_num[gamePad.X];
                Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y] = cont_num[gamePad.Y];

                Lemur::Scene::SceneManager::Instance().ChangeScene(new GameScene);
            }
        }
    }

    // ライトの更新
    LightUpdate();

    // DebugImgui
    //DebugImgui();
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

    // ディファードレンダリングの設定
    SetUpDeffered();


    // テクスチャをセット
    {
        // ノイズ
        immediate_context->PSSetShaderResources(9/*slot(1番にセットします)*/, 1, mask_texture.GetAddressOf());//TODO
        // シャドウ
        immediate_context->PSSetShaderResources(8, 1, double_speed_z->shader_resource_view.GetAddressOf());
        //　深度値
        immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
    }


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
            line_1->Render(immediate_context, (660 + 210 * i), 0, 210, SCREEN_HEIGHT);
        }
        line_2->Render(immediate_context, (660 + 210 * choose_num), 0, 210, SCREEN_HEIGHT);

        unit_1->Render(immediate_context,(660 + 210 * 0), 0, 210, SCREEN_HEIGHT);
        unit_2->Render(immediate_context,(660 + 210 * 1), 0, 210, SCREEN_HEIGHT);
        unit_3->Render(immediate_context,(660 + 210 * 2), 0, 210, SCREEN_HEIGHT);
        unit_4->Render(immediate_context,(660 + 210 * 3), 0, 210, SCREEN_HEIGHT);
        unit_5->Render(immediate_context,(660 + 210 * 4), 0, 210, SCREEN_HEIGHT);
        unit_6->Render(immediate_context,(660 + 210 * 5), 0, 210, SCREEN_HEIGHT);

        for (int j = 0; j < 4; j++)
        {
            base->Render(immediate_context, (691.5f + 300 * j), 680, 300, 300);
        }


        //mark_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_1_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_2_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
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

        if(enable_units[0])gltf_unit_1->Render(1.0f, gltf_ps.Get());
        if(enable_units[1])gltf_unit_2->Render(1.0f, gltf_ps.Get());
        if(enable_units[2])gltf_unit_3->Render(1.0f, gltf_ps.Get());
        if(enable_units[3])gltf_unit_4->Render(1.0f, gltf_ps.Get());
        if(enable_units[4])gltf_unit_5->Render(1.0f, gltf_ps.Get());
        if(enable_units[5])gltf_unit_6->Render(1.0f, gltf_ps.Get());
    }

    RenderingDeffered();

    // ポストエフェクトの実行
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
        ExePostEffct();
    }

    // 2D描画（3Dの前面）
    {
        // ボタンのアルファベット
        if (enable_controllers[gamePad.A])Controller_UI_A->Render(immediate_context, controllers_position[controllers_num[gamePad.A]].x, controllers_position[controllers_num[gamePad.A]].y, 150, 150);
        if (enable_controllers[gamePad.B])Controller_UI_B->Render(immediate_context, controllers_position[controllers_num[gamePad.B]].x, controllers_position[controllers_num[gamePad.B]].y, 150, 150);
        if (enable_controllers[gamePad.X])Controller_UI_X->Render(immediate_context, controllers_position[controllers_num[gamePad.X]].x, controllers_position[controllers_num[gamePad.X]].y, 150, 150);
        if (enable_controllers[gamePad.Y])Controller_UI_Y->Render(immediate_context, controllers_position[controllers_num[gamePad.Y]].x, controllers_position[controllers_num[gamePad.Y]].y, 150, 150);

        // ボタン
        Button->RenderCenter(immediate_context, 1300.0f, 980.0f, 500* button.value, 200* button.value);
    }
}

void FormationScene::DebugImgui()
{
    ImGui::Begin("Contlo");
    ImGui::SliderFloat2("position", &controllers_position[0].x, 0.0f, 1920.0f);
    ImGui::End();
    BaseScene::DebugImgui();
    Camera::Instance().DrawDebug();
    gltf_unit_1->GetTransform()->DrawDebug("unit_1");
    gltf_unit_2->GetTransform()->DrawDebug("unit_2");
    gltf_unit_3->GetTransform()->DrawDebug("unit_3");
    gltf_unit_4->GetTransform()->DrawDebug("unit_4");
}

void FormationScene::InitializeLight()
{
    ZeroMemory(&point_light[8], sizeof(pointLights) * 8);
    ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);

    BaseScene::InitializeLight();

}
