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
    // エフェクト終了
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
    // 速度を０に
    velocity.x = velocity.y = velocity.z = 0.0f;

    // アニメーションの切り替え
    if (enemy_type == EnemyType::A || enemy_type == EnemyType::A_2)
    {
        PlayAnimation(1, false);
    }
    else
    {
        PlayAnimation(Animation::Out, false);
    }

    death_handle = death_effect->Play(position, death_effect_size);


    // ステート切り替え
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
    // 速度を０に
    velocity.x = velocity.y = velocity.z = 0.0f;

    // アニメーションの切り替え
    SetLoopFlag(false);

    // ステート切り替え
    state = State::Attack;
}

bool Enemy::ApplyDamage(int damage)
{
    // 死亡している間は健康状態を変更しない
    if (health <= 0)return false;

    hit_handle = hit_effect->Play(position, hit_effect_size);

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

