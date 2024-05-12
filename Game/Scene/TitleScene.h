
#pragma once
#include "./Lemur/Scene/BaseScene.h"

class TitleScene :public Lemur::Scene::BaseScene
{
public:
    TitleScene() {}
    ~TitleScene() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
    void Render(float elapsedTime)override;

    void DebugImgui()override;
private:
private:
    std::shared_ptr<Sprite> title;

    // MASK
    struct option_constants {
        DirectX::XMFLOAT4 parameters{ 1.0f,0.0f,0.0f,0.0f }; // x : �f�B�]���u�K���ʁAyzw : ��
    };
    float dissolve_value{ 0.0f };
    option_constants option_constant;
    Microsoft::WRL::ComPtr<ID3D11Buffer> option_constant_buffer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;


};