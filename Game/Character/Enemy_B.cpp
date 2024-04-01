#include "Enemy_B.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_B::Enemy_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    radius = 0.5f;
    height = 1.0f;

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

void Enemy_B::Update(float elapsedTime)
{
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);
}

void Enemy_B::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}
