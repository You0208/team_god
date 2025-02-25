#include "GameObject.h"

#include "Lemur/Math/Easing.h"
#include "Lemur/Math/MathHelper.h"
#include "Lemur/Collision/Collision.h"


void GameObject::AnimationUpdate(float elapsedTime)
{
    UpdateAnimation(elapsedTime);
}

void GameObject::RootmationUpdate(float elapsedTime) 
{
    if (root_motion_node_index < 0)return;
    if (frame_index < 1)return;
    Animation::keyframe::node node;
    Animation::keyframe::node old_node;
    Animation::keyframe::node begin_node;


    Model->ComputeAnimation(animation_index, root_motion_node_index, frame_index, node);
    Model->ComputeAnimation(animation_index, root_motion_node_index, frame_index-1, old_node);
    Model->ComputeAnimation(animation_index, root_motion_node_index, 0, begin_node);

    keyframe.nodes.at(root_motion_node_index).translation =begin_node.translation;
    Model->UpdateAnimation(keyframe);

    DirectX::XMFLOAT3 transration = node.translation - old_node.translation;
    DirectX::XMMATRIX World = DirectX::XMLoadFloat4x4(&world);
    DirectX::XMVECTOR Translation = DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&transration), World);
    DirectX::XMStoreFloat3(&transration, Translation);
    // todo ここ数字いじれる。
    position += transration * 1.0f;
}

bool GameObject::ApplyDamage(int damage)
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
        OnDamaged();
    }

    // 健康状態が変更した場合はtrueを返す
    return true;
}

void GameObject::Render(float elapsedTime, ID3D11PixelShader* replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();

    // 左手系・Y 軸アップへ変換
    const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
    };
#if 0
    // 単位をセンチメートルからメートルに変更するには、「scale_factor」を 0.01 に設定
    const float scale_factor = 1.0f; // To change the units from centimeters to meters, set 'scale_factor' to 0.01.
#else
    const float scale_factor = 0.01f; // To change the units from centimeters to meters, set 'scale_factor' to 0.01.
#endif
    // 変換用
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[0])* DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x,scale.y,scale.z) };
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y,rotation.z) };
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };
    // ワールド変換行列を作成
    DirectX::XMStoreFloat4x4(&world, C * S * R * T);


    if (Model->animation_clips.size() > 0)
    {
#if 1

#else
        animation::keyframe keyframe;
        const animation::keyframe* keyframes[2]{
            &skinned_meshes[0]->animation_clips.at(0).sequence.at(40),
            &skinned_meshes[0]->animation_clips.at(0).sequence.at(80)
        };
        skinned_meshes[0]->BlendAnimations(keyframes, factors[2], keyframe);
        skinned_meshes[0]->update_animation(keyframe);

# endif
        Model->Render(immediate_context, world,material_color, &keyframe, replaced_pixel_shader);
    }
    else
    {
        Model->Render(immediate_context, world, material_color, nullptr, replaced_pixel_shader);
    }
}

void GameObject::Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();

    // 左手系・Y 軸アップへ変換
    const DirectX::XMFLOAT4X4 coordinate_system_transforms[]{
        { -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
        { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },		// 1:LHS Y-UP
        { -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
        { 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
    };
#if 0
    // 単位をセンチメートルからメートルに変更するには、「scale_factor」を 0.01 に設定
    const float scale_factor = 1.0f; // To change the units from centimeters to meters, set 'scale_factor' to 0.01.
#else
    const float scale_factor = 0.01f; // To change the units from centimeters to meters, set 'scale_factor' to 0.01.
#endif
    // 変換用
    DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[0])* DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };
    DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };
    DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) };
    DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };
    // ワールド変換行列を作成
    DirectX::XMStoreFloat4x4(&world, C * S * R * T);


    if (Model->animation_clips.size() > 0)
    {
#if 1
#else
        animation::keyframe keyframe;
        const animation::keyframe* keyframes[2]{
            &skinned_meshes[0]->animation_clips.at(0).sequence.at(40),
            &skinned_meshes[0]->animation_clips.at(0).sequence.at(80)
        };
        skinned_meshes[0]->BlendAnimations(keyframes, factors[2], keyframe);
        skinned_meshes[0]->update_animation(keyframe);

# endif
        Model->Render(immediate_context, world, material_color, &keyframe, replaced_pixel_shader);
    }
    else
    {
        Model->Render(immediate_context, world, material_color, nullptr, replaced_pixel_shader);
    }
}

void GameObject::Move(float vx, float vz, float speed)
{
    // 移動方向ベクトルを設定
    moveVecX = vx;
    moveVecZ = vz;

    // 最大速度設定
    maxMoveSpeed = speed;
}

