#include "SelectScene.h"
#include "FormationScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"

void SelectScene::Initialize()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    dissolve_value = 0.0f;

    // �V�F�[�_�[�֘A
    {
        InitializeState();
        HRESULT hr{ S_OK };
        // �V�[���萔�o�b�t�@�I�u�W�F�N�g�𐶐�
        {
            D3D11_BUFFER_DESC buffer_desc{};
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;// �ǂݎ�肨��я������݂��ǂ̂悤�ɍs���邩
            buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// �o�b�t�@���p�C�v���C���ɂǂ̂悤�Ƀo�C���h(���т�)����邩�����
            buffer_desc.CPUAccessFlags = 0;// CPU �A�N�Z�X �t���O�iCPU �A�N�Z�X���K�v�Ȃ��ꍇ�� 0�j
            buffer_desc.MiscFlags = 0;// ���̑��̃t���O�i���g�p�ɏꍇ��0�j
            buffer_desc.StructureByteStride = 0;//�o�b�t�@���\�����o�b�t�@��\���ꍇ�́A�o�b�t�@�\�����̊e�v�f�̃T�C�Y
            {
                // �I�v�V����
                buffer_desc.ByteWidth = sizeof(option_constants);
                hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, option_constant_buffer.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }

        load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", mask_texture.GetAddressOf(), graphics.GetTexture2D());//TODO

    }

    // �Q�[���֘A
    {
        select_1 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select1.png");
        select_2 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select2.png");
        select_3 = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Select3.png");
    }
}

void SelectScene::Finalize()
{
}

void SelectScene::Update(HWND hwnd, float elapsedTime)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // �Q�[���p�b�h
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButtonDown() & gamePad.BTN_RIGHT)
    {
        if (stage_num < 2)stage_num += 1;
    }
    else if (gamePad.GetButtonDown() & gamePad.BTN_LEFT)
    {
        if (stage_num > 0)stage_num -= 1;
    }


    if (gamePad.GetButtonDown() & gamePad.BTN_B)
    {
        Lemur::Scene::SceneManager::Instance().SetStageNum(stage_num);
        Lemur::Scene::SceneManager::Instance().ChangeScene(new FormationScene);
    }

    DebugImgui();
}

void SelectScene::Render(float elapsedTime)
{
    HRESULT hr{ S_OK };

    Camera& camera = Camera::Instance();
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
    ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
    ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

    SetUpRendering();

    // �m�C�Y
    immediate_context->PSSetShaderResources(9/*slot(1�ԂɃZ�b�g���܂�)*/, 1, mask_texture.GetAddressOf());//TODO

    // �I�v�V����
    option_constant.parameters.x = dissolve_value;
    immediate_context->UpdateSubresource(option_constant_buffer.Get(), 0, 0, &option_constant, 0, 0);
    immediate_context->VSSetConstantBuffers(3, 1, option_constant_buffer.GetAddressOf());
    immediate_context->PSSetConstantBuffers(3, 1, option_constant_buffer.GetAddressOf());

    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    if (stage_num == 0) select_1->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (stage_num == 1) select_2->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (stage_num == 2) select_3->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);


}

void SelectScene::DebugImgui()
{
    ImGui::Begin("ImGUI");

    ImGui::Text("stage_num : %d", &stage_num);
    ImGui::SliderFloat("dissolve", &dissolve_value, 0.0f, 1.0f);

    ImGui::End();
}

