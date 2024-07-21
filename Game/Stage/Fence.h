#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

// ステージ
class Fence :public Character
{
public:
    Fence();
    ~Fence()override;

    // 更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    void FenceShake(float elapsedTime);
    void CallFenceShake();
    void StopFenceShake();

    // Imgui
    void DrawDebugGui();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

    // インスタンス取得
    static Fence& Instance();

    // ゲッター
    Rect GetLeftRect() { return left_rect; }
    Rect GetFrontRect() { return front_rect; }
    Rect GetBackRect() { return back_rect; }
    Rect GetRightRect() { return right_rect; }

public:
    enum FENCE_STATE// 状態
    {
        FANCE_0,
        FANCE_1,
        FANCE_2,
    };
    float           health_prsent = 0.0f;
    int             fence_state   = 0;
    // イージング
    EasingFunction  fence1_pos    = {};
    EasingFunction  fence2_pos    = {};
private:    
    // デバッグ用に四角使ってる
    Rect left_rect      = {};
    Rect front_rect     = {};
    Rect back_rect      = {};   // 柵はないけど当たり判定用に
    Rect right_rect     = {};   // 柵はないけど当たり判定用に
    float rect_width    = 0.0f; 

};