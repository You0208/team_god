#include "FormationScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"
#include "./Lemur/Effekseer/EffekseerManager.h"
#include"Game/Stage/StageManager.h"

#include "GameScene.h"
#include "LoadingScene.h"
#include "SelectScene.h"

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
        create_ps_from_cso(graphics.GetDevice(), "./Shader/unit_ps.cso", chara_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_select_ps.cso", stage_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_chara_ps.cso", gltf_ps.GetAddressOf());

        create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_select_ps.cso", select_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
        create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
        // �|�X�g�G�t�F�N�g�ŏI
        pixel_shaders[static_cast<size_t>(PS::FINAL)].Reset();// ���������[�N�΍�
        create_ps_from_cso(graphics.GetDevice(), "./Shader/formation_final_pass.cso", pixel_shaders[static_cast<size_t>(PS::FINAL)].GetAddressOf());
    }

    // ���f���A�e�N�X�`���ǂݍ���
    {
        // 2D
        back             = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene.png");
        front            = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_front.png");
        line_1           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_1.png");
        line_2           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_2.png");
        line_blue        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Line_3.png");
        unit_line[0]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_1.png");
        unit_line[1]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_2.png");
        unit_line[2]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_3.png");
        unit_line[3]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_4.png");
        unit_line[4]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_5.png");
        unit_line[5]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_6.png");
        unit_line[6]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Unit_7.png");
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
        
        Best = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Best.png");
        arrow = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Arrow.png");

        // 3D
        gltf_unit[0]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb", true);
        gltf_unit[1]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Shishito.glb", true);
        gltf_unit[2]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\OrangePumpkin.glb", true);
        gltf_unit[3]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\GreenPumpkin.glb", true);
        gltf_unit[4]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Broccoli.glb", true);
        gltf_unit[5]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Cauliflower.glb", true);
        gltf_unit[6]      = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Mustard.glb", true);

        unit_attack[UNIT::Chili]         = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit1.png");
        unit_attack[UNIT::Shishito]      = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit2.png");
        unit_attack[UNIT::OrangePumpkin] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit3.png");
        unit_attack[UNIT::GreenPumpkin]  = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit4.png");
        unit_attack[UNIT::Broccoli]      = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit5.png");
        unit_attack[UNIT::Cauliflower]   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit6.png");
        unit_attack[UNIT::J]             = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation\\Formation_scene_unit7.png");

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

        for (int i = 0; i < UNIT_MAX; i++)
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
        line_x.value = (660 + 210 * choose_num);
        line_add.value = 0.0f;

        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::FORMATION, true);
        // �}�X�N���Ă�
        //CallTransition(false);
    }
    // �A�C���X�A�E�g���Ă�
    CallTransition(false);


    // �|�C���g���C�g�E�X�|�b�g���C�g�̏����ʒu�ݒ�
    InitializeLight();
}

void FormationScene::Finalize()
{
    // BGM�I��
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
    // �G�t�F�N�g�I��
    effect->Stop(effect_handle[0]);
    effect->Stop(effect_handle[1]);
    effect->Stop(effect_handle[2]);
    effect->Stop(effect_handle[3]);
    if (effect != nullptr)
    {
        delete effect;
        effect = nullptr;
    }
    EffectManager::Instance().Update(0.01f);
}

void FormationScene::Update(HWND hwnd, float elapsedTime)
{
    using namespace DirectX;
    Camera& camera = Camera::Instance();
    // �Q�[���p�b�h
    GamePad& gamePad = Input::Instance().GetGamePad();

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

    if (!start_transition && is_in)
    {
        if (is_next_select)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
        else if (is_next_game)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new GameScene));
    }

    // �A�C���X�C���̍Œ�������󂯕t���Ȃ�
    if (start_transition)return;

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

    if (gamePad.GetButtonDown() & gamePad.BTN_START)
    {
        is_next_select = true;
        CallTransition(true);
    }

    DebugImgui();
}

