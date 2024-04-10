#pragma once
#include "Enemy.h"
class Enemy_B :public Enemy
{
public:
    Enemy_B();
    ~Enemy_B()override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

private:
    // �U���X�V
    void AttackUpdate(float elapsedTime) override;

    // �ړ��X�V
    void MoveUpdate(float elapsedTime) override;

    void DrawDebugGUI()override;

    // ���S����
    void OnDead()override;

private:
    // �ړ��p�̈ʒu�ۑ�
    float old_position;
    // �i�ދ���
    float dis;
    // �U���Ԋu
    float attack_timer;
    // �ړ��^�C�}�[
    float move_timer;
    // �ړ��Ԋu
    float move_timer_max;
};

