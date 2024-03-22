#pragma once
#include "Lemur/Model/FbxModelManager.h"
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // 死亡したときに呼ばれる
    void OnDead() override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive() override;

    // 縄張り設定
    void SetTerritory(const DirectX::XMFLOAT3& origin, float range);

protected:
    // ダメージを受けたときに呼ばれる
    void OnDamage() override;

private:
    // ターゲット位置をランダム設定
    void SetRandomTargetPosition();

    // 目標地点へ移動
    void MoveToTarget(float elapsedTime, float speedRate);

    // プレイヤー索敵
    bool SearchPlayer();

    // ノードとプレイヤーの衝突処理
    void CollisionNodeVsPlayer(const char* nodeName, float boneRadius);

    // 徘徊ステートへ遷移
    void TransitionWanderState();

    // 徘徊ステート更新処理
    void UpdateWanderState(float elapsedTime);

    // 待機ステートへ遷移
    void TransitionIdleState();

    // 待機ステート更新処理
    void UpdateIdleState(float elapsedTime);

    // 追跡ステートへ遷移
    void TransitionPursuitState();

    // 追跡ステート更新処理
    void UpdatePursuitState(float elapsedTime);

    // 攻撃ステートへ遷移
    void TransitionAttackState();

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsedTime);

    // 戦闘待機ステートへ遷移
    void TransitionIdleBattleState();

    // 戦闘待機ステート更新処理
    void UpdateIdleBattleState(float elapsedTime);

    // ダメージステートへ遷移
    void TransitionDamageState();

    // ダメージステート更新処理
    void UpdateDamageState(float elapsedTime);

    // 死亡ステートへ遷移
    void TransitionDeathState();

    // 死亡ステート更新処理
    void UpdateDeathState(float elapsedTime);

private:

    // ステート
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

    // アニメーション
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

    // 最初に徘徊ステートを設定
    State state = State::Wander;

    // ターゲット
    DirectX::XMFLOAT3 targetPosition = { 0,0,0 };

    // 縄張りの原点
    DirectX::XMFLOAT3 territoryOrigin = { 0,0,0 };
    // 縄張り半径
    float territoryRange = 10.0f;
    // 縄張りを持ってるか
    bool be_terrttory = false;

    float moveSpeed = 3.0f;
    // 旋回速度
    float turnSpeed = DirectX::XMConvertToRadians(360);


    // ステート切り替え補正時間
    float stateTimer = 0.0f;

    // 索敵範囲の半径
    float searchRange = 5.0f;

    // 攻撃範囲
    float attackRange = 1.5f;

    // 攻撃当たり判定するか
    bool attackCollisionFlag = false;

    std::shared_ptr<FbxModelManager> ModelEnemy_A;
};

