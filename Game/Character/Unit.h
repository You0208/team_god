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

    // Getter
    int GetCategory() { return category; }
    float GetAttackRadius() { return attack_radius; }
    float GetDecPos() { return dec_pos; }
    Triangle GetTriangle1() { return triangle_1; }
    Triangle GetTriangle2() { return triangle_2; }
    Rect GetRect() { return square; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetSquare(Rect square_) { 
        square.left_up = square_.left_up;
        square.right_down = square_.right_down;
    }

protected:
    float dec_pos;          // í‚ªƒ†ƒjƒbƒg‚É’µ‚Ë•Ô‚³‚ê‚é‹——£
    int category;           // ƒ†ƒjƒbƒg‚ÌƒJƒeƒSƒŠ[
    int attack_times;       // UŒ‚‰ñ”
    float attack_radius;    // ‰~UŒ‚”ÍˆÍ”¼Œa
    Triangle triangle_1;    // ¶OŠpUŒ‚”ÍˆÍ
    Triangle triangle_2;    // ‰EOŠpUŒ‚”ÍˆÍ
    Rect square;            // ƒ†ƒjƒbƒg‚ÌŒã•û”ÍˆÍ
};