#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"

class Enemy :public Character
{
public:
    Enemy() {}
    ~Enemy() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    // �`�揈��
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // �j��
    void Destroy();

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive();

};