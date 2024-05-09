#pragma once
#include "Unit.h"

class Unit_J :public Unit
{
public:
    Unit_J();
    ~Unit_J()override {};

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

private:
    // 待機ステート更新処理
    void UpdateIdleState(float elapsed_time)override;

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
};
