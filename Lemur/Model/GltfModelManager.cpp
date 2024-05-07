#include "GltfModelManager.h"

GltfModelManager::GltfModelManager(ID3D11Device* device, const std::string& filename, bool external_texture)
{
    gltf_model = std::make_unique<GltfModel>(device, filename, external_texture);
}

void GltfModelManager::Render(const float& scale, ID3D11PixelShader* replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
    // world行列更新
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));
    gltf_model->Render(graphics.GetDeviceContext(), world, animated_nodes, replaced_pixel_shader);
}

void GltfModelManager::Render(const float& scale, ID3D11PixelShader** replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // world行列更新
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));
    gltf_model->Render(graphics.GetDeviceContext(), world, animated_nodes, replaced_pixel_shader);
}


void GltfModelManager::DrawDebug()
{
    //GetTransform()->DrawDebug();
}

void GltfModelManager::PlayAnimation(const int& index, const bool& loop, const float& speed, const float& blendSeconds)
{
    // 設定用のアニメーション番号が現在のアニメーション番号と同じ場合はreturn
    if (current_animation_index == index) return;

    blend_current_animation_index = index;    // 再生するアニメーション番号を設定
    //current_animation_seconds = 0.0f;     // アニメーション再生時間リセット

    animation_loop_flag = loop;     // ループさせるか
    animation_end_flag = false;    // 再生終了フラグをリセット

    animation_speed = speed;    // アニメーション再生速度

    animation_blend_time = 0.0f;
    animation_blend_seconds = blendSeconds;
    blend_animated_nodes = gltf_model->nodes;
}

void GltfModelManager::UpdateBlendRate(float blendRate, const float& elapsedTime)
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

