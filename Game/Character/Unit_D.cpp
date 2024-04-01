#include "Unit_D.h"

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Unit_D::~Unit_D()
{
}

void Unit_D::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新処理
    model->UpdateAnimation(elapsedTime);
}

void Unit_D::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_D::DrawDebugGUI()
{
    model->DrawDebug("Unit_C");
}

void Unit_D::DrawDebugPrimitive()
{
}
