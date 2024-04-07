#pragma once
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};

    // XVˆ—
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    void DrawDebugGUI()override;
private:
};

