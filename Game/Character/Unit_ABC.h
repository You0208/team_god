#pragma once
#include "Unit.h"

// 唐辛子
class Chili :public Unit
{
public:
    Chili();
    ~Chili()override {};

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
    bool first_attack = true;
};

class Shishito :public Unit
{
public:
    Shishito();
    ~Shishito()override {};

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

class OrangePumpkin :public Unit
{
public:
    OrangePumpkin();
    ~OrangePumpkin()override {};

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
