#pragma once
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // �U���X�V
    void AttackUpdate(float elapsedTime) override;

    // �ړ��X�V
    void MoveUpdate(float elapsedTime) override;

    void DrawDebugGUI()override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
private:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

    int state_index;

    float timer;
};

