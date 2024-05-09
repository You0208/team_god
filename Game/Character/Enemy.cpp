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
    PlayAnimation(Animation::Out, false);

    death_effect->Play(position, death_effect_size);
    // ステート切り替え
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
    // 速度を０に
    velocity.x = velocity.y = velocity.z = 0.0f;
    // アニメーションの切り替え
    PlayAnimation(Animation::Attack, false);
    // ステート切り替え
    state = State::Attack;
}

bool Enemy::ApplyDamage(int damage)
{
    // 死亡している間は健康状態を変更しない
    if (health <= 0)return false;

    hit_effect->Play(position, hit_effect_size);

    // ダメージ処理
    health -= damage;

    // 死亡通知
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

    // 健康状態が変更した場合はtrueを返す
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

    // ステート関係なく実行
    // 速力処理更新
    UpdateVelocity(elapsed_time);

    // 無的時間更新
    UpdateInvincibleTimer(elapsed_time);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新
    UpdateAnimation(elapsed_time);

    DrawDebugPrimitive();
}

