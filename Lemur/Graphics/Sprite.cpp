#include "Sprite.h"
#include "misc.h"
#include "Texture.h"
#include "Shader.h"
#include <sstream>
#include <functional>
#include "interval.h"
#include "high_resolution_timer.h"

Sprite::Sprite(ID3D11Device* device, const wchar_t* filename)
{
    // 頂点バッファオブジェクトの生成
    HRESULT hr{ S_OK };

    // 頂点情報のセット（0.5なのはNDCだから）
    vertex vertices[]
    {
     { { -1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 0, 0 } },
     { { +1.0, +1.0, 0 }, { 1, 1, 1, 1 }, { 1, 0 } },
     { { -1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 0, 1 } },
     { { +1.0, -1.0, 0 }, { 1, 1, 1, 1 }, { 1, 1 } },
    };

    // 頂点バッファオブジェクトの生成
    D3D11_BUFFER_DESC buffer_desc{};
    buffer_desc.ByteWidth = sizeof(vertices);// バッファーのサイズ
    buffer_desc.Usage = D3D11_USAGE_DYNAMIC;// バッファーの読み取りと書き込み方法
    buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER; 
    buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // CPUアクセスフラグ（ない場合は0）
    buffer_desc.MiscFlags = 0; // その他のフラグ（未使用の場合は0）
    buffer_desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA subresource_data{};
    subresource_data.pSysMem = vertices;// 初期化データへのポインター
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

    LoadTextureFromFile(device, filename, shader_resource_view.GetAddressOf(), &texture2d_desc);
}

