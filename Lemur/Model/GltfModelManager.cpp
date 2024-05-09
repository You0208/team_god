#include "GltfModelManager.h"

GltfModelManager::GltfModelManager(ID3D11Device* device, const std::string& filename, bool external_texture)
{
    gltf_model = std::make_unique<GltfModel>(device, filename, external_texture);
    animated_nodes = gltf_model->nodes;
    blend_animated_nodes = gltf_model->nodes;
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

void GltfModelManager::PlayAnimation(const int& index, const bool& loop, const bool& is_blend_, const float& speed, const float& blendSeconds)
{
    // 設定用のアニメーション番号が現在のアニメーション番号と同じ場合はreturn
    if (current_animation_index == index) return;
    is_blend = is_blend_;
    if (is_blend)
    {
        animation_blend_seconds = blendSeconds;
        animation_blend_time = 0.0f;
        blend_current_animation_index = index;    // 再生するアニメーション番号を設定

    }
    else
    {
        current_animation_index = index;
        current_animation_seconds = 0.0f;     // アニメーション再生時間リセット
    }

    animation_loop_flag = loop;     // ループさせるか
    animation_end_flag = false;    // 再生終了フラグをリセット
    gltf_model->end_flag = false;

    animation_speed = speed;    // アニメーション再生速度

}

void GltfModelManager::UpdateAnimation(const float& elapsed_time)
{
    using namespace std;
    using namespace DirectX;

    // 再生中でないなら処理しない
    if (!IsPlayAnimation()) return;

    // 最終フレーム処理
    if (gltf_model->end_flag)
    {
        animation_end_flag = false;
        gltf_model->end_flag = false; // 終了フラグをリセット
        current_animation_index = -1;    // アニメーション番号リセット
        return;
    }
    if (current_animation_index < 0)return;


    if (is_blend)
    {
        // ブレンド率の計算
        blend_rate = 1.0f;
        if (animation_blend_seconds > 0.0f)
        {
            blend_rate = animation_blend_time / animation_blend_seconds;
            blend_rate *= blend_rate;
            animation_blend_time += elapsed_time;
        }

        // ブレンド率が1.0以下の時
        if (blend_rate < 1.0f)
        {
            gltf_model->Animate(blend_current_animation_index, 0, blend_animated_nodes, blend_animation_loop_flag);
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
            // 一度のみ初期化
            if (once)
            {
                current_animation_index = blend_current_animation_index;
                current_animation_seconds = 0;
                once = false;
            }

            // アニメーション再生時間経過
            current_animation_seconds += elapsed_time;

            // 通常通りアニメーション
            gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
            if (gltf_model->end_flag)
            {
                if (animation_loop_flag)
                {
                    current_animation_seconds = 0.0f;
                    return;
                }
                // ループ再生しない場合
                else
                {
                    animation_end_flag = true;
                    return;
                }
            }

        }
    }
    else// 通常アニメーション
    {
        // アニメーション再生時間経過
        current_animation_seconds += elapsed_time;

        gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
        if (gltf_model->end_flag)
        {
            if (animation_loop_flag)
            {
                current_animation_seconds = 0.0f;
                return;
            }
            // ループ再生しない場合
            else
            {
                animation_end_flag = true;
                return;
            }
        }

    }
}

bool GltfModelManager::IsPlayAnimation() const
{
    if (current_animation_index < 0) return false;

    return true;
}

void GltfModelManager::Dissolve(const float& elapsedTime)
{
    if (gltf_model->threshold >= 0)
    {
        gltf_model->threshold -= elapsedTime;
    }
    else  is_dissolve = false;
}
