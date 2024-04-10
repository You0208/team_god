#pragma once
#include "Unit.h"
#include "Lemur/Model/FbxModelManager.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override;

    void Update(float elapsed_time)override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

private:

    // 待機ステートへ遷移
    void TransitionIdleState()override;

    // 待機ステート更新処理
    void UpdateIdleState(float elapsed_time)override;

    // 攻撃ステートへ遷移
    void TransitionAttackState()override;

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsed_time)override;

    // 敵への攻撃
    void AttackEnemy(float elapsed_time);
    // Imgui
    void DrawDebugGUI()override;
private:

    float attack_timer;
};


