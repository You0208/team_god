#pragma once
#include "BaseScene.h"

#include "./Lemur/Model/FbxModelManager.h"

class DemoScene2 :public Lemur::Scene::BaseScene
{
public:
    DemoScene2() {}
    ~DemoScene2() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

private:
    // Model test
    std::unique_ptr<FbxModelManager> test_model;

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;
};