#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Effekseer/Effect.h"
#include "Lemur/Math/MathHelper.h"


class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {
        // エフェクト終了
        if (attack_effect != nullptr)
        {
            delete attack_effect;
            attack_effect = nullptr;
        }
        if (set_effect != nullptr)
        {
            delete set_effect;
            set_effect = nullptr;
        }
        if (death_effect != nullptr)
        {
            delete death_effect;
            death_effect = nullptr;
        }    
    }

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
    float       GetTriangleHeight() { return t_height; }
    float       GetTriangleBase() { return t_base; }
    Triangle    GetTriangle1() { return triangle_1; }
    Triangle    GetTriangle2() { return triangle_2; }
    Rect        GetRect() { return unit_back; }
    Rect        GetAttackRect() { return attack_rect; }
    float       GetAttackRadiusIn() { return attack_radius_in; }
    Effect*     GetSetEffect() { return set_effect; };
    float       GetSetEffectSize() { return set_effect_size; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetStrengAttack(int streng_attack_power_) { streng_attack_power = streng_attack_power_; }
    void SetBuff(bool is_streng_) { is_streng = is_streng_; }
    void SetSquare(Rect square_) { 
        unit_back.left_up = square_.left_up;
        unit_back.right_down = square_.right_down;
    }
    void SetTimerMax(float timer_max_) { timer_max = timer_max_; }
    void SetStrengWidth(int streng_width_) { streng_width = streng_width_; }
    void SetAttackTimes(int attack_times_) { attack_times = attack_times_; }
    void SetAttackWidth(float attack_width_) { attack_width = attack_width_; }
    void SetAttackWidth(DirectX::XMFLOAT2 attack_width_) { attack_rect.width = attack_width_; }
    void SetTHeight(float t_height_) { t_height = t_height_; }
    void SetTBase(float t_base_) { t_base = t_base_; }
    void SetRectAngle(float rect_angle_) { rect_angle = rect_angle_; }
    void SetAttackRadiusIn(float attack_radius_in_) { attack_radius_in = attack_radius_in_; }

    void SetAttackEffectSize(float attack_effect_size_) { attack_effect_size = attack_effect_size_; }
    void SetDeathEffectSize(float death_effect_size_) { death_effect_size = death_effect_size_; }
    void SetSetEffectSize(float set_effect_size_) { set_effect_size = set_effect_size_; }


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
    float       t_height                    = 0.0f;             // 三角形の高さ
    float       t_base                      = 0.0f;             // 三角形の底辺長
    Triangle    triangle_1                  = {};               // 三角攻撃範囲１
    Triangle    triangle_2                  = {};               // 三角攻撃範囲２
    Rect        attack_rect                 = {};               // ユニットの四角攻撃範囲
    Rect        unit_back                   = {};               // ユニットの後方範囲
    float       attack_timer                = 0.0f;             // 攻撃間隔タイマー
    float       rect_angle                  = 0.0f;             // 回転矩形角度

    float       timer_max                   = 0.0f;             // タイマーの最大
    int         streng_width                = 0.0f;             // 強化幅
    float       attack_width                = 0.0f;             // 四角の幅
    float       attack_radius_in            = 0.0f;             // ドーナツ型用の中身円半径

    Effekseer::Handle attack_effect_handle  = 0;
    Effect*     attack_effect               = nullptr;          // 攻撃エフェクト
    Effect*     death_effect                = nullptr;          // 死亡エフェクト
    Effect*     set_effect                  = nullptr;          // 設置エフェクト

    float       attack_effect_size          = 0.0f;             // 攻撃エフェクトサイズ
    float       death_effect_size           = 0.0f;             // 死亡エフェクトサイズ
    float       set_effect_size             = 0.0f;             // 設置エフェクトサイズ

};