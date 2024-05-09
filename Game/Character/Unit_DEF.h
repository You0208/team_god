#pragma once
#include "Unit.h"


// 
class GreenPumpkin :public Unit
{
public:
    GreenPumpkin();
    ~GreenPumpkin()override {};

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

class Broccoli :public Unit
{
public:
    Broccoli();
    ~Broccoli()override {};

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

class Cauliflower :public Unit
{
public:
    Cauliflower();
    ~Cauliflower()override {};

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
