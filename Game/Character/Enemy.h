#pragma once
#include "Lemur/Object/Character.h"
#include "../Stage/Fence.h"
#include "Lemur/Effekseer/Effect.h"


enum EnemyType
{
    A,
    B,
    C,
    D,
    A_2,
    B_2,
    C_2,
    D_2,
    Summon,
    Boss,
    Nuisance
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
    ~Enemy() {
        attack_effect->Stop(attack_handle);
        death_effect->Stop(death_handle);
        hit_effect->Stop(hit_handle);
        attack_handle = 0;
        death_handle = 0;
        hit_handle = 0;
        // �G�t�F�N�g�I��
        if (attack_effect != nullptr)
        {
            delete attack_effect;
            attack_effect = nullptr;
        }
        if (hit_effect != nullptr)
        {
            delete hit_effect;
            hit_effect = nullptr;
        }
        if (death_effect != nullptr)
        {
            delete death_effect;
            death_effect = nullptr;
        }
    }

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
    void SetEnemyType(int type) { enemy_type = type; }
    void SetMoveTimerMax(float move_timer_max_) { move_timer_max = move_timer_max_; }
    void SetDis(float dis_) { dis = dis_; }
    void SetSpeedPowerY(float speed_power_Y_) { speed_power_Y = speed_power_Y_; }

    void SetAttackEffectSize(float attack_effect_size_) { attack_effect_size = attack_effect_size_; }
    void SetDeathEffectSize(float death_effect_size_) { death_effect_size = death_effect_size_; }
    void SetHitEffectSize(float hit_effect_size_) { hit_effect_size = hit_effect_size_; }

    // �_���[�W��^����
    bool ApplyDamage(int damage);

    void PowerUp(int add) 
    {
        if (!is_power_up)
        {
            attack_power += add;
            is_power_up = true;
        }
    }
protected:
    enum Animation
    {
        Move,
        Attack,
        Out
    };
    enum class State
    {
        Move,
        Attack,
        Death
    };

    State       state               = State::Move;      // �X�e�[�g

    Effect*     attack_effect       = nullptr;          // �U���G�t�F�N�g
    Effect*     death_effect        = nullptr;          // ���S�G�t�F�N�g
    Effect*     hit_effect          = nullptr;          // ��e�G�t�F�N�g

    Effekseer::Handle attack_handle = 0;                // �U���G�t�F�N�g
    Effekseer::Handle hit_handle    = 0;                // ��e�G�t�F�N�g
    Effekseer::Handle death_handle  = 0;                // ���S�G�t�F�N�g


    float       attack_effect_size  = 0.0f;             // �U���G�t�F�N�g�T�C�Y
    float       death_effect_size   = 0.0f;             // ���S�G�t�F�N�g�T�C�Y
    float       hit_effect_size     = 0.0f;             // �ݒu�G�t�F�N�g�T�C�Y

    int         shaft               = Shaft::Side;      // �G�̏o�Ă��鎲
    bool        is_hit_unit         = false;            // �v���C���[�ɓ���������    
    float       move_timer_max      = 0.0f;             // �ړ��Ԋu
    float       dis                 = 0.0f;             // �i�ދ���
    float       speed_power_Y       = 0.0f;             // �΂߂ɐi�ނƂ��̃X�s�[�h

    bool        is_power_up         = false;            // �p���[�A�b�v������

    int         enemy_type          = 0;
};