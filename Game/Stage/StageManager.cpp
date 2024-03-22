#include "StageManager.h"

void StageManager::update(float elapsedTime)
{
    for (auto& stage : stages)
    {
        stage->Update(elapsedTime);
    }
}

void StageManager::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    for (auto& stage : stages)
    {
        stage->Render(scale, replaced_pixel_shader);
    }
}

void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

void StageManager::Clear()
{
    for (Stage* stage : stages)
    {
        delete stage;
    }
    stages.clear();
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool result = false;
    float distance = FLT_MAX;
    for (Stage* stage : stages)
    {
        if (stage->RayCast(start, end, hit))
        {
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
            DirectX::XMVECTOR Hit = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Hit, Start);
            DirectX::XMVECTOR Length = DirectX::XMVector3Length(Vec);
            float length;
            DirectX::XMStoreFloat(&length, Length);
            if (length < distance)
            {
                distance = length;
                result = true;
            }
        }
    }
    return result;
}

void StageManager::DrawDebugGui()
{
    for (Stage* stage : stages)
    {
        stage->DrawDebugGui();
    }
}
