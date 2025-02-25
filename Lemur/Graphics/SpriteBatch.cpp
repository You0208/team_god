#include "SpriteBatch.h"
#include "misc.h"
#include "Texture.h"
#include "Shader.h"
#include <sstream>
#include <functional>

SpriteBatch::SpriteBatch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites) : max_vertices(max_sprites * 6)
{
    // 頂点バッファオブジェクトの生成
    HRESULT hr{ S_OK };

    std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) };

    // 頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * max_vertices);// バッファーのサイズ
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;// バッファーの読み取りと書き込み方法
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUアクセスフラグ（ない場合は0）
    buffer_desc.MiscFlags = 0; // その他のフラグ（未使用の場合は0）
    buffer_desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = vertices.get();// 初期化データへのポインター
    subresource_data.SysMemPitch = 0; // テクスチャの 1 行の先頭から次の行までの距離
    subresource_data.SysMemSlicePitch = 0; // 1 つの深度レベルの先頭から次の深さレベルまでの距離
    // GPUのVRAMにバッファを作成している
    hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    // 入力レイアウトオブジェクトの生成
    // ""の中身はセマンティック
    D3D11_INPUT_ELEMENT_DESC input_element_desc[]
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
        D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    create_vs_from_cso(device, "Shader/sprite_vs.cso", vertex_shader.GetAddressOf(), input_layout.GetAddressOf(), input_element_desc, _countof(input_element_desc));
    create_ps_from_cso(device, "Shader/sprite_ps.cso", pixel_shader.GetAddressOf());
    
    load_texture_from_file(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
}

void SpriteBatch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,// 矩形の左上の座標（スクリーン座標系）
    float dw, float dh, // 矩形のサイズ（スクリーン座標系）
    float r, float g, float b, float a,
    float angle/*degree*/)
{
    Render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteBatch::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,
    float dw, float dh,
    float r, float g, float b, float a,
    float angle,
    float sx, float sy, float sw, float sh)
{
    // スクリーン（ビューポート）のサイズを取得する
    D3D11_VIEWPORT viewport{};
    UINT num_viewports{ 1 };
    // RSGetViewports は現在バインドされているビューポートの数を変数に代入
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    //　render メンバ関数の引数（dx, dy, dw, dh）から矩形の各頂点の位置（スクリーン座標系）を計算する
    // (x0, y0) *----* (x1, y1)
    //         |   / |
    //         |  /  |
    //         | /   |
    //         |/    |
    // (x2, y2) *----* (x3, y3) 

    // left-top
    float x0{ dx };
    float y0{ dy };
    // right-top
    float x1{ dx + dw };
    float y1{ dy };
    // left-bottom
    float x2{ dx };
    float y2{ dy + dh };
    // right-bottom
    float x3{ dx + dw };
    float y3{ dy + dh };


#if 0
#if 0
    std::function<void(float&, float&, float, float, float, float)> rotate{ [](float& x, float& y, float cx, float cy, float sin, float cos)
        {
            x -= cx;
            y -= cy;

            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
    } };
#endif
    // UNIT.09
    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, sin, cos);
    rotate(x1, y1, cx, cy, sin, cos);
    rotate(x2, y2, cx, cy, sin, cos);
    rotate(x3, y3, cx, cy, sin, cos);
#else
    // UNIT.09
    // Translate sprite's centre to origin (rotate centre)
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    x0 -= cx;
    y0 -= cy;
    x1 -= cx;
    y1 -= cy;
    x2 -= cx;
    y2 -= cy;
    x3 -= cx;
    y3 -= cy;

    // Rotate each sprite's vertices by angle
    float tx, ty;
    float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
    float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
    tx = x0;
    ty = y0;
    x0 = cos * tx + -sin * ty;
    y0 = sin * tx + cos * ty;
    tx = x1;
    ty = y1;
    x1 = cos * tx + -sin * ty;
    y1 = sin * tx + cos * ty;
    tx = x2;
    ty = y2;
    x2 = cos * tx + -sin * ty;
    y2 = sin * tx + cos * ty;
    tx = x3;
    ty = y3;
    x3 = cos * tx + -sin * ty;
    y3 = sin * tx + cos * ty;

    // Translate sprite's centre to original position
    x0 += cx;
    y0 += cy;
    x1 += cx;
    y1 += cy;
    x2 += cx;
    y2 += cy;
    x3 += cx;
    y3 += cy;
#endif

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    float u0{ sx / texture2d_desc.Width };
    float v0{ sy / texture2d_desc.Height };
    float u1{ (sx + sw) / texture2d_desc.Width };
    float v1{ (sy + sh) / texture2d_desc.Height };

    vertices.push_back({ { x0, y0 , 0 }, { r, g, b, a }, { u0, v0 } });
    vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
    vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
    vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
    vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
    vertices.push_back({ { x3, y3 , 0 }, { r, g, b, a }, { u1, v1 } });
}

void SpriteBatch::Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
    Render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void SpriteBatch::Begin(ID3D11DeviceContext* immediate_context,
    ID3D11PixelShader* replaced_pixel_shader, ID3D11ShaderResourceView* replaced_shader_resource_view)
{
    vertices.clear();
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    replaced_pixel_shader ? immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0) : immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    if (replaced_shader_resource_view)
    {
        HRESULT hr{ S_OK };
        Microsoft::WRL::ComPtr<ID3D11Resource> resource;
        replaced_shader_resource_view->GetResource(resource.GetAddressOf());

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
        hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
        _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

        texture2d->GetDesc(&texture2d_desc);

        immediate_context->PSSetShaderResources(0, 1, &replaced_shader_resource_view);
    }
    else
    {
        immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());
    }
}

void SpriteBatch::End(ID3D11DeviceContext* immediate_context)
{
    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    size_t vertex_count = vertices.size();
    _ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
    vertex* data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (data != nullptr)
    {
        const vertex* p = vertices.data();
        memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
    }
    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // プリミティブの描画
    immediate_context->Draw(static_cast<UINT>(vertex_count), 0);

    Microsoft::WRL::ComPtr<ID3D11Resource> resource;
    shader_resource_view.Get()->GetResource(resource.GetAddressOf());

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
    hr = resource->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    texture2d->GetDesc(&texture2d_desc);
}