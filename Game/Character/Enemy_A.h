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

    // ���S�����Ƃ��ɌĂ΂��
    void OnDead() override;

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive() override;

    // �꒣��ݒ�
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

protected:
    // �_���[�W���󂯂��Ƃ��ɌĂ΂��
    void OnDamage() override;

private:
    // �^�[�Q�b�g�ʒu�������_���ݒ�
    void SetRandomTargetPosition();

    // �ڕW�n�_�ֈړ�
    void MoveToTarget(float elapsedTime, float speedRate);

    // �v���C���[���G
    bool SearchPlayer();

    // �m�[�h�ƃv���C���[�̏Փˏ���
    void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

    // �p�j�X�e�[�g�֑J��
    void TransitionWanderState();

    // �p�j�X�e�[�g�X�V����
    void UpdateWanderState(float elapsedTime);

    // �ҋ@�X�e�[�g�֑J��
    void TransitionIdleState();

    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsedTime);

    // �ǐՃX�e�[�g�֑J��
    void TransitionPursuitState();

    // �ǐՃX�e�[�g�X�V����
    void UpdatePursuitState(float elapsedTime);

    // �U���X�e�[�g�֑J��
    void TransitionAttackState();

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsedTime);

    // �퓬�ҋ@�X�e�[�g�֑J��
    void TransitionIdleBattleState();

    // �퓬�ҋ@�X�e�[�g�X�V����
    void UpdateIdleBattleState(float elapsedTime);

    // �_���[�W�X�e�[�g�֑J��
    void TransitionDamageState();

    // �_���[�W�X�e�[�g�X�V����
    void UpdateDamageState(float elapsedTime);

    // ���S�X�e�[�g�֑J��
    void TransitionDeathState();

    // ���S�X�e�[�g�X�V����
    void UpdateDeathState(float elapsedTime);

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

    std::shared_ptr<FbxModelManager> ModelEnemy_A;
};

