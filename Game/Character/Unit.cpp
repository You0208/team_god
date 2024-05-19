#include "Unit.h"
#include "UnitManager.h"

void Unit::Update(float elapsed_time)
{
    EasingScaleUpdate(elapsed_time);
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

    UpdateScale();

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
    set_effect->Stop(set_handle);
    attack_effect->Stop(attack_handle);
    death_effect->Stop(death_handle);
    EffectManager::Instance().Update(0.01f);
    if (!IsPlayAnimation())UnitManager::Instance().Remove(this);
}

void Unit::TransitionDeathState()
{
    death = true;
    // �A�j���[�V�����̐؂�ւ�
    PlayAnimation(Animation::Out, false);
    // �G�t�F�N�g�̍Đ�
    death_handle = death_effect->Play(position, death_effect_size);
    // �U���͈͂�����
    collision_model->GetTransform()->SetScaleFactor(0.0f);

    EasingScaleOut();

    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Unit::UpdateDeathState(float elapsed_time)
{
    radius = 0.0f;
    attack_collision_range = 0.0f;

    //Dissolve(elapsed_time);

    //if(!GetIsDissolve())Destroy();
    if (!IsPlayAnimation() && !easing_scale.is_easing)Destroy();
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
