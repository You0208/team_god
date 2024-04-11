#include "Enemy.h"
#include "Lemur/Graphics/Graphics.h"
#include "EnemyManager.h"
void Enemy::Destroy()
{
    EnemyManager::Instance().Remove(this);
}

void Enemy::TransitionDeathState()
{
    // 速度を０に
    velocity.x = velocity.y = velocity.z = 0.0f;
    // アニメーションの切り替え
    model->PlayAnimation(Animation::Out, false);
    // ステート切り替え
    state = State::Death;
}

void Enemy::UpdateDeathState(float elapsed_time)
{
    EnemyManager::Instance().Remove(this);
}

void Enemy::TransitionAttackState()
{
    // 速度を０に
    velocity.x = velocity.y = velocity.z = 0.0f;
    // アニメーションの切り替え
    model->PlayAnimation(Animation::Attack, false);
    // ステート切り替え
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

    // ステート関係なく実行
    // 速力処理更新
    UpdateVelocity(elapsed_time);

    // 無的時間更新
    UpdateInvincibleTimer(elapsed_time);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新
    model->UpdateAnimation(elapsed_time);

    DrawDebugPrimitive();
}

