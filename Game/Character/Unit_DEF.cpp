#include "Unit_DEF.h"
#include "UnitManager.h"

//--------Unit_D--------------------------------------------------

Unit_D::Unit_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    attack_times            = 5;    // �U����
    attack_power            = 1;    // �U����
    streng_width            = 2;    // ������
    attack_interval         = 0.5f; // �U���Ԋu
    attack_collision_range  = 1.0f; // �U���͈�
    radius                  = 0.3f; // ���a
    height                  = 0.5f; // �f�o�b�O�p
    dec_pos                 = 1.0f; // ���j�b�g�ɐڐG�����킪�ǂ̂��炢���˕Ԃ���邩
    timer_max               = 1.0f; // �o�t�̕b��

    // �Ƃ肠�����A�j���[�V����
    model->PlayAnimation(Animation::Idle, true);
}

void Unit_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, attack_collision_range, height, { 1,0,0,1 });
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Unit_D::UpdateIdleState(float elapsed_time)
{
    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();

    attack_timer += elapsed_time;// �U���^�C�}�[

    //TODO ���[�V������������܂��ς���
    // ���j�b�g�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // ���̃��j�b�g�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                        // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,                           // �U���͈�
            { unit->GetPosition().x,unit->GetPosition().z },  // �G�̈ʒu(XZ����)
            unit->GetRadius()                                 // �G�̓����蔻��
        ))
        {
            // ������Ԃ�true��
            unit->SetBuff(true);
            unit->SetStrengAttack(unit->GetAttackPower() + streng_width);
            // �U���X�e�[�g�ɐ؂�ւ�
            TransitionAttackState();
        }
    }
    // �U���񐔂���������������
    if (attack_times <= 0)
    {
        TransitionDeathState();
    }
}

void Unit_D::UpdateAttackState(float elapsed_time)
{
    attack_timer += elapsed_time;// �U���^�C�}�[

    UnitManager& unitManager = UnitManager::Instance();
    int unitCount = unitManager.GetUnitCount();
    bool is_intersected = false;

    //TODO ���[�V������������܂��ς���
    // ���j�b�g�̑�������
    for (int j = 0; j < unitCount; ++j)
    {
        Unit* unit = unitManager.GetUnit(j);

        // ���̃��j�b�g�����j�b�g�̍U���͈͂ɓ����Ă���Ƃ�
        if (Collision::IntersectCircleVsCircle
        (
            { position.x,position.z },                        // ���j�b�g�̈ʒu(XZ����)
            attack_collision_range,                           // �U���͈�
            { unit->GetPosition().x,unit->GetPosition().z },  // �G�̈ʒu(XZ����)
            unit->GetRadius()                                 // �G�̓����蔻��
        ))
        {
            is_intersected = true;
            // ������Ԃ�true��
            unit->SetBuff(true);
            unit->SetStrengAttack(unit->GetAttackPower() + streng_width);
        }
        else// �͈͓��ɂ��Ȃ��G��
        {
            // ������Ԃ�؂�
            unit->SetBuff(true);
        }
    }

    // �͈͓��ɓG����̂����Ȃ���Αҋ@
    if (!is_intersected)
    {
        TransitionIdleState();
    }

    // �U�����Ԃ��߂��������
    if (attack_timer >= timer_max)
    {
        TransitionDeathState();
    }
}

void Unit_D::DrawDebugGUI(int n)
{
    Character::DrawDebugGUI("Unit_D", n);
}


//--------Unit_E--------------------------------------------------

Unit_E::Unit_E()
{
}

void Unit_E::DrawDebugPrimitive()
{
}

void Unit_E::UpdateIdleState(float elapsed_time)
{
}

void Unit_E::UpdateAttackState(float elapsed_time)
{
}

void Unit_E::DrawDebugGUI(int n)
{
}


//--------Unit_F--------------------------------------------------

Unit_F::Unit_F()
{
}

void Unit_F::DrawDebugPrimitive()
{
}

void Unit_F::UpdateIdleState(float elapsed_time)
{
}

void Unit_F::UpdateAttackState(float elapsed_time)
{
}

void Unit_F::DrawDebugGUI(int n)
{
}
