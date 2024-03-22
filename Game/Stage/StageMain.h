#pragma once
#include "Stage.h"
#include "Lemur/Model/FbxModelManager.h"

class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain()override;


    void Update(float elapsedTime) override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    // レイキャスト
    bool RayCast(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        HitResult& hit
    )override;


private:
    std::shared_ptr<FbxModelManager> stage_main_model;
};



