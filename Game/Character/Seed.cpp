#include "Seed.h"
#include "Lemur/Input/Input.h"
#include "UnitManager.h"
#include "SeedManager.h"
#include "Unit_ABC.h"
#include "Unit_DEF.h"
#include "Unit_HIJ.h"
#include "../Stage/Fence.h"

#include "Lemur/Effekseer/EffekseerManager.h"

#include <random>

Seed::Seed()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\seed.fbx");
    error_effect = new Effect(".\\resources\\Effect\\SEED_ERROR\\SEED_ERROR.efk");

    scaleFactor = 0.1f;

    //TODO�@����
    radius = 0.28f;
    height = 0.28f;
    rotation.y = DirectX::XMConvertToRadians(90.0f);

    //TODO ���ˁ@
    effect_size = 0.3f;

    // ��
    PlayAnimation(Animation_Index::Throw, true);
}

Seed::~Seed()
{
    //error_effect->Stop(error_effect_handle);
    delete error_effect;
    //EffectManager::Instance().Update(0.01f);
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
        UpdateAnimation(elapsedTime);

        // �s��X�V����
        UpdateTransform();

        // Imgui����
        //DrawDebugGUI();
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
            if (!unit) {
                switch (category)
                {
                case UnitManager::UNIT_INDEX::Chili:
                    unit = new Chili();
                    break;
                case UnitManager::UNIT_INDEX::Shishito:
                    unit = new Shishito();
                    break;
                case UnitManager::UNIT_INDEX::GreenPumpkin:
                    unit = new GreenPumpkin();
                    break;
                case UnitManager::UNIT_INDEX::OrangePumpkin:
                    unit = new OrangePumpkin();
                    break;
                case UnitManager::UNIT_INDEX::Broccoli:
                    unit = new Broccoli();
                    break;
                case UnitManager::UNIT_INDEX::Cauliflower:
                    unit = new Cauliflower();
                    break;
                case UnitManager::UNIT_INDEX::J:
                    unit = new Unit_J();
                    break;
                default:
                    unit = new Chili();
                    break;
                }
                UnitManager::Instance().SetUpUnit(unit, category);
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
                // �ݒu�G�t�F�N�g���Đ�
                unit->PlaySetEffect(unit->GetPosition(), unit->GetSetEffectSize());

                // �K���Ȋp�x�ɂ���
                unit->SetAngle({ 0.0f,DirectX::XMConvertToRadians(rand() % 360),0.0f });

                // �p�����X�V���Ă���
                unit->collision_pos = unit->GetPosition();
                unit->collision_scale = unit->GetScale();
                //unit->collision_rotation = unit->GetAngle();
                unit->Update(elapsedTime);

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

        PlayAnimation(Animation_Index::Idle, true);
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
        PlayAnimation(Animation_Index::Jump, true);
        error_effect_handle = error_effect->Play(position, effect_size);
        velocity.z = 0;
        death = true;
        is_dis_direction = false;
    }
    else
    {
        velocity.z = throw_speed;
    }
}

void Seed::Render(float scale, ID3D11PixelShader* replaced_pixel_shader)
{
    Character::Render(scale, replaced_pixel_shader);
}

void Seed::DrawDebugGUI()
{/*
    ImGui::Begin("Seed");

    ImGui::DragInt("number", &number);
    ImGui::DragFloat("timer", &timer);
    ImGui::Checkbox("born", &born);

    ImGui::End();*/
}

void Seed::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //debug_renderer->DrawCylinder(position, radius, height, { 1,0,0,1 });
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

    // �킪��̊O���ɂł������
    if (Fence::Instance().GetLeftRect().right_down.x + seed_fence_dis >= outPosition.x||// ��
        Fence::Instance().GetRightRect().left_up.x - seed_fence_dis <= outPosition.x|| // �E
        Fence::Instance().GetBackRect().right_down.y - seed_fence_dis <= outPosition.y ||// ��
        Fence::Instance().GetFrontRect().left_up.y + seed_fence_dis >= outPosition.y // ��O
        )
    {
        error_effect_handle = error_effect->Play(position, effect_size);
        death = true;// ���j��
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

        error_effect_handle = error_effect->Play(position, effect_size);
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
