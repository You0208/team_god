#include "Unit_A.h"
#include "SeedManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_radius = 1.0f;
    radius = 0.3f;
    height = 0.5f;
    dec_pos = 1.0f;

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);

    DrawDebugGUI();
}

void Unit_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_A::DrawDebugGUI()
{
    //TODO 同じ種類複製するとやばい
    model->DrawDebug("Unit_A");
}

void Unit_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_radius, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

