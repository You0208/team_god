#include "Enemy_A.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ModelEnemy_A = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    height = 1.0f;


    // ���߂͜p�j�X�e�[�g�֑J��
    TransitionWanderState();
}

void Enemy_A::Update(float elapsedTime)
{
    // �X�e�[�g���̍X�V����
    switch (state)
    {
    case State::Wander:
        UpdateWanderState(elapsedTime);
        break;
    case State::Idle:
        UpdateIdleState(elapsedTime);
        break;
    case State::Pursuit:
        UpdatePursuitState(elapsedTime);
        break;
    case State::Attack:
        UpdateAttackState(elapsedTime);
        break;
    case State::IdleBattle:
        UpdateIdleBattleState(elapsedTime);
        break;
    case State::Damage:
        UpdateDamageState(elapsedTime);
        break;
    case State::Death:
        UpdateDeathState(elapsedTime);
        break;
    }
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // ���f���A�j���[�V�����X�V
    ModelEnemy_A->UpdateAnimation(elapsedTime);
}

void Enemy_A::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    ModelEnemy_A->Render(elapsedTime, replaced_pixel_shader);
}

void Enemy_A::OnDead()
{
    // ���񂾂玀�S�X�e�[�g�ɐ؂�ւ�
    TransitionDeathState();
}

// �f�o�b�N�v���~�e�B�u
void Enemy_A::DrawDebugPrimitive()
{
}

void Enemy_A::SetTerritory(const DirectX::XMFLOAT3& origin, float range)
{
    territoryOrigin = origin;
    territoryRange = range;
}

void Enemy_A::OnDamage()
{
    TransitionDamageState();
}

// �^�[�Q�b�g�ʒu�������_���ݒ�
void Enemy_A::SetRandomTargetPosition()
{
    targetPosition.x = Mathf::RandomRange(territoryOrigin.x - territoryRange,
        territoryOrigin.x + territoryRange);
    targetPosition.y = position.y;
    targetPosition.z = Mathf::RandomRange(territoryOrigin.z - territoryRange,
        territoryOrigin.z + territoryRange);
}

void Enemy_A::MoveToTarget(float elapsedTime, float speedRate)
{
    // �^�[�Q�b�g�����ւ̐i�s�x�N�g�����Z�o
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;

    // �������Ői�s���������肷��
    float dist = sqrtf((vx * vx) + (vz * vz));
    // ���K�����ĕ��������擾
    vx /= dist;
    vz /= dist;

    // �ړ�����
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool Enemy_A::SearchPlayer()
{
    //TODO �v���C���[�Ƃ̍��፷���l������3D�ł̋������������
    DirectX::XMFLOAT3 playerPosition = {};
    float vx = playerPosition.x - position.x;
    float vy = playerPosition.y - position.y;
    float vz = playerPosition.z - position.z;
    float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));

    // ���G�͈͓��ɓ����Ă�����
    if (dist < searchRange)
    {
        // ����͈͓��ɓ����Ă��邩����

        float distXZ = sqrtf((vx * vx) + (vz * vz));

        // �P�ʃx�N�g����
        vx /= distXZ;
        vz /= distXZ;

        // �O���x�N�g��
        float frontX = sinf(rotation.y);
        float frontZ = cosf(rotation.y);

        // ��̃x�N�g���̓��ςőO�㔻��
        float dot = (frontX * vx) + (frontZ * vz);
        if (dot > 0.0f)return true;
    }
    return false;
}

void Enemy_A::CollisionNodeVsPlayer(const char* nodeName, float boneRadius)
{

}

void Enemy_A::TransitionWanderState()
{
    state = State::Wander;

    // �ڕW�n�_�ݒ�
    SetRandomTargetPosition();

    // �����A�j���[�V�����Đ�
    ModelEnemy_A->PlayAnimation(Anim_WalkFWD, true);
}

void Enemy_A::UpdateWanderState(float elapsedTime)
{
    // �ڕW�n�_�܂�XZ���ʂł̋�������
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    // �O�����̒藝�ŋ������߂�
    float distSq = (vx * vx) + (vz * vz);

    // �ڕW�n�_�ɓ��B������
    if (distSq < radius * radius)
    {
        // �ҋ@�X�e�[�g�֑J��
        TransitionIdleState();
    }

    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 0.5f);

    // ���G
    if (SearchPlayer())
    {
        // ����������ǐՃX�e�[�g�ɑJ��
        TransitionPursuitState();
    }
}

