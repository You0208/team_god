#include "Enemy_A.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    ModelEnemy_A=ResourceManager::Instance().LoadModelResource(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx");

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

    // ���f���A�j���[�V�����X�V
    ModelEnemy_A->UpdateAnimation(elapsedTime);

    // �I�u�W�F�N�g�̍s����X�V
    UpdateTransform();


}

void Enemy_A::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
}

void Enemy_A::OnDead()
{
    // ���񂾂玀�S�X�e�[�g�ɐ؂�ւ�
    TransitionDeathState();
}

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
}

bool Enemy_A::SearchPlayer()
{
    return false;
}

void Enemy_A::CollisionNodeVsPlayer(const char* nodeName, float boneRadius)
{
}

void Enemy_A::TransitionWanderState()
{
}

void Enemy_A::UpdateWanderState(float elapsedTime)
{
}

void Enemy_A::TransitionIdleState()
{
}

void Enemy_A::UpdateIdleState(float elapsedTime)
{
}

void Enemy_A::TransitionPursuitState()
{
}

void Enemy_A::UpdatePursuitState(float elapsedTime)
{
}

void Enemy_A::TransitionAttackState()
{
}

void Enemy_A::UpdateAttackState(float elapsedTime)
{
}

void Enemy_A::TransitionIdleBattleState()
{
}

void Enemy_A::UpdateIdleBattleState(float elapsedTime)
{
}

void Enemy_A::TransitionDamageState()
{
}

void Enemy_A::UpdateDamageState(float elapsedTime)
{
}

void Enemy_A::TransitionDeathState()
{
}

void Enemy_A::UpdateDeathState(float elapsedTime)
{
}
