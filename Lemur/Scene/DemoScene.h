#pragma once
#include "BaseScene.h"
#include "../Component/GameObject.h"
#include "../Component/DemoPlayer.h"
#include "../Component/DemoEnemy.h"
#include "../Graphics/Texture.h"
#include "../Graphics/Framework.h"

// BLOOM
#include "../Graphics/Bloom.h"

// Audio
#include <wrl.h>
#include "../Audio/audio.h"

#include "../Effekseer/Effect.h"

// Font
#include"../Font/Font.h"
#include <dwrite.h>                
#pragma comment(lib,"d2d1.lib")    
#pragma comment(lib,"dwrite.lib")

// GLTF
#include "./Lemur/Model/GltfModel.h"


#include <SpriteBatch.h>

#include "./Lemur/Model/FbxModelManager.h"



class DemoScene :public Lemur::Scene::BaseScene
{
public:
    DemoScene() {}
    ~DemoScene() override {}

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
    std::unique_ptr<FbxModelManager> test_model_2;


    //DemoPlayer
    DemoPlayer* player = nullptr;
    DemoEnemy* enemy = nullptr;

    float second;

    // skkind
    std::shared_ptr<SkinnedMesh> skinned_meshes[8];

    // gltf
    std::unique_ptr<GltfModel> gltf_models[8];

    DirectX::XMFLOAT3 translation{ 0, 0, 0 };
    DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    DirectX::XMFLOAT4 material_color{ 1, 1, 1, 1 };
private:
    std::unique_ptr<SpriteBatch> sprite_batches[8];


    Microsoft::WRL::ComPtr<ID3D11PixelShader> defefferd_model;

    // shader
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;

    std::unique_ptr<StaticMeshBatch>  static_meshes[8];

    // SHADOW
    const uint32_t shadowmap_width = 2048;
    const uint32_t shadowmap_height = 2048;
    std::unique_ptr<ShadowMap> double_speed_z;
    DirectX::XMFLOAT4 light_view_focus{ 0, 0, 0, 1 };
    float light_view_distance{ 10.0f };
    float light_view_size{ 12.0f };
    float light_view_near_z{ 2.0f };
    float light_view_far_z{ 18.0f };


    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;    //�X�v���C�g�o�b�`
};