#include "Enemy_C.h"
#include "EnemyManager.h"
#include "UnitManager.h"


#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"
#include "../Stage/Fence.h"

Enemy_C::Enemy_C()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;         // �U���́@
    attack_interval = 3.0f;      // �U���Ԋu

    radius          = 1.0f;      // ���a
    height          = 1.0f;      // �f�o�b�O�v
    position.x      = 5.0f;      // �����ʒu
    rotation.y      = -90.0f;    // �����p�x
    speed_power     = -1.0f;     // ���x
    health          = 10;        // HP

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

void Enemy_C::OnDead()
{
    EnemyManager::Instance().Remove(this);
}

void Enemy_C::JudgeUnit()
{
    UnitManager& unitManager = UnitManager::Instance();

    int unitCount = unitManager.GetUnitCount();

    // �G�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // �G�����j�b�g�̍U���͈͂ɐG�ꂽ�Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { unit->GetPosition().x,unit->GetPosition().z },   // ���j�b�g�̈ʒu(XZ����)
            unit->GetAttackRadius(),                 // �U���͈�
            { position.x,position.z },               // �G�̈ʒu(XZ����)
            radius                                   // �G�̓����蔻��
        ))
        {
            is_touched_unit = true;
            destination = {
                unit->GetPosition().x,
                unit->GetPosition().z + unit->GetRadius() + radius
            };
        }
    }
}

void Enemy_C::Update(float elapsedTime)
{
    switch (state_index)
    {
    case StateIndex::Move_State:
        MoveUpdate(elapsedTime);
        break;
    case StateIndex::Attack_State:
        AttackUpdate(elapsedTime);
        break;
    };

    // �ڐG���肪OFF�̎��̂ݎ��s
    if (!is_touched_unit)JudgeUnit();

    if (health <= 0)
    {
        OnDead();
    }

    // �X�e�[�g�֌W�Ȃ����s
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsedTime);

    // �f�o�b�O
    DrawDebugGUI();
    DrawDebugPrimitive();
}

void Enemy_C::MoveUpdate(float elapsedTime)
{
    // ��ɐG�ꂽ��
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius)||
        Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
    {
        velocity.x = 0.0f;
        model->PlayAnimation(7, true);
        state_index = int(StateIndex::Attack_State);
    }
    else
    {
        switch (move_state)
       {
        case 0:
            // �ړ�
            velocity.x = speed_power;
            if (is_touched_unit)move_state = 1;
            break;
        case 1:// ���j�b�g�ɐG������
            // ���ݒn����ړI�n�܂ł̃x�N�g�����Z�o
            DirectX::XMFLOAT2 Vec =
            {
                destination.x - position.x,
                destination.y - position.z
            };
            // ���K��
            Vec = Normalize(Vec);
            // �X�s�[�h�ɂ�����
            Vec = { Vec.x * abs(speed_power),Vec.y * abs(speed_power) };
            // �ړ�
            velocity.x = Vec.x;
            velocity.z = Vec.y;

            // �ړI�n�ɒ�������߂�
            if (position.z >= destination.y)
            {
                is_touched_unit = false;
                velocity.x = velocity.z = 0.0f;
                move_state = 0;
            }

            break;
        }
    }

}

void Enemy_C::DrawDebugGUI()
{
    std::string name = "Enemy_C";

    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string spe = std::string("health") + name;
        ImGui::DragInt(spe.c_str(), &health, 5.0f, -5.0f);

        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string r = std::string("rotation") + name;
        DirectX::XMFLOAT3 rot{};
        rot.x = DirectX::XMConvertToDegrees(rotation.x);
        rot.y = DirectX::XMConvertToDegrees(rotation.y);
        rot.z = DirectX::XMConvertToDegrees(rotation.z);
        ImGui::DragFloat3(r.c_str(), &rot.x, 0.5f, -FLT_MAX, FLT_MAX);
        rotation.x = DirectX::XMConvertToRadians(rot.x);
        rotation.y = DirectX::XMConvertToRadians(rot.y);
        rotation.z = DirectX::XMConvertToRadians(rot.z);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scaleFactor, 0.001f, 0.001f, 1.0f);
        ImGui::TreePop();
    }
}

void Enemy_C::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_C::AttackUpdate(float elapsedTime)
{
    timer += elapsedTime;

    if (timer >= attack_interval)
    {
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            model->PlayAnimation(1, false);
            timer = 0.0f;
        }
    }
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }
}
