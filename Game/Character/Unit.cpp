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

    // ステート関係なく実行
    // 速力処理更新
    UpdateVelocity(elapsed_time);

    // 無的時間更新
    UpdateInvincibleTimer(elapsed_time);

    // Transform の更新
    UpdateTransform();

    UpdateScale();

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
    set_effect->Stop(set_handle);
    attack_effect->Stop(attack_handle);
    death_effect->Stop(death_handle);
    EffectManager::Instance().Update(0.01f);
    if (!IsPlayAnimation())UnitManager::Instance().Remove(this);
}

void Unit::TransitionDeathState()
{
    death = true;
    // アニメーションの切り替え
    PlayAnimation(Animation::Out, false);
    // エフェクトの再生
    death_handle = death_effect->Play(position, death_effect_size);
    // 攻撃範囲を消す
    collision_model->GetTransform()->SetScaleFactor(0.0f);

    EasingScaleOut();

    // ステート切り替え
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
