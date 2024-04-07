#pragma once
#include "Enemy.h"
class Enemy_B :public Enemy
{
public:
    Enemy_B();
    ~Enemy_B()override {};

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
    float old_position;
    float dis;

    float move_dis;

    int state_index;

    float attack_timer;
    float move_timer;
};

