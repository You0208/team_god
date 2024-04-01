#include "FbxModelManager.h"

// TODO: リソースマネージャーの使用
#define USE_RESOURCE_MANAGER

FbxModelManager::FbxModelManager(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate)
{
#ifdef USE_RESOURCE_MANAGER // リソースマネージャーあり
    fbx_model = ResourceManager::Instance().LoadModelResource(
        device,
        fbx_filename,
        triangulate,
        sampling_rate
    );
#else // リソースマネージャーなし
    fbx_model = std::make_unique<skinned_mesh>(device, fbx_filename, triangulate, sampling_rate);
#endif

}

FbxModelManager::FbxModelManager(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate, float sampling_rate)
{
    fbx_model = ResourceManager::Instance().LoadModelResource(
        device, fbx_filename,
        animation_filenames,
        triangulate,
        sampling_rate
    );
}

// 描画
void FbxModelManager::Render(const float& scale, ID3D11PixelShader* replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // world行列更新
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));

    // Model描画
    if (&keyframe)
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), &keyframe, replaced_pixel_shader);
    }
    else
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), nullptr, replaced_pixel_shader);
    }
}

void FbxModelManager::Render(const float& scale, ID3D11PixelShader** replaced_pixel_shader)
{
    Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

    // world行列更新
    DirectX::XMFLOAT4X4 world = {};
    DirectX::XMStoreFloat4x4(&world, GetTransform()->CalcWorldMatrix(scale));

    // Model描画
    if (&keyframe)
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), &keyframe, replaced_pixel_shader);
    }
    else
    {
        fbx_model->Render(graphics.GetDeviceContext(), world, GetModelColor(), nullptr, replaced_pixel_shader);
    }
}

void FbxModelManager::DrawDebug(std::string i)
{
    GetTransform()->DrawDebug(i);
}


void FbxModelManager::PlayAnimation(
    const int& index, const bool& loop,
    const float& speed, const float& blendSeconds)
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


void FbxModelManager::UpdateBlendRate(float blendRate, const float& elapsedTime)
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

void FbxModelManager::UpdateAnimation(const float& elapsedTime)
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
    // アニメーション再生時間経過
    current_animation_seconds += elapsedTime;

    // 指定のアニメーションデータを取得
    Animation& animation = GetAnimation()->at(current_animation_index);

    // 現在の時間がどのキーフレームの間にいるのか判定する
    const float  frameIndex_float = (current_animation_seconds * animation.sampling_rate) * animation_speed;
    const size_t frameIndex = static_cast<const size_t>(frameIndex_float);

    // 最終フレーム
    const size_t frameEnd = (animation.sequence.size() - 1);

    // 再生時間が終端時間を超えたら
    if (frameIndex > frameEnd)
    {
        // ループ再生する場合
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
    // 再生時間とキーフレームの時間かた補間率を算出する
    else if ((keyframe.nodes.size() > 0) && frameIndex < frameEnd)
    {
        // ブレンド率の計算
        float blendRate = 0.1f;
        UpdateBlendRate(blendRate, elapsedTime);

        // キーフレーム取得
        const std::vector<Animation::keyframe>& keyframes = animation.sequence;

        // 現在の時間がどのキーフレームの間にいるのか判定する
        const Animation::keyframe* keyframe_[2] = {
            &keyframe,
            &keyframes.at(frameIndex + 1)
        };

        // 再生時間とキーフレームの時間から補間率を計算する
        fbx_model->BlendAnimations(keyframe_, blendRate, keyframe);

        // トランスフォーム更新
        fbx_model->UpdateAnimation(keyframe);
    }
    else
    {
        keyframe = animation.sequence.at(frameIndex);
    }
}

bool FbxModelManager::IsPlayAnimation() const
{
    if (current_animation_index < 0) return false;

    const int animationIndexEnd = static_cast<int>(fbx_model->animation_clips.size());
    if (current_animation_index >= animationIndexEnd) return false;

    return true;
}
