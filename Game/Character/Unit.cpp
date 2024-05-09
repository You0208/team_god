#include "Unit.h"
#include "Lemur/Graphics/Graphics.h"
#include "UnitManager.h"
#include "Lemur/Effekseer/EffekseerManager.h"

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
    UpdateAnimation(elapsed_time);

    DrawDebugPrimitive();
}

int Unit::ReturnDamage()
{
    if (is_streng)return streng_attack_power;
    return attack_power;
}

void Unit::Destroy()
{
    if (!IsPlayAnimation())UnitManager::Instance().Remove(this);
}

void Unit::TransitionDeathState()
{
    death = true;
    // �A�j���[�V�����̐؂�ւ�
    PlayAnimation(Animation::Out, false);
    // �G�t�F�N�g�̍Đ�
    death_effect->Play(position, death_effect_size);
    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Unit::UpdateDeathState(float elapsed_time)
{
    radius = 0.0f;
    attack_collision_range = 0.0f;

    //Dissolve(elapsed_time);

    //if(!GetIsDissolve())Destroy();
    if(!IsPlayAnimation())Destroy();
}

void Unit::TransitionIdleState()
{
    // �A�j���[�V�����̐؂�ւ�
   PlayAnimation(Animation::Idle, true);
    // �X�e�[�g�؂�ւ�
    state = State::Idle;
}

void Unit::TransitionAttackState()
{
    // �X�e�[�g�؂�ւ�
    state = State::Attack;
}
