#include "Enemy.h"
#include "Lemur/Graphics/Graphics.h"
#include "EnemyManager.h"
#include "Lemur/Audio/AudioManager.h"

void Enemy::Destroy()
{
    if (set_effect != nullptr)
    {
        set_effect->Stop(set_handle);
        delete set_effect;
        set_effect = nullptr;
    }
    // エフェクト終了
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
    // 速度を０に
    velocity.x = velocity.y = velocity.z = 0.0f;

    // アニメーションの切り替え
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

    // イージング
    EasingScaleOut();

    // ステート切り替え
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

    // ステート関係なく実行
    // 速力処理更新
    UpdateVelocity(elapsed_time);

    // 無的時間更新
    UpdateInvincibleTimer(elapsed_time);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新
    UpdateAnimation(elapsed_time);

    // さいずの更新
    UpdateScale();

    DrawDebugPrimitive();
}

