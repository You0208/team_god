#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <string>

class Sprite
{
private:
    // メンバ変数（COMオブジェクトを ComPtr スマートポインターテンプレートに）
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    
    bool isLoadFile = true;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    D3D11_TEXTURE2D_DESC texture2d_desc;

public:
    int anime_x_max;
    int anime_y_max;

    int anime_x;
    int anime_y;

    // 頂点フォーマット（自由に作れる）
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texcoord;
    };

    // コンストラクタ・デストラクタ
    Sprite(ID3D11Device* device, const wchar_t* filename);
    Sprite(ID3D11Device* device, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view);
    virtual ~Sprite();

    // メンバ関数

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
    void Textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a);
    void Textout(ID3D11DeviceContext* immediate_context, int n, float x, float y, float w, float h, float r, float g, float b, float a);

    void Animation(ID3D11DeviceContext* immediate_context,DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color,float angle, DirectX::XMFLOAT2 texsize);

    float timer;
};