#pragma once
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // �U��
    void UpdateAttackState(float elapsed_time)override;
    // �ړ�
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
private:
    float timer;
};

class Enemy_B :public Enemy
{
public:
    Enemy_B();
    ~Enemy_B()override {};
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // �U��
    void UpdateAttackState(float elapsed_time)override;
    // �ړ�
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
private:
    float old_position;// �ړ��p�̈ʒu�ۑ�
    float dis;// �i�ދ���
    float attack_timer;// �U���Ԋu
    float move_timer;// �ړ��^�C�}�[
    float move_timer_max;// �ړ��Ԋu
};

class Enemy_C :public Enemy
{
public:
    Enemy_C();
    ~Enemy_C()override {};
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // �U��
    void UpdateAttackState(float elapsed_time)override;
    // �ړ�
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;

    void JudgeUnit();
private:
    DirectX::XMFLOAT2 destination = { 0,0 };// �ړI�n

    float timer = 0.0f;// �^�C�}�[
    int move_state = 0;// �ړ��X�e�[�g
    bool is_touched_unit = false;// ���j�b�g�ɐG�ꂽ��
};

class Enemy_D :public Enemy
{
public:
    Enemy_D();
    ~Enemy_D()override {};
    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // �U��
    void UpdateAttackState(float elapsed_time)override;
    // �ړ�
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
private:
    int move_state;             // �ړ��̏��
    float dis;                  // �ړ���
    float dis_max;              // �ړ��ʋK��l
    float speed_power_Y = 10.0f;// Y�����̈ړ���
    int direction_state = 0;    // �����̏��
    bool is_last_touched;       // �O��̈ړ��ō�ɐG�ꂽ��

    float attack_timer;         // �U���Ԋu
};