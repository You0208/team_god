#pragma once
#include "Unit.h"

class Unit_B :public Unit
{
public:
    Unit_B();
    ~Unit_B()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

private:
    // �G�ւ̍U��
    void AttackEnemy(float elapsedTime);

    void DrawDebugGUI();
private:
    float attack_timer;

    float t_height;         // �O�p�`�̍���
    float t_base;           // �O�p�`�̒�Ӓ�
};


