#include "TitleScene.h"
#include "SelectScene.h"
#include "LoadingScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"

void TitleScene::Initialize()
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
        title_left = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_left.png");
        title_right = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_right.png");
        title_back = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Title_scene.png");
        title_logo = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title\\Titlelogo.png");
    }

    // �Q�[��
    {
        left_scale.CallValueEasing(1.2f, left_scale.value, EasingFunction::EasingType::OutSine, 0.8f);
        Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::TITLE, true);
        //CallTransition(false);
    }
}

void TitleScene::Finalize()
{
    // BGM�I��
    // BGM
    Lemur::Audio::AudioManager::Instance().StopAllBGM();
    Lemur::Audio::AudioManager::Instance().StopAllSE();
}

void TitleScene::Update(HWND hwnd, float elapsedTime)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // �Q�[���p�b�h
    GamePad& gamePad = Input::Instance().GetGamePad();
    Camera& camera = Camera::Instance();
    camera.Update(elapsedTime);

    TransitionMask(elapsedTime);

    if (start_transition)return;

    // �C�[�W���O�X�V
    {
        left_scale.EasingValue(elapsedTime);
        right_scale.EasingValue(elapsedTime);
        logo_scale.EasingValue(elapsedTime);
    }

    switch (direction_num)
    {
    case LEFT:
        if (!left_scale.is_easing)
        {
            left_scale.CallValueEasing(1.0f, left_scale.value, EasingFunction::EasingType::InSine, 0.3f);
            right_scale.CallValueEasing(1.2f, right_scale.value, EasingFunction::EasingType::OutSine, 0.8f);
            direction_num = RIGHT;
        }
        break;
    case RIGHT:

        if (!right_scale.is_easing)
        {
            right_scale.CallValueEasing(1.0f, right_scale.value, EasingFunction::EasingType::InSine, 0.3f);
            direction_num = RIGHT_2;
        }
        break;
    case RIGHT_2:

        if (!right_scale.is_easing)
        {
            logo_scale.CallValueEasing(1.2f, logo_scale.value, EasingFunction::EasingType::InSine, 0.5f);
            direction_num = LOGO;
        }
        break;
    case LOGO:
        if (!logo_scale.is_easing)
        {
            logo_scale.CallValueEasing(1.0f, logo_scale.value, EasingFunction::EasingType::OutBounce, 0.5f);
            direction_num = LOGO_2;
        }
        break;
    case LOGO_2:
        if (!logo_scale.is_easing)
        {
            is_direction = false;
        }
        break;
    }

    if (is_direction)return;

    if (gamePad.GetButton() & gamePad.BTN_B)
    {
        Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
        if (!is_in)CallTransition(true);
    }

    // �A�C���X�C�����Ă�
    if (!start_transition && is_in)        Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
    DebugImgui();
}

void TitleScene::Render(float elapsedTime)
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
    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    title_back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    //title_left->RenderCenter(immediate_context, left_x.value,left_y.value, 1010 * left_scale.value, SCREEN_HEIGHT * left_scale.value);
    title_left->RenderLeftDown(immediate_context, 0, 1080.0f, 1010 * left_scale.value, SCREEN_HEIGHT * left_scale.value, 0.0f);
    title_right->RenderRightDown(immediate_context, 1920.0f, 1080.0f, 1010 * right_scale.value, SCREEN_HEIGHT * right_scale.value, 0.0f);
    title_logo->RenderCenter(immediate_context, 1000, 190, 1100 * logo_scale.value, 300 * logo_scale.value);

    // �}�X�N
    RenderTransitionMask(elapsedTime);
}

void TitleScene::DebugImgui()
{
}
