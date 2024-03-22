#include "StageMain.h"

StageMain::StageMain()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    stage_main_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\grid.fbx");
}

StageMain::~StageMain()
{
}

void StageMain::Update(float elapsedTime)
{
}

void StageMain::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    stage_main_model->Render(scale, replaced_pixel_shader);
    stage_main_model->DrawDebug(0);
}

bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return false;
}