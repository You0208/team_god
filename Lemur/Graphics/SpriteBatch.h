#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>

class SpriteBatch
{
private:

    // メンバ変数
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    D3D11_TEXTURE2D_DESC texture2d_desc;

public:
    // 頂点フォーマット（自由に作れる）
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

private:
    const size_t max_vertices;
    std::vector<vertex> vertices;

public:
    // コンストラクタ・デストラクタ
    SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites);
    ~SpriteBatch() {}

    // メンバ関数
    void Begin(ID3D11DeviceContext* immediate_context,
        ID3D11PixelShader* replaced_pixel_shader = nullptr, ID3D11ShaderResourceView* replaced_shader_resource_view = nullptr);
    void End(ID3D11DeviceContext* immediate_context);

    void Render(ID3D11DeviceContext*,
        float dx, float dy,// 矩形の左上の座標（スクリーン座標系）
        float dw, float dh, // 矩形のサイズ（スクリーン座標系）
        float r, float g, float b, float a,
        float angle/*degree*/);
    // オーバーロード
    void Render(ID3D11DeviceContext*,
        float dx, float dy,// 矩形の左上の座標（スクリーン座標系）
        float dw, float dh, // 矩形のサイズ（スクリーン座標系）
        float r, float g, float b, float a,
        float angle/*degree*/,
        float sx, float sy, float sw, float sh);
    void Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

};