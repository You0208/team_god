#include "Unit.h"
#include "Lemur/Graphics/Graphics.h"
#include "UnitManager.h"
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
    model->UpdateAnimation(elapsed_time);

    DrawDebugPrimitive();
}

int Unit::ReturnDamage()
{
    if (!is_streng)return attack_power;
    if (is_streng)return streng_attack_power;
}

void Unit::Destroy()
{
    UnitManager::Instance().Remove(this);
}

void Unit::TransitionDeathState()
{
    // アニメーションの切り替え
    model->PlayAnimation(Animation::Out, false);
    // ステート切り替え
    state = State::Death;
}

void Unit::UpdateDeathState(float elapsed_time)
{
    Destroy();
}

void Unit::TransitionIdleState()
{
    // アニメーションの切り替え
    model->PlayAnimation(Animation::Idle, true);
    // ステート切り替え
    state = State::Idle;
}

void Unit::TransitionAttackState()
{
    // アニメーションの切り替え
    model->PlayAnimation(Animation::Attack, false);
    // ステート切り替え
    state = State::Attack;
}