void Sprite::Render(ID3D11DeviceContext* immediate_context,
    float dx, float dy,// 矩形の左上の座標（スクリーン座標系）
    float dw, float dh, // 矩形のサイズ（スクリーン座標系）
    float r, float g, float b, float a,
    float angle/*degree*/)
{
    Render(immediate_context, dx, dy, dw, dh, r, g, b, a, angle, 0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
    {
        x -= cx;
        y -= cy;

        float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
        float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
        float tx{ x }, ty{ y };
        x = cos * tx + -sin * ty;
        y = sin * tx + cos * ty;

        x += cx;
        y += cy;
    };
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}

void Sprite::RenderCenter(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
    float x = dx - dw * 0.5f;
    float y = dy - dh * 0.5f;
    Render(immediate_context, x, y, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::RenderCenter(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx - dw * 0.5f;
    float y = dy - dh * 0.5f;
    Render(immediate_context, x, y, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}
void Sprite::RenderLeftDown(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx;
    float y = dy - dh;
    RenderRightDown(immediate_context, x, y, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}
void Sprite::RenderRightDown(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx - dw;
    float y = dy - dh;
    RenderRightDown(immediate_context, x, y, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::RenderRightDown(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx - dw;
    float y = dy - dh;
    RenderRightDown(immediate_context, replaced_pixel_shader, x, y, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::RenderRightUp(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx - dw;
    RenderRightUp(immediate_context, x, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::RenderRightUp(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx - dw;
    RenderRightUp(immediate_context, replaced_pixel_shader, x, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::Render(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float angle)
{
    Render(immediate_context, replaced_pixel_shader, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::Render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh)
{
    Render(immediate_context, dx, dy, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}

void Sprite::RenderLeftDown(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を矩形の中心点にした場合
    float cx = dx;
    float cy = dy + dh;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}

void Sprite::RenderRightDown(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw;
    float cy = dy + dh;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}

void Sprite::RenderRightDown(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw;
    float cy = dy + dh;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    if (replaced_pixel_shader)
    {
        immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0);
    }
    else
    {
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    }
    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}


void Sprite::Render(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw * 0.5f;
    float cy = dy + dh * 0.5f;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    if (replaced_pixel_shader)
    {
        immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0);
    }
    else
    {
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    }

    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}


void Sprite::Textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
{
    float sw = static_cast<float>(texture2d_desc.Width / 11);
    float sh = static_cast<float>(texture2d_desc.Height);
    float carriage = 0;
    for (char c : s)
    {
        if (c =='.')
        {
            Render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * 10, 0, sw, sh);
        }
        else 
        {
            Render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), 0, sw, sh);
        }
        carriage += w;
    }
}

void Sprite::Textout(ID3D11DeviceContext* immediate_context, int n, float x, float y, float w, float h, float r, float g, float b, float a)
{
    float sw = static_cast<float>(texture2d_desc.Width / 10);
    float sh = static_cast<float>(texture2d_desc.Height);
    float carriage = 0;
    for (int i = n; i > 0; i /= 10)
    {
        Render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * i, 0, sw, sh);
        carriage += w;
    }
}

void Sprite::Animation(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color,
    float angle, DirectX::XMFLOAT2 texsize, int last, bool loop, bool stop)
{
    if (stop)
    {
        Render(immediate_context, pos.x, pos.y, size.x, size.y,
            color.x, color.y, color.z, color.w, angle, anime_x * texsize.x, anime_y * texsize.y, texsize.x, texsize.y);
        return;
    }

    float sw = static_cast<float>(texture2d_desc.Width);
    float sh = static_cast<float>(texture2d_desc.Height);

    anime_x_max = sw / int(texsize.x)-1;
    anime_y_max = sh / int(texsize.y)-1;

    interval<10>::run([&] {
        if (anime_x < anime_x_max)
        {
            if (anime_y != anime_y_max)  anime_x++;
            else if (anime_y == anime_y_max && anime_x < last) anime_x++;
            else if (anime_y == anime_y_max && anime_x == last && loop)
            {
                anime_x = 0;
                anime_y = 0;
            }
        }
        else
        {
            if (anime_y < anime_y_max)anime_y++;
            else if (loop) { anime_y = 0; }
            anime_x = 0;
        }
        });
    if (anime_x == last && anime_y == anime_y_max && !loop)
    {
        is_anime_end = true;
    }

    Render(immediate_context, pos.x, pos.y, size.x, size.y,
        color.x, color.y, color.z, color.w, angle, anime_x * texsize.x, anime_y * texsize.y, texsize.x, texsize.y);
}

void Sprite::Animation( ID3D11DeviceContext* immediate_context, float elapsed_time,float speed, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 size, DirectX::XMFLOAT4 color, float angle, DirectX::XMFLOAT2 texsize, int last, bool loop, bool stop)
{
    if (stop)
    {
        Render(immediate_context, pos.x, pos.y, size.x, size.y,
            color.x, color.y, color.z, color.w, angle, anime_x * texsize.x, anime_y * texsize.y, texsize.x, texsize.y);
        return;
    }

    float sw = static_cast<float>(texture2d_desc.Width);
    float sh = static_cast<float>(texture2d_desc.Height);

    anime_x_max = sw / int(texsize.x) - 1;
    anime_y_max = sh / int(texsize.y) - 1;

    timer += elapsed_time;

    if (timer >= speed)
    {
        if (anime_x < anime_x_max)
        {
            if (anime_y != anime_y_max)  anime_x++;
            else if (anime_y == anime_y_max && anime_x < last) anime_x++;
            else if (anime_y == anime_y_max && anime_x == last && loop)
            {
                anime_x = 0;
                anime_y = 0;
            }
        }
        else
        {
            if (anime_y < anime_y_max)anime_y++;
            else if (loop) { anime_y = 0; }
            anime_x = 0;
        }
        timer = 0.0f;
    }

    Render(immediate_context, pos.x, pos.y, size.x, size.y,
        color.x, color.y, color.z, color.w, angle, anime_x * texsize.x, anime_y * texsize.y, texsize.x, texsize.y);
    if (anime_x == last && anime_y == anime_y_max && !loop)
    {
        anime_x = 0;
        anime_y = 0;
        is_anime_end = true;
    }

}

void Sprite::RenderRightUp(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw;
    float cy = dy;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}

void Sprite::RenderRightUp(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
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

    // ラムダ式
    //auto rotate = [](float& x, float& y, float cx, float cy, float angle)
    std::function<void(float&, float&, float, float, float)> rotate = [](float& x, float& y, float cx, float cy, float angle)
        {
            x -= cx;
            y -= cy;

            float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
            float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
            float tx{ x }, ty{ y };
            x = cos * tx + -sin * ty;
            y = sin * tx + cos * ty;

            x += cx;
            y += cy;
        };
    //回転の中心を矩形の中心点にした場合
    float cx = dx + dw;
    float cy = dy;
    rotate(x0, y0, cx, cy, angle);
    rotate(x1, y1, cx, cy, angle);
    rotate(x2, y2, cx, cy, angle);
    rotate(x3, y3, cx, cy, angle);

    // スクリーン座標系から NDC への座標変換をおこなう
    x0 = 2.0f * x0 / viewport.Width - 1.0f;
    y0 = 1.0f - 2.0f * y0 / viewport.Height;
    x1 = 2.0f * x1 / viewport.Width - 1.0f;
    y1 = 1.0f - 2.0f * y1 / viewport.Height;
    x2 = 2.0f * x2 / viewport.Width - 1.0f;
    y2 = 1.0f - 2.0f * y2 / viewport.Height;
    x3 = 2.0f * x3 / viewport.Width - 1.0f;
    y3 = 1.0f - 2.0f * y3 / viewport.Height;

    // 計算結果で頂点バッファオブジェクトを更新する
    HRESULT hr{ S_OK };
    D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
    // D3D11_MAP_WRITE_DISCARD→リソースは書き込み用にマップされます
    hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
    _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

    vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
    if (vertices != nullptr)
    {
        vertices[0].position = { x0, y0 , 0 };
        vertices[1].position = { x1, y1 , 0 };
        vertices[2].position = { x2, y2 , 0 };
        vertices[3].position = { x3, y3 , 0 };
        vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

        vertices[0].texcoord = { sx / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[1].texcoord = { (sx + sw) / texture2d_desc.Width, sy / texture2d_desc.Height };
        vertices[2].texcoord = { sx / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
        vertices[3].texcoord = { (sx + sw) / texture2d_desc.Width, (sy + sh) / texture2d_desc.Height };
    }

    immediate_context->Unmap(vertex_buffer.Get(), 0);

    // 頂点バッファーのバインド
    UINT stride{ sizeof(vertex) };
    UINT offset{ 0 };
    immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

    // プリミティブタイプおよびデータの順序に関する情報のバインド
    // D3D11_PRIMITIVE(基本形)_TOPOLOGY(つなげ方)_TRIANGLESTRIP(STRIP＝つながっている線)
    immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // 入力レイアウトオブジェクトのバインド
    immediate_context->IASetInputLayout(input_layout.Get());

    // シェーダーのバインド
    immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
    if (replaced_pixel_shader)
    {
        immediate_context->PSSetShader(replaced_pixel_shader, nullptr, 0);
    }
    else
    {
        immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
    }
    //シェーダー リソースのバインド
    immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

    // プリミティブの描画
    immediate_context->Draw(4, 0);
}

void Sprite::RenderCenter(ID3D11DeviceContext* immediate_context, ID3D11PixelShader* replaced_pixel_shader, float dx, float dy, float dw, float dh, float angle)
{
    float x = dx - dw * 0.5f;
    float y = dy - dh * 0.5f;
    Render(immediate_context, replaced_pixel_shader, x, y, dw, dh, 1.0f, 1.0f, 1.0f, 1.0f, angle,
        0.0f, 0.0f, static_cast<float>(texture2d_desc.Width), static_cast<float>(texture2d_desc.Height));
}



//void sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
//{
//    float sw = static_cast<float>(texture2d_desc.Width / 10);
//    float sh = static_cast<float>(texture2d_desc.Height);
//    float carriage = 0;
//    for (const char c : s)
//    {
//        render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), 0, sw, sh);
//        carriage += w;
//    }
//}


//void sprite::textout(ID3D11DeviceContext* immediate_context, std::string s, float x, float y, float w, float h, float r, float g, float b, float a)
//{
//    float sw = static_cast<float>(texture2d_desc.Width / 16);
//    float sh = static_cast<float>(texture2d_desc.Height / 16);
//    float carriage = 0;
//    for (const char c : s)
//    {
//        render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0, sw * (c & 0x0F), sh * (c >> 4), sw, sh);
//        carriage += w;
//    }
//}