#if 1
void GltfModelManager::UpdateAnimation(const float& elapsed_time)
{
    using namespace std;
    using namespace DirectX;

    // 再生中でないなら処理しない
    if (!IsPlayAnimation()) return;

    //// ブレンド率の計算
    //float blendRate = 1.0f;
    //if (animation_blend_seconds > 0.0f)
    //{
    //    //blendRate = 0.0f;
    //    blendRate = animation_blend_time / animation_blend_seconds;
    //    blendRate *= blendRate;
    //    animation_blend_time += elapsed_time;
    //}
    //if (GetAnimation()->size() > 0)
    //{
    //    // 現在のアニメーション取得
    //    GltfModel::animation& animation = GetAnimation()->at(current_animation_index);
    //    // アニメーションの各チャンネルに対して処理を行う
    //    for (vector<GltfModel::animation::channel>::const_reference channel : animation.channels)
    //    {
    //        // チャンネルに対応するサンプラーを取得
    //        const GltfModel::animation::sampler& sampler{ animation.samplers.at(channel.sampler) };
    //        // サンプラーの入力となるタイムラインを取得
    //        const vector<float>& timeline{ animation.timelines.at(sampler.input) };
    //        // タイムラインのサイズが0の場合はスキップ（不正なデータの可能性があるため）
    //        if (timeline.size() == 0)
    //        {
    //            continue;
    //        }
    //        // 一つのアニメーションに何個のキーフレームがあるか
    //        int keyCount = static_cast<int>(timeline.size());
    //        for (int keyIndex = 0; keyIndex < keyCount - 1; ++keyIndex)
    //        {
    //            //現在のキーフレーム
    //            const float& keyframe0 = timeline.at(keyIndex);
    //            //次のキーフレーム
    //            const float& keyframe1 = timeline.at(keyIndex + 1);
    //            if (current_animation_seconds >= keyframe0 &&
    //                current_animation_seconds < keyframe1)
    //            {
    //                // 再生時間とキーフレームの時間から補完率を算出する
    //                //二つのキーフレームのフレーム数の間の大きさ
    //                float fream_length = keyframe1 - keyframe0;
    //                //現在が現在のキーフレームのフレーム数からどれだけ進んだか
    //                float elapsedFrame = (current_animation_seconds - keyframe0);
    //                // 補完率
    //                float rate = elapsedFrame / fream_length;
    //                int nodeCount = static_cast<int>(gltf_model->nodes.size());
    //                // キーフレームのすべてのノードの値を補完して変更する
    //                for (int nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
    //                {
    //                    // 現在のモデル姿勢が入ってる
    //                    GltfModel::node& node = gltf_model->nodes[nodeIndex];
    //                    // ブレンド補完処理
    //                    if (blendRate < 1.0f)
    //                    {
    //                        // 次のキーフレームのデータが入る
    //                        const ModelResource::NodeKeyData& key1 = keyframe1.nodeKeys.at(nodeIndex);
    //                        // 位置補完
    //                        DirectX::XMVECTOR key0_pos = DirectX::XMLoadFloat3(&node.translate);
    //                        DirectX::XMVECTOR key1_pos = DirectX::XMLoadFloat3(&key1.translate);
    //                        DirectX::XMVECTOR Position = DirectX::XMVectorLerp(key0_pos, key1_pos, blendRate);
    //                        // 回転補完
    //                        DirectX::XMVECTOR key0_rotate = DirectX::XMLoadFloat4(&node.rotate);
    //                        DirectX::XMVECTOR key1_rotate = DirectX::XMLoadFloat4(&key1.rotate);
    //                        DirectX::XMVECTOR Rotation = DirectX::XMQuaternionSlerp(key0_rotate, key1_rotate, blendRate);
    //                        // 代入
    //                        DirectX::XMStoreFloat3(&node.translate, Position);
    //                        DirectX::XMStoreFloat4(&node.rotate, Rotation);
    //                    }
    //                    else
    //                    {
    //                    }
    //                }
    //            }
    //        }
    //    }
    //}
    //// タイムラインを取得

    // 最終フレーム処理
    if (animation_end_flag)
    {
        animation_end_flag = false; // 終了フラグをリセット
        current_animation_index = -1;    // アニメーション番号リセット
        return;
    }
    if (current_animation_index < 0)return;


    // ブレンド率の計算
    blend_rate = 1.0f;

    if (animation_blend_seconds > 0.0f)
    {
        //blendRate = 0.0f;
        blend_rate = animation_blend_time / animation_blend_seconds;
        blend_rate *= blend_rate;
        animation_blend_time += elapsed_time; 
    }

    if (blend_rate < 1.0f)
    {
        gltf_model->Animate(blend_current_animation_index, 0, blend_animated_nodes, blend_animation_loop_flag);
        //gltf_model->Animate(blend_current_animation_index, 0.0f, blend_animated_nodes, blend_animation_loop_flag);
        gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);

        const std::vector<GltfModel::node>* nodes[2] = {
             &animated_nodes,
             &blend_animated_nodes
        };

        gltf_model->BlendAnimation(nodes, blend_rate, &animated_nodes);

        // 通常アニメーションの初期化フラグ
        once = true;
    }
    else// 通常アニメーション
    {
        animated_nodes = gltf_model->nodes;
        // 一度のみ初期化
        if (once)
        {
            current_animation_index = blend_current_animation_index;
            current_animation_seconds = 0;
            once = false;
        }

        // アニメーション再生時間経過
        current_animation_seconds += elapsed_time;


        gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
    }
}
#else
void GltfModelManager::UpdateAnimation(const float& elapsed_time)
{
 
    if (current_animation_index < 0)return;

    animated_nodes = gltf_model->nodes;
    gltf_model->Animate(current_animation_index, time += elapsed_time, animated_nodes, animation_loop_flag);
}
#endif

bool GltfModelManager::IsPlayAnimation() const
{
    if (current_animation_index < 0) return false;

    const int animationIndexEnd = static_cast<int>(gltf_model->animations.size());
    if (current_animation_index >= animationIndexEnd)
    {
        return false;
    }
    return true;
}
