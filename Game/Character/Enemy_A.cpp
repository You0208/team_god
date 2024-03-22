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

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // モデルアニメーション更新
    ModelEnemy_A->UpdateAnimation(elapsedTime);
}

void Enemy_A::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    ModelEnemy_A->Render(elapsedTime, replaced_pixel_shader);
}

void Enemy_A::OnDead()
{
    // 死んだら死亡ステートに切り替え
    TransitionDeathState();
}

// デバックプリミティブ
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

// ターゲット位置をランダム設定
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
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;

    // 平方根で進行方向を決定する
    float dist = sqrtf((vx * vx) + (vz * vz));
    // 正規化して方向だけ取得
    vx /= dist;
    vz /= dist;

    // 移動処理
    Move(vx, vz, moveSpeed * speedRate);
    Turn(elapsedTime, vx, vz, turnSpeed * speedRate);
}

bool Enemy_A::SearchPlayer()
{
    //TODO プレイヤーとの高低差を考慮して3Dでの距離判定をする
    DirectX::XMFLOAT3 playerPosition = {};
    float vx = playerPosition.x - position.x;
    float vy = playerPosition.y - position.y;
    float vz = playerPosition.z - position.z;
    float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));

    // 索敵範囲内に入っていたら
    if (dist < searchRange)
    {
        // 視野範囲内に入っているか判定

        float distXZ = sqrtf((vx * vx) + (vz * vz));

        // 単位ベクトル化
        vx /= distXZ;
        vz /= distXZ;

        // 前方ベクトル
        float frontX = sinf(rotation.y);
        float frontZ = cosf(rotation.y);

        // 二つのベクトルの内積で前後判定
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

    // 目標地点設定
    SetRandomTargetPosition();

    // 歩きアニメーション再生
    ModelEnemy_A->PlayAnimation(Anim_WalkFWD, true);
}

void Enemy_A::UpdateWanderState(float elapsedTime)
{
    // 目標地点までXZ平面での距離判定
    float vx = targetPosition.x - position.x;
    float vz = targetPosition.z - position.z;
    // 三平方の定理で距離求める
    float distSq = (vx * vx) + (vz * vz);

    // 目標地点に到達したら
    if (distSq < radius * radius)
    {
        // 待機ステートへ遷移
        TransitionIdleState();
    }

    // 目標地点へ移動
    MoveToTarget(elapsedTime, 0.5f);

    // 索敵
    if (SearchPlayer())
    {
        // 見つかったら追跡ステートに遷移
        TransitionPursuitState();
    }
}

void Enemy_A::TransitionIdleState()
{
    state = State::Idle;

    // 待機タイマーをランダム設定
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    // 待機アニメーション再生
    ModelEnemy_A->PlayAnimation(Anim_IdleNormal, true);
}

void Enemy_A::UpdateIdleState(float elapsedTime)
{
    // タイマー処理
    stateTimer -= elapsedTime;

    if (stateTimer < 0.0f)
    {
        // 徘徊ステートへ遷移
        TransitionWanderState();
    }
}

void Enemy_A::TransitionPursuitState()
{
    state = State::Pursuit;

    // 数秒間追跡するタイマーをランダム設定
    stateTimer = Mathf::RandomRange(3.0f, 5.0f);

    ModelEnemy_A->PlayAnimation(Anim_RunFWD, true);
}

void Enemy_A::UpdatePursuitState(float elapsedTime)
{
    //TODO 目標地点をプレイヤーの位置に設定
    targetPosition = {};

    // 目標地点へ移動
    MoveToTarget(elapsedTime, 1.0f);

    // プレイヤー近くまで来ると
    float vx = targetPosition.x - position.x;
    float vy = targetPosition.y - position.y;
    float vz = targetPosition.z - position.z;
    float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));
    if (dist < attackRange)
    {
        // 攻撃ステートに切り替え
        // todo いろんなパターンの攻撃をする敵の場合はこのタイミングで処理分けする？
        TransitionAttackState();
    }
}

void Enemy_A::TransitionAttackState()
{
    state = State::Attack;

    //攻撃アニメーション再生
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
        // 目玉ノードとプレイヤーの衝突処理
        CollisionNodeVsPlayer("EyeBall", 0.2f);
    }

    // 攻撃アニメーションが終わったら戦闘待機ステートへ遷移
    if (!ModelEnemy_A->IsPlayAnimation())
    {
        TransitionIdleBattleState();
    }
}

void Enemy_A::TransitionIdleBattleState()
{
    state = State::IdleBattle;

    // 何秒待機するかランダム設定
    stateTimer = Mathf::RandomRange(2.0f, 3.0f);

    // 戦闘待機アニメーション再生
    ModelEnemy_A->PlayAnimation(Anim_IdleBattle, true);
}

void Enemy_A::UpdateIdleBattleState(float elapsedTime)
{
    //TODO ターゲット位置をプレイヤーに設定
    targetPosition = {};

    stateTimer -= elapsedTime;
    if (stateTimer < 0)
    {
        // プレイヤーが攻撃範囲内に居たら
        float vx = targetPosition.x - position.x;
        float vy = targetPosition.y - position.y;
        float vz = targetPosition.z - position.z;
        float dist = sqrtf((vx * vx) + (vy * vy) + (vz * vz));
        if (dist < attackRange)
        {
            // 攻撃ステートへ遷移
            TransitionAttackState();
        }
        else
        {
            // いなかったら徘徊ステートへ遷移
            TransitionWanderState();
        }
    }
    MoveToTarget(elapsedTime, 0.0f);
}

void Enemy_A::TransitionDamageState()
{
    state = State::Damage;

    // ダメージアニメーション再生
    ModelEnemy_A->PlayAnimation(Anim_GetHit, false);
}

void Enemy_A::UpdateDamageState(float elapsedTime)
{
    // ダメージアニメーションが終わったら
    if (!ModelEnemy_A->IsPlayAnimation())
        // 戦闘待機ステートに遷移
        TransitionIdleBattleState();
}

void Enemy_A::TransitionDeathState()
{
    state = State::Death;

    // 死亡アニメーション再生
    ModelEnemy_A->PlayAnimation(Anim_Die, false);
}

void Enemy_A::UpdateDeathState(float elapsedTime)
{
    // 死亡アニメーションが終わったら
    if (!ModelEnemy_A->IsPlayAnimation())
        // 自分を破棄
        Destroy();
}
