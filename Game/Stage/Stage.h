#pragma once
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Collision/Collision.h"

// ステージ
class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画処理
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // レイキャスト
    virtual bool RayCast(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        HitResult& hit) = 0;

    virtual void DrawDebugGui() {}

    Rect GetStageSize() { return stage_size; }

protected:
    Rect stage_size = {};// ステージ範囲
};