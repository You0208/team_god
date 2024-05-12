#include "SelectScene.h"
#include "FormationScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "Game/Stage/StageManager.h"
#include "Game/Scene/LoadingScene.h"
#include "./high_resolution_timer.h"

void SelectScene::Initialize()
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
        // �|�C���g���C�g�E�X�|�b�g���C�g�̏����ʒu�ݒ�
        InitializeLight();
        // �e�N�X�`���̏�����
        InitializeMask();
    }

    // �e�N�X�`��&PS
    {    
        ui = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_select_UI.png");

        kakashi_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\kakashi1.png");
        kakashi_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\kakashi2.png");
        kakashi_3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\kakashi3.png");

        highlight_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage1.png");
        highlight_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage2.png");
        highlight_3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage3.png");

        select_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\stage_back.png");

        joro = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\jouro_UI_highlight.png");

        // �J��
        transition = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition.png");
        transition_line1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line1.png");
        transition_line2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line2.png");
        transition_line3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line3.png");

        transition_line1_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line1_2.png");
        transition_line2_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line2_2.png");
        transition_line3_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select\\transition_line3_2.png");

        create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_select_ps.cso", select_ps.GetAddressOf());
    }

    // �Q�[��
    {
        CallTransition(false);
        transition_angle.value = 140.0f;
        transition_line1_angle.value = 90.0f;
        transition_line2_angle.value = 110.0f;
        transition_line3_angle.value = 180.0f;
        transition_angle.value = 90.0f;
        transition_line1_angle_2.value = 110.0f;
        transition_line2_angle_2.value = 180.0f;
        transition_line3_angle_2.value =90.0f;

        switch_direction = false;
    }
}

void SelectScene::Finalize()
{
}

void SelectScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // �Q�[���p�b�h
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    TransitionMask(elapsed_time);

    // �C�[�W���O���X�V
    transition_angle.EasingValue(elapsed_time);
    transition_line1_angle.EasingValue(elapsed_time);
    transition_line2_angle.EasingValue(elapsed_time);
    transition_line3_angle.EasingValue(elapsed_time);
    transition_line1_angle_2.EasingValue(elapsed_time);
    transition_line2_angle_2.EasingValue(elapsed_time);
    transition_line3_angle_2.EasingValue(elapsed_time);

    if (switch_direction)
    {
        switch (direction_num)
        {
        case 0:
            // ���o���I�������
            if (!transition_angle.is_easing
                && !transition_line1_angle.is_easing
                && !transition_line2_angle.is_easing
                && !transition_line3_angle.is_easing)
            {
                // �p�x��������
                transition_angle.value = 90.0f;
                transition_line1_angle.value = 90.0f;
                transition_line2_angle.value = 110.0f;
                transition_line3_angle.value = 180.0f;

                // �C�[�W���O���Ă�
                transition_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.7f);
                transition_line1_angle_2.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.5f);
                transition_line2_angle_2.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.3f);
                transition_line3_angle_2.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.1f);

                // ���̃Z���N�g��ʂ�������
                if(add)world_num++;
                else if(minus)world_num--;
                stage_num = 0;

                // ���̉��o��
                direction_num++;
            }
            break;
        case 1:
            // ���o���I�������
            if (!transition_angle.is_easing
                && !transition_line1_angle_2.is_easing
                && !transition_line2_angle_2.is_easing
                && !transition_line3_angle_2.is_easing)
            {
                // �p�x��������
                transition_angle.value = 140.0f;
                transition_line1_angle_2.value = 110.0f;
                transition_line2_angle_2.value = 180.0f;
                transition_line3_angle_2.value = 90.0f;

                // ������
                direction_num = 0;
                switch_direction = false;
                add = false;
                minus = false;
            }
            break;
        }

        return;
    }

    // �X�e�[�W�I��
    if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
    {
        if (stage_num < 2)stage_num++;
    }
    else if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
    {
        if (stage_num > 0)stage_num--;
    }

    // ���[���h�I��
    if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT_SHOULDER)
    {
        if (world_num < 2)
        {
            switch_direction = true;
            add = true;
            transition_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.3f);
            transition_line1_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.1f);
            transition_line2_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.5f);
            transition_line3_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.7f);
            direction_num = 0;
        }
    }
    else if (gamePad.GetButtonDown() & gamePad.BTN_LEFT_SHOULDER)
    {
        if (world_num > 0)
        {
            switch_direction = true;
            minus = true;
            transition_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.3f);
            transition_line1_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.1f);
            transition_line2_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.5f);
            transition_line3_angle.CallValueEasing(0.0f, transition_angle.value, EasingFunction::EasingType::InSine, 1.7f);
            direction_num = 0;
        }
    }

    // �X�e�[�W����
    if (gamePad.GetButtonDown() & gamePad.BTN_A)
    {
        CallTransition(true, stage_mask_pos[stage_num]);
    }
    if (!start_transition && is_in)
    {
        StageManager::Instance().SetStageLevel(set_level[world_num][stage_num]);
        // ���̃V�[����
        Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new FormationScene));
    }
}

void SelectScene::Render(float elapsedTime)
{
    HRESULT hr{ S_OK };

    Camera& camera = Camera::Instance();
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

    camera.SetPerspectiveFov(immediate_context);

    // �`��̐ݒ�
    SetUpRendering();

    // �X�e�[�g�̐ݒ�
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    // 2D
    {
        // �w�i
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

        if (switch_direction)
        {

            switch (direction_num)
            {
            case 0:
                transition->RenderRightDown(immediate_context, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_angle.value);
                transition_line1->RenderRightDown(immediate_context, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line1_angle.value);
                transition_line2->RenderRightDown(immediate_context, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line2_angle.value);
                transition_line2->RenderRightDown(immediate_context, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line3_angle.value);
                break;

            case 1:
                transition->RenderRightUp(immediate_context, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 2.0, transition_angle.value);
                transition_line1_2->RenderRightUp(immediate_context, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line1_angle_2.value);
                transition_line2_2->RenderRightUp(immediate_context, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line2_angle_2.value);
                transition_line2_2->RenderRightUp(immediate_context, SCREEN_WIDTH + 100, SCREEN_HEIGHT + 100, SCREEN_WIDTH * 1.5, SCREEN_HEIGHT * 1.5, transition_line3_angle_2.value);
                break;

            }
        }
    }

    // �}�X�N
    RenderTransitionMask(elapsedTime);
}

void SelectScene::DebugImgui()
{
}

