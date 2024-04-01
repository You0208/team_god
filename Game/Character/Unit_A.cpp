#include "Unit_A.h"
#include "Lemur/Input/Input.h"

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

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
}

void Unit_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_A::DrawDebugGUI()
{
    model->DrawDebug("Unit_A");
}

void Unit_A::DrawDebugPrimitive()
{
}

