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
    float timer;// �^�C�}�[
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
    float   old_position   = 0.0f;      // �ړ��p�̈ʒu�ۑ�
    float   dis            = 1.0f;      // �i�ދ���
    float   attack_timer   = 1.0f;      // �U���Ԋu
    float   move_timer     = 0.0f;      // �ړ��^�C�}�[
    float   move_timer_max = 1.0f;      // �ړ��Ԋu
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
    void UpdateMoveState_S(float elapsed_time);// �������ړ�
    void UpdateMoveState_V(float elapsed_time);// �c�����ړ�
    // ���j�b�g�Ƃ̓����蔻��
    void JudgeUnit_S();
    void JudgeUnit_V();
private:
    enum Move
    {
        Straight,
        Avoid
    };
    enum UnitDestDir
    {
        Back,
        Front,
        Right,
        Left
    };

    float   timer                       = 0.0f;      // �^�C�}�[
    int     move_state                  = Straight;  // �ړ��X�e�[�g
    bool    is_touched_unit             = false;     // ���j�b�g�ɐG�ꂽ��
    int     unit_edst_dir               = 0;
    DirectX::XMFLOAT2   destination     = { 0,0 };   // �ړI�n
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
    enum Move
    {
        Straight,
        Diagonal
    };
    enum Direction
    {
        Under,
        Up
    };

    int     move_state      = Move::Straight;         // �ړ��̏��
    int     direction_state = Direction::Under;       // �����̏��
    float   dis             = 0.0f;                   // �ړ���
    float   dis_max         = 1.0f;                   // �ړ��ʋK��l
    float   speed_power_Y   = 10.0f;                  // Y�����̈ړ���
    bool    is_last_touched = false;                  // �O��̈ړ��ō�ɐG�ꂽ��

    float   attack_timer    = 1.0f;                   // �U���Ԋu
};