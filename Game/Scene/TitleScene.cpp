#include "TitleScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "./high_resolution_timer.h"

#include "SelectScene.h"
void TitleScene::Initialize()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    dissolve_value = 0.0f;

    // シェーダー関連
    {
        InitializeState();
        HRESULT hr{ S_OK };
        // シーン定数バッファオブジェクトを生成
        {
            D3D11_BUFFER_DESC buffer_desc{};
            buffer_desc.Usage = D3D11_USAGE_DEFAULT;// 読み取りおよび書き込みがどのように行われるか
            buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;// バッファがパイプラインにどのようにバインド(結びつけ)されるかを特定
            buffer_desc.CPUAccessFlags = 0;// CPU アクセス フラグ（CPU アクセスが必要ない場合は 0）
            buffer_desc.MiscFlags = 0;// その他のフラグ（未使用に場合は0）
            buffer_desc.StructureByteStride = 0;//バッファが構造化バッファを表す場合の、バッファ構造内の各要素のサイズ
            {
                // オプション
                buffer_desc.ByteWidth = sizeof(option_constants);
                hr = graphics.GetDevice()->CreateBuffer(&buffer_desc, nullptr, option_constant_buffer.GetAddressOf());
                _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
            }
        }

        LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", mask_texture.GetAddressOf(), graphics.GetTexture2D());//TODO

    }

    // ゲーム関連
    {
        title = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Title.png");
    }
}

void TitleScene::Finalize()
{
}

void TitleScene::Update(HWND hwnd, float elapsedTime)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // ゲームパッド
    GamePad& gamePad = Input::Instance().GetGamePad();

    if (gamePad.GetButton() & gamePad.BTN_B)
    {
        Lemur::Scene::SceneManager::Instance().ChangeScene(new SelectScene);
    }
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

    SetUpRendering();

    // ノイズ
    immediate_context->PSSetShaderResources(9/*slot(1番にセットします)*/, 1, mask_texture.GetAddressOf());//TODO

    // オプション
    option_constant.parameters.x = dissolve_value;
    immediate_context->UpdateSubresource(option_constant_buffer.Get(), 0, 0, &option_constant, 0, 0);
    immediate_context->VSSetConstantBuffers(3, 1, option_constant_buffer.GetAddressOf());
    immediate_context->PSSetConstantBuffers(3, 1, option_constant_buffer.GetAddressOf());

    immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
    immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
    immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

    title->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void TitleScene::DebugImgui()
{
    ImGui::Begin("ImGUI");

    ImGui::SliderFloat("dissolve", &dissolve_value, 0.0f, 1.0f);

    ImGui::End();
}
