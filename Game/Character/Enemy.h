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
    virtual void Update(float elapsedTime);

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive() {};

    // Imgui
    virtual void DrawDebugGUI(int n) {};

    // 破棄
    void Destroy();

protected:
    // 死亡ステートへ遷移
    virtual void TransitionDeathState();

    // 死亡ステート更新処理
    virtual void UpdateDeathState(float elapsed_time);

    // 攻撃ステートへ遷移
    virtual void TransitionAttackState();

    // 攻撃ステート更新処理
    virtual void UpdateAttackState(float elapsed_time) = 0;

    // 移動ステートへ遷移
    virtual void TransitionMoveState() {}

    // 移動ステート更新処理
    virtual void UpdateMoveState(float elapsed_time) = 0;

protected:
    enum Animation
    {
        Move = 0,
        Attack = 1,
        Out = 6
    };
    enum class State
    {
        Move,
        Attack,
        Death
    };
    State state         = State::Move; // ステート

    bool is_hit_unit    = false;       // プレイヤーに当たったか    
};