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
    // アニメーションの切り替え
    PlayAnimation(Animation::Out, false);
    // エフェクトの再生
    death_effect->Play(position, death_effect_size);
    // ステート切り替え
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
    // アニメーションの切り替え
   PlayAnimation(Animation::Idle, true);
    // ステート切り替え
    state = State::Idle;
}

void Unit::TransitionAttackState()
{
    // ステート切り替え
    state = State::Attack;
}
