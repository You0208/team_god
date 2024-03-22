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

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // インスタンス取得
    static Unit_A& Instance();
private:
    // スティック入力値から進行ベクトルを取得。
    DirectX::XMFLOAT3 GetMoveVec()const;

    // 移動入力処理
    bool InputMove(float elapsedTime);

    // プレイヤーとエネミーとの衝突処理
    void CollisionPlayerVsEnemies();

    // ノードとエネミーの衝突処理
    void CollisionNodeVsEnemies(const char* nodeName, float nodeRadius);

    // 弾丸発射処理
    void LuanchProjectile(float elapsedTime);

    // 弾丸入力処理
    bool InputProjectile();

    // 攻撃入力処理
    bool InputAttack();

    // 待機ステートへ遷移
    void TransitionIdleState();

    // 待機ステート更新処理
    void UpdateIdleState(float elapsedTime);

    // 移動ステートへ遷移
    void TransitionMoveState();

    // 移動ステート更新処理
    void UpdateMoveState(float elapsedTime);

    // ジャンプステートへ遷移
    void TransitionJumpState();

    // ジャンプステート更新処理
    void UpdateJumpState(float elapsedTime);

    // 落下ステートへ遷移
    void TransitionFallState();

    // 落下ステート更新処理
    void UpdateFallState(float elapsedTime);

    // 着地ステートへ遷移
    void TransitionLandState();

    // 着地ステート更新処理
    void UpdateLandState(float elapsedTime);

    // 攻撃ステートへ遷移
    void TransitionAttackState();

    // 攻撃ステート更新処理
    void UpdateAttackState(float elapsedTime);

    // ダメージステートへ遷移
    void TransitionDamageState();

    // ダメージステート更新処理
    void UpdateDamageState(float elapsedTime);

    // 死亡ステートへ遷移
    void TransitionDeathState();

    // 死亡ステート更新処理
    void UpdateDeathState(float elapsedTime);

    // 復活ステートへ遷移
    void TransitionReviveState();

    // 復活ステート更新処理
    void UpdateReviveState(float elapsedTime);

protected:
    // 着地したときに呼ばれる
    void OnLanding() override;

    // ダメージを受けた時に呼ばれる
    void OnDamage() override;

    // 死亡したときに呼ばれる
    void OnDead() override;
private:
    // アニメーション
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

    // ステート
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

    // 状態
    State state = State::Idle;

    float leftHandRadius = 0.4f;

    // 攻撃当たり判定するか
    bool attackCollisionFlag = false;
};


