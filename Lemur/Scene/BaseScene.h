#pragma once

#include "../Graphics/Graphics.h"
#include <windows.h>
#include <sstream>
#include <d3d11.h>
#include <wrl.h>

#include "../Math/MathHelper.h"
#include "../Math/EasingFunction.h"
#include "../Math/Vector.h"

#include "../Graphics/Shader.h"

#include "../Input/Input.h"
#include "../imgui/ImGuiCtrl.h"

#include "../Graphics/ParticleSystem.h"
#include "../Graphics/GaussianBlur.h"

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

        // テクスチャの初期化
        void InitializeMask();

        // レンダリング前設定
        void SetUpRendering();

        // 定数バッファー更新
        void SetUpConstantBuffer();

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

        // ライトの更新
        void LightUpdate();

        // シャドウマップの準備
        void SetUpShadowMap();

        // 遷移
        DirectX::XMFLOAT2 mask_pos;
        void CallTransition(bool in, DirectX::XMFLOAT2 mask_pos_ = { SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.5f });
        void TransitionMask(float elapsed_time);
        void RenderTransitionMask(float elapsed_time);

    protected:// イージング
#if 0

        bool is_down_scale_easing = false;
        bool is_up_scale_easing = false;
        //float timer = 0.0f;
        //float time_max = 0.2f;
        //float target_value = 0.0f;
        //float start_value = 0.0f;

        void DownScaleEasing(float& scale, float& easing_timer, float easing_target_scale, float easing_start_scale, float easing_time_max = 0.2f);
        void UpScaleEasing(float& scale, float& easing_timer, float easing_target_scale, float easing_start_scale, float easing_time_max = 0.2f);
        void CallUpScaleEasing(float& easing_timer, float& start_scale, float scale)
        {
            easing_timer = 0.0f;
            start_scale = scale;
            is_up_scale_easing = true;
        }
        void CallDownScaleEasing(float& easing_timer, float& start_scale, float scale)
        {
            easing_timer = 0.0f;
            start_scale = scale;
            is_up_scale_easing = true;
        }

        bool is_down_scale_easing = false;
        bool is_up_scale_easing = false;
        float easing_timer = 0.0f;
        float easing_time_max = 0.2f;
        float easing_target_scale = 0.0f;
        float easing_start_scale = 0.0f;

        void DownScaleEasing(float& scale);
        void UpScaleEasing(float& scale);
        void CallScaleEasing(float target_scale, float start_scale, float t = 0.2f, bool down)
        {
            easing_timer = 0.0f;
            easing_time_max = t;
            easing_target_scale = target_scale;
            easing_start_scale = start_scale;
            if (down)is_down_scale_easing = true;
            if (!down)is_down_scale_easing = true;
        }
#endif
    protected:
        // サンプラーステート
        enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, COMPARISON_LINEAR_BORDER_WHITE, LINEAR_CLAMP };
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[7];

        // 深度ステート
        enum  DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF,NONE_D, MASK, APPLY_MASK,EXCLUSIVE };
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[8];

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
        enum class FRAME_BUFFER { SCENE, FOG, DEPTH, TEX, MASK };

        // MASK
        struct option_constants {
            DirectX::XMFLOAT4 parameters{ 0.0f,0.0f,0.0f,0.0f }; // x : ディゾルブ適応量、yzw : 空き
            DirectX::XMFLOAT4 rgb_adjustment{ 1.0f,1.0f,1.0f,1.0f }; // rgb補正
            DirectX::XMFLOAT4 hsv_adjustment{ 1.0f,1.0f,1.0f,1.0f }; // hsv補正
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

            // ジオメトリシェーダーにてビルボード計算するため
            DirectX::XMFLOAT4X4 view_matrix;
            DirectX::XMFLOAT4X4 projection_matrix;

            // PROJECTION_MAPPING
            DirectX::XMFLOAT4X4 projection_mapping_transform;
        };
        scene_constants scene_constant;

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
        DirectX::XMFLOAT4 directional_light_direction{ -0.6f, -1.0f, 1.0f, 1.0f };
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


        // パーティクルシステム
        std::unique_ptr<ParticleSystem> particle_system;
        // 爆発アニメーション付きパーティクルシステム
        std::unique_ptr<ParticleSystem> particle_bomb;


        // bloom
        std::unique_ptr<Bloom> bloomer;
        // 画面用
        std::unique_ptr<FullscreenQuad> fullscreenQuad;


        // PROJECTION_MAPPING
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> projection_mapping_texture;
        DirectX::XMFLOAT4X4 projection_mapping_transform;
        DirectX::XMFLOAT3 projection_mapping_eye = { 0, 50, 0 };
        DirectX::XMFLOAT3 projection_mapping_focus = { 0, 0, 0 };
        float projection_mapping_rotation = 0.0f;
        float projection_mapping_fovy = 10.0f;

        bool is_bloom = true;// ブルームかけますか？
        std::unique_ptr<GaussianBlur> gaussian_blur;

    protected:
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> transition_mask_texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mask_texture;
        std::shared_ptr<Sprite> dummy_sprite;
        Microsoft::WRL::ComPtr<ID3D11VertexShader> sprite_vertex_shader;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> sprite_input_layout;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> sprite_pixel_shader;

        // Zelda_Shader
        Microsoft::WRL::ComPtr<ID3D11PixelShader> zelda_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> Wall;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> Try;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> collision;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> chara_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> stage_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> stage_ps_1;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> stage_ps_2;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> fbx_gbuffer_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> gltf_gbuffer_ps;

        Microsoft::WRL::ComPtr<ID3D11PixelShader> gltf_ps;
        Microsoft::WRL::ComPtr<ID3D11PixelShader> transition_mask_ps;

        // SHADOW
        const uint32_t shadowmap_width = 2048;
        const uint32_t shadowmap_height = 2048;
        std::unique_ptr<ShadowMap> shadow_map;
        DirectX::XMFLOAT4 light_view_focus{ 0, 0, 0, 1 };
        float light_view_distance{ 23.0f };
        float light_view_size{ 45.0f };
        float light_view_near_z{ -10.0f };
        float light_view_far_z{ 40.0f };

        bool enable_shadow = true;
        bool enable_skymap = false;
        bool enable_bloom = false;
        bool enable_fog = false;

        bool enable_deferred = false;
        bool enable_deferred_post = false;
        bool enable_post_effect = true;

        // 遷移
        std::shared_ptr<Sprite> spr_transition_mask;
        std::shared_ptr<Sprite> spr_transition_mask_back;
        EasingFunction mask_scale;
        float mask_angle = 0.0f;
        bool start_transition = false;
        bool is_in = false;
        int next_scene = 0;
    private:
        bool ready = false;
    };

}