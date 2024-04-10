#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "../Stage/Fence.h"


class Enemy :public Character
{
public:
    Enemy() {}
    ~Enemy() {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    // 描画処理
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive();

    // 破棄
    void Destroy();

protected:
    // 移動ステート更新
    virtual void MoveUpdate(float elapsed_time) {};

    // 攻撃ステート更新
    virtual void AttackUpdate(float elapsed_time) {};

protected:
    enum StateIndex
    {
        Move_State,
        Attack_State
    };

    bool is_hit_unit = false;// プレイヤーに当たったか    
    int state_index;// ステート
};