#include "Seed.h"
#include "Lemur/Input/Input.h"
static Seed* instance = nullptr;

Seed::Seed()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model_seed = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    height = 1.0f;
    model_seed->transform.SetPositionZ(-5.0f);
}

Seed::~Seed()
{
}

void Seed::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // モデルアニメーション更新
    model_seed->UpdateAnimation(elapsedTime);

    // Imgui
    DrawDebugGUI();
}

void Seed::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model_seed->Render(scale, replaced_pixel_shader);
}

void Seed::DrawDebugGUI()
{
    ImGui::Begin("Seed");


    ImGui::End();
}

Seed& Seed::Instance()
{
    return *instance;
}