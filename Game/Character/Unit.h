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

    // �U���͂�Ԃ�
    int ReturnDamage();

    // �j��
    void Destroy();

    // Getter
    int         GetCategory() { return category; }
    float       GetDecPos() { return dec_pos; }
    Triangle    GetTriangle1() { return triangle_1; }
    Triangle    GetTriangle2() { return triangle_2; }
    Rect        GetRect() { return unit_back; }
    Rect        GetAttackRect() { return attack_rect; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetStrengAttack(int streng_attack_power_) { streng_attack_power = streng_attack_power_; }
    void SetBuff(bool is_streng_) { is_streng = is_streng_; }
    void SetSquare(Rect square_) { 
        unit_back.left_up = square_.left_up;
        unit_back.right_down = square_.right_down;
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

    State		state                       = State::Idle;      // �X�e�[�g

    float       dec_pos                     = 0.0f;             // �킪���j�b�g�ɒ��˕Ԃ���鋗��
    bool        is_attack                   = false;            // �U�������ǂ���
    bool        is_streng                   = false;            // ���������ǂ���
    int         category                    = 0;                // ���j�b�g�̃J�e�S���[
    int         attack_times                = 0;                // �U����
    int         streng_attack_power         = 0;                // �����U����
    int         original_attack_power       = 0;                // �����O �U����
    Triangle    triangle_1                  = {};               // �O�p�U���͈͂P
    Triangle    triangle_2                  = {};               // �O�p�U���͈͂Q
    Rect        attack_rect                 = {};               // ���j�b�g�̎l�p�U���͈�
    Rect        unit_back                   = {};               // ���j�b�g�̌���͈�

};