#pragma once
#include "Stage.h"

class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain()override;

    void Update(float elapsedTime) override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // レイキャスト
    void DrawDebugGui();
private:
};



