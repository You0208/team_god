#pragma once
#include "Unit.h"

class Unit_D :public Unit
{
public:
    Unit_D();
    ~Unit_D()override {};

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

private:
    // 待機ステート更新処理
    void UpdateIdleState(float elapsed_time)override;

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
private:
    float attack_timer;
    float timer_max;
    int   streng_width;
};

class Unit_E :public Unit
{
public:
    Unit_E();
    ~Unit_E()override {};

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

    void Update(float elapsed_time)override;
private:
    // 待機ステート更新処理
    void UpdateIdleState(float elapsed_time)override;

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
private:
    float attack_timer;
    float attack_width;
};

class Unit_F :public Unit
{
public:
    Unit_F();
    ~Unit_F()override {};

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

    void Update(float elapsed_time)override;
private:
    // 待機ステート更新処理
    void UpdateIdleState(float elapsed_time)override;

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
private:
    float attack_timer;
    float attack_width;
};
