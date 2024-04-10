#pragma once
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};

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
    float timer;
};

