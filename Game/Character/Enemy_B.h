#pragma once
#include "Enemy.h"
class Enemy_B :public Enemy
{
public:
    Enemy_B();
    ~Enemy_B()override {};

    // XVˆ—
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

private:
};

