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

    // デバック
    void DebugImgui()override;

private:

    //DemoPlayer
    DemoPlayer* player = nullptr;
    DemoEnemy* enemy = nullptr;

    float second;

    // skkind
    std::shared_ptr<SkinnedMesh> skinned_meshes[8];

    // gltf
    std::unique_ptr<GltfModel> gltf_models[8];

    // ADAPTER用
    float adapter_timer;

    //TODO 調整用
    DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };

    DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT3 camera_target{ 0.0f, 0.0f, 0.0f };
    float camera_range = 12.0f;
    //DirectX::XMFLOAT4 light_direction{ 0.0f, -1.0f, 1.0f, 1.0f };

    DirectX::XMFLOAT3 translation{ 0, 0, 0 };
    DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    DirectX::XMFLOAT4 material_color{ 1, 1, 1, 1 };
private:
    std::unique_ptr<SpriteBatch> sprite_batches[8];

    std::unique_ptr<Framebuffer> framebuffers[8];
    enum class FRAME_BUFFER { SCENE, FOG };

    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
    enum class CONSTANT_BUFFER { SCENE, FOG, OPTION, PBR, D_FOG };
    enum class CONSTANT_BUFFER_R { NONE, SCENE, FOG, OPTION, PBR, D_FOG, LIGHT, HEMISPERE/*register用*/ };

    std::unique_ptr<FullscreenQuad> fullscreenQuad;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
    enum class PS { FOG, FINAL, SKY };

    // shader
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];

    // dissolve
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> noise;

    // PBR
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> BaseColor;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Roughness;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Normal;

    // bloom
    std::unique_ptr<Bloom> bloomer;

    // skyMap
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skymap;


    // MASK
    struct option_constants {
        DirectX::XMFLOAT4 parameters{ 1.0f,0.0f,0.0f,0.0f }; // x : ディゾルブ適応量、yzw : 空き
    };
    option_constants option_constant;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
    std::shared_ptr<Sprite> dummy_sprite;
    Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

    // バッファー＝データを一時的に保持するメモリストレージ領域
    // ・プログラム側で定数バッファに値を格納することで
    // 　シェーダー内でプログラム側のパラメータを参照できる
    // ・ワールド行列、ビュー行列、プロジェクション行列などの
    // 　パラメータを定数バッファに書き込むことで、
    // 　シェーダー内で座標変換の計算ができる
    struct scene_constants
    {
        DirectX::XMFLOAT4X4 view_projection;// ビュー行列
        //DirectX::XMFLOAT4 light_direction;// ライト方向
        DirectX::XMFLOAT4 camera_position;// カメラの位置
        DirectX::XMFLOAT4X4 inverse_projection;// 逆行列
        float time;// 時間
        float pads[3];
        //TODO ここら辺ちゃんと調べる        
        DirectX::XMFLOAT4X4 inv_view_projection;// 逆ビュー行列

        // 影用
        DirectX::XMFLOAT4X4 light_view_projection;// 光のビュー行列
        float shadow_depth_bias;
        float pads2[3];
    };
    scene_constants scene_constants;

    // FOG
    struct fog_constants
    {
        //float fog_color[4] = { 0.322f, 0.765f, 0.882f, 0.894f }; // w: fog intensuty
        float fog_color[4] = { 1.000f, 1.000f, 1.000f, 0.894f }; // w: fog intensuty
        float fog_density = 0.0007f;
        float fog_height_falloff = 0.9313f;
        float start_distance = 5.00f;
        float fog_cutoff_distance = 500.0f;
        float time_scale = 0.5f;
        float seed_scale = 0.2f;
        float pads[2];
    };
    fog_constants fog_constants;

    // STATIC_BATCHING
    size_t drawcall_count = 0;
    std::unique_ptr<StaticMeshBatch>  static_meshes[8];

    // ポイントライト
    struct pointLights
    {
        DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };       // 位置
        DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };          // 色
        float range{ 0 };                               // 範囲
        float pad[3];
    };

    // スポットライト
    struct spotLights
    {
        DirectX::XMFLOAT4 position{ 0, 0, 0, 0 };       // 位置
        DirectX::XMFLOAT4 direction{ 0, 0, 1, 0 };      // 方向
        DirectX::XMFLOAT4 color{ 1, 1, 1, 1 };          // 色
        float range{ 0 };                               // 範囲
        float pad[3];
        float angle;                                    // 角度
        float pad2[3];
    };

    struct lightConstants
    {
        DirectX::XMFLOAT4 ambient_color;                // アンビエントカラー
        DirectX::XMFLOAT4 directional_light_direction;  // ディレクションライトの方向
        DirectX::XMFLOAT4 directional_light_color;      // ディレクションライトの色

        pointLights point_light[8];                      // ポイントライト
        spotLights spot_light[8];                        // スポットライト
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> light_constant_buffer;
    lightConstants lights{};

    DirectX::XMFLOAT4 ambient_color{ 0.2f, 0.2f, 0.2f, 0.2f };
    DirectX::XMFLOAT4 directional_light_direction{ 1.0f, -1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT4 directional_light_color{ 1.0f, 1.0f, 1.0f, 1.0f };
    pointLights point_light[8];
    spotLights spot_light[8];
    float sp_angle = 0;

    Vector3 spDirection = {1,1,1};

    // PBR
    struct adjust_constants
    {
        DirectX::XMFLOAT4 materialColor;
        DirectX::XMFLOAT4 adjustMetalness = { 0.0f,0,0,0 }; // メタルネスの調整値
        DirectX::XMFLOAT4 adjustSmoothness = { 0.0f,0,0,0 }; // 滑らかさの調整値
    };
    adjust_constants pbr;

    // 距離フォグ
    struct dis_fog_constants
    {
        DirectX::XMFLOAT4 fog_color;
        DirectX::XMFLOAT4 fog_range; // x : near, y : far, zw : 空き
    };
    DirectX::XMFLOAT4 fog_color{ 0.2f, 0.2f, 0.2f, 1.0f }; // クリアカラーにしています
    DirectX::XMFLOAT4 fog_range{ 0.1f, 100.0f, 0, 0 };

    // 半球ライト
    struct hemisphere_light_constants
    {
        DirectX::XMFLOAT4 sky_color;
        DirectX::XMFLOAT4 ground_color;
        DirectX::XMFLOAT4 hemisphere_weight; // x : weight, yzw : 空き
    };
    Microsoft::WRL::ComPtr<ID3D11Buffer> hemisphere_light_constant_buffer;
    DirectX::XMFLOAT4 sky_color{ 1.0f, 0.0f, 0.0f, 1.0f };
    DirectX::XMFLOAT4 ground_color{ 0.0f, 0.0f, 1.0f, 1.0f };
    float hemisphere_weight{ 0.0f };


    // SHADOW
    const uint32_t shadowmap_width = 2048;
    const uint32_t shadowmap_height = 2048;
    std::unique_ptr<ShadowMap> double_speed_z;
    DirectX::XMFLOAT4 light_view_focus{ 0, 0, 0, 1 };
    float light_view_distance{ 10.0f };
    float light_view_size{ 12.0f };
    float light_view_near_z{ 2.0f };
    float light_view_far_z{ 18.0f };

    // コントローラー用
    float s_l;
    float timer_s;
    float s_l_max;
    float f_d=0;
    std::shared_ptr<Sprite> ohajiki;


    
};