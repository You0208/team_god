#include "Unit.h"
#include "Lemur/Graphics/Graphics.h"
#include "UnitManager.h"
void Unit::Update(float elapsed_time)
{
    switch (state)
    {
    case State::Idle:
        UpdateIdleState(elapsed_time);
        break;
    case State::Attack:
        UpdateAttackState(elapsed_time);
        break;
    case State::Death:
        UpdateDeathState(elapsed_time);
        break;
    };

    // �X�e�[�g�֌W�Ȃ����s
    // ���͏����X�V
    UpdateVelocity(elapsed_time);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsed_time);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V
    model->UpdateAnimation(elapsed_time);

    DrawDebugPrimitive();
}

int Unit::ReturnDamage()
{
    if (!is_streng)return attack_power;
    if (is_streng)return streng_attack_power;
}

void Unit::Destroy()
{
    UnitManager::Instance().Remove(this);
}

void Unit::TransitionDeathState()
{
    // �A�j���[�V�����̐؂�ւ�
    model->PlayAnimation(Animation::Out, false);
    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Unit::UpdateDeathState(float elapsed_time)
{
    Destroy();
}

void Unit::TransitionIdleState()
{
    // �A�j���[�V�����̐؂�ւ�
    model->PlayAnimation(Animation::Idle, true);
    // �X�e�[�g�؂�ւ�
    state = State::Idle;
}

void Unit::TransitionAttackState()
{
    // �A�j���[�V�����̐؂�ւ�
    model->PlayAnimation(Animation::Attack, false);
    // �X�e�[�g�؂�ւ�
    state = State::Attack;
}