void GameObject::Turn(float vx, float vz, float speed)
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

    // dot は -1.0f 〜 1.0f になる。なので rot は 0.0f 〜 2.0f になる。
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

void GameObject::Jump(float speed)
{
    // 上方向の力を設定
    velocity.y = speed;
}

void GameObject::UpdateVelocity(float elapsedTime)
{
    // 経過フレーム
    float elapsedFrame = 60.0f * elapsedTime;

    // 垂直速力更新処理
    UpdataVerticalVelocity(elapsedFrame);

    // 水平速力更新処理
    UpdataHorizontalVelocity(elapsedFrame);

    // 垂直移動更新処理
    UpdateVerticalMove(elapsedTime);

    // 水平移動更新処理
    UpdateHorizontalMove(elapsedTime);
}

void GameObject::HitStopON(float hit_stop_time_)
{
    hit_stop_time = hit_stop_time_;
    hit_stop_timer = 0;
    is_hit_stop = true;
}

void GameObject::UpdataVerticalVelocity(float elapsedFrame)
{
    // 重力処理
    velocity.y += gravity * elapsedFrame;//グラビティと単位を合わせるためにelapsedFrameを使っている
}

void GameObject::UpdateVerticalMove(float elapsedTime)
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

void GameObject::UpdataHorizontalVelocity(float elapsedFrame)
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

void GameObject::UpdateHorizontalMove(float elapsedTime)
{
    // 移動処理
    position.x += velocity.x * elapsedTime;
    position.z += velocity.z * elapsedTime;

    if (position.x < -20)
        position.x = -20;
    else if (position.x > 22)
        position.x = 22;

    if (position.z < 3)
        position.z = 3;
    //else if (position.z > 50)
    //    position.z = 50;
    else if (position.z > 55)
        position.z = 55;


#if 0
    // 移動量取得
    float VecX = position.x + velocity.x - position.x;
    float VecZ = position.z + velocity.z - position.z;

    // 水平速力量計算
    float velocityLengthXZ = sqrtf((VecX * VecX) + (VecZ * VecZ));

    if (velocityLengthXZ > 0.0f)
    {
        // 水平移動値
        float mx = velocity.x * elapsedTime;
        float mz = velocity.z * elapsedTime;

        // レイの開始位置と終点位置設定
        DirectX::XMFLOAT4 start = { position.x,position.y,position.z,1 };
        DirectX::XMFLOAT4 end = {
            position.x + mx,
            position.y ,
            position.z + mz,
            1
        };
        start.y += 0.1;
        end.y += 0.1;
        // レイ始点
        DirectX::XMVECTOR Start = DirectX::XMLoadFloat4(&start);
        // レイ終点
        DirectX::XMVECTOR End = DirectX::XMLoadFloat4(&end);

        // レイの方向
        DirectX::XMVECTOR RayVec = DirectX::XMVector4Normalize(DirectX::XMVectorSubtract(End, Start));
        DirectX::XMFLOAT4 ray_vec;
        DirectX::XMStoreFloat4(&ray_vec, RayVec);

        // レイキャストによる壁判定

        // 交点
        DirectX::XMFLOAT4 hit_pos;

        std::string intersected_mesh;
        std::string intersected_material;
        DirectX::XMFLOAT3 hit_normal;

        if(Model->RayCast(start, ray_vec, world, hit_pos, hit_normal, intersected_mesh, intersected_material))
        {
            DirectX::XMVECTOR Hit = DirectX::XMLoadFloat4(&hit_pos);
            DirectX::XMVECTOR Start_to_Hit = DirectX::XMVectorSubtract(Hit, Start);
            float ray_length = DirectX::XMVectorGetX(DirectX::XMVector4Length(Start_to_Hit));


            // ヒットした
            if (ray_length < velocityLengthXZ)
            {
                // 移動量ベクトル終点
                DirectX::XMVECTOR MoveVecEnd = DirectX::XMVectorScale(DirectX::XMVector3Normalize(Start), velocityLengthXZ);

                // ヒットしたポリゴンの法線
                DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit_normal);

                DirectX::XMVECTOR Hit_To_MoveVecEnd = DirectX::XMVectorSubtract(MoveVecEnd, Hit);
                // 射影ベクトルを算出
                DirectX::XMVECTOR Projection = DirectX::XMVectorScale(Normal,
                    DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVectorNegate(Hit_To_MoveVecEnd), Normal)));

                // 多めに補正するためにちょっと大きくする
                Projection = DirectX::XMVectorScale(Projection, 1.01f);

                // 進行ベクトルの終点と射影ベクトルを足して補正後の位置ベクトル算出。
                DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Projection, MoveVecEnd);

                // 補正後の位置
                DirectX::XMFLOAT4 new_pos;
                DirectX::XMStoreFloat4(&new_pos, CollectPosition);

                /*------------ 交点から補正後の位置の間でもっかいレイキャスト ----------*/

                // 交点から補正後の位置までのベクトル
                DirectX::XMVECTOR Hit_To_NewPos = DirectX::XMVectorSubtract(CollectPosition, Hit);
                Hit_To_NewPos = DirectX::XMVector3Normalize(Hit_To_NewPos);

                DirectX::XMStoreFloat4(&ray_vec, Hit_To_NewPos);
                if (Model->RayCast(new_pos, ray_vec, world, hit_pos, hit_normal, intersected_mesh, intersected_material))
                {
                    position.x = hit_pos.x;
                    position.y = hit_pos.y;
                    position.z = hit_pos.z;
                }
                else
                {
                    position.x = new_pos.x;
                    position.y = new_pos.y;
                    position.z = new_pos.z;
                }

            }
        }
        else
        {
            position.x += mx;
            position.z += mz;
        }
    }
