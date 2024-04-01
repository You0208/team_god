#pragma once
#include "Enemy.h"
class Enemy_D :public Enemy
{
public:
    Enemy_D();
    ~Enemy_D()override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

private:
};

