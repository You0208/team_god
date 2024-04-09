#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Math/MathHelper.h"

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

    void SetSquare(Rect square_) { 
        square.left_up = square_.left_up;
        square.right_down = square_.right_down;
    }

    int category;

    int attack_times;

    // lŠp
    Rect square;
    float dec_pos;
protected:
    float attack_radius;
};