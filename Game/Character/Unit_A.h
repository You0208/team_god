#pragma once
#include "Unit.h"
#include "Lemur/Model/FbxModelManager.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override;

    void Update(float elapsed_time)override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

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
    void AttackEnemy(float elapsed_time);
    // Imgui
    void DrawDebugGUI()override;
private:

    float attack_timer;
};


