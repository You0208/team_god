#pragma once
#include "Enemy.h"
class Enemy_C :public Enemy
{
public:
    Enemy_C();
    ~Enemy_C()override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

private:
};

