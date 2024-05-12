
#pragma once
#include "./Lemur/Scene/BaseScene.h"

class TitleScene :public Lemur::Scene::BaseScene
{
public:
    TitleScene() {}
    ~TitleScene() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    void DebugImgui()override;
private:
private:
    std::shared_ptr<Sprite> title;

    // MASK
    struct option_constants {
        DirectX::XMFLOAT4 parameters{ 1.0f,0.0f,0.0f,0.0f }; // x : ディゾルブ適応量、yzw : 空き
    };
    float dissolve_value{ 0.0f };
    option_constants option_constant;
    Microsoft::WRL::ComPtr<ID3D11Buffer> option_constant_buffer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;


};