#pragma once
#include "Unit.h"

class Unit_J :public Unit
{
public:
    Unit_J();
    ~Unit_J()override {};

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

private:
    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsed_time)override;

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
};
