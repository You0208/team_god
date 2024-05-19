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
#include "./Lemur/Model/GltfModelManager.h"



class DemoScene :public Lemur::Scene::BaseScene
{
public:
    DemoScene() {}
    ~DemoScene() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    // 光源の初期化
    void InitializeLight()override;
private:
    // Model test
    std::unique_ptr<FbxModelManager> test_model;
    std::unique_ptr<FbxModelManager> test_model_2;

    std::shared_ptr<FbxModelManager>   fild1 = nullptr;
    std::shared_ptr<FbxModelManager>   fild2 = nullptr;

    std::unique_ptr<GltfModelManager> gltf_test_model;
    std::unique_ptr<GltfModelManager> gltf_test_model_2;

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

    std::shared_ptr<Sprite> result;

    // shader
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;

    std::unique_ptr<StaticMeshBatch>  static_meshes[8];


    std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;    //スプライトバッチ

    Effect* debugEffect;
};