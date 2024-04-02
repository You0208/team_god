#include "Unit_A.h"
#include "SeedManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    radius = 1.0f;
    height = 0.5f;

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
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
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 1,0,0,1 });
}

