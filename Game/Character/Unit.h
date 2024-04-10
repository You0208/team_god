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

    // 破棄
    void Destroy();

    // Getter
    int         GetCategory() { return category; }
    float       GetDecPos() { return dec_pos; }
    Triangle    GetTriangle1() { return triangle_1; }
    Triangle    GetTriangle2() { return triangle_2; }
    Rect        GetRect() { return square; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetSquare(Rect square_) { 
        square.left_up = square_.left_up;
        square.right_down = square_.right_down;
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

    State		state           = State::Idle;      // ステート

    Enemy*      attack_enemy    = nullptr;          // 攻撃対象エネミー
    Unit*       buff_unit       = nullptr;          // バフ対象ユニット
    float       dec_pos         = 0.0f;             // 種がユニットに跳ね返される距離
    int         category        = 0;                // ユニットのカテゴリー
    int         attack_times    = 0;                // 攻撃回数
    Triangle    triangle_1      = {};               // 左三角攻撃範囲
    Triangle    triangle_2      = {};               // 右三角攻撃範囲
    Rect        square          = {};               // ユニットの後方範囲
};