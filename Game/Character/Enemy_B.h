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

    // 死亡処理
    void OnDead()override;

private:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

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

