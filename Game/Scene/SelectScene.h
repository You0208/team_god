#pragma once
#include "./Lemur/Scene/BaseScene.h"
#include "./Lemur/Component/GameObject.h"
#include "./Lemur/Component/DemoPlayer.h"
#include "./Lemur/Component/DemoEnemy.h"
#include "./Lemur/Graphics/Shader.h"
#include "./Lemur/Graphics/Texture.h"
#include "./Lemur/Graphics/Framework.h"

// BLOOM
#include "./Lemur/Graphics/Bloom.h"

// Audio
#include <wrl.h>
#include "./Lemur/Audio/audio.h"

#include "./Lemur/Effekseer/Effect.h"

// Font
#include"./Lemur/Font/Font.h"
#include <dwrite.h>                
#pragma comment(lib,"d2d1.lib")    
#pragma comment(lib,"dwrite.lib")

// GLTF
#include "./Lemur/Model/GltfModel.h"

#include "./Lemur/Math/Vector.h"
#include "./Lemur/Math/MathHelper.h"

class SelectScene :public Lemur::Scene::BaseScene
{
public:
    SelectScene() {}
    ~SelectScene() override {}

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
    int stage_num = 0;

private:
    std::shared_ptr<Sprite> select_1;
    std::shared_ptr<Sprite> select_2;
    std::shared_ptr<Sprite> select_3;

    // MASK
    struct option_constants {
        DirectX::XMFLOAT4 parameters{ 1.0f,0.0f,0.0f,0.0f }; // x : �f�B�]���u�K���ʁAyzw : ��
    };
    float dissolve_value{ 0.0f };
    option_constants option_constant;
    Microsoft::WRL::ComPtr<ID3D11Buffer> option_constant_buffer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;


};
