#include "Seed.h"
#include "Lemur/Input/Input.h"
#include "UnitManager.h"
#include "SeedManager.h"
#include "Unit_ABC.h"
#include "Unit_DEF.h"
#include "Unit_HIJ.h"

#include <random>

Seed::Seed()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\seed.fbx");
    scaleFactor = 0.1f;
    radius = 0.3f;
    height = 0.3f;
    model->PlayAnimation(0, false);
}

Seed::~Seed()
{
}

void Seed::Update(float elapsedTime)
{
    // ��Ɏ��s����Ă���
    {
        // �X�P�[���X�V
        UpdateScale();

        // ���͏����X�V
        UpdateVelocity(elapsedTime);

        // ���f���A�j���[�V�����X�V
        model->UpdateAnimation(elapsedTime);

        // �s��X�V����
        UpdateTransform();

        // Imgui
        DrawDebugGUI();
    }

    if (is_direction)// ��̉��o
    {
        throwDirection();
    }
    else if (is_dis_direction)// �Ԃ����ď������̉��o
    {
        DisDirection();
    }
    else
    {
        timer += elapsedTime;

        // ����t���O�������Ă�����
        if (born)
        {
            Unit* unit = nullptr;

            // ���j�b�g�̎�ނɂ���Ĕ����̃��j�b�g��ύX
            switch (category)
            {
            case 0:
                unit = new Unit_A;
                UnitManager::Instance().SetUpUnit_A(unit);
                break;
            case 1:
                unit = new Unit_B;
                UnitManager::Instance().SetUpUnit_B(unit);
                break;
            case 2:
                unit = new Unit_C;
                UnitManager::Instance().SetUpUnit_C(unit);
                break;
            case 3:
                unit = new Unit_D;
                UnitManager::Instance().SetUpUnit_D(unit);
                break;
            case 4:
                unit = new Unit_E;
                UnitManager::Instance().SetUpUnit_E(unit);
                break;
            case 5:
                unit = new Unit_F;
                UnitManager::Instance().SetUpUnit_F(unit);
                break;
            case 6:
                unit = new Unit_H;
                UnitManager::Instance().SetUpUnit_H(unit);
                break;
            case 7:
                unit = new Unit_I;
                UnitManager::Instance().SetUpUnit_I(unit);
                break;
            case 8:
                unit = new Unit_J;
                UnitManager::Instance().SetUpUnit_J(unit);
                break;
            }

            // ���j�b�g�����܂ꂽ��
            if (unit != nullptr)
            {
                //����ʒu
                unit->SetPosition(position);
                // ���j�b�g���̎l�p�i���s�͓K���ɐݒ�j
                Rect square = {
                    {position.x - radius,position.z + 100.0f},
                    {position.x + radius,position.z - radius},
                };
                unit->SetSquare(square);
                // �J�e�S���[���Z�b�g
                unit->SetCategory(category);
                // �p�����X�V���Ă���
                unit->UpdateTransform();
                // Manager�ɃZ�b�g
                UnitManager::Instance().Register(unit);
                // �������
                death = true;
            }
        }

        // ��΂����Ă̎�̈ʒu����
        if (!decision_pos)DecisionPos();
    }
}

void Seed::throwDirection()
{
    if (position.z>= distination_position.z)
    {
        velocity.z = 0;
        is_direction = false;
    }
    else
    {
        velocity.z = throw_speed;
    }
}

void Seed::DisDirection()
{
    if (position.z >= distination_position.z)
    {
        velocity.z = 0;
        death = true;
        is_dis_direction = false;
    }
    else
    {
        velocity.z = throw_speed;
    }
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

void Seed::DecisionPos()
{
    SeedManager& seedManager = SeedManager::Instance();

    // �S�Ă̎�Ƒ�������ŏՓ˔���
    int seedCount = seedManager.GetSeedCount();
    for (int i = 0; i < seedCount; ++i)
    {
        Seed* seed = seedManager.GetSeed(i);
        if (!seed->decision_pos)continue;

        if (Collision::IntersectCircleVsCircle(
            { position.x,position.z }, radius,
            { seed->GetPosition().x, seed->GetPosition().z },
            seed->GetRadius()
        ))
        {
            if (overlap_num == 0)
            {
                outPosition.x = MoveInRandomDirection({ position.x,position.z }, 1.0f).x;
                outPosition.y = MoveInRandomDirection({ position.x,position.z }, 1.0f).y;
                overlap_num++;// �d�Ȃ����񐔂����Z
            }

            // �����o���ꂽ�n�_�ɂ܂��킪���邩�m�F
            for (int j = 0; j < seedCount; ++j)
            {
                if (j == i)continue;

                Seed* seed2 = seedManager.GetSeed(j);
                if (!seed2->decision_pos)continue;

                if (Collision::IntersectCircleVsCircle(
                    outPosition, radius,
                    { seed2->GetPosition().x, seed2->GetPosition().z },
                    seed2->GetRadius()
                ))
                {
                    // �d�Ȃ����񐔂����Z
                    overlap_num++;
                }
            }

        }
    }
    // ������Ȃ�������
    if (overlap_num == 0)
    {
        outPosition = {};
        decision_pos = true;// ���������ʒu����t���O��ON
    }
    else if (overlap_num == 1)// �d�Ȃ����񐔂��P��Ȃ�
    {
        // �����o���ʒu�ɂ��炵
        position.x = outPosition.x;
        position.z = outPosition.y;
        decision_pos = true;// �ʒu����t���O��ON
        outPosition = {};
    }
    else if (overlap_num >= 2)// �d�Ȃ����񐔂��Q��ȏ�Ȃ�
    {
        outPosition = {};
        death = true;// ���j��
    }
}

DirectX::XMFLOAT2 Seed::GenerateRandomDirection()
{
    // ����������̏���
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);

    // �����_���Ȋp�x���v�Z
    float angle = dis(gen) * DirectX::XM_2PI;

    // �x�N�g���𐶐�
    DirectX::XMFLOAT2 direction;
    direction.x = cos(angle);
    direction.y = sin(angle);

    return direction;
}

DirectX::XMFLOAT2 Seed::MoveInRandomDirection(const DirectX::XMFLOAT2& position, float distance)
{
    // �����_���ȕ����x�N�g���𐶐�
    DirectX::XMFLOAT2 direction = GenerateRandomDirection();

    // �������|���ĐV�������W���v�Z
    DirectX::XMFLOAT2 newPosition;
    newPosition.x = position.x + direction.x * distance;
    newPosition.y = position.y + direction.y * distance;

    return newPosition;
}
