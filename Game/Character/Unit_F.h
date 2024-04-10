#pragma once
#include "Unit.h"

class Unit_F :public Unit
{
public:
    Unit_F();
    ~Unit_F()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

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

    void DrawDebugGUI();

private:
};


