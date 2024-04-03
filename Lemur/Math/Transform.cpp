#include "Transform.h"

// �f�o�b�O�p
void Transform::DrawDebug(std::string name)
{
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string r = std::string("rotation") + name;
        DirectX::XMFLOAT3 rot{};
        rot.x = DirectX::XMConvertToDegrees(rotation.x);
        rot.y = DirectX::XMConvertToDegrees(rotation.y);
        rot.z = DirectX::XMConvertToDegrees(rotation.z);
        ImGui::DragFloat3(r.c_str(), &rot.x, 0.5f, -FLT_MAX, FLT_MAX);
        rotation.x = DirectX::XMConvertToRadians(rot.x);
        rotation.y = DirectX::XMConvertToRadians(rot.y);
        rotation.z = DirectX::XMConvertToRadians(rot.z);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scaleFactor, 0.001f, 0.001f, 1.0f);

        std::string c = std::string("Coordinate System") + name;
        ImGui::SliderInt(c.c_str(), &coordinateSystem, 0, static_cast<int>(CoordinateSystem::cLeftZup));
        ImGui::Text(coordinateSystemName[coordinateSystem].c_str());

        std::string re = std::string("Reset") + name;
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

// ���[���h���W�擾
DirectX::XMMATRIX Transform::CalcWorld()
{
    const DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) * DirectX::XMMatrixScaling(scaleFactor,scaleFactor,scaleFactor) };
    const DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x,rotation.y,rotation.z) };
    const DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x,position.y,position.z) };

    return S * R * T;
}

// ���W�n�����������[���h�s��
DirectX::XMMATRIX Transform::CalcWorldMatrix(float scale_factor)
{
    const DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&CoordinateSystemTransforms[coordinateSystem])
    * DirectX::XMMatrixScaling(scale_factor,scale_factor,scale_factor) };
    return C * CalcWorld();
}

// �O�����x�N�g���擾
DirectX::XMFLOAT3 Transform::CalcForward() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT3 forward{};
    DirectX::XMStoreFloat3(&forward, DirectX::XMVector3Normalize(rotationMatrix.r[2]));
    return forward;
}

// ������x�N�g���擾
DirectX::XMFLOAT3 Transform::CalcUp() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT3 up{};
    DirectX::XMStoreFloat3(&up, DirectX::XMVector3Normalize(rotationMatrix.r[1]));
    return up;
}

// �E�����x�N�g���擾
DirectX::XMFLOAT3 Transform::CalcRight() const
{
    DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
    DirectX::XMFLOAT3 right{};
    DirectX::XMStoreFloat3(&right, DirectX::XMVector3Normalize(rotationMatrix.r[0]));
    return right;
}