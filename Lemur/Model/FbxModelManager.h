#pragma once
#include "Lemur/Graphics/Graphics.h"
#include "Lemur/Resource/ResourceManager.h"
#include "Lemur/Model/Model.h"
#include "Lemur/Math/Transform.h"
class FbxModelManager
{
public:
    FbxModelManager(ID3D11Device* device, const char* fbx_filename, bool triangulate = true, float sampling_rate = 0);
    FbxModelManager(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate = true, float sampling_rate = 0);
    ~FbxModelManager() {}

    void Render(const float& scale, ID3D11PixelShader* replaced_pixel_shader);
    void Render(const float& scale, ID3D11PixelShader** replaced_pixel_shader); 

    void DrawDebug(std::string i);

    Transform* GetTransform() { return &transform; }

    void SetModelColor(DirectX::XMFLOAT4 c) { color = c; }
    DirectX::XMFLOAT4 GetModelColor() { return color; }

public: // 取得・設定関数関連
    // アニメーションデータ取得
    std::vector<Animation>* GetAnimation() { return &fbx_model->animation_clips; };
    const bool GetIsDissolve()& { return is_dissolve; }
    // 現在のアニメーション番号の取得・設定
    const int GetCurrentAnimationIndex()& { return current_animation_index; }
    const int GetCurrentAnimationSecond()& { return current_animation_seconds; }
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

    void Dissolve(const float& elapsedTime);
public: // デバッグ確認用
    bool is_blend_animation = true;     // アニメーションブレンドオンオフ

public:
    std::shared_ptr<SkinnedMesh> fbx_model;
    Transform    transform;

    Animation::keyframe keyframe = {};
    DirectX::XMFLOAT4 color = { 1,1,1,1 };

    float       animation_blend_time      = 0.0f;     // 現在のブレンドタイマー
private:
    float       animation_speed           = 1.0f;     // アニメーション再生速度
    float       current_animation_seconds = 0.0f;     // 現在のアニメーション再生時間

    float       animation_blend_seconds   = 0.0f;     // ブレンド時間

    int         current_animation_index   = -1;	      // 現在のアニメーション番号

    bool        animation_loop_flag       = false;    // アニメーションをループ再生するか
    bool        animation_end_flag        = false;    // アニメーションが終了したか

    bool        is_dissolve               = true;     // 消えてる最中か
};