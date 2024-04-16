#pragma once
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
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
};

class Enemy_B :public Enemy
{
public:
    Enemy_B();
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
private:
    float   old_position   = 0.0f;      // 移動用の位置保存
    float   dis            = 1.0f;      // 進む距離
    float   attack_timer   = 1.0f;      // 攻撃間隔
    float   move_timer     = 0.0f;      // 移動タイマー
    float   move_timer_max = 1.0f;      // 移動間隔
};

class Enemy_C :public Enemy
{
public:
    Enemy_C();
    ~Enemy_C()override {};
    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;
    void DrawDebugGUI(int n)override;
private:
    // 攻撃
    void UpdateAttackState(float elapsed_time)override;
    // 移動
    void TransitionMoveState()override {}
    void UpdateMoveState(float elapsed_time)override;
    void UpdateMoveState_S(float elapsed_time);// 横方向移動
    void UpdateMoveState_V(float elapsed_time);// 縦方向移動
    // ユニットとの当たり判定
    void JudgeUnit_S();
    void JudgeUnit_V();
private:
    enum Move
    {
        Straight,
        Avoid
    };
    enum UnitDestDir
    {
        Back,
        Front,
        Right,
        Left
    };

    float   timer                       = 0.0f;      // タイマー
    int     move_state                  = Straight;  // 移動ステート
    bool    is_touched_unit             = false;     // ユニットに触れたか
    int     unit_edst_dir               = 0;
    DirectX::XMFLOAT2   destination     = { 0,0 };   // 目的地
};

class Enemy_D :public Enemy
{
public:
    Enemy_D();
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
private:
    enum Move
    {
        Straight,
        Diagonal
    };
    enum Direction
    {
        Under,
        Up
    };

    int     move_state      = Move::Straight;         // 移動の状態
    int     direction_state = Direction::Under;       // 方向の状態
    float   dis             = 0.0f;                   // 移動量
    float   dis_max         = 1.0f;                   // 移動量規定値
    float   speed_power_Y   = 10.0f;                  // Y方向の移動量
    bool    is_last_touched = false;                  // 前回の移動で柵に触れたか

    float   attack_timer    = 1.0f;                   // 攻撃間隔
};