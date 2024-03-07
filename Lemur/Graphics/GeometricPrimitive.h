#pragma once
#include <d3d11.h>
#include <wrl.h>

#include <directxmath.h>

class GeometricPrimitive
{
public:
	// geometric_primitiveで使うvertexなので中に（名前がぶつからないように）
	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};
	struct constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

public:
	virtual ~GeometricPrimitive() = default;

	void Render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);
protected:
	GeometricPrimitive(ID3D11Device* device);
	void CreateComBuffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);
};

class GeometricCube : public GeometricPrimitive
{
public:
	GeometricCube(ID3D11Device* device);
};

class GeometricCylinder : public GeometricPrimitive
{
public:
	GeometricCylinder(ID3D11Device* device, uint32_t slices);
};

class GeometricSphere : public GeometricPrimitive
{
public:
	GeometricSphere(ID3D11Device* device, uint32_t slices, uint32_t stacks);
};

class GeometricCapsule : public GeometricPrimitive
{
public:
	GeometricCapsule(ID3D11Device* device, float mantle_height, const DirectX::XMFLOAT3& radius, uint32_t slices, uint32_t ellipsoid_stacks, uint32_t mantle_stacks);
};
