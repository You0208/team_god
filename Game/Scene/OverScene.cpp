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

    // �e�N�X�`��
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

    // �Q�[��
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
    // BGM�I��
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void OverScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // �Q�[���p�b�h
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    TransitionMask(elapsed_time);

    // �A�C���X�A�E�g���I����Ă����x�̂ݎ��s
    if (!start_transition && once_only)
    {
        switch (direction_num)// ���݂̉��o�����Ԃ�
        {
        case 0:// �ォ�珙�X�ɕ����������Ă���
            timer += elapsed_time;// �^�C�}�[

            // ���݂̃J�E���g�̃C�[�W���O�𓮂���
            text_y[count_num].CallValueEasing(160.0f, text_y[count_num].value, EasingFunction::EasingType::OutSine, 0.15f);

            // �C���^�[�o�������Ԃ����ĂΎ���
            if (timer >= interval)
            {
                count_num++;// �J�E���g��i�߂�
                timer = 0.0f;// �^�C�}�[��������
                // �J�E���g���K��l�𒴂��Ă�����
                if (count_num >= 8)
                {
                    timer = 0;// �^�C�}�[�̏�����
                    count_num = 0;// �J�E���g�̏�����
                    interval = 0.05f;// ���񉉏o�̂��߃C���^�[�o�����Đݒ�
                    direction_num++;// ���o�����̒i�K
                }
            }
            anim_stop = false;
            break;
        case 1:
            timer += elapsed_time;// �^�C�}�[

            // ���݂̃J�E���g�̃C�[�W���O�𓮂���
            text_angle[random_num[count_num]].CallValueContinue(-10.0f, 10.0f, text_angle[random_num[count_num]].value, EasingFunction::EasingType::InOutSine, EasingFunction::EasingType::InOutSine);

            // �C���^�[�o�������Ԃ����ĂΎ���
            if (timer >= interval)
            {
                count_num++;// �J�E���g��i�߂�
                timer = 0.0f;// �^�C�}�[��������                
                if (count_num >= 8)// �J�E���g���K��l�𒴂��Ă�����
                {
                    timer = 0;// �^�C�}�[�̏�����
                    count_num = 0;// �J�E���g�̏�����
                    // �Z���N�g�{�^���𓮂����n�߂�
                    select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                    once_only = false;// �t���O��OFF
                }

            }
            break;
        }

        // �e�L�X�g�̃C�[�W���O�X�V
        for (int i = 0; i < 8; i++)
        {
            text_y[i].EasingValue(elapsed_time);
            text_angle[i].ContinueEasing(elapsed_time);
        }

        return;
    }
    else if (start_transition && once_only) return;// ���o���̓{�^���𐄂��Ȃ��悤��
    
    // �ǂ̃{�^����I�����Ă��邩
    switch (select_num)
    {
    case Button::Select :
        if (!is_continue)// �I������Ă��Ȃ���
        {
            // �E��I�񂾂Ƃ�
            if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
            {
                again.CallValueEasing(1.1f, again.value, EasingFunction::EasingType::InSine);
                select.CallValueEasing(1.0f, select.value, EasingFunction::EasingType::InSine);
                again.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Again;
                break;
            }
            // �C�[�W���O���X�V
            select.EasingValue(elapsed_time);
            again.EasingValue(elapsed_time);
            select.ContinueEasing(elapsed_time);

            // �I��������
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // �A�C���X�C�����Ă�
        if (continue_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // ���̃V�[����
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
        }
        break;

    case Button::Again:

        if (!is_continue)// �I������Ă��Ȃ���
        {
            if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)// ���I�������
            {
                select.CallValueEasing(1.1f, select.value, EasingFunction::EasingType::InSine);
                again.CallValueEasing(1.0f, again.value, EasingFunction::EasingType::InSine);
                select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Select;
                break;
            }

            // �C�[�W���O���X�V
            again.EasingValue(elapsed_time);
            select.EasingValue(elapsed_time);
            again.ContinueEasing(elapsed_time);

            // �I��������
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // �A�C���X�C�����Ă�
        if (continue_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // ���̃V�[����
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new FormationScene));
        }
        break;
    }

    // ���o�O�ŃC�[�W���O���X�V
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

    // �`��̐ݒ�
    SetUpRendering();
    SetUpConstantBuffer();

    // �X�e�[�g�̐ݒ�
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    // 2D��{
    {
        // ��{�w�i
        if (!is_continue)game_over_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, false, anim_stop);
        // �R���e�B�j���[�w�i
        else if (is_continue)continue_back->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, false);

        // UI�̕���
        for (int i = 0; i < 8; i++)
        {
            ui_text[i]->RenderCenter(immediate_context, 530 + float(130 * i), text_y[i].value, 150, 150, text_angle[i].value);
        }

        // �{�^��
        stageselect_text->RenderCenter(immediate_context, 550, 950, 500 * select.value, 200 * select.value);
        again_text->RenderCenter(immediate_context, 1430, 950, 500 * again.value, 200 * again.value);
    }
    // �}�X�N
    RenderTransitionMask(elapsedTime);
}

void OverScene::DebugImgui()
{
    ImGui::Begin("ImGUI");
    ImGui::End();
}
