#include "Enemy_B.h"
#include "EnemyManager.h"
#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_B::Enemy_B()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power = 1;           // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius = 1.0f;              // 半径
    height = 1.0f;              // デバッグ用
    position.x = 5.0f;          // 初期位置
    rotation.y = -90.0f;        // 初期方向

    dis = 3.0f;                 // 一気に進む距離
    move_timer = 0.0f;          // タイマー
    move_timer_max = 3.0f;      // 動くまでの時間
    speed_power = -3.0f;        // スピード

    health = 10;                // HP
    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

void Enemy_B::Update(float elapsedTime)
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


    // 速力処理更新
    UpdateVelocity(elapsedTime);

    // 無的時間更新
    UpdateInvincibleTimer(elapsedTime);

    // モデルアニメーション更新
    model->UpdateAnimation(elapsedTime);

    // Transform の更新
    UpdateTransform();

    // デバッグ
    DrawDebugGUI();
    DrawDebugPrimitive();
}

void Enemy_B::Render(float scale, ID3D11PixelShader** replaced_pixel_shader)
{
    model->Render(scale, replaced_pixel_shader);
}

void Enemy_B::AttackUpdate(float elapsedTime)
{
    attack_timer += elapsedTime;

    if (attack_timer >= attack_interval)
    {
        if (Fence::Instance().ApplyDamage(attack_power))
        {
            model->PlayAnimation(1, false);
            attack_timer = 0.0f;
        }
    }
    if (!model->IsPlayAnimation())
    {
        model->PlayAnimation(7, true);
    }
}

void Enemy_B::MoveUpdate(float elapsedTime)
{
    // タイマーを加算
    move_timer += elapsedTime;

    if (move_timer >= move_timer_max)                   //タイマーが4秒を超えたとき
    {
        velocity.x = speed_power;                       // 速度を代入
        if (abs(old_position - position.x) >= dis)      // 既定の距離進んだら
        {
            move_timer = 0.0f;                          // 再び待機
        }
    }
    // 柵に当たったら
    else if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
    {
        velocity.x = 0.0f;                              // 速度を０に
        model->PlayAnimation(7, true);                  // アニメーションを待機に
        state_index = int(StateIndex::Attack_State);    // ステートを変更
    }
    else
    {
        // 前位置を変更
        old_position = position.x;
        // 移動
        velocity.x = 0.0f;
    }
}

void Enemy_B::DrawDebugGUI()
{
    std::string name = "Enemy_B";
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

void Enemy_B::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_B::OnDead()
{
    EnemyManager::Instance().Remove(this);
}
