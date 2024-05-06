#pragma once

#include "../Graphics/Graphics.h"
#include <windows.h>
#include <sstream>
#include <d3d11.h>
#include <wrl.h>

#include "../Math/MathHelper.h"
#include "../Math/Vector.h"

#include "../Graphics/Shader.h"

#include "../Input/Input.h"
#include "../imgui/ImGuiCtrl.h"

namespace Lemur::Scene
{
    // ベースシーン
    class BaseScene
    {
    public:
        BaseScene() {}
        virtual ~BaseScene() {}

        // 初期化
        virtual void Initialize() = 0;
        // 終了化
        virtual void Finalize() = 0;
        // 更新処理
        virtual void Update(HWND hwnd, float elapsefTime) = 0;
        // 描画処理
        virtual void Render(float elapsedTime) = 0;
        // デバッグ
        virtual void DebugImgui();
        // 準備完了しているか
        bool IsReady()const { return ready; }
        // 準備完了設定
        void SetReady() { ready = true; }

    protected:
        // ステートの初期化
        void InitializeState();

        // レンダリング前設定
        void SetUpRendering();

        // ディファードレンダリングの初期化
        void InitializeDeffered(int textureWidth, int textureHeight);

        // ディファードレンダリングのセット
        void SetUpDeffered();

        // ディファードレンダリングの描画
        void RenderingDeffered();

        // ライトのセット
        virtual void InitializeLight();

        // バッファーの初期化
        void InitializeFramebuffer();

        // ピクセルシェーダーの初期化
        void InitializePS();

        // ポストエフェクトの実行
        void ExePostEffct();

        void LightUpdate();
    protected:
        // サンプラーステート
        enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, COMPARISON_LINEAR_BORDER_WHITE, LINEAR_CLAMP };
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[7];

        // 深度ステート
        enum  DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

        // ブレンドステート
        enum  BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY, MLT_ALPHA };
        Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[5];

        // ライスタライザステート
        enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE };
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];

        // ピクセルシェーダー
        Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shaders[8];
        enum class PS { FOG, FINAL, SKY, DEFFERED };

        // コンスタントバッファー
        Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];
        enum class CONSTANT_BUFFER { SCENE, FOG, OPTION, PBR, D_FOG };
        enum class CONSTANT_BUFFER_R { NONE, SCENE, FOG, OPTION, PBR, D_FOG, LIGHT, HEMISPERE/*register用*/ };

        std::unique_ptr<Framebuffer> framebuffers[8];
        enum class FRAME_BUFFER { SCENE, FOG, DEPTH,TEX };

        // MASK
        struct option_constants {
            DirectX::XMFLOAT4 parameters{ 0.0f,0.0f,0.0f,0.0f }; // x : ディゾルブ適応量、yzw : 空き
        };
        option_constants option_constant;


        // バッファー＝データを一時的に保持するメモリストレージ領域
        // ・プログラム側で定数バッファに値を格納することで
        // 　シェーダー内でプログラム側のパラメータを参照できる
        // ・ワールド行列、ビュー行列、プロジェクション行列などの
        // 　パラメータを定数バッファに書き込むことで、
        // 　シェーダー内で座標変換の計算ができる
        struct scene_constants
        {
            DirectX::XMFLOAT4X4 view_projection;// ビュー行列
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

        Vector3 spDirection;

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

        // カメラ
        DirectX::XMFLOAT4 camera_position{ 0.0f, 0.0f, 0.0f, 1.0f };
        DirectX::XMFLOAT3 camera_target{ 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 camera_angle{ 0.0f, 0.0f, 0.0f };
        float camera_range = 12.0f;

        // ディファードレンダリング
        enum GBUFFER { BASECOLOR, NORMAL, POSITION, MS, EMISSIVE, OCCLUSION, G_MAX };

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViewArray[G_MAX];
        Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetViewArray[G_MAX];


        // bloom
        std::unique_ptr<Bloom> bloomer;
        // 画面用
        std::unique_ptr<FullscreenQuad> fullscreenQuad;
    protected:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
        std::shared_ptr<Sprite> dummy_sprite;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

        // Zelda_Shader
        Microsoft::WRL::ComPtr<ID3D11PixelShader> zelda_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> Wall;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> Try;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> chara_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> stage_ps;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> fbx_gbuffer_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gltf_gbuffer_ps;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> gltf_ps;

        // SHADOW
        const uint32_t shadowmap_width = 2048;
        const uint32_t shadowmap_height = 2048;
        std::unique_ptr<ShadowMap> double_speed_z;
        DirectX::XMFLOAT4 light_view_focus{ 0, 0, 0, 1 };
        float light_view_distance{ 10.0f };
        float light_view_size{ 12.0f };
        float light_view_near_z{ 2.0f };
        float light_view_far_z{ 18.0f };


        bool enable_shadow = true;
        bool enable_skymap = false;
        bool enable_bloom = false;
        bool enable_fog = false;

        bool enable_deferred = false;
        bool enable_deferred_post = false;
        bool enable_post_effect = false;
    private:
        bool ready = false;
    };

}