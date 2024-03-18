#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"

class Enemy :public Character
{
public:
    Enemy() {}
    ~Enemy() {}

    // 更新処理
    virtual void Update(float elapsedTime) = 0;

    // 描画処理
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // 破棄
    void Destroy();

    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive();

};