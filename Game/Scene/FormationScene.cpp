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

    // ゲーム関連
    {
        // カメラ
        Camera& camera = Camera::Instance();
        //camera_angle = { DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(180),DirectX::XMConvertToRadians(0) };
        //directional_light_direction = { 0.0f,-0.3f,-1.0f,1.0f };

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
        gltf_unit_1 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit3_RE.glb", true);
        gltf_unit_2 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit2_RE.glb", true);
        gltf_unit_3 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit3_RE.glb", true);
        gltf_unit_4 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit4_RE.glb", true);
        gltf_unit_5 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit5_RE.glb", true);
        gltf_unit_6 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit6_RE.glb", true);

        gltf_unit_1->PlayAnimation(0, true);
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
    }

    // カメラ
    {
        camera.Update(elapsedTime);
        camera.SetTarget(camera_target);
        camera.SetRange(camera_range);
        camera.SetEyeYOffset(0);
       // camera.SetAngle(camera_angle);
    }
    // ライトの更新
    LightUpdate();

    // DebugImgui
    DebugImgui();
}

void FormationScene::Render(float elapsedTime)
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
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
    }

    // 2D描画
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
        line_2->Render(immediate_context, 660, 0, 210, SCREEN_HEIGHT);

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

        Button->Render(immediate_context, 1060, 885, 500, 200);
        //Controller_UI_A->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //Controller_UI_B->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //Controller_UI_X->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //Controller_UI_Y->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_1_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
        //mark_2_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    // 3D描画
    {
        immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

        gltf_unit_1->Render(1.0f, gltf_ps.Get());
    }

    RenderingDeffered();


    // ポストエフェクトの実行
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
        ExePostEffct();
    }
}

void FormationScene::DebugImgui()
{
    BaseScene::DebugImgui();
    Camera::Instance().DrawDebug();
}

void FormationScene::InitializeLight()
{

    point_light[0].position.x = 10;
    point_light[0].position.y = 1;
    point_light[0].range = 10;
    point_light[0].color = { 1, 0, 0, 1 };
    point_light[1].position.x = -10;
    point_light[1].position.y = 1;
    point_light[1].range = 10;
    point_light[1].color = { 0, 1, 0, 1 };
    point_light[2].position.y = 1;
    point_light[2].position.z = 10;
    point_light[2].range = 10;
    point_light[2].position.y = 1;
    point_light[2].color = { 0, 0, 1, 1 };
    point_light[3].position.y = 1;
    point_light[3].position.z = -10;
    point_light[3].range = 10;
    point_light[3].color = { 1, 1, 1, 1 };
    point_light[4].range = 10;
    point_light[4].color = { 1, 1, 1, 1 };
    ZeroMemory(&point_light[8], sizeof(pointLights) * 8);

    spot_light[0].position = { 0, 10, 0, 0 };
    spot_light[0].direction = { -1, -1, 1, 0 };
    Mathf::NormalizeDirectXVector(spot_light[0].direction);

    spot_light[0].range = 300;
    spot_light[0].angle = DirectX::XMConvertToRadians(25.0f);
    spot_light[0].color = { 1, 0, 0, 1 };


    spot_light[0].position = { 0, 50, 0, 0 };
    spot_light[0].direction = { -0.79, -0.79, -1, 0 };
    Mathf::NormalizeDirectXVector(spot_light[0].direction);
    spDirection.Set(-0.79, -0.79, -1);
    spDirection.Normalize();
    spot_light[0].range = 300;
    spot_light[0].angle = DirectX::XMConvertToRadians(25.0f);
    spot_light[0].color = { 1, 0, 0, 1 };

    spot_light[1].position = { -15, 3, 15, 0 };
    spot_light[1].direction = { +1, -1, -1, 0 };
    spot_light[1].range = 100;
    spot_light[1].color = { 0, 1, 0, 1 };
    spot_light[2].position = { 15, 3, -15, 0 };
    spot_light[2].direction = { -1, -1, +1, 0 };
    spot_light[2].range = 100;
    spot_light[2].color = { 0, 0, 1, 1 };
    spot_light[3].position = { -15, 3, -15, 0 };
    spot_light[3].direction = { +1, -1, +1, 0 };
    spot_light[3].range = 100;
    spot_light[3].color = { 1, 1, 1, 1 };
    ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);

    BaseScene::InitializeLight();

}
