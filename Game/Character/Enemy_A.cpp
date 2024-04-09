#include "Enemy_A.h"
#include "EnemyManager.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"
#include "../Stage/Fence.h"

Enemy_A::Enemy_A()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;
    attack_interval = 3.0f;

    health = 10;
    radius = 1.0f;
    height = 1.0f;
    position.x = 5.0f;
    rotation.y = -90.0f;
    speed_power = -1.0f;

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

void Enemy_A::Update(float elapsedTime)
{
    switch (state_index)
    {
    case StateIndex::Move_State:
        MoveUpdate(elapsedTime);
        break;
    case StateIndex::Attack_State:
        AttackUpdate(elapsedTime);
        break;
    };

    if (health <= 0)
    {
        OnDead();
    }

    // ステート関係なく実行
    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // モデルアニメーション更新
    model->UpdateAnimation(elapsedTime);

    // デバッグ
    DrawDebugGUI();
    DrawDebugPrimitive();
}

void Enemy_A::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Enemy_A::AttackUpdate(float elapsedTime)
{
    timer += elapsedTime;

    if (timer >= attack_interval)
    {
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            model->PlayAnimation(1, false);
            timer = 0.0f;
        }
    }
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }
    
}

void Enemy_A::MoveUpdate(float elapsedTime)
{
    if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        velocity.x = 0.0f;
        model->PlayAnimation(7, true);
        state_index = int(StateIndex::Attack_State);
    }
    else
    {
        // 移動
        velocity.x = speed_power;
    }
}

void Enemy_A::DrawDebugGUI()
{
    std::string name = "Enemy_A";

    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string spe = std::string("speed") + name;
        ImGui::DragFloat(spe.c_str(), &speed_power, 5.0f, -5.0f);

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

void Enemy_A::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_A::OnDead()
{
    EnemyManager::Instance().Remove(this);
}
