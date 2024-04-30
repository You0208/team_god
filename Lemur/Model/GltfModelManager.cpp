#include "GltfModelManager.h"

GltfModelManager::GltfModelManager(ID3D11Device* device, const std::string& filename)
{
    gltf_model= std::make_unique<GltfModel>(device, filename);
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

    current_animation_index = index;    // 再生するアニメーション番号を設定
    current_animation_seconds = 0.0f;     // アニメーション再生時間リセット

    animation_loop_flag = loop;     // ループさせるか
    animation_end_flag = false;    // 再生終了フラグをリセット

    animation_speed = speed;    // アニメーション再生速度

    animation_blend_time = 0.0f;
    animation_blend_seconds = blendSeconds;
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
    // 再生中でないなら処理しない
    if (!IsPlayAnimation()) return;

    // 最終フレーム処理
    if (animation_end_flag)
    {
        animation_end_flag = false; // 終了フラグをリセット
        current_animation_index = -1;    // アニメーション番号リセット
        return;
    }
    if (current_animation_index < 0)return;


    // アニメーション再生時間経過
    current_animation_seconds += elapsed_time;

    GltfModel::animation animation = GetAnimation()->at(current_animation_index);

    animated_nodes = gltf_model->nodes;
    gltf_model->Animate(current_animation_index, current_animation_seconds, animated_nodes, animation_loop_flag);
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
    if (current_animation_index >= animationIndexEnd) return false;

    return true;
}
