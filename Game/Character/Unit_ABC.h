#pragma once
#include "Unit.h"

// ���h�q
class Chili :public Unit
{
public:
    Chili();
    ~Chili()override {};

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

private:
    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsed_time)override;

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsed_time)override;

    // Imgui
    void DrawDebugGUI(int n)override;

private:
    bool first_attack = true;
};

class Shishito :public Unit
{
public:
    Shishito();
    ~Shishito()override {};

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

class OrangePumpkin :public Unit
{
public:
    OrangePumpkin();
    ~OrangePumpkin()override {};

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
