#pragma once
#include "Enemy.h"
class Enemy_A :public Enemy
{
public:
    Enemy_A();
    ~Enemy_A()override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // 攻撃更新
    void AttackUpdate(float elapsedTime) override;

    // 移動更新
    void MoveUpdate(float elapsedTime) override;

    void DrawDebugGUI()override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;
private:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

    int state_index;

    float timer;
};

