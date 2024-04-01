#pragma once
#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>

#include "imgui.h"
#include "imgui_internal.h"
//#include "../../imgui/imgui.h"
//#include "../../imgui/imgui_internal.h"

class Transform
{
public:
	enum class CoordinateSystem
	{
		cRightYup,
		cLeftYup,
		cRightZup,
		cLeftZup,
		cNone,
	};

	Transform() :
		position(DirectX::XMFLOAT3())
		, scale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f))
		, rotation(DirectX::XMFLOAT4())
		, scaleFactor(1.0f)
		, coordinateSystem(static_cast<int>(CoordinateSystem::cRightYup))
	{}
	~Transform() {}

	void DrawDebug(std::string i);
	void Reset();

	DirectX::XMMATRIX CalcWorld();
	DirectX::XMMATRIX CalcWorldMatrix(float scale_factor);
	DirectX::XMFLOAT3 CalcForward()const;
	DirectX::XMFLOAT3 CalcUp()const;
	DirectX::XMFLOAT3 CalcRight()const;

	DirectX::XMFLOAT3 GetPosition()const { return position; }
	DirectX::XMFLOAT3 GetScale()const { return scale; }
	DirectX::XMFLOAT4 GetRotation()const { return rotation; }

	void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
	void SetPositionX(const float& posX) { position.x = posX; }
	void SetPositionY(const float& posY) { position.y = posY; }
	void SetPositionZ(const float& posZ) { position.z = posZ; }

	void SetScale(const DirectX::XMFLOAT3& scl) { scale = scl; }

	void SetRotation(const DirectX::XMFLOAT4& rot) { rotation = rot; }
	void SetRotationX(const float rotX) { rotation.x = rotX; }
	void SetRotationY(const float rotY) { rotation.y = rotY; }
	void SetRotationZ(const float rotZ) { rotation.z = rotZ; }
	void AddRotationX(const float rotX) { rotation.x += rotX; }
	void AddRotationY(const float rotY) { rotation.y += rotY; }
	void AddRotationZ(const float rotZ) { rotation.z += rotZ; }

	void AddPosition(const DirectX::XMFLOAT3& pos)
	{
		position.x += pos.x;
		position.y += pos.y;
		position.z += pos.z;
	}
	void AddPositionX(const float posX) { position.x += posX; }
	void AddPositionY(const float posY) { position.y += posY; }
	void AddPositionZ(const float posZ) { position.z += posZ; }


	float GetScaleFactor()const { return scaleFactor; }
	void SetScaleFactor(float f) { scaleFactor = f; }

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 rotation;

	float scaleFactor;

	int coordinateSystem;
	const DirectX::XMFLOAT4X4 CoordinateSystemTransforms[static_cast<int>(CoordinateSystem::cNone)]
	{
		{
			-1, 0, 0, 0,
			 0, 1, 0, 0,
			 0, 0, 1, 0,
			 0, 0, 0, 1,
		},

		{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		},

		{
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		},
		{
			1, 0, 0, 0,
			0, 0, 1, 0,
			0, 1, 0, 0,
			0, 0, 0, 1
		}
	};

	const std::string coordinateSystemName[static_cast<int>(CoordinateSystem::cNone)]
	{
		"Right Hand, Y up",
		"Left Hand, Y up",
		"Right Hand, Z up",
		"Left Hand, Z up"
	};
};

