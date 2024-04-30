#pragma once
#include "Unit.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override {};

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

class Unit_B :public Unit
{
public:
    Unit_B();
    ~Unit_B()override {};

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

    void Update(float elapsed_time)override;
private:
    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsed_time)override;

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
};

class Unit_C :public Unit
{
public:
    Unit_C();
    ~Unit_C()override {};

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

    void Update(float elapsed_time)override;
private:
    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsed_time)override;

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;
};
