#pragma once
#include "Unit.h"

class Unit_C :public Unit
{
public:
    Unit_C();
    ~Unit_C()override;

    void Update(float elapsedTime)override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

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
    void AttackEnemy(float elapsedTime);

    // Imgui
    void DrawDebugGUI();
private:
    float attack_timer;

    float t_height;         // 三角形の高さ
    float t_base;           // 三角形の底辺長
};


