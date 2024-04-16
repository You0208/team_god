#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

// ステージ
class Fence :public Character
{
private:

public:
    Fence();
    ~Fence()override;

    // 更新処理
    void Update(float elapsedTime);

    //描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // Imgui
    void DrawDebugGui();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive()override;

    // インスタンス取得
    static Fence& Instance();

    Rect GetLeftRect() { return left_rect; }
    Rect GetFrontRect() { return front_rect; }
    Rect GetBackRect() { return back_rect; }

private:    
    // デバッグ用に四角使ってる
    Rect left_rect      = {};
    Rect front_rect     = {};
    Rect back_rect      = {};   // 柵はないけど当たり判定用に
    Rect right_rect = {};   // 柵はないけど当たり判定用に
    float rect_width    = 0.0f;
};