#include "Enemy_D.h"
#include "EnemyManager.h"

#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

Enemy_D::Enemy_D()
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\spider_v009.fbx");

    attack_power    = 1;        // 攻撃力
    attack_interval = 3.0f;     // 攻撃間隔

    radius          = 1.0f;     // 半径
    height          = 1.0f;     //デバッグ用
    position.x      = 5.0f;     // 初期位置
    rotation.y      = -90.0f;   // 初期方向

    move_state      = 0;        // 移動状態
    speed_power     = -1.0f;    // 横移動量
    speed_power_Y   = -1.0f;    // 縦移動量
    direction_state = 0;        // 方向の状態
    dis_max         = 1.0f;     // 移動量規定値
    is_last_touched = false;    // 前回柵に触れたか

    health          = 10;       // HP

    // とりあえずアニメーション
    model->PlayAnimation(0, true);
}

void Enemy_D::Update(float elapsedTime)
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

void Enemy_D::AttackUpdate(float elapsedTime)
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

void Enemy_D::MoveUpdate(float elapsedTime)
{
    switch (move_state)
    {
    case 0:
        // 横移動
        velocity.x = speed_power;
        // 移動量を記録
        dis += abs(speed_power) * elapsedTime;
        // 移動量が規定値を超えたら
        if (dis >= dis_max)
        {
            dis = 0.0f;                             // 移動量を初期化
            if (is_last_touched)direction_state = 0;// 前回柵に触れて居たら移動方向を固定
            else direction_state = rand() % 2;      // 次回の方向を決定
            is_last_touched = false;                // 初期化
            move_state = 1;                         // 移動状態を変化
        }

        // 左柵に当たったら
        if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
        {
            velocity.x = 0.0f;                              // 速度を０に
            model->PlayAnimation(7, true);                  // アニメーションを待機に
            state_index = int(StateIndex::Attack_State);    // ステートを変更
        }

        break;
    case 1:
        // 横移動
        velocity.x = speed_power;
        // 移動量を記録
        dis += abs(speed_power) * elapsedTime;

        // 移動量が規定値を超えたらor後ろに行ききったら
        if (dis >= dis_max|| is_last_touched)
        {
            dis = 0.0f;       // 移動量を初期化
            velocity.z = 0.0f;// 速度を初期化
            move_state = 0;   // 移動状態を変化
        }

        // 縦移動
        switch (direction_state)
        {
        case 0:
            velocity.z = speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            else if (Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
            {
                velocity.x = velocity.z = 0.0f;                 // 速度を０に
                model->PlayAnimation(7, true);                  // アニメーションを待機に
                state_index = int(StateIndex::Attack_State);    // ステートを変更
            }

            break;
        case 1:
            velocity.z = -speed_power_Y;// 縦移動

            // 後ろ方向に振り切っていたら
            if (Collision::IntersectRectVsCircle(Fence::Instance().back_rect, { position.x,position.z }, radius))
            {
                is_last_touched = true;
            }
            else if (Collision::IntersectRectVsCircle(Fence::Instance().front_rect, { position.x,position.z }, radius))
            {
                velocity.x = velocity.z = 0.0f;                 // 速度を０に
                model->PlayAnimation(7, true);                  // アニメーションを待機に
                state_index = int(StateIndex::Attack_State);    // ステートを変更
            }
            break;
        }

        // 柵に当たったら
        if (Collision::IntersectRectVsCircle(Fence::Instance().left_rect, { position.x,position.z }, radius))
        {
            velocity.x = velocity.z = 0.0f;                 // 速度を０に
            model->PlayAnimation(7, true);                  // アニメーションを待機に
            state_index = int(StateIndex::Attack_State);    // ステートを変更
        }

        break;
    }
}

void Enemy_D::DrawDebugGUI()
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

void Enemy_D::DrawDebugPrimitive()
{
    DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
    debug_renderer->DrawCylinder(position, radius, height, { 0,1,0,1 });
}

void Enemy_D::OnDead()
{
    EnemyManager::Instance().Remove(this);
}
