#pragma once
#include "Lemur/Model/GltfModel.h"
#include "Lemur/Math/Transform.h"
#include "Lemur/Graphics/Graphics.h"

class GltfModelManager
{
public:
    GltfModelManager(ID3D11Device* device, const std::string& filename);
    ~GltfModelManager() {}

    void Render(float elapsed_time, const float& scale, ID3D11PixelShader* replaced_pixel_shader);

    void DrawDebug();

    Transform* GetTransform() { return &transform; }

    void SetModelColor(DirectX::XMFLOAT4 c) { color = c; }
    DirectX::XMFLOAT4 GetModelColor() { return color; }

public: // 取得・設定関数関連
    std::vector<GltfModel::animation>* GetAnimation() { return &gltf_model->animations; };

    // 現在のアニメーション番号の取得・設定
    const int GetCurrentAnimationIndex()& { return current_animation_index; }
    void SetCurrentAnimationIndex(const int& animationIndex) { current_animation_index = animationIndex; }

    // アニメーション再生速度設定（途中で再生速度を変えたいときなどに）
    void SetAnimationSpeed(const float& speed) { animation_speed = speed; }

public: // アニメーション関数関連
    // アニメーション再生設定
    // (アニメーション番号・ループするかどうか・アニメーション再生速度・スムーズ切り替え時間（速度）)
    void PlayAnimation(
        const int& index,
        const bool& loop,
        const float& speed = 1.0f,
        const float& blendSeconds = 1.0f
    );

    // ブレンド率の計算更新処理
    void UpdateBlendRate(float blendRate, const float& elapsedTime);

    // アニメーション更新処理
    void UpdateAnimation(const float& elapsedTime);

    // アニメーションが再生中かどうか
    bool IsPlayAnimation() const;

public: // デバッグ確認用
    bool is_blend_animation = true;     // アニメーションブレンドオンオフ

public:
    std::shared_ptr<GltfModel> gltf_model;
    Transform    transform;

    DirectX::XMFLOAT4 color = { 1,1,1,1 };

    std::vector<GltfModel::node> animated_nodes;
    float time{ 0 };
private:
    float       animation_speed = 1.0f;     // アニメーション再生速度
    float       current_animation_seconds = 0.0f;     // 現在のアニメーション再生時間

    float       animation_blend_time = 0.0f;     // 現在のブレンドタイマー
    float       animation_blend_seconds = 0.0f;     // ブレンド時間

    int         current_animation_index = 0;	    // 現在のアニメーション番号

    bool        animation_loop_flag = true;    // アニメーションをループ再生するか
    bool        animation_end_flag = false;    // アニメーションが終了したか
};