#pragma once
#include "Unit.h"

class Unit_C :public Unit
{
public:
    Unit_C();
    ~Unit_C()override;

    void Update(float elapsedTime)override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

private:

    // �ҋ@�X�e�[�g�֑J��
    void TransitionIdleState()override;

    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsed_time)override;

    // �U���X�e�[�g�֑J��
    void TransitionAttackState()override;

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsed_time)override;

    // �G�ւ̍U��
    void AttackEnemy(float elapsedTime);

    // Imgui
    void DrawDebugGUI();
private:
    float attack_timer;

    float t_height;         // �O�p�`�̍���
    float t_base;           // �O�p�`�̒�Ӓ�
};


