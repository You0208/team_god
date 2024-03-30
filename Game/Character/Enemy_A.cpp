#include "Enemy_A.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model_enemy_A = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    height = 1.0f;

    // とりあえずアニメーション
    model_enemy_A->PlayAnimation(0, true);
}

void Enemy_A::Update(float elapsedTime)
{
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // モデルアニメーション更新
    model_enemy_A->UpdateAnimation(elapsedTime);
}

void Enemy_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model_enemy_A->Render(scale, replaced_pixel_shader);
}
