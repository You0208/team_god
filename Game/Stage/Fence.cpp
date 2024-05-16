#include "Fence.h"
#include "StageManager.h"
#include "Lemur/Scene/SceneManager.h"
#include "Game/Scene/OverScene.h"
#include "Lemur/Graphics/DebugRenderer.h"

static Fence* instance = nullptr;

Fence::Fence()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // モデルの初期化
    LoadFBXModel(graphics.GetDevice(), ".\\resources\\Model\\Stage\\Fence.fbx");

    rotation.y = DirectX::XMConvertToRadians(270.0f);
    scale.x = scale.y = scale.z = 50.0f;
    rect_width = 1.0f;

    Rect stage_size = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision();

    left_rect = {
        { stage_size.left_up.x-rect_width,stage_size.left_up.y},
        { stage_size.left_up.x,stage_size.right_down.y}
    };

    front_rect = {
        { stage_size.left_up.x ,stage_size.right_down.y},
        { stage_size.right_down.x,stage_size.right_down.y-rect_width}
    };

    back_rect = {
        { stage_size.left_up.x ,stage_size.left_up.y+rect_width},
        { stage_size.right_down.x,stage_size.left_up.y}
    };

    right_rect = {
        { stage_size.right_down.x ,stage_size.left_up.y },
        { stage_size.right_down.x + rect_width,stage_size.right_down.y}
    };

    //TODO もね ここで柵の耐久値を変える
    switch (StageManager::Instance().GetStageLevel())
    {

    case 0:// レベル１
        health = 40;
        break;
    case 1:// レベル２
        health = 45;
        break;
    case 2:// レベル３
        health = 45;
        break;
    case 3:// レベル４
        health = 40;
        break;
    case 4:// レベル５
        health = 45;
        break;

    case 5:// レベル6
        health = 45;
        break;
    case 6:// レベル7
        health = 200;
        break;
    case 7:// レベル8
        health = 200;
        break;
    case 8:// レベル9
        health = 200;
        break;
    }

    
    instance = this;
}

Fence::~Fence()
{
}

void Fence::Update(float elapsedTime)
{
    if (health <= 0)
    {
        Lemur::Scene::SceneManager::Instance().ChangeScene(new OverScene);
    }

    Rect stage_size = StageManager::Instance().GetStage(StageManager::Instance().GetStageIndex())->GetStageCollision();

    left_rect = {
        { stage_size.left_up.x - rect_width,stage_size.left_up.y},
        { stage_size.left_up.x,stage_size.right_down.y}
    };

    front_rect = {
        { stage_size.left_up.x ,stage_size.right_down.y},
        { stage_size.right_down.x,stage_size.right_down.y - rect_width}
    };

    back_rect = {
        { stage_size.left_up.x ,stage_size.left_up.y + rect_width},
        { stage_size.right_down.x,stage_size.left_up.y}
    };

    right_rect = {
        { stage_size.right_down.x ,stage_size.left_up.y },
        { stage_size.right_down.x + rect_width,stage_size.right_down.y}
    };


    // 行列更新処理
    UpdateTransform();

    DrawDebugGui();
}

void Fence::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    DrawDebugPrimitive();
    // かかしモデル描画
    //model->Render(scale, replaced_pixel_shader);
}

void Fence::DrawDebugGui()
{
    ImGui::Begin(u8"柵う");
    ImGui::SliderInt(u8"耐久値", &health, 0, 100);
    ImGui::End();

    std::string name = "Fence";
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        ImGui::DragInt("Health", &health, 0, 10);
        //ImGui::SliderFloat2("back_rect_left_up", &back_rect.left_up.x, 10.0f, -10.0f);
        //ImGui::SliderFloat2("back_rect_right_down", &back_rect.right_down.x, 10.0f, -10.0f);

        //ImGui::SliderFloat2("left_rect_left_up", &left_rect.left_up.x, 10.0f, -10.0f);
        //ImGui::SliderFloat2("left_rect_right_down", &left_rect.right_down.x, 10.0f, -10.0f);

        //ImGui::SliderFloat2("front_rect_left_up", &front_rect.left_up.x, 10.0f, -10.0f);
        //ImGui::SliderFloat2("front_rect_right_down", &front_rect.right_down.x, 10.0f, -10.0f);

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
        ImGui::TreePop();
    }
}

void Fence::DrawDebugPrimitive()
{
    //DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    //DirectX::XMFLOAT3 left_rect_size = {
    //    (left_rect.right_down.x - left_rect.left_up.x) * 0.5f,
    //    0.5f,
    //    (left_rect.left_up.y - left_rect.right_down.y) * 0.5f
    //};
    //DirectX::XMFLOAT3 left_rect_center = {
    //    left_rect.left_up.x + left_rect_size.x,
    //    0,
    //    left_rect.right_down.y + left_rect_size.z
    //};

    //DirectX::XMFLOAT3 front_rect_size = {
    //(front_rect.right_down.x - front_rect.left_up.x) * 0.5f,
    //0.5f,
    //(front_rect.left_up.y - front_rect.right_down.y) * 0.5f
    //};
    //DirectX::XMFLOAT3 front_rect_center = {
    //    front_rect.left_up.x + front_rect_size.x,
    //    0,
    //    front_rect.right_down.y + front_rect_size.z
    //};

    //DirectX::XMFLOAT3 back_rect_size = {
    //(back_rect.right_down.x - back_rect.left_up.x) * 0.5f,
    //0.5f,
    //(back_rect.left_up.y - back_rect.right_down.y) * 0.5f
    //};
    //DirectX::XMFLOAT3 back_rect_center = {
    //    back_rect.left_up.x + back_rect_size.x,
    //    0,
    //    back_rect.right_down.y + back_rect_size.z
    //};

    //DirectX::XMFLOAT3 right_rect_size = {
    //(right_rect.right_down.x - right_rect.left_up.x) * 0.5f,
    //0.5f,
    //(right_rect.left_up.y - right_rect.right_down.y) * 0.5f
    //};
    //DirectX::XMFLOAT3 right_rect_center = {
    //    right_rect.left_up.x + right_rect_size.x,
    //    0,
    //    right_rect.right_down.y + right_rect_size.z
    //};

    //debug_renderer->DrawBox(left_rect_center, { 0,0,0 }, left_rect_size, { 1,0,0,1 });
    //debug_renderer->DrawBox(front_rect_center, { 0,0,0 }, front_rect_size, { 1,0,0,1 });
    //debug_renderer->DrawBox(back_rect_center, { 0,0,0 }, back_rect_size, { 1,0,0,1 });
    //debug_renderer->DrawBox(right_rect_center, { 0,0,0 }, right_rect_size, { 1,0,0,1 });
}

Fence& Fence::Instance()
{
    return *instance;
}