void FormationScene::UpdateUnit(float elapsedTime)
{
    // ���j�b�g�̍X�V
    {
        for (int i = 0; i < UNIT_MAX; i++)
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
    line_x.EasingValue(elapsedTime);
    line_add.EasingValue(elapsedTime);
    // �{�^����I�����Ă鎞
    if (select_button)
    {
        // �C�[�W���O���X�V
        button.ContinueEasing(elapsedTime);

        // ��ɍs������
        if (gamePad.GetAxisLY() >= 0.5f || gamePad.GetAxisRY() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_UP)
        {
            Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
            select_button = false;
            interval_timer = 0.0f;
            button.CallValueEasing(0.9f, button.value, EasingFunction::EasingType::InSine);
        }

        // B�{�^������������
        if (gamePad.GetButtonDown() & gamePad.BTN_B && all_unit_num >= 4)
        {
            Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A] = cont_num[gamePad.A];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B] = cont_num[gamePad.B];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X] = cont_num[gamePad.X];
            Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y] = cont_num[gamePad.Y];

            is_next_game = true;
            CallTransition(true);
        }
    }


    // �{�^����I�����Ă��Ȃ���
    if (!select_button)
    {
        // �S��������
        if (gamePad.GetButtonDown() & gamePad.BTN_LEFT_SHOULDER|| gamePad.GetButtonDown() & gamePad.BTN_RIGHT_SHOULDER)
        {
            bool is = false;
            for (int n = 0; n < UNIT_MAX; n++)
            {
                if (gltf_unit[n]->GetIsDissolve())
                {
                    is = true;
                }
            }
            if (is)return;
            for (int i = 0; i < UNIT_MAX; i++)
            {
                enable_units[i] = {};
                units_rotation[i] = {};
                units_position[i] = {};
            }
            for (int j = 0; j < 4; j++)
            {
                enable_controllers[j] = {};
                cont_num[j] = {};
                enable_lineblue[j] = false;
                lineblue_pos_x[j] = {};
            }
            all_unit_num = 0;
            select_button = false;

            for (int i = 0; i < UNIT_MAX; i++)
            {
                // �����x�̃N���A
                gltf_unit[i]->ClearThreshold();
            }
        }

        // ����I�����鎞
        if (gamePad.GetAxisLY() <= -1.0f || gamePad.GetAxisRY() <= -1.0f || gamePad.GetButtonDown() & gamePad.BTN_DOWN)
        {
            Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
            select_button = true;
            interval_timer = 0.0f;
            // �{�^���R���e�B�j���[���Ă�
            button.CallValueContinue(0.9f, 1.1f, button.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
        }


        // ���j�b�g�̑I��
        {
            // �E
            if (gamePad.GetAxisLX() >= 0.5f|| gamePad.GetAxisRX() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
            {
                // �C���^�[�o�����l�����ăJ�E���g�𓮂���
                interval_timer += elapsedTime;

                // ���j�b�g�T��I�𒆂܂ł͂���
                if (first_touch && choose_num < UNIT_MAX - 2)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num++;
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                    first_touch = false;
                }
                else if (choose_num < UNIT_MAX - 2 && interval_timer_max <= interval_timer)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num++;
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                }
                // ���j�b�g6�I�𒆂͎��̉�ʂɍs���̂ŕς���
                else if (first_touch && choose_num == UNIT_MAX - 2)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num++;
                    line_add.CallValueEasing(-210, line_add.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                    first_touch = false;
                }
                else if (choose_num == UNIT_MAX - 2 && interval_timer_max <= interval_timer)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num++;
                    line_add.CallValueEasing(-210, line_add.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                }
            }
            // ��
            else if (gamePad.GetAxisLX() <= -0.5f || gamePad.GetAxisRX() <= -0.5f || gamePad.GetButtonDown() & gamePad.BTN_LEFT)
            {
                // �C���^�[�o�����l�����ăJ�E���g�𓮂���
                interval_timer += elapsedTime;
                // ���̉�ʂɍs���Ă�̂�
                if (first_touch && choose_num == 1 && line_add.value < -200.0f)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num--;
                    line_add.CallValueEasing(0, line_add.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                    first_touch = false;
                }
                if (choose_num == 1 && interval_timer_max <= interval_timer && line_add.value < -200.0f)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num--;
                    line_add.CallValueEasing(0, line_add.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                }
                else if (first_touch && choose_num > 0)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num--;
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                    first_touch = false;
                }
                else if (choose_num > 0 && interval_timer_max <= interval_timer)
                {
                    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
                    choose_num--;
                    line_x.CallValueEasing((660 + 210 * choose_num), line_x.value, EasingFunction::EasingType::InSine, interval_timer_max);
                    interval_timer = 0.0f;
                }
            }
            else
            {
                first_touch = true;
            }
        }

        // ���j�b�g���S���I�΂�Ă��Ȃ���
        if (all_unit_num < 4 && choose_num < UNIT_MAX)
        {
            // ���j�b�g���I�ׂ��Ԃ̎�
            if (!enable_units[choose_num])
            {
                if(gamePad.GetButtonDown() & gamePad.BTN_A)SelectUnit(gamePad.A);
                else if(gamePad.GetButtonDown() & gamePad.BTN_B)SelectUnit(gamePad.B);
                else if(gamePad.GetButtonDown() & gamePad.BTN_X)SelectUnit(gamePad.X);
                else if(gamePad.GetButtonDown() & gamePad.BTN_Y)SelectUnit(gamePad.Y);
            }
        }
    }
}

