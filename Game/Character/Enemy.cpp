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
    model->PlayAnimation(Animation::Out, false);
    // �X�e�[�g�؂�ւ�
    state = State::Death;
}

void Enemy::UpdateDeathState(float elapsed_time)
{
    EnemyManager::Instance().Remove(this);
}

void Enemy::TransitionAttackState()
{
    // ���x���O��
    velocity.x = velocity.y = velocity.z = 0.0f;
    // �A�j���[�V�����̐؂�ւ�
    model->PlayAnimation(Animation::Attack, false);
    // �X�e�[�g�؂�ւ�
    state = State::Attack;
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
    model->UpdateAnimation(elapsed_time);

    DrawDebugPrimitive();
}

