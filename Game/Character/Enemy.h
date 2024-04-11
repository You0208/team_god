#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "../Stage/Fence.h"


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

protected:
    enum Animation
    {
        Move = 0,
        Attack = 1,
        Out = 6
    };
    enum class State
    {
        Move,
        Attack,
        Death
    };
    State state         = State::Move; // �X�e�[�g

    bool is_hit_unit    = false;       // �v���C���[�ɓ���������    
};