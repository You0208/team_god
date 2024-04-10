#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Math/MathHelper.h"

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
    float dec_pos;          // �킪���j�b�g�ɒ��˕Ԃ���鋗��
    int category;           // ���j�b�g�̃J�e�S���[
    int attack_times;       // �U����
    float attack_radius;    // �~�U���͈͔��a
    Triangle triangle_1;    // ���O�p�U���͈�
    Triangle triangle_2;    // �E�O�p�U���͈�
    Rect square;            // ���j�b�g�̌���͈�
};