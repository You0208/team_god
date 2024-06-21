#pragma once
#include "BaseScene.h"

#include "./Lemur/Model/FbxModelManager.h"

class DemoScene2 :public Lemur::Scene::BaseScene
{
public:
    DemoScene2() {}
    ~DemoScene2() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

private:
    // Model test
    std::unique_ptr<FbxModelManager> test_model;

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;
};