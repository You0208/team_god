#pragma once
#include "./Lemur/Scene/BaseScene.h"
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

//Game
#include "../Stage/Stage.h"
#include "../Character/Player.h"

class GameScene :public Lemur::Scene::BaseScene
{
public:
    GameScene() {}
    ~GameScene() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    // Imgui
    void DebugImgui()override;
private:// ゲーム
    Stage* stage = nullptr;

    Player* player = nullptr;

    float second;

    // コントローラー用
    float s_l;
    float timer_s;
    float s_l_max;
    float f_d = 0;
    std::shared_ptr<Sprite> ohajiki;

private:// シェーダー
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
 
};