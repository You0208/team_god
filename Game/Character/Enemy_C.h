#pragma once
#include "Enemy.h"
class Enemy_C :public Enemy
{
public:
    Enemy_C();
    ~Enemy_C()override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

private:
};

