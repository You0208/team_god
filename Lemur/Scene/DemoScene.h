#pragma once
#include "BaseScene.h"

#include "./Lemur/Model/FbxModelManager.h"
#include "./Lemur/Model/GltfModelManager.h"



class DemoScene :public Lemur::Scene::BaseScene
{
public:
    DemoScene() {}
    ~DemoScene() override {}

    // ‰Šú‰»
    void Initialize()override;

    // I—¹‰»
    void Finalize()override;

    // XVˆ—
    void Update(HWND hwnd, float elapsedTime)override;

    // •`‰æˆ—
    void Render(float elapsedTime)override;

    // ŒõŒ¹‚Ì‰Šú‰»
    void InitializeLight()override;
private:
    // Model test
    std::unique_ptr<FbxModelManager> test_model;
    std::unique_ptr<FbxModelManager> test_model_2;

    std::shared_ptr<FbxModelManager>   fild1 = nullptr;
    std::shared_ptr<FbxModelManager>   fild2 = nullptr;

    std::unique_ptr<GltfModelManager> gltf_test_model;
    std::unique_ptr<GltfModelManager> gltf_test_model_2;

    float second;

    DirectX::XMFLOAT3 translation{ 0, 0, 0 };
    DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    DirectX::XMFLOAT4 material_color{ 1, 1, 1, 1 };
private:
    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;
};