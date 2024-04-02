#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"

class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {}

    // XVˆ—
    virtual void Update(float elapsedTime) = 0;

    // •`‰æˆ—
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;
    // ”jŠü
    void Destroy();

    float GetAttackRadius() { return attack_radius; }

    void SetSquare(DirectX::XMFLOAT2 square_[2]) { 
        square[0] = square_[0]; 
        square[1] = square_[1];
    }

    int category;

    // lŠp
    DirectX::XMFLOAT2 square[2];
    float dec_pos;
protected:
    float attack_radius;


};