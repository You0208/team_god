#pragma once
#include "Unit.h"
#include "Lemur/Model/FbxModelManager.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // 敵への攻撃
    void AttackEnemy(float elapsedTime);

    void DrawDebugGUI()override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

private:

    float attack_timer;
};


