#pragma once
#include "Unit.h"


// 
class GreenPumpkin :public Unit
{
public:
    GreenPumpkin();
    ~GreenPumpkin()override {};

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

class Broccoli :public Unit
{
public:
    Broccoli();
    ~Broccoli()override {};

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

class Cauliflower :public Unit
{
public:
    Cauliflower();
    ~Cauliflower()override {};

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
