#pragma once
#include "Lemur/Collision/Collision.h"
#include "Lemur/Model/FbxModelManager.h"

// ステージ
class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    //描画処理
    //virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader, ID3D11PixelShader** replaced_pixel_shader2) {};

    virtual void DrawDebugGui() {}

    Rect                GetStageCollision() { return stage_collision; }
    DirectX::XMFLOAT3   GetStagePos()       { return position; }
    DirectX::XMFLOAT2   GetVariableStageWidth()     { return variable_stage_width; }

protected:
    Rect                                stage_collision         = {};           // ステージの当たり判定用
    DirectX::XMFLOAT2                   stage_width             = {};           // ステージの幅（元）
    DirectX::XMFLOAT3                   position                = {};           // ステージの中心
    DirectX::XMFLOAT3                   scale                   = {};           // ステージのサイズ（調整用）
    float                               scale_facter            = 0.0f;         // スケールファクター
    DirectX::XMFLOAT2                   variable_stage_width    = {};           // 可変のステージ幅
};