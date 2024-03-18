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


    // 初めは徘徊ステートへ遷移
    TransitionWanderState();
}

void Enemy_A::Update(float elapsedTime)
{
    // ステート毎の更新処理
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
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // モデルアニメーション更新
    ModelEnemy_A->UpdateAnimation(elapsedTime);

    // オブジェクトの行列を更新
    UpdateTransform();


}

void Enemy_A::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
}

void Enemy_A::OnDead()
{
    // 死んだら死亡ステートに切り替え
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
    // ターゲット方向への進行ベクトルを算出
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
