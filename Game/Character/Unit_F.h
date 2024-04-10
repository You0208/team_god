#pragma once
#include "Unit.h"

class Unit_F :public Unit
{
public:
    Unit_F();
    ~Unit_F()override;

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

    void DrawDebugGUI();

private:
};


