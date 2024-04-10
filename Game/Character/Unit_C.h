#pragma once
#include "Unit.h"

class Unit_C :public Unit
{
public:
    Unit_C();
    ~Unit_C()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // 敵への攻撃
    void AttackEnemy(float elapsedTime);

    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

private:
    float attack_timer;

    float t_height;         // 三角形の高さ
    float t_base;           // 三角形の底辺長
};


