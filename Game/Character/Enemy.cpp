#include "Enemy.h"
#include "Lemur/Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Lemur/Audio/AudioManager.h"

void Enemy::Destroy()
{
    // �G�t�F�N�g�I��
    if (set_effect != nullptr)
    {
        set_effect->Stop(set_handle);
        delete set_effect;
        set_effect = nullptr;
    }
    if (attack_effect != nullptr)
    {
        attack_effect->Stop(attack_handle);
        delete attack_effect;
        attack_effect = nullptr;
    }
    if (hit_effect != nullptr)
    {
        hit_effect->Stop(hit_handle);
        delete hit_effect;
        hit_effect = nullptr;
    }
    if (death_effect != nullptr)
    {
        death_effect->Stop(death_handle);
        delete death_effect;
        death_effect = nullptr;
    }
    EffectManager::Instance().Update(0.01f);
}

void Enemy::TransitionDeathState()
{
    // ���x���O��
    velocity.x = velocity.y = velocity.z = 0.0f;

    // �A�j���[�V�����̐؂�ւ�
    if (enemy_type == EnemyType::A || enemy_type == EnemyType::A_2 || enemy_type == EnemyType::Boss)
    {
        PlayAnimation(1, false);
    }
    else
    {
        PlayAnimation(Animation::Out, false);
    }

    Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DEATH, false);

    death_handle = death_effect->Play(position, death_effect_size);

    // �C�[�W���O
    EasingScaleOut();

    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Enemy::UpdateDeathState(float elapsed_time)
{
    if (!IsPlayAnimation() && !easing_scale.is_easing)
    {
        EnemyManager::Instance().Remove(this);
    }
}

void Enemy::TransitionAttackState()
{
    // ���x���O��
    velocity.x = velocity.y = velocity.z = 0.0f;

    // �A�j���[�V�����̐؂�ւ�
    SetLoopFlag(false);

    // �X�e�[�g�؂�ւ�
    state = State::Attack;
}

bool Enemy::ApplyDamage(int damage)
{
    // ���S���Ă���Ԃ͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0)return false;

    hit_handle = hit_effect->Play(position, hit_effect_size);

    // �_���[�W����
    health -= damage;

    // ���S�ʒm
    if (health <= 0)
    {
        health = 0;
        death = true;
        OnDead();
    }
    else
    {
        OnDamage();
    }

    // ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
    return true;
}

void Enemy::Update(float elapsed_time)
{
    EasingScaleUpdate(elapsed_time);
    switch (state)
    {
    case State::Move:
        UpdateMoveState(elapsed_time);
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

    // �T�C�Y�̍X�V
    UpdateScale();
}

