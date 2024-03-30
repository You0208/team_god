#include "Unit_A.h"
#include "Lemur/Input/Input.h"

Unit_A::Unit_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model_unit_A = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Unit\\Chili_24_0305_01.fbx");

    scale.x = scale.y = scale.z = 0.01f;
    radius = 0.5f;
    position = { 0,5.0f,0 };
    height = 1.0f;

    // �Ƃ肠�����A�j���[�V����
    model_unit_A->PlayAnimation(0, true);
}

Unit_A::~Unit_A()
{
}

void Unit_A::Update(float elapsedTime)
{
    // ���͏����X�V
    UpdateVelocity(elapsedTime);

    // ���I���ԍX�V
    UpdateInvincibleTimer(elapsedTime);

    // Transform �̍X�V
    UpdateTransform();

    // ���f���A�j���[�V�����X�V����
    model_unit_A->UpdateAnimation(elapsedTime);

}

void Unit_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model_unit_A->Render(scale, replaced_pixel_shader);
}

void Unit_A::DrawDebugGUI()
{
    if (ImGui::TreeNode("Unit_A"))
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            // �ʒu
            ImGui::InputFloat3("position", &position.x);

            // ��]
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(rotation.x);
            a.y = DirectX::XMConvertToDegrees(rotation.y);
            a.z = DirectX::XMConvertToDegrees(rotation.z);
            ImGui::InputFloat3("rotation", &a.x);
            rotation.x = DirectX::XMConvertToRadians(a.x);
            rotation.y = DirectX::XMConvertToRadians(a.y);
            rotation.z = DirectX::XMConvertToRadians(a.z);

            // �X�P�[��
            ImGui::InputFloat3("Scale", &scale.x);

            // velocity
            ImGui::InputFloat3("velocity", &velocity.x);
            // ���n���Ă邩
            ImGui::Checkbox("isGround", &isGround);
            // �W�����v�J�E���g
            ImGui::InputInt("jumpCount", &jumpCount);

            // ���˃��[�g
            ImGui::InputFloat("LuanchRate", &LuanchRate);
            // ���˃N�[���^�C��
            ImGui::InputFloat("LuanchCollTime", &LuanchCollTime);
        }
        ImGui::TreePop();
    }
}

void Unit_A::DrawDebugPrimitive()
{
}

