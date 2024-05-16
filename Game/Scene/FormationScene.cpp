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

    // �V�F�[�_�[�֘A
    {
        // �X�e�[�g�̏�����
        InitializeState();
        // �f�B�t�@�[�h�����_�����O�̏�����
        InitializeDeffered(SCREEN_WIDTH, SCREEN_HEIGHT);
        // �t���[���o�b�t�@�[�̏�����
        InitializeFramebuffer();
        // �s�N�Z���V�F�[�_�[�̏�����
        InitializePS();
        // �}�X�N�̏�����
        InitializeMask();

        create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", Try.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", chara_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps.cso", stage_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_chara_ps.cso", gltf_ps.GetAddressOf());

        create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
    }

    // ���f���A�e�N�X�`���ǂݍ���
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
   
        // �G�t�F�N�g
        effect           = new Effect(".\\resources\\Effect\\UNIT_DEATH\\UNIT_DEATH.efk");
    }

    // �Q�[���֘A
    {
        // �J����
        Camera& camera = Camera::Instance();
        camera_angle = { DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(180),DirectX::XMConvertToRadians(0) };

        // ���s���C�g
        directional_light_direction = { 0.0f,-0.3f,-1.0f,1.0f };

        for (int i = 0; i < 6; i++)
        {
            // �A�j���[�V�����̍Đ�
            gltf_unit[i]->PlayAnimation(0, true);
            // �����x�̃N���A
            gltf_unit[i]->ClearThreshold();
        }

        // �u���[���̒���
        bloomer->bloom_extraction_threshold = 0.05f;
        bloomer->bloom_intensity = 0.15f;

        enable_post_effect = true;

        line_y.value = -1080.0f;
        button.value = 0.9f;

        // �}�X�N���Ă�
        CallTransition(false);
        // �|�X�g�G�t�F�N�g�ŏI
        create_ps_from_cso(graphics.GetDevice(), "Shader/formation_final_pass.cso", pixel_shaders[static_cast<size_t>(PS::FINAL)].GetAddressOf());
    }

    // �|�C���g���C�g�E�X�|�b�g���C�g�̏����ʒu�ݒ�
    InitializeLight();
}

void FormationScene::Finalize()
{
    // �G�t�F�N�g�I��
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
    // �G�t�F�N�g�X�V����
    EffectManager::Instance().Update(elapsedTime);
    // �J����
    {
        camera.Update(elapsedTime);
        camera.SetTarget(camera_target);
        camera.SetRange(camera_range);
        camera.SetEyeYOffset(0);
        camera.SetAngle(camera_angle);
    }
    // ���C�g�̍X�V
    LightUpdate();

    // �}�X�N�̍X�V
    TransitionMask(elapsedTime);

    // �A�C���X�C���̍Œ�������󂯕t���Ȃ�
    if (start_transition && is_in)return;

    // �C�[�W���O�̍X�V
    line_y.EasingValue(elapsedTime);

    if (once_only && !start_transition)
    {
        line_y.CallValueEasing(0, line_y.value, EasingFunction::EasingType::OutBounce, 1.5f);
        once_only = false;
    }
    if (line_y.is_easing)return;

    // ���j�b�g�̍X�V
    UpdateUnit(elapsedTime);

    // ����
    UpdateOperate(elapsedTime);

    DebugImgui();
}

