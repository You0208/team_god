#include "Enemy.h"
#include "Lemur/Graphics/Graphics.h"
#include "EnemyManager.h"

void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}

void Enemy::TransitionDeathState()
{
    // ���x���O��
    velocity.x = velocity.y = velocity.z = 0.0f;
    // �A�j���[�V�����̐؂�ւ�
    PlayAnimation(Animation::Out, false);

    death_effect->Play(position, death_effect_size);
    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Enemy::UpdateDeathState(float elapsed_time)
{
   // radius = 0.0f;
    if(!IsPlayAnimation())
    {
        //Dissolve(elapsed_time);
       /* if (!GetIsDissolve()) */EnemyManager::Instance().Remove(this);
    }
    //Dissolve(elapsed_time);

    //if (!GetIsDissolve() && !IsPlayAnimation())EnemyManager::Instance().Remove(this);
}

void Enemy::TransitionAttackState()
{
    // ���x���O��
    velocity.x = velocity.y = velocity.z = 0.0f;
    // �A�j���[�V�����̐؂�ւ�
    PlayAnimation(Animation::Attack, false);
    // �X�e�[�g�؂�ւ�
    state = State::Attack;
}

bool Enemy::ApplyDamage(int damage)
{
    // ���S���Ă���Ԃ͌��N��Ԃ�ύX���Ȃ�
    if (health <= 0)return false;

    hit_effect->Play(position, hit_effect_size);

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

