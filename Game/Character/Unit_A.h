#pragma once
#include "Unit.h"
#include "Lemur/Model/FbxModelManager.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    // �C���X�^���X�擾
    static Unit_A& Instance();
private:
    // �X�e�B�b�N���͒l����i�s�x�N�g�����擾�B
    DirectX::XMFLOAT3 GetMoveVec()const;

    // �ړ����͏���
    bool InputMove(float elapsedTime);

    // �v���C���[�ƃG�l�~�[�Ƃ̏Փˏ���
    void CollisionPlayerVsEnemies();

    // �m�[�h�ƃG�l�~�[�̏Փˏ���
    void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

    // �e�۔��ˏ���
    void LuanchProjectile(float elapsedTime);

    // �e�ۓ��͏���
    bool InputProjectile();

    // �U�����͏���
    bool InputAttack();

    // �ҋ@�X�e�[�g�֑J��
    void TransitionIdleState();

    // �ҋ@�X�e�[�g�X�V����
    void UpdateIdleState(float elapsedTime);

    // �ړ��X�e�[�g�֑J��
    void TransitionMoveState();

    // �ړ��X�e�[�g�X�V����
    void UpdateMoveState(float elapsedTime);

    // �W�����v�X�e�[�g�֑J��
    void TransitionJumpState();

    // �W�����v�X�e�[�g�X�V����
    void UpdateJumpState(float elapsedTime);

    // �����X�e�[�g�֑J��
    void TransitionFallState();

    // �����X�e�[�g�X�V����
    void UpdateFallState(float elapsedTime);

    // ���n�X�e�[�g�֑J��
    void TransitionLandState();

    // ���n�X�e�[�g�X�V����
    void UpdateLandState(float elapsedTime);

    // �U���X�e�[�g�֑J��
    void TransitionAttackState();

    // �U���X�e�[�g�X�V����
    void UpdateAttackState(float elapsedTime);

    // �_���[�W�X�e�[�g�֑J��
    void TransitionDamageState();

    // �_���[�W�X�e�[�g�X�V����
    void UpdateDamageState(float elapsedTime);

    // ���S�X�e�[�g�֑J��
    void TransitionDeathState();

    // ���S�X�e�[�g�X�V����
    void UpdateDeathState(float elapsedTime);

    // �����X�e�[�g�֑J��
    void TransitionReviveState();

    // �����X�e�[�g�X�V����
    void UpdateReviveState(float elapsedTime);

protected:
    // ���n�����Ƃ��ɌĂ΂��
    void OnLanding() override;

    // �_���[�W���󂯂����ɌĂ΂��
    void OnDamage() override;

    // ���S�����Ƃ��ɌĂ΂��
    void OnDead() override;
private:
    // �A�j���[�V����
    enum Animation
    {
        Anim_Attack,
        Anim_Death,
        Anim_Falling,
        Anim_GetHit1,
        Anim_GetHit2,
        Anim_Idle,
        Anim_Jump,
        Anim_Jump_Flip,
        Anim_Landing,
        Anim_Revive,
        Anim_Running,
        Anim_Walking
    };

    // �X�e�[�g
    enum class State
    {
        Idle,
        Move,
        Jump,
        Fall,
        Land,
        Attack,
        Damage,
        Death,
        Revive,
    };

private:
    std::shared_ptr<FbxModelManager> model_unit_A;
    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    float jumpSpeed = 20.0f;

    int jumpCount = 0;
    int jumpLimit = 2;

    float LuanchRate = 0.1f;
    float LuanchCollTime = 0.1f;

    // ���
    State state = State::Idle;

    float leftHandRadius = 0.4f;

    // �U�������蔻�肷�邩
    bool attackCollisionFlag = false;
};


