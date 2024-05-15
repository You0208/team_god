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
    float   second;    //スポーン時間(秒)
    int     enemy_type;// 敵のタイプ
    int     shaft;     // 軸
    float   enemy_pos; // 敵の軸の長さに対する位置

    DirectX::XMFLOAT3 pos;// スポーン位置
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
        // エフェクト終了
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

    // 更新処理
    virtual void Update(float elapsedTime);

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive() {};

    // Imgui
    virtual void DrawDebugGUI(int n) {};

    // 破棄
    void Destroy();

    void SetShaft(int shaft_) { shaft = shaft_; }
protected:
    // 死亡ステートへ遷移
    virtual void TransitionDeathState();

    // 死亡ステート更新処理
    virtual void UpdateDeathState(float elapsed_time);

    // 攻撃ステートへ遷移
    virtual void TransitionAttackState();

    // 攻撃ステート更新処理
    virtual void UpdateAttackState(float elapsed_time) = 0;

    // 移動ステートへ遷移
    virtual void TransitionMoveState() {}

    // 移動ステート更新処理
    virtual void UpdateMoveState(float elapsed_time) = 0;

public:
    void SetEnemyType(int type) { enemy_type = type; }
    void SetMoveTimerMax(float move_timer_max_) { move_timer_max = move_timer_max_; }
    void SetDis(float dis_) { dis = dis_; }
    void SetSpeedPowerY(float speed_power_Y_) { speed_power_Y = speed_power_Y_; }

    void SetAttackEffectSize(float attack_effect_size_) { attack_effect_size = attack_effect_size_; }
    void SetDeathEffectSize(float death_effect_size_) { death_effect_size = death_effect_size_; }
    void SetHitEffectSize(float hit_effect_size_) { hit_effect_size = hit_effect_size_; }

    // ダメージを与える
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

    State       state               = State::Move;      // ステート

    Effect*     attack_effect       = nullptr;          // 攻撃エフェクト
    Effect*     death_effect        = nullptr;          // 死亡エフェクト
    Effect*     hit_effect          = nullptr;          // 被弾エフェクト

    Effekseer::Handle attack_handle = 0;                // 攻撃エフェクト
    Effekseer::Handle hit_handle    = 0;                // 被弾エフェクト
    Effekseer::Handle death_handle  = 0;                // 死亡エフェクト


    float       attack_effect_size  = 0.0f;             // 攻撃エフェクトサイズ
    float       death_effect_size   = 0.0f;             // 死亡エフェクトサイズ
    float       hit_effect_size     = 0.0f;             // 設置エフェクトサイズ

    int         shaft               = Shaft::Side;      // 敵の出てくる軸
    bool        is_hit_unit         = false;            // プレイヤーに当たったか    
    float       move_timer_max      = 0.0f;             // 移動間隔
    float       dis                 = 0.0f;             // 進む距離
    float       speed_power_Y       = 0.0f;             // 斜めに進むときのスピード

    bool        is_power_up         = false;            // パワーアップしたか

    int         enemy_type          = 0;
};