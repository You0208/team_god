#include "Unit_E.h"

Unit_E::Unit_E()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Unit_E::~Unit_E()
{
}

void Unit_E::Update(float elapsedTime)
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

void Unit_E::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Unit_E::DrawDebugGUI()
{
    model->DrawDebug("Unit_C");
}

void Unit_E::DrawDebugPrimitive()
{
}