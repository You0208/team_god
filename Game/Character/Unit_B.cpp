#include "Unit_B.h"

Unit_B::Unit_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

Unit_B::~Unit_B()
{
}

void Unit_B::Update(float elapsedTime)
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

void Unit_B::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_B::DrawDebugGUI()
{
    model->DrawDebug("Unit_B");
}

void Unit_B::DrawDebugPrimitive()
{
}
