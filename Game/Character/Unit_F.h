#pragma once
#include "Unit.h"

class Unit_F :public Unit
{
public:
    Unit_F();
    ~Unit_F()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

private:

    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    float jumpSpeed = 20.0f;

    // 攻撃当たり判定するか
    bool attackCollisionFlag = false;
};


