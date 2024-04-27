#pragma once
#include "Unit.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override {};

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

class Unit_B :public Unit
{
public:
    Unit_B();
    ~Unit_B()override {};

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
};

class Unit_C :public Unit
{
public:
    Unit_C();
    ~Unit_C()override {};

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
};
