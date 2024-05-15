#include "Enemy.h"
#include "Lemur/Graphics/Graphics.h"
#include "EnemyManager.h"

void Enemy::Destroy()
{
    attack_effect->Stop(attack_handle);
    death_effect->Stop(death_handle);
    hit_effect->Stop(hit_handle);
    attack_handle = 0;
    death_handle = 0;
    hit_handle = 0;
    // �G�t�F�N�g�I��
    if (attack_effect != nullptr)
    {
        delete attack_effect;
        attack_effect = nullptr;
    }
    if (hit_effect != nullptr)
    {
        delete hit_effect;
        hit_effect = nullptr;
    }
    if (death_effect != nullptr)
    {
        delete death_effect;
        death_effect = nullptr;
    }
    EnemyManager::Instance().Remove(this);
}

void Enemy::TransitionDeathState()
{
    // ���x���O��
    velocity.x = velocity.y = velocity.z = 0.0f;

    // �A�j���[�V�����̐؂�ւ�
    if (enemy_type == EnemyType::A || enemy_type == EnemyType::A_2)
    {
        PlayAnimation(1, false);
    }
    else
    {
        PlayAnimation(Animation::Out, false);
    }

    death_handle = death_effect->Play(position, death_effect_size);


    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Enemy::UpdateDeathState(float elapsed_time)
{
    if(!IsPlayAnimation())
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

    DrawDebugPrimitive();
}

