#include "Character.h"

#include "Lemur/Math/Easing.h"
#include "Lemur/Math/MathHelper.h"
#include "Lemur/Collision/Collision.h"


bool Character::ApplyDamage(int damage)
{
    // 死亡している間は健康状態を変更しない
    if (health <= 0)return false;

    // ダメージ処理
    health -= damage;

    // 死亡通知
    if (health <= 0)
    {
        health = 0;
        death = true;
        OnDead();
    }
    else
    {
        OnDamage();
    }

    // 健康状態が変更した場合はtrueを返す
    return true;
}

void Character::DrawDebugGUI(std::string name, int i)
{
    std::string num = std::to_string(i);
    name = name + num;
    std::string T = std::string("Transform") + name;
    if (ImGui::TreeNode(T.c_str()))
    {
        std::string de = std::string("death") + name;
        ImGui::Checkbox(de.c_str(), &death);

        std::string hp = std::string("HP") + name;
        ImGui::DragInt(hp.c_str(), &health, 5.0f, -5.0f);

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

void Character::Render(float scale_, ID3D11PixelShader* replaced_pixel_shader)
{
    if (is_gltf)
    {
       gltf_model->Render(scale_* model_scale, replaced_pixel_shader);
    }
    else
    {
        model->Render(scale_* model_scale, replaced_pixel_shader);
    }
}

void Character::Render(float scale_, ID3D11PixelShader** replaced_pixel_shader)
{
    if (is_gltf)
    {
        gltf_model->Render(scale_* model_scale,replaced_pixel_shader);
    }
    else
    {
        model->Render(scale_* model_scale, replaced_pixel_shader);
    }
}

void Character::CollisionRender(float scale_, ID3D11PixelShader* replaced_pixel_shader)
{
    if (collision_model)collision_model->Render(scale_ * collision_scale_facter, replaced_pixel_shader);
}


void Character::Move(float vx, float vz, float speed)
{
    // 移動方向ベクトルを設定
    moveVecX = vx;
    moveVecZ = vz;

    // 最大速度設定
    maxMoveSpeed = speed;
}

void Character::Turn(float elapsed_time,float vx, float vz, float speed)
{
    speed *= high_resolution_timer::Instance().time_interval();

    // ベクトルの大きさを取得
    float Length = sqrtf(vx * vx + vz * vz);

    // ベクトルの大きさが0なら(ゼロベクトルなら)
    if (Length <= 0.01)
    {
        return;
    }

    // 進行ベクトルの正規化
    vx = vx / Length;
    vz = vz / Length;

    // 自身の回転値から前方向を求める。
    float frontX = sinf(rotation.y);
    float frontZ = cosf(rotation.y);

    // 回転角を求めるために、2つの単位ベクトルの内積を計算する
    float dot = (vx * frontX) + (vz * frontZ);

    // dot は -1.0f ～ 1.0f になる。なので rot は 0.0f ～ 2.0f になる。
    float rot = 1.0f - dot;

    // 内積が小さくなったら
    if (rot < speed) speed = rot; // その分向きを変える角度も小さくする

    // 左右判定を行うために2つの単位ベクトルの外積を計算する
    float cross = (vx * frontZ) - (vz * frontX);

    // 2Dの外積値が正の場合か負の場合によって左右反転が行える
    // 左右判定を行うことによって左右回転を選択する
    if (cross < 0.0f)
    {
        rotation.y -= speed;
    }
    else
    {
        rotation.y += speed;
    }

}

void Character::Jump(float speed)
{
    // 上方向の力を設定
    velocity.y = speed;
}

void Character::UpdateVelocity(float elapsed_time)
{
    // 経過フレーム
    float elapsed_frame = 60.0f * elapsed_time;

    if (!is_fly)
    {
        // 垂直速力更新処理
        UpdataVerticalVelocity(elapsed_frame);

        // 垂直移動更新処理
        UpdateVerticalMove(elapsed_time);

        // 水平速力更新処理
        UpdataHorizontalVelocity(elapsed_frame);

        // 水平移動更新処理
        UpdateHorizontalMove(elapsed_time);
    }
}

void Character::HitStopON(float hit_stop_time_)
{
    hit_stop_time = hit_stop_time_;
    hit_stop_timer = 0;
    is_hit_stop = true;
}

void Character::UpdataVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    velocity.y += gravity * elapsedFrame;//グラビティと単位を合わせるためにelapsedFrameを使っている
}

void Character::UpdateVerticalMove(float elapsedTime)
{
    // 移動処理
    position.y += velocity.y * elapsedTime;

    // 地面判定
    if (position.y < 0.0f)
    {
        position.y = 0.0f;
        velocity.y = 0.0f;

        // 着地した
        if (!isGround)
        {
            OnLanding();
        }
        isGround = true;
    }
    else
    {
        isGround = false;
    }
    position.y = 0.0f;
}

void Character::UpdataHorizontalVelocity(float elapsedFrame)
{   // XZ平面の速力を減速する
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // 摩擦力
        float friction = this->friction * elapsedFrame;

        // 空中にいるときは摩擦力を減らす
        if (!isGround)friction *= airControl;

        // 摩擦による横方向の減速処理
        if (length > friction)
        {
            float vx = velocity.x / length;
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        // 横方向の速力が摩擦力以下になったので速力を無効化
        else
        {
            velocity.x = 0;
            velocity.z = 0;
        }
    }

    // XZ平面の速力を加速する
    if (length <= maxMoveSpeed)
    {
        // 移動ベクトルがゼロベクトル出ないなら加速する
        float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
        if (moveVecLength > 0.0f)
        {
            // 加速力
            float acceleration = this->acceleration * elapsedFrame;

            // 空中にいるときは摩擦力を減らす
            if (!isGround)acceleration *= airControl;

            // 移動ベクトルによる加速処理
            velocity.x += moveVecX * acceleration;
            velocity.z += moveVecZ * acceleration;

            // 最大速度制限
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                float vx = velocity.x / length;
                float vz = velocity.z / length;

                velocity.x = vx * maxMoveSpeed;
                velocity.z = vz * maxMoveSpeed;
            }
        }
    }
    // 移動ベクトルをリセット
    moveVecX = 0.0f;
    moveVecZ = 0.0f;
}

