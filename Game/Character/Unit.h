#pragma once
#include "Enemy.h"
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Math/MathHelper.h"

class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {}

    // 更新処理
    virtual void Update(float elapsed_time);

    // Imgui
    virtual void DrawDebugGUI(int n) {};

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive() {};

    // 攻撃力を返す
    int ReturnDamage();

    // 破棄
    void Destroy();

    // Getter
    int         GetCategory() { return category; }
    float       GetDecPos() { return dec_pos; }
    Triangle    GetTriangle1() { return triangle_1; }
    Triangle    GetTriangle2() { return triangle_2; }
    Rect        GetRect() { return unit_back; }
    Rect        GetAttackRect() { return attack_rect; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetStrengAttack(int streng_attack_power_) { streng_attack_power = streng_attack_power_; }
    void SetBuff(bool is_streng_) { is_streng = is_streng_; }
    void SetSquare(Rect square_) { 
        unit_back.left_up = square_.left_up;
        unit_back.right_down = square_.right_down;
    }

protected:
    // 死亡ステートへ遷移
    virtual void TransitionDeathState();

    // 死亡ステート更新処理
    virtual void UpdateDeathState(float elapsed_time);

    // 待機ステートへ遷移
    virtual void TransitionIdleState();

    // 待機ステート更新処理
    virtual void UpdateIdleState(float elapsed_time) = 0;

    // 攻撃ステートへ遷移
    virtual void TransitionAttackState();

    // 攻撃ステート更新処理
    virtual void UpdateAttackState(float elapsed_time) = 0;

protected:
    enum Animation
    {
        Idle,
        Attack,
        Out
    };
    enum class State
    {
        Idle,   // 待機行動
        Attack,  // 攻撃行動
        Death   // 死亡
    };

    State		state                       = State::Idle;      // ステート

    float       dec_pos                     = 0.0f;             // 種がユニットに跳ね返される距離
    bool        is_attack                   = false;            // 攻撃中かどうか
    bool        is_streng                   = false;            // 強化中かどうか
    int         category                    = 0;                // ユニットのカテゴリー
    int         attack_times                = 0;                // 攻撃回数
    int         streng_attack_power         = 0;                // 強化攻撃力
    int         original_attack_power       = 0;                // 強化前 攻撃力
    Triangle    triangle_1                  = {};               // 三角攻撃範囲１
    Triangle    triangle_2                  = {};               // 三角攻撃範囲２
    Rect        attack_rect                 = {};               // ユニットの四角攻撃範囲
    Rect        unit_back                   = {};               // ユニットの後方範囲

};