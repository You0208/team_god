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
    float timer;
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
    float old_position;// 移動用の位置保存
    float dis;// 進む距離
    float attack_timer;// 攻撃間隔
    float move_timer;// 移動タイマー
    float move_timer_max;// 移動間隔
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

    void JudgeUnit();
private:
    DirectX::XMFLOAT2 destination = { 0,0 };// 目的地

    float timer = 0.0f;// タイマー
    int move_state = 0;// 移動ステート
    bool is_touched_unit = false;// ユニットに触れたか
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
    int move_state;             // 移動の状態
    float dis;                  // 移動量
    float dis_max;              // 移動量規定値
    float speed_power_Y = 10.0f;// Y方向の移動量
    int direction_state = 0;    // 方向の状態
    bool is_last_touched;       // 前回の移動で柵に触れたか

    float attack_timer;         // 攻撃間隔
};