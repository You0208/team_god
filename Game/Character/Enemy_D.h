#pragma once
#include "Enemy.h"
class Enemy_D :public Enemy
{
public:
    Enemy_D();
    ~Enemy_D()override {};

    // XVˆ—
    void Update(float elapsedTime) override;

    // •`‰æˆ—
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

private:
};

