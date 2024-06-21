#pragma once
#include "Stage.h"

class StageMain : public Stage
{
public:
    StageMain();
    ~StageMain()override;

    void Update(float elapsedTime) override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader, ID3D11PixelShader** replaced_pixel_shader2) override;

    // レイキャスト
    void DrawDebugGui();
private:
    // スケールファクター
    std::unique_ptr<FbxModelManager>   ground = nullptr;
    std::unique_ptr<FbxModelManager>   fild1 = nullptr;
    std::unique_ptr<FbxModelManager>   fild2 = nullptr;
    std::unique_ptr<FbxModelManager>   fence = nullptr;
    std::unique_ptr<FbxModelManager>   fence_type1 = nullptr;
    std::unique_ptr<FbxModelManager>   fence_type2 = nullptr;
    std::unique_ptr<FbxModelManager>   fence2 = nullptr;
    std::unique_ptr<FbxModelManager>   fence2_type1 = nullptr;
    std::unique_ptr<FbxModelManager>   fence2_type2 = nullptr;
    std::unique_ptr<FbxModelManager>   fence_end = nullptr;


    std::unique_ptr<FbxModelManager>   ido = nullptr;
    std::unique_ptr<FbxModelManager>   ido_yane = nullptr;
    std::unique_ptr<FbxModelManager>   cave = nullptr;
    std::unique_ptr<FbxModelManager>   rock = nullptr;
    std::unique_ptr<FbxModelManager>   silo = nullptr;
    std::unique_ptr<FbxModelManager>   leavs_LV1[7] = {};
    std::unique_ptr<FbxModelManager>   leavs_LV2[7] = {};
    std::unique_ptr<FbxModelManager>   une[3] = {};
    std::unique_ptr<FbxModelManager>   log = {};
    std::unique_ptr<FbxModelManager>   tree[7] = {};

    float dec_y = -0.5f;
};



