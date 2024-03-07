#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <cstdint>

// framebuffer オブジェクトをテクスチャ（シェーダーリソースビュー）として扱い、画面に描画する
// 「フルスクリーンクアッド」にはピクセル シェーダとサンプラーの状態がありません。 ピクセル シェーダとサンプラーの状態を自分で作成して設定する必要があります
class FullscreenQuad
{
public:
	FullscreenQuad(ID3D11Device* device);
	virtual ~FullscreenQuad() = default;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> embedded_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> embedded_pixel_shader;

public:
	void Blit(ID3D11DeviceContext* immediate_contextbool, ID3D11ShaderResourceView** shader_resource_view, uint32_t start_slot, uint32_t num_views, ID3D11PixelShader* replaced_pixel_shader = nullptr);
};
