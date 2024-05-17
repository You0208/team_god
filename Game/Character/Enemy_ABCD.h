#pragma once
#include "Enemy.h"
#include "Lemur/Math/EasingFunction.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A(bool is_minor);
    ~Enemy_A()override {};
    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // 攻撃
    void UpdateAttackState(float elapsed_time)override;
    // 移動
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
private:
    float timer;// タイマー
    EasingFunction easing_scale;
};

class Enemy_B :public Enemy
{
public:
    Enemy_B(bool is_minor);
    ~Enemy_B()override {};
    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // 攻撃
    void UpdateAttackState(float elapsed_time)override;
    // 移動
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
    float   old_position   = 0.0f;      // 移動用の位置保存
private:
    float   dis            = 1.0f;      // 進む距離
    float   attack_timer   = 1.0f;      // 攻撃間隔
    float   move_timer     = 0.0f;      // 移動タイマー
    float   move_timer_max = 1.0f;      // 移動間隔
};

class Enemy_C :public Enemy
{
public:
    Enemy_C(bool is_minor);
    ~Enemy_C()override {};
    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    enum Move
    {
        Straight,
        Avoid
    };

    // 攻撃
    void UpdateAttackState(float elapsed_time)override;
    // 移動
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;

    // ユニットとの当たり判定
    void JudgeUnit(bool isVertical);

    // 移動
    void HandleMovementState(const Rect& rect1, const Rect& rect2, float speedPower,
        Move straightState, Move avoidState, float& velocityComponent,bool& touchedUnit, float elapsedTime);
    void MoveToDestination(float elapsed_time, Move nextState);

    bool ReachedDestination()
    {
        return (Equal(position.z, destination.y, 0.01f) && Equal(position.x, destination.x, 0.01f));
    }

    void ResetMovementState(Move nextState)
    {
        is_touched_unit = false;
        velocity.x = velocity.z = 0.0f;
        move_state = nextState;
    }

    bool CheckFenceCollision(const Rect& rect1, const Rect& rect2)
    {
        return (Collision::IntersectRectVsCircle(rect1, { position.x, position.z }, radius) ||
            Collision::IntersectRectVsCircle(rect2, { position.x, position.z }, radius));
    }

private:
    float   timer                       = 0.0f;      // タイマー
    int     move_state                  = Straight;  // 移動ステート
    bool    is_touched_unit             = false;     // ユニットに触れたか
    DirectX::XMFLOAT2   destination     = { 0,0 };   // 目的地
};

class Enemy_D :public Enemy
{
public:
    Enemy_D(bool is_minor);
    ~Enemy_D()override {};
    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // 攻撃
    void UpdateAttackState(float elapsed_time)override;
    // 移動
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;

    void UpdateMoveState_S(float elapsed_time);
    void UpdateMoveState_V(float elapsed_time);
private:
    enum Move
    {
        Straight,
        Diagonal
    };
    enum Direction
    {
        Under,
        Up,
    };



    int     move_state      = Move::Straight;         // 移動の状態
    int     direction_state = Direction::Under;       // 方向の状態
    float   dis             = 0.0f;                   // 移動量
    float   dis_max         = 1.0f;                   // 移動量規定値
    float   speed_power_Y   = 10.0f;                  // Y方向の移動量
    bool    is_last_touched = false;                  // 前回の移動で柵に触れたか

    float   attack_timer    = 1.0f;                   // 攻撃間隔
};