void Enemy_A::TransitionIdleState()
{
    state = State::Idle;

    // �ҋ@�^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // �ҋ@�A�j���[�V�����Đ�
    ModelEnemy_A->PlayAnimation(Anim_IdleNormal, true);
}

void Enemy_A::UpdateIdleState(float elapsedTime)
{
    // �^�C�}�[����
    stateTimer -= elapsedTime;

    if (stateTimer < 0.0f)
    {
        // �p�j�X�e�[�g�֑J��
        TransitionWanderState();
    }
}

void Enemy_A::TransitionPursuitState()
{
    state = State::Pursuit;

    // ���b�ԒǐՂ���^�C�}�[�������_���ݒ�
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    ModelEnemy_A->PlayAnimation(Anim_RunFWD, true);
}

void Enemy_A::UpdatePursuitState(float elapsedTime)
{
    //TODO �ڕW�n�_���v���C���[�̈ʒu�ɐݒ�
    targetPosition = {};

    // �ڕW�n�_�ֈړ�
    MoveToTarget(elapsedTime, 1.0f);

    // �v���C���[�߂��܂ŗ����
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));
    if (dist < attackRange)
    {
        // �U���X�e�[�g�ɐ؂�ւ�
        // todo �����ȃp�^�[���̍U��������G�̏ꍇ�͂��̃^�C�~���O�ŏ�����������H
        TransitionAttackState();
    }
}

void Enemy_A::TransitionAttackState()
{
    state = State::Attack;

    //�U���A�j���[�V�����Đ�
    ModelEnemy_A->PlayAnimation(Anim_Attack1, false);
}

void Enemy_A::UpdateAttackState(float elapsedTime)
{
    float attackStart = 0.25f;
    float attackEnd = 0.35f;
    float animeTime = ModelEnemy_A->GetCurrentAnimationSecond();

    attackCollisionFlag = (animeTime > attackStart && animeTime < attackEnd);

    if (attackCollisionFlag)
    {
        // �ڋʃm�[�h�ƃv���C���[�̏Փˏ���
        CollisionNodeVsPlayer("EyeBall", 0.2f);
    }

    // �U���A�j���[�V�������I�������퓬�ҋ@�X�e�[�g�֑J��
    if (!ModelEnemy_A->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

void Enemy_A::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    // ���b�ҋ@���邩�����_���ݒ�
    stateTimer = Mathf::RandomRange(2.0f, 3.0f);

    // �퓬�ҋ@�A�j���[�V�����Đ�
    ModelEnemy_A->PlayAnimation(Anim_IdleBattle, true);
}

void Enemy_A::UpdateIdleBattleState(float elapsedTime)
{
    //TODO �^�[�Q�b�g�ʒu���v���C���[�ɐݒ�
    targetPosition = {};

    stateTimer -= elapsedTime;
    if (stateTimer < 0)
    {
        // �v���C���[���U���͈͓��ɋ�����
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));
        if (dist < attackRange)
        {
            // �U���X�e�[�g�֑J��
            TransitionAttackState();
        }
        else
        {
            // ���Ȃ�������p�j�X�e�[�g�֑J��
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

void Enemy_A::TransitionDamageState()
{
    state = State::Damage;

    // �_���[�W�A�j���[�V�����Đ�
    ModelEnemy_A->PlayAnimation(Anim_GetHit, false);
}

void Enemy_A::UpdateDamageState(float elapsedTime)
{
    // �_���[�W�A�j���[�V�������I�������
    if (!ModelEnemy_A->IsPlayAnimation())
        // �퓬�ҋ@�X�e�[�g�ɑJ��
        TransitionIdleBattleState();
}

void Enemy_A::TransitionDeathState()
{
    state = State::Death;

    // ���S�A�j���[�V�����Đ�
    ModelEnemy_A->PlayAnimation(Anim_Die, false);
}

void Enemy_A::UpdateDeathState(float elapsedTime)
{
    // ���S�A�j���[�V�������I�������
    if (!ModelEnemy_A->IsPlayAnimation())
        // ������j��
        Destroy();
}
