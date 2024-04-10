#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Math/MathHelper.h"

class Unit :public Character
{
public:
    Unit() {}
    ~Unit() {}

    // 更新処理
    virtual void Update(float elapsed_time) = 0;

    // 描画処理
    virtual void Render(float elapsed_time, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // 破棄
    void Destroy();

    // Getter
    int GetCategory() { return category; }
    float GetAttackRadius() { return attack_radius; }
    float GetDecPos() { return dec_pos; }
    Triangle GetTriangle1() { return triangle_1; }
    Triangle GetTriangle2() { return triangle_2; }
    Rect GetRect() { return square; }

    // Setter
    void SetCategory(int category_) { category = category_; }
    void SetSquare(Rect square_) { 
        square.left_up = square_.left_up;
        square.right_down = square_.right_down;
    }

protected:

    // 待機ステートへ遷移
    virtual void TransitionIdleState() = 0;

    // 待機ステート更新処理
    virtual void UpdateIdleState(float elapsed_time) = 0;

    // 攻撃ステートへ遷移
    virtual void TransitionAttackState() = 0;

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
        Attack  // 攻撃行動
    };
    State				state = State::Idle;


    float dec_pos;          // 種がユニットに跳ね返される距離
    int category;           // ユニットのカテゴリー
    int attack_times;       // 攻撃回数
    float attack_radius;    // 円攻撃範囲半径
    Triangle triangle_1;    // 左三角攻撃範囲
    Triangle triangle_2;    // 右三角攻撃範囲
    Rect square;            // ユニットの後方範囲
};