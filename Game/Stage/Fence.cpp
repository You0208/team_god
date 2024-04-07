#include "Fence.h"
#include "Lemur/Graphics/DebugRenderer.h"
static Fence* instance = nullptr;

Fence::Fence()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // モデルの初期化
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\Fence.fbx");

    rotation.y = DirectX::XMConvertToRadians(270.0f);
    scale.x = scale.y = scale.z = 50.0f;

    left_rect.left_up.x = -6.0f;
    left_rect.left_up.y = 3.0f;
    left_rect.right_down.x = -5.0f;
    left_rect.right_down.y = -5.0f;

    front_rect.left_up.x = -5.0f;
    front_rect.left_up.y = -5.0f;
    front_rect.right_down.x = 3.0f;
    front_rect.right_down.y = -6.0f;

    health = 10;

    instance = this;
}

Fence::~Fence()
{
}

void Fence::Update(float elapsedTime)
{
    // 行列更新処理
    UpdateTransform();

    DrawDebugGui();
}

void Fence::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    DrawDebugPrimitive();
    // かかしモデル描画
    model->Render(scale, replaced_pixel_shader);
}

void Fence::DrawDebugGui()
{
    std::string name = "Fence";
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        ImGui::DragInt("Health", &health, 0, 10);
        ImGui::SliderFloat2("left_rect_left_up", &left_rect.left_up.x, 10.0f, -10.0f);
        ImGui::SliderFloat2("left_rect_right_down", &left_rect.right_down.x, 10.0f, -10.0f);

        ImGui::SliderFloat2("front_rect_left_up", &front_rect.left_up.x, 10.0f, -10.0f);
        ImGui::SliderFloat2("front_rect_right_down", &front_rect.right_down.x, 10.0f, -10.0f);

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
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    // 左四角の左上
    debug_renderer->DrawSphere({ left_rect.left_up.x,0.5f,left_rect.left_up.y }, 0.1f, { 0,1,0,1 });
    // 左四角の右下
    debug_renderer->DrawSphere({ left_rect.right_down.x,0.5f,left_rect.right_down.y }, 0.1f, { 0,1,0,1 });

    // 手前四角の左上
    debug_renderer->DrawSphere({ front_rect.left_up.x,0.5f,front_rect.left_up.y }, 0.1f, { 0,1,0,1 });
    // 手前四角の右下
    debug_renderer->DrawSphere({ front_rect.right_down.x,0.5f,front_rect.right_down.y }, 0.1f, { 0,1,0,1 });
}

Fence& Fence::Instance()
{
    return *instance;
}
