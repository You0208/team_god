#pragma once
#include "BaseScene.h"

#include "./Lemur/Model/FbxModelManager.h"

class DemoScene2 :public Lemur::Scene::BaseScene
{
public:
    DemoScene2() {}
    ~DemoScene2() override {}

    // ‰Šú‰»
    void Initialize()override;

    // I—¹‰»
    void Finalize()override;

    // XVˆ—
    void Update(HWND hwnd, float elapsedTime)override;

    // •`‰æˆ—
    void Render(float elapsedTime)override;

private:
    // Model test
    std::unique_ptr<FbxModelManager> test_model;

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;
};