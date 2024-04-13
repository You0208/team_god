#include "StageMain.h"

StageMain::StageMain()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    stage_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\grid.fbx");
    
    scale_facter = 1.0f;
    stage_width = { 5,5 };
    scale = { 1.0f,1.0f,1.0f };
    position = { 0.0f,0.0f,0.0f };
    stage_collision = {
        {position.x - stage_width.x,position.z + stage_width.y},
        {position.x + stage_width.x,position.z - stage_width.y}
    };
}

StageMain::~StageMain()
{
}

void StageMain::Update(float elapsedTime)
{
    DrawDebugGui();
    scale.x = scale.y = scale.z = scale_facter;
    stage_model->GetTransform()->SetPosition(position);
    stage_model->GetTransform()->SetScale(scale);
}

void StageMain::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    stage_model->Render(scale, replaced_pixel_shader);
}

void StageMain::DrawDebugGui()
{
    std::string name = "StageMain";
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string de = std::string("death") + name;
        std::string p = std::string("position") + name;
        ImGui::DragFloat3(p.c_str(), &position.x, 1.0f, -FLT_MAX, FLT_MAX);
        std::string s = std::string("scale") + name;
        ImGui::DragFloat3(s.c_str(), &scale.x, 0.001f, -FLT_MAX, FLT_MAX);

        std::string s_f = std::string("scale_facter") + name;
        ImGui::DragFloat(s_f.c_str(), &scale_facter, 0.001f, 0.001f, 1.0f);
        ImGui::TreePop();
    }
}