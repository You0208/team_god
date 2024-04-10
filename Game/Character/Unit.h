#pragma once
#include "Enemy.h"
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Math/MathHelper.h"

class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {}

    // �X�V����
    virtual void Update(float elapsed_time);

    // Imgui
    virtual void DrawDebugGUI(int n) {};

    // �f�o�b�O�v���~�e�B�u�`��
    virtual void DrawDebugPrimitive() {};

    // �j��
    void Destroy();

    // Getter
    int GetCategory() { return category; }
    float GetAttackRadius() { return attack_radius; }
    float GetDecPos() { return dec_pos; }
    Triangle GetTriangle1() { return triangle_1; }
    Triangle GetTriangle2() { return triangle_2; }
    Rect GetRect() { return square; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetSquare(Rect square_) { 
        square.left_up = square_.left_up;
        square.right_down = square_.right_down;
    }

protected:
    // ���S�X�e�[�g�֑J��
    virtual void TransitionDeathState();

    // ���S�X�e�[�g�X�V����
    virtual void UpdateDeathState(float elapsed_time);

    // �ҋ@�X�e�[�g�֑J��
    virtual void TransitionIdleState();

    // �ҋ@�X�e�[�g�X�V����
    virtual void UpdateIdleState(float elapsed_time) = 0;

    // �U���X�e�[�g�֑J��
    virtual void TransitionAttackState();

    // �U���X�e�[�g�X�V����
    virtual void UpdateAttackState(float elapsed_time) = 0;

protected:
    enum Animation
    {
        Idle,
        Attack,
        Out
    };
    enum class State
    {
        Idle,   // �ҋ@�s��
        Attack,  // �U���s��
        Death   // ���S
    };
    State				state = State::Idle;

    Enemy* attack_enemy;    // �U���ΏۃG�l�~�[
    Unit* buff_unit;        // �o�t�Ώۃ��j�b�g
    float dec_pos;          // �킪���j�b�g�ɒ��˕Ԃ���鋗��
    int category;           // ���j�b�g�̃J�e�S���[
    int attack_times;       // �U����
    float attack_radius;    // �~�U���͈͔��a
    Triangle triangle_1;    // ���O�p�U���͈�
    Triangle triangle_2;    // �E�O�p�U���͈�
    Rect square;            // ���j�b�g�̌���͈�
};