#pragma once
#include "Unit.h"

class Unit_C :public Unit
{
public:
    Unit_C();
    ~Unit_C()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // �G�ւ̍U��
    void AttackEnemy(float elapsedTime);

    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

private:
    float attack_timer;

    float t_height;         // �O�p�`�̍���
    float t_base;           // �O�p�`�̒�Ӓ�
};


