#include "Transform.h"

// デバッグ用
void Transform::DrawDebug(int i)
{
    if (ImGui::TreeNode("Transform"))
    {
        std::string p = std::string("position") + std::to_string(i);
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + std::to_string(i);
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string r = std::string("rotation") + std::to_string(i);
        DirectX::XMFLOAT3 rot{};
        rot.x = DirectX::XMConvertToDegrees(rotation.x);
        rot.y = DirectX::XMConvertToDegrees(rotation.y);
        rot.z = DirectX::XMConvertToDegrees(rotation.z);
        ImGui::DragFloat3(r.c_str(), &rot.x, 0.5f, -FLT_MAX, FLT_MAX);
        rotation.x = DirectX::XMConvertToRadians(rot.x);
        rotation.y = DirectX::XMConvertToRadians(rot.y);
        rotation.z = DirectX::XMConvertToRadians(rot.z);

        std::string s_f = std::string("scale_facter") + std::to_string(i);
        ImGui::DragFloat(s_f.c_str(), &scaleFactor, 0.001f, 0.001f, 1.0f);

        std::string c = std::string("Coordinate System") + std::to_string(i);
        ImGui::SliderInt(c.c_str(), &coordinateSystem, 0, static_cast<int>(CoordinateSystem::cLeftZup));
        ImGui::Text(coordinateSystemName[coordinateSystem].c_str());

        std::string re = std::string("Reset") + std::to_string(i);
        if (ImGui::Button(re.c_str()))
        {
            Reset();
        }

        ImGui::TreePop();
    }
}

void Transform::Reset()
{
    position = DirectX::XMFLOAT3(0, 0, 0);
    scale = DirectX::XMFLOAT3(1, 1, 1);
    rotation = DirectX::XMFLOAT4(0, 0, 0, 0);
}

// ワールド座標取得
DirectX::XMMATRIX Transform::CalcWorld()
{
    const DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) * DirectX::XMMatrixScaling(scaleFactor,scaleFactor,scaleFactor) };
    const DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
    const DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x,position.y,position.z) };

    return S * R * T;
}

// 座標系をかけたワールド行列
DirectX::XMMATRIX Transform::CalcWorldMatrix(float scale_factor)
{
    const DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&CoordinateSystemTransforms[coordinateSystem])
    * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };
    return C * CalcWorld();
}

// 前方向ベクトル取得
DirectX::XMFLOAT3 Transform::CalcForward() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT3 forward{};
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(rotationMatrix.r[2]));
    return forward;
}

// 上方向ベクトル取得
DirectX::XMFLOAT3 Transform::CalcUp() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT3 up{};
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(rotationMatrix.r[1]));
    return up;
}

// 右方向ベクトル取得
DirectX::XMFLOAT3 Transform::CalcRight() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT3 right{};
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3Normalize(rotationMatrix.r[0]));
    return right;
}
