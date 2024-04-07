#include "Seed.h"
#include "Lemur/Input/Input.h"
#include "UnitManager.h"
#include "Unit_A.h"
#include "Unit_B.h"
#include "Unit_C.h"
#include "Unit_D.h"
#include "Unit_E.h"
#include "Unit_F.h"

Seed::Seed()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\seed.fbx");
    scaleFactor = 0.1f;
    radius = 0.3f;
    height = 0.3f;

}

Seed::~Seed()
{
}

void Seed::Update(float elapsedTime)
{
    timer += elapsedTime;

#if 0
    // �킪���肵����
    if (born)
    {
        switch (category)
        {
        case 0:
        {
            Unit_A* unit = new Unit_A;
            unit->SetPosition(position);// ���W��n��
            UnitManager::Instance().Register(unit);
            break;
        }
        case 1:
        {
            Unit_B* unit = new Unit_B;
            unit->SetPosition(position);// ���W��n��
            UnitManager::Instance().Register(unit);
            break;
        }
        case 2:
        {
            Unit_C* unit = new Unit_C;
            unit->SetPosition(position);// ���W��n��
            UnitManager::Instance().Register(unit);
            break;
        }
        case 3:
        {
            Unit_D* unit = new Unit_D;
            unit->SetPosition(position);// ���W��n��
            UnitManager::Instance().Register(unit);
            break;
        }
        case 4:
        {
            Unit_E* unit = new Unit_E;
            unit->SetPosition(position);// ���W��n��
            UnitManager::Instance().Register(unit);
            break;
        }
        case 5:
        {
            Unit_F* unit = new Unit_F;
            unit->SetPosition(position);// ���W��n��
            UnitManager::Instance().Register(unit);
            break;
        }
        }

        death = true;// �������
    }
#else
    if (born)
    {
        Unit* unit = nullptr;

        switch (category)
        {
        case 0:
            unit = new Unit_A;
            break;
        case 1:
            unit = new Unit_B;
            break;
        case 2:
            unit = new Unit_C;
            break;
        case 3:
            unit = new Unit_D;
            break;
        case 4:
            unit = new Unit_E;
            break;
        case 5:
            unit = new Unit_F;
            break;
        }

        if (unit != nullptr)
        {
            unit->SetPosition(position);
            // ���s�͓K���ɐݒ�
            Rect square = {
                {position.x - radius,position.z + 100.0f},
                {position.x + radius,position.z - radius}
            };
            unit->SetSquare(square);
            unit->category = category;
            UnitManager::Instance().Register(unit);
        }

        death = true;// �������
    }
#endif
    // �X�P�[���X�V
    UpdateScale();

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);

    // �s��X�V����
    UpdateTransform();

    // Imgui
    DrawDebugGUI();
}

void Seed::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Seed::DrawDebugGUI()
{
    ImGui::Begin("Seed");

    ImGui::DragInt("number", &number);
    ImGui::DragFloat("timer", &timer);
    ImGui::Checkbox("born", &born);

    ImGui::End();
}

void Seed::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 1,0,0,1 });
}
