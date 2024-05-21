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
    // �Q�[��
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
    // BGM�I��
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void ClearScene::Update(HWND hwnd, float elapsed_time)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // �Q�[���p�b�h
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsed_time);

    // �}�X�N�̍X�V
    TransitionMask(elapsed_time);


    // �C�[�W���O�̍X�V
    star1_scale.EasingValue(elapsed_time);
    star2_scale.EasingValue(elapsed_time);
    star3_scale.EasingValue(elapsed_time);

    // �A�C���X�C���̍Œ�������󂯕t���Ȃ�
    if (start_transition && is_in)return;

    // �X�^�[�̉��o
    if (is_star_1 || is_star_2 || is_star_3)
    {
        if (!start_transition)
        {
            timer += elapsed_time;
        }

        // ���o�p�����_��
        auto star_direction = [&](EasingFunction& scale, int& direction_num, bool& is_star) {

            switch (direction_num)
            {
            case 0:
                // �A�C���X�A�E�g���I����Ă����x�̂ݎ��s
                if (!start_transition)
                {
                    // �傫������
                    scale.CallValueEasing(1.4f, scale.value, EasingFunction::EasingType::OutSine, 0.4f);
                    direction_num++;
                }
                break;
            case 1:
                // �傫���Ȃ�C�[�W���O���I�������
                if (!scale.is_easing)
                {
                    // ����������
                    scale.CallValueEasing(0.7f, scale.value, EasingFunction::EasingType::InSine, 0.2f);
                    direction_num++;
                }
                break;
            case 2:
                // �������Ȃ�����
                if (!scale.is_easing)
                {
                    // �傫����߂�
                    scale.CallValueEasing(1.0f, scale.value, EasingFunction::EasingType::OutSine, 0.1f);
                    // ���������ăt���O��OFF
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
        // ���փ{�^���𓮂����n�߂�
        next.CallValueContinue(1.0f, 1.1f, next.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
        once_only = false;
    }


    // �ǂ̃{�^����I�����Ă��邩
    switch (select_num)
    {
    case Button::Next:// ����
        if (!is_continue)// �I������Ă��Ȃ���
        {
            // ����I�񂾂Ƃ�
            if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
            {
                next.CallValueEasing(1.0f, next.value, EasingFunction::EasingType::InSine);
                select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Select;
                break;
            }

            // �C�[�W���O���X�V
            select.EasingValue(elapsed_time);
            again.EasingValue(elapsed_time);
            next.ContinueEasing(elapsed_time);

            // �I��������
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // �A�C���X�C�����Ă�
        if (clear_start_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // �X�e�[�W�̃��x�������Z
            StageManager::Instance().AddStageLevel();
            // ���̃V�[����
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new FormationScene));
        }
        break;

    case Button::Select:
        if (!is_continue)// �I������Ă��Ȃ���
        {
            // ����I�񂾂Ƃ�
            if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
            {
                select.CallValueEasing(1.0f, select.value, EasingFunction::EasingType::InSine);
                again.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Again;
                break;
            }
            // ���I�񂾂Ƃ�
            else if (gamePad.GetButtonDown() & gamePad.BTN_UP)
            {
                select.CallValueEasing(1.0f, select.value, EasingFunction::EasingType::InSine);
                next.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Next;
                break;
            }

            // �C�[�W���O���X�V
            next.EasingValue(elapsed_time);
            again.EasingValue(elapsed_time);
            select.ContinueEasing(elapsed_time);

            // �I��������
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // �A�C���X�C�����Ă�
        if (clear_start_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // ���̃V�[����
            Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
        }
        break;
    case Button::Again:

        if (!is_continue)// �I������Ă��Ȃ���
        {
            if (gamePad.GetButtonDown() & gamePad.BTN_UP)// ��I�������
            {
                again.CallValueEasing(1.0f, again.value, EasingFunction::EasingType::InSine);
                select.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
                select_num = Button::Select;
                break;
            }

            // �C�[�W���O���X�V
            next.EasingValue(elapsed_time);
            select.EasingValue(elapsed_time);
            again.ContinueEasing(elapsed_time);

            // �I��������
            if (gamePad.GetButtonDown() & gamePad.BTN_B)
            {
                is_continue = true;
            }
        }
        // �A�C���X�C�����Ă�
        if (clear_start_back->is_anime_end && is_continue && !is_in)  CallTransition(true);
        if (!start_transition && is_in)
        {
            // ���̃V�[����
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

    // �`��̐ݒ�
    SetUpRendering();
    SetUpConstantBuffer();

    // �X�e�[�g�̐ݒ�
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    // 2D��{
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
    
        // �{�^��
        next_text->RenderCenter(immediate_context, 1616, 573, 500 * next.value, 200 * next.value);
        select_text->RenderCenter(immediate_context, 1616, 756, 500 * select.value, 200 * select.value);
        again_text->RenderCenter(immediate_context, 1616, 940, 500 * again.value, 200 * again.value);
    }
    // �}�X�N
    RenderTransitionMask(elapsedTime);
}

void ClearScene::DebugImgui()
{
}
