#pragma once
#include "Enemy.h"
class Enemy_B :public Enemy
{
public:
    Enemy_B();
    ~Enemy_B()override {};

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
    float old_position;
    float dis;

    float move_dis;

    int state_index;

    float attack_timer;
    float move_timer;
};