#endif
    

}

void GameObject::HitStopCalc()
{
    //todo ヒットストップどっちにするか決める

#if 0 ヒットストップをブールでして完全に止まるパターン
    if(hit_stop_timer>hit_stop_time)
    {
        is_hit_stop = false;
    }

    hit_stop_timer += high_resolution_timer::Instance().time_interval();
#else ヒットストップをイージングでしてちょっとずつ動き出すパターン

    if (hit_stop_timer > hit_stop_time)
    {
        hit_stop_rate = 1.0f;
        is_hit_stop = false;

        return;
    }

    hit_stop_rate = Easing::InSine(hit_stop_timer, hit_stop_time, 1.0f, 0.0f);

    hit_stop_timer += high_resolution_timer::Instance().time_interval();
#endif

}

void GameObject::UpdateAnimation(float elapsedTime)
{
    // 再生中でないなら処理しない
    if (!IsPlayAnimation()) return;

    // 最終フレーム処理
    if (end_animation)
    {
        end_animation = false; // 終了フラグをリセット
        animation_index = -1;    // アニメーション番号リセット
        return;
    }
    // アニメーション再生時間経過
    animation_tick += elapsedTime;

    // 指定のアニメーションデータを取得
    Animation& animation = GetAnimation()->at(animation_index);

    // 現在の時間がどのキーフレームの間にいるのか判定する
    const float  frameIndex_float = (animation_tick * animation.sampling_rate) * anim_calc_rate * hit_stop_rate;
    const size_t frameIndex = static_cast<const size_t>(frameIndex_float);

    // 最終フレーム
    const size_t frameEnd = (animation.sequence.size() - 1);

    // 再生時間が終端時間を超えたら
    if (frameIndex > frameEnd)
    {
        // ループ再生する場合
        if (animation_loop_flag)
        {
            animation_tick = 0.0f;
            return;
        }
        // ループ再生しない場合
        else
        {
            end_animation = true;
            return;
        }

    }
    // 再生時間とキーフレームの時間かた補間率を算出する
    else if ((keyframe.nodes.size() > 0) && frameIndex < frameEnd)
    {
        // ブレンド率の計算
        float blendRate = 0.2f;
        UpdateBlendRate(blendRate, elapsedTime);

        // キーフレーム取得
        const std::vector<Animation::keyframe>& keyframes = animation.sequence;

        // 現在の時間がどのキーフレームの間にいるのか判定する
        const Animation::keyframe* keyframe_[2] = {
            &keyframe,
            &keyframes.at(frameIndex + 1)
        };

        // 再生時間とキーフレームの時間から補間率を計算する
        Model->BlendAnimations(keyframe_, blendRate, keyframe);

        // トランスフォーム更新
        Model->UpdateAnimation(keyframe);
    }
    else
    {
        keyframe = animation.sequence.at(frameIndex);
    }
}

void GameObject::UpdateBlendRate(float blendRate, const float& elapsedTime)
{
    if (animation_blend_time < animation_blend_seconds)
    {
        animation_blend_time += elapsedTime;
        if (animation_blend_time >= animation_blend_seconds)
        {
            animation_blend_time = animation_blend_seconds;
        }
        blendRate = animation_blend_time / animation_blend_seconds;
        blendRate *= blendRate;
    }
}

bool GameObject::IsPlayAnimation() const
{
    if (animation_index < 0)return false;
    if (animation_index >= Model->animation_clips.size()) return false;

    return true;
}