void FormationScene::SelectUnit(int button_num)
{
    if (!enable_controllers[button_num])
    {
        // ���C�������낷
        lineblue_pos_x[all_unit_num] = (660 + 210 * choose_num);
        enable_lineblue[all_unit_num] = true;
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
        effect_handle[button_num] = effect->Play(position[button_num], effect_scale);
        gltf_unit[choose_num]->SetIsDissolve(true);
        Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::CHARACTER_SET, false);
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


    // �QD�̏������݊J�n
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

        // �w�i
        back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

        // ���j�b�g���̃��C��
        for (int i = 0; i < UNIT_MAX; i++)
        {
            line_1->Render(immediate_context, (660 + 210 * i)+ line_add.value, line_y.value, 210, SCREEN_HEIGHT);
        }
        // �ԃ��C��
        line_2->Render(immediate_context, line_x.value+ line_add.value, line_y.value, 210, SCREEN_HEIGHT);

        for (int l = 0; l < 4; l++)
        {
            if (enable_lineblue[l])line_blue->Render(immediate_context, lineblue_pos_x[l] + line_add.value, 0.0f, 210, SCREEN_HEIGHT);
        }
        for (int n = 0; n < UNIT_MAX; n++)
        {  
            // �QD���j�b�g
            unit_line[n]->Render(immediate_context, (660 + 210 * n) + line_add.value, line_y.value, 210, SCREEN_HEIGHT);
        }
        for (int n = 0; n < UNIT_MAX; n++)
        {
            if(enable_best[StageManager::Instance().GetStageLevel()][n])Best->Render(immediate_context, (660 + 210 * n) + line_add.value, line_y.value, 210, 90);
        }

        // �O�ɒu�����
        front->Render(immediate_context, 0, 0, 660, SCREEN_HEIGHT);

        for (int j = 0; j < 4; j++)
        {
            // �}�[�N�P
            if (j < status[choose_num][0])mark_1->Render(immediate_context, 100 + float(110 * j), 740, 100, 100);
            else mark_1_1->Render(immediate_context, 100 + float(110 * j), 740, 100, 100);
            // �}�[�N�Q
            if (j < status[choose_num][1])mark_2->Render(immediate_context, 100 + float(110 * j), 900, 100, 100);
            else mark_2_2->Render(immediate_context, 100 + float(110 * j), 900, 100, 100);
            // ��
            base->Render(immediate_context, (691.5f + 300 * j), 680, 300, 300);
        }

        if (line_add.value >= -1.0f)arrow->RenderRightDown(immediate_context, SCREEN_WIDTH, SCREEN_HEIGHT*0.5f + 50, 100, 100, 0.0f);

        unit_attack[choose_num]->Render(immediate_context,stage_ps.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,0.0f);
    }
    // �������ݏI��
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

        for (int i = 0; i < UNIT_MAX; i++)
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
#ifdef DEBUG_IMGUI
    BaseScene::DebugImgui();
    Camera::Instance().DrawDebug();
    gltf_unit[0]->DrawDebug();
#endif // DEBUG_IMGUI
}

void FormationScene::InitializeLight()
{
    ZeroMemory(&point_light[8], sizeof(pointLights) * 8);
    ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);
}
