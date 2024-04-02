#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"

class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    // �`�揈��
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;
    // �j��
    void Destroy();

    int category;
};