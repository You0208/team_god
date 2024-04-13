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

void StageManager::DrawDebugGui()
{
    for (Stage* stage : stages)
    {
        stage->DrawDebugGui();
    }
}
