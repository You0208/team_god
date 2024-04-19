#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "../Stage/Fence.h"

enum EnemyType
{
    A,
    B,
    C,
    D
};

enum Shaft
{
    Side,
    Vertical
};

struct EnemyScript
{
    float   second;    //�X�|�[������(�b)
    int     enemy_type;// �G�̃^�C�v
    int     shaft;     // ��
    float   enemy_pos; // �G�̎��̒����ɑ΂���ʒu

    DirectX::XMFLOAT3 pos;// �X�|�[���ʒu
};



class Enemy :public Character
{
public:
    Enemy() {}
    ~Enemy() {}

    // �X�V����
    virtual void Update(float elapsedTime);

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive() {};

    // Imgui
    virtual void DrawDebugGUI(int n) {};

    // �j��
    void Destroy();

    void SetShaft(int shaft_) { shaft = shaft_; }
protected:
    // ���S�X�e�[�g�֑J��
    virtual void TransitionDeathState();

    // ���S�X�e�[�g�X�V����
    virtual void UpdateDeathState(float elapsed_time);

    // �U���X�e�[�g�֑J��
    virtual void TransitionAttackState();

    // �U���X�e�[�g�X�V����
    virtual void UpdateAttackState(float elapsed_time) = 0;

    // �ړ��X�e�[�g�֑J��
    virtual void TransitionMoveState() {}

    // �ړ��X�e�[�g�X�V����
    virtual void UpdateMoveState(float elapsed_time) = 0;

public:
    void SetMoveTimerMax(float move_timer_max_) { move_timer_max = move_timer_max_; }
    void SetDis(float dis_) { dis = dis_; }
    void SetSpeedPowerY(float speed_power_Y_) { speed_power_Y = speed_power_Y_; }

protected:
    enum Animation
    {
        Move,
        Out,
        Attack
    };
    enum class State
    {
        Move,
        Attack,
        Death
    };

    State   state               = State::Move;  // �X�e�[�g

    int     shaft               = Shaft::Side;  // �G�̏o�Ă��鎲
    bool    is_hit_unit         = false;        // �v���C���[�ɓ���������    
    float   move_timer_max      = 0.0f;         // �ړ��Ԋu
    float   dis                 = 0.0f;         // �i�ދ���
    float   speed_power_Y     = 0.0f;         // �΂߂ɐi�ނƂ��̃X�s�[�h
};