#pragma once

#include "../Graphics/Graphics.h"
#include <windows.h>
#include <sstream>
#include <d3d11.h>
#include <wrl.h>

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

        virtual void DebugImgui() {};

        // 準備完了しているか
        bool IsReady()const { return ready; }
        // 準備完了設定
        void SetReady() { ready = true; }

    protected:
        enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK, LINEAR_BORDER_WHITE, COMPARISON_LINEAR_BORDER_WHITE, LINEAR_CLAMP };
        Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_states[7];

        enum  DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

        enum  BLEND_STATE { NONE, ALPHA, ADD, MULTIPLY };
        Microsoft::WRL::ComPtr<ID3D11BlendState> blend_states[4];

        enum class RASTER_STATE { SOLID, WIREFRAME, CULL_NONE, WIREFRAME_CULL_NONE };
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizer_states[4];

        void SetState();

        void SetUpRendering();

    protected:
        D3D11_TEXTURE2D_DESC mask_texture2dDesc{};
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

        bool enable_shadow = true;
        bool enable_skymap = false;
        bool enable_bloom = false;
        bool enable_fog = false;

    private:
        bool ready = false;
    };

}