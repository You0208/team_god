#include "Unit_D.h"
#include "UnitManager.h"
#include "Lemur/Input/Input.h"
#include "Lemur/Graphics/DebugRenderer.h"
#include "Lemur/Collision/Collision.h"

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times    = 5;    // �U����
    attack_power    = 1;    // �U����
    attack_interval = 0.5f; // �U���Ԋu
    attack_radius   = 1.0f; // �U���͈�
    radius          = 0.3f; // ���a
    height          = 0.5f; // �f�o�b�O�p
    dec_pos         = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(0, true);
}

Unit_D::~Unit_D()
{
}

void Unit_D::Update(float elapsedTime)
{
    AttackEnemy(elapsedTime);

    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model->UpdateAnimation(elapsedTime);

    DrawDebugGUI();
}

void Unit_D::AttackEnemy(float elapsedTime)
{
    UnitManager& unitManager = UnitManager::Instance();

    int unitCount = unitManager.GetUnitCount();

    //TODO ���[�V������������܂��ς���
    // �G�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // ���j�b�g�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                // ���j�b�g�̈ʒu(XZ����)
            attack_radius,                          // �U���͈�
            { unit->GetPosition().x,unit->GetPosition().z },  // �G�̈ʒu(XZ����)
            unit->GetRadius()                           // �G�̓����蔻��
        ))
        {
            attack_timer += elapsedTime;

            if (attack_timer >= attack_interval)
            {
                //TODO �o�t
            }
        }
    }

    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        UnitManager::Instance().Remove(this);
    }
}

void Unit_D::DrawDebugGUI()
{
    model->DrawDebug("Unit_D");
}

void Unit_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_radius, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_D::TransitionIdleState()
{
}

void Unit_D::UpdateIdleState(float elapsed_time)
{
}

void Unit_D::TransitionAttackState()
{
}

void Unit_D::UpdateAttackState(float elapsed_time)
{
}