void FormationScene::UpdateUnit(float elapsedTime)
{
    // ���j�b�g�̍X�V
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
        // ���̃V�[����
        Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
    }
    // �{�^����I�����Ă鎞
    if (select_button)
    {
        // �C�[�W���O���X�V
        button.ContinueEasing(elapsedTime);

        // ��ɍs������
        if (gamePad.GetAxisLY() >= 1.0f || gamePad.GetButtonDown() & gamePad.BTN_UP)
        {
            select_button = false;
            interval_timer = 0.0f;
            button.CallValueEasing(0.9f, button.value, EasingFunction::EasingType::InSine);
        }

        // A�{�^������������
        if (gamePad.GetButtonDown() & gamePad.BTN_A && all_unit_num >= 4)
        {
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A] = cont_num[gamePad.A];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B] = cont_num[gamePad.B];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X] = cont_num[gamePad.X];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y] = cont_num[gamePad.Y];

            CallTransition(true);
        }
    }


    // �{�^����I�����Ă��Ȃ���
    if (!select_button)
    {
        // �C�[�W���O���X�V
        button.EasingValue(elapsedTime);

        // �S��������
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
                // �����x�̃N���A
                gltf_unit[i]->ClearThreshold();
            }
        }

        // ����I�����鎞
        if (gamePad.GetAxisLY() <= -1.0f || gamePad.GetButtonDown() & gamePad.BTN_DOWN)
        {
            select_button = true;
            interval_timer = 0.0f;
            // �{�^���R���e�B�j���[���Ă�
            button.CallValueContinue(0.9f, 1.1f, button.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
        }


        // ���j�b�g�̑I��
        {
            // �E
            if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
            {
                if (choose_num < 6)choose_num++;
            }
            // ��
            else if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
            {
                if (choose_num > 0)choose_num--;
            }
            // �E
            else if (gamePad.GetAxisLX() >= 0.5f)
            {
                // �C���^�[�o�����l�����ăJ�E���g�𓮂���
                interval_timer += elapsedTime;
                if (choose_num < 5 && interval_timer_max <= interval_timer)
                {
                    choose_num++;
                    interval_timer = 0.0f;
                }
            }
            // ��
            else if (gamePad.GetAxisLX() <= -0.5f)
            {
                // �C���^�[�o�����l�����ăJ�E���g�𓮂���
                interval_timer += elapsedTime;
                if (choose_num > 0 && interval_timer_max <= interval_timer)
                {
                    choose_num--;
                    interval_timer = 0.0f;
                }
            }
        }

        // ���j�b�g���S���I�΂�Ă��Ȃ���
        if (all_unit_num < 4)
        {
            // ���j�b�g���I�ׂ��Ԃ̎�
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
    // �`��̐ݒ�
    SetUpRendering();
    SetUpConstantBuffer();

    // �e�N�X�`�����Z�b�g
    {
        // �m�C�Y
        immediate_context->PSSetShaderResources(9/*slot(1�ԂɃZ�b�g���܂�)*/, 1, mask_texture.GetAddressOf());//TODO
        // �V���h�E
        //immediate_context->PSSetShaderResources(8, 1, shadow_map->shader_resource_view.GetAddressOf());
        //�@�[�x�l
        immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
    }

    //�X�e�[�g�̐ݒ�
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].Get(), nullptr, 0xFFFFFFFF);
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());



    // �|�X�g�G�t�F�N�g�̊J�n
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->Clear(immediate_context);
        framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->Activate(immediate_context);
    }

    // 2D�`��i3D�̌�ʁj
    {
        // �X�e�[�g�̐ݒ�
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

    // �|�X�g�G�t�F�N�g�̎��s
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::TEX)]->Deactivate(immediate_context);
    }

    // �|�X�g�G�t�F�N�g�̊J�n
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context, 0, 0, 0, 0);
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
    }

    // 3D�`��
    {
        immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
        immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

        for (int i = 0; i < 6; i++)
        {
            if (enable_units[i])           gltf_unit[i]->Render(1.0f, gltf_ps.Get());
        }
    }

    // �|�X�g�G�t�F�N�g�̎��s
    if (enable_post_effect)
    {
        framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
        ExePostEffct();
    }

    // 2D�`��i3D�̑O�ʁj
    {
        // �{�^���̃A���t�@�x�b�g
        for (int i = 0; i < 4; i++)
        {
            Controller_UI[i]->Render(immediate_context, float(850 + i * 300), 800, 150, 150);
        }
        // �{�^��
        Button->RenderCenter(immediate_context, 1300.0f, 980.0f, 500* button.value, 200* button.value);

        {
            DirectX::XMFLOAT4X4 view;
            DirectX::XMFLOAT4X4 projection;
            DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
            DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
            graphics.GetDebugRenderer()->Render(immediate_context, view, projection);

            // �G�t�F�N�g�Đ�
            EffectManager::Instance().Render(view, projection);
        }

        // �}�X�N
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
        // ���j�b�g�̐ݒ�
        units_position[choose_num] = position[button_num];
        units_rotation[choose_num] = rotation[button_num];
        enable_units[choose_num] = true;
        all_unit_num++;

        // �R���g���[���[
        enable_controllers[button_num] = true;
        // ���Ԗڂ̃��j�b�g��A�{�^���ɑI�΂ꂽ��
        cont_num[button_num] = choose_num;

        // �G�t�F�N�g�̍Đ�
        effect->Play(position[button_num], effect_scale);
        gltf_unit[choose_num]->SetIsDissolve(true);
    }
}
