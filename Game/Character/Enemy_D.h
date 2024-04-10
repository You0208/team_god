#pragma once
#include "Enemy.h"
class Enemy_D :public Enemy
{
public:
    Enemy_D();
    ~Enemy_D()override {};

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

private:
    // 攻撃更新
    void AttackUpdate(float elapsedTime) override;

    // 移動更新
    void MoveUpdate(float elapsedTime) override;

    // Imgui
    void DrawDebugGUI()override;
    // 死亡処理
    void OnDead()override;

private:
    int move_state;             // 移動の状態
    float dis;                  // 移動量
    float dis_max;              // 移動量規定値
    float speed_power_Y = 10.0f;// Y方向の移動量
    int direction_state = 0;    // 方向の状態
    bool is_last_touched;       // 前回の移動で柵に触れたか
    
    float attack_timer;         // 攻撃間隔
};

