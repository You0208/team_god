#pragma once
#include "Enemy.h"
class Enemy_B :public Enemy
{
public:
    Enemy_B();
    ~Enemy_B()override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

private:
    // 攻撃更新
    void AttackUpdate(float elapsedTime) override;

    // 移動更新
    void MoveUpdate(float elapsedTime) override;

    void DrawDebugGUI()override;

    // 死亡処理
    void OnDead()override;

private:
    // 移動用の位置保存
    float old_position;
    // 進む距離
    float dis;
    // 攻撃間隔
    float attack_timer;
    // 移動タイマー
    float move_timer;
    // 移動間隔
    float move_timer_max;
};

