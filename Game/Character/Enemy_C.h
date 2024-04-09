#pragma once
#include "Enemy.h"
class Enemy_C :public Enemy
{
public:
    Enemy_C();
    ~Enemy_C()override {};

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

    //TODO ユニットに接触したか（ユニットの方法当たり判定に纏められないか）
    void JudgeUnit();
private:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

    DirectX::XMFLOAT2 destination = { 0,0 };

    float timer = 0.0f;
    int move_state = 0;
    bool is_touched_unit = false;
};