void Character::UpdateHorizontalMove(float elapsedTime)
{
    // 移動処理
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;
}

void Character::HitStopCalc()
{
    if (hit_stop_timer > hit_stop_time)
    {
        hit_stop_rate = 1.0f;
        is_hit_stop = false;

        return;
    }

    hit_stop_rate = Easing::InSine(hit_stop_timer, hit_stop_time, 1.0f, 0.0f);

    hit_stop_timer += high_resolution_timer::Instance().time_interval();
}

void Character::UpdateTransform()
{
    if (!is_gltf)
    {
        model->GetTransform()->SetPosition(position);
        model->GetTransform()->SetScale(scale);
        model->GetTransform()->SetRotation({ rotation.x,rotation.y,rotation.z,0 });

        if (collision_model)
        {
            collision_model->GetTransform()->SetPosition(collision_pos);
            collision_model->GetTransform()->SetScale(collision_scale);
            collision_model->GetTransform()->SetRotation({ collision_rotation.x,collision_rotation.y,collision_rotation.z,0 });
        }
    }
    else 
    {
        gltf_model->GetTransform()->SetPosition(position);
        gltf_model->GetTransform()->SetScale(scale);
        gltf_model->GetTransform()->SetRotation({ rotation.x,rotation.y,rotation.z,0 });

        if (collision_model)
        {
            collision_model->GetTransform()->SetPosition(collision_pos);
            collision_model->GetTransform()->SetScale(collision_scale);
            collision_model->GetTransform()->SetRotation({ collision_rotation.x,collision_rotation.y,collision_rotation.z,0 });
        }
    }
}

void Character::UpdateInvincibleTimer(float elapsedTime)
{
    if (invincibleTimer > 0.0f)
    {
        invincibleTimer -= elapsedTime;
    }
}
