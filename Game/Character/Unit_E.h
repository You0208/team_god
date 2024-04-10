#pragma once
#include "Unit.h"

class Unit_E :public Unit
{
public:
    Unit_E();
    ~Unit_E()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

private:
    void DrawDebugGUI();

private:
};


