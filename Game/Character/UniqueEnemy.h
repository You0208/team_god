#pragma once
#include "Enemy.h"
#include "EnemySpawner.h"
class SummonEnemy :public Enemy
{
public:
    SummonEnemy(bool is_minor);
    ~SummonEnemy()override {};
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
    EnemySpawner::EnemyStatus status[4];
    float mover_timer_max_B = 1.0f; // 一気に進むまでの時間
    float dis_B = 3.0f;     // 一気に進む距離
    bool once = true;
};

class BossEnemy :public Enemy
{
public:
    BossEnemy(bool is_minor);
    ~BossEnemy()override {};
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
