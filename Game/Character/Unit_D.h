#pragma once
#include "Unit.h"

class Unit_D :public Unit
{
public:
    Unit_D();
    ~Unit_D()override;

    void Update(float elapsedTime)override;

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
    void AttackEnemy(float elapsedTime);

    void DrawDebugGUI()override;
private:

    float attack_timer;
};



