#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Effekseer/Effect.h"
#include "Lemur/Math/MathHelper.h"


class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {
        // �G�t�F�N�g�I��
        if (attack_effect != nullptr)
        {
            delete attack_effect;
            attack_effect = nullptr;
        }
        if (set_effect != nullptr)
        {
            delete set_effect;
            set_effect = nullptr;
        }
        if (death_effect != nullptr)
        {
            delete death_effect;
            death_effect = nullptr;
        }    
    }

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
    float       GetTriangleHeight() { return t_height; }
    float       GetTriangleBase() { return t_base; }
    Triangle    GetTriangle1() { return triangle_1; }
    Triangle    GetTriangle2() { return triangle_2; }
    Rect        GetRect() { return unit_back; }
    Rect        GetAttackRect() { return attack_rect; }
    float       GetAttackRadiusIn() { return attack_radius_in; }
    Effect*     GetSetEffect() { return set_effect; };
    float       GetSetEffectSize() { return set_effect_size; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetStrengAttack(int streng_attack_power_) { streng_attack_power = streng_attack_power_; }
    void SetBuff(bool is_streng_) { is_streng = is_streng_; }
    void SetSquare(Rect square_) { 
        unit_back.left_up = square_.left_up;
        unit_back.right_down = square_.right_down;
    }
    void SetTimerMax(float timer_max_) { timer_max = timer_max_; }
    void SetStrengWidth(int streng_width_) { streng_width = streng_width_; }
    void SetAttackTimes(int attack_times_) { attack_times = attack_times_; }
    void SetAttackWidth(float attack_width_) { attack_width = attack_width_; }
    void SetAttackWidth(DirectX::XMFLOAT2 attack_width_) { attack_rect.width = attack_width_; }
    void SetTHeight(float t_height_) { t_height = t_height_; }
    void SetTBase(float t_base_) { t_base = t_base_; }
    void SetRectAngle(float rect_angle_) { rect_angle = rect_angle_; }
    void SetAttackRadiusIn(float attack_radius_in_) { attack_radius_in = attack_radius_in_; }

    void SetAttackEffectSize(float attack_effect_size_) { attack_effect_size = attack_effect_size_; }
    void SetDeathEffectSize(float death_effect_size_) { death_effect_size = death_effect_size_; }
    void SetSetEffectSize(float set_effect_size_) { set_effect_size = set_effect_size_; }


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
    float       t_height                    = 0.0f;             // �O�p�`�̍���
    float       t_base                      = 0.0f;             // �O�p�`�̒�Ӓ�
    Triangle    triangle_1                  = {};               // �O�p�U���͈͂P
    Triangle    triangle_2                  = {};               // �O�p�U���͈͂Q
    Rect        attack_rect                 = {};               // ���j�b�g�̎l�p�U���͈�
    Rect        unit_back                   = {};               // ���j�b�g�̌���͈�
    float       attack_timer                = 0.0f;             // �U���Ԋu�^�C�}�[
    float       rect_angle                  = 0.0f;             // ��]��`�p�x

    float       timer_max                   = 0.0f;             // �^�C�}�[�̍ő�
    int         streng_width                = 0.0f;             // ������
    float       attack_width                = 0.0f;             // �l�p�̕�
    float       attack_radius_in            = 0.0f;             // �h�[�i�c�^�p�̒��g�~���a

    Effekseer::Handle attack_effect_handle  = 0;
    Effect*     attack_effect               = nullptr;          // �U���G�t�F�N�g
    Effect*     death_effect                = nullptr;          // ���S�G�t�F�N�g
    Effect*     set_effect                  = nullptr;          // �ݒu�G�t�F�N�g

    float       attack_effect_size          = 0.0f;             // �U���G�t�F�N�g�T�C�Y
    float       death_effect_size           = 0.0f;             // ���S�G�t�F�N�g�T�C�Y
    float       set_effect_size             = 0.0f;             // �ݒu�G�t�F�N�g�T�C�Y

};