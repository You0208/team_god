#include "Unit_F.h"

Unit_F::Unit_F()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Unit_F::~Unit_F()
{
}

void Unit_F::Update(float elapsedTime)
{
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);
}

void Unit_F::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_F::UpdateAttackState(float elapsed_time)
{
}

void Unit_F::DrawDebugGUI()
{
    model->DrawDebug("Unit_C");
}

void Unit_F::DrawDebugPrimitive()
{
}

void Unit_F::TransitionIdleState()
{
}

void Unit_F::UpdateIdleState(float elapsed_time)
{
}

void Unit_F::TransitionAttackState()
{
}
