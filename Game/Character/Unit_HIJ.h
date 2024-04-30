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

class Unit_H :public Unit
{
public:
    Unit_H();
    ~Unit_H()override {};

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

class Unit_I :public Unit
{
public:
    Unit_I();
    ~Unit_I()override {};

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
