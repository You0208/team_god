#pragma once
#include "Lemur/Model/FbxModelManager.h"
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

private:

    // �X�e�[�g
    enum class State
    {
        Wander,
        Idle,
        Pursuit,
        Attack,
        IdleBattle,
        Damage,
        Death,
    };

    // �A�j���[�V����
    enum Animation
    {
        Anim_IdleNormal,
        Anim_IdleBattle,
        Anim_Attack1,
        Anim_Attack2,
        Anim_WalkFWD,
        Anim_WalkBWD,
        Anim_WalkLeft,
        Anim_WalkRight,
        Anim_RunFWD,
        Anim_SenseSomthingST,
        Anim_SenseSomthingPRT,
        Anim_Taunt,
        Anim_Victory,
        Anim_GetHit,
        Anim_Dizzy,
        Anim_Die,
    };
private:

    // �ŏ��ɜp�j�X�e�[�g��ݒ�
    State state = State::Wander;

    // �^�[�Q�b�g
    DirectX::XMFLOAT3 targetPosition = { 0,0,0 };

    // �꒣��̌��_
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    // �꒣�蔼�a
    float territoryRange = 10.0f;
    // �꒣��������Ă邩
    bool be_terrttory = false;

    float moveSpeed = 3.0f;
    // ���񑬓x
    float turnSpeed = DirectX::XMConvertToRadians(360);


    // �X�e�[�g�؂�ւ��␳����
    float stateTimer = 0.0f;

    // ���G�͈͂̔��a
    float searchRange = 5.0f;

    // �U���͈�
    float attackRange = 1.5f;

    // �U�������蔻�肷�邩
    bool attackCollisionFlag = false;

    std::shared_ptr<FbxModelManager> model_enemy_A;
};

