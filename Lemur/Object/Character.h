#pragma once
#include "../Model/Model.h"
#include "../Graphics/Graphics.h"
#include "../Model/FbxModelManager.h"
#include "../Model/GltfModelManager.h"
#include "../Resource/ResourceManager.h"

class Character
{
public:
    Character() {};
    virtual ~Character() {}

    // 描画設定
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    // Imgui
    void DrawDebugGUI(std::string name,int i);
    virtual void DrawDebugGUI() {}
    // デバッグプリミティブ描画
    virtual void DrawDebugPrimitive() {};
    // ダメージを与える
    bool ApplyDamage(int damage);

    // 行列の更新処理
    void UpdateTransform();

protected:
    //----------モデル関連------------------------------------------------
    // サイズ更新
    void UpdateScale() {
        scale.x = scale.y = scale.z = scaleFactor;
    }

    void LoadFBXModel(ID3D11Device* device, const char* fbx_filename, bool triangulate = true, float sampling_rate = 0)
    {
        model = std::make_unique<FbxModelManager>(device, fbx_filename, triangulate, sampling_rate);
        is_gltf = false;
    }
    void LoadFBXModel(ID3D11Device* device, const char* fbx_filename, std::vector<std::string>& animation_filenames, bool triangulate = true, float sampling_rate = 0)
    {
        model = std::make_unique<FbxModelManager>(device, fbx_filename, animation_filenames, triangulate, sampling_rate);
        is_gltf = false;
    }

    void LoadGltfModel(ID3D11Device* device, const std::string& filename, bool external_texture)
    {
        gltf_model = std::make_unique<GltfModelManager>(device, filename, external_texture);
        is_gltf = true;
    }
    //----------移動関連------------------------------------------------
    // 速力更新
    void UpdateVelocity(float elapsed_fime);

    // 垂直速力更新処理
    void UpdataVerticalVelocity(float elapsed_frame);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsed_time);

    // 水平速力更新処理
    void UpdataHorizontalVelocity(float elapsed_frame);

    // 水平移動更新処理
    void UpdateHorizontalMove(float elapsed_time);

    //----------ゲーム関連------------------------------------------------
    // 無敵時間更新
    void UpdateInvincibleTimer(float elapsed_time);

    // 移動処理
    void Move(float vx, float vz, float speed);

    // 方向転換
    void Turn(float elapsed_time, float vx, float vz, float speed);

    // ジャンプ処理
    void Jump(float speed);

    // 着地した時に呼ばれる
    virtual void OnLanding() {};

    // ダメージを受けたときに呼ばれる
    virtual void OnDamage() {}

    // 死亡した時に呼ばれる
    virtual void OnDead() {}

    //----------アニメーション関連------------------------------------------------
    // 
    // ヒットストップの計算
    void HitStopCalc();

    // ヒットストップする(秒)
    void HitStopON(float hit_stop_time_);

    // アニメーションの更新
    void UpdateAnimation(float elapsedTime)
    {
        if (is_gltf)gltf_model->UpdateAnimation(elapsedTime);
        else model->UpdateAnimation(elapsedTime);
    }

    // アニメーション再生中か
    bool IsPlayAnimation()const
    {
        if (is_gltf)return gltf_model->IsPlayAnimation();
        else return  model->IsPlayAnimation();
        return false;
    }

    void PlayAnimation(
        const int& index,
        const bool& loop,
        const float& speed = 1.0f,
        const float& blendSeconds = 0.05f)
    {
        if (is_gltf)gltf_model->PlayAnimation(index, loop, speed, blendSeconds);
        else model->PlayAnimation(index, loop, speed, blendSeconds);
    }

    const int GetCurrentAnimationIndex()&
    {
        if (is_gltf)return gltf_model->GetCurrentAnimationIndex();
        else return  model->GetCurrentAnimationIndex();
    }

public:
    //---------Getter--------------------------------------------------------------------------
    
    // モデルの数値
    const DirectX::XMFLOAT3& GetPosition() const { return position; }                         // 位置
    const DirectX::XMFLOAT3& GetAngle() const { return rotation; }                            // 回転
    const DirectX::XMFLOAT3& GetScale() const { return scale; }                               // スケール
    const float& GetScaleFactor() const { return scaleFactor; }                               // スケールファクター
    const float& GetRadius() const { return radius; }                                         // 半径
    const float& GetHeight() const { return height; }                                         // 高さ
    const DirectX::XMFLOAT4& GetColor()const { return material_color; }                       // 色
    const FbxModelManager*   GetModel()const { return model.get(); }                          // モデル

    // ステータス
    const int&  GetHealth() const { return health; }                                          // HP
    const int&  GetMaxHealth() const { return max_health; }                                   // MaxHP
    const int&  GetAttackPower() const { return attack_power; }                                   // MaxHP
    const float& GetAttackCollisionRange()const { return attack_collision_range; }            // 攻撃当たり判定
    const bool& IsGround() const { return isGround; }                                         // 設置判定
    const bool& IsDead() const { return death; }                                              // 死亡判定

    // アニメーション
    const bool& GetEndAnimation()const { return end_animation; }                              // アニメーション終了フラグ
    const int&  GetFrameIndex()const { return frame_index; }                                  // アニメーションフレーム
    const std::vector<Animation>* GetAnimation()const { return model->GetAnimation(); };      // アニメーションデータ

    //---------Setter--------------------------------------------------------------------------
     
    // モデルの数値
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }              // 位置
    void SetPosition(const float pos_x, const float pos_y, const float pos_z) { position.x = pos_x, position.y = pos_y, position.z = pos_z; }
    void SetAngle(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }                 // 回転
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }                          // スケール
    void SetScaleFactor(const float scaleFactor) { this->scaleFactor = scaleFactor; }               // スケールファクター
    void SetModel(std::shared_ptr<FbxModelManager> Model) { this->model = Model; }                  // モデル
    void SetAnimCalcRate(const float calc_rate_) { anim_calc_rate = calc_rate_; }                   // アニメーションの再生速度倍率

    // ステータス
    void SetDead(bool death_) { this->death = death_; }                                               // 死亡
    void SetAttackPower(const int attack_power_) { this->attack_power = attack_power_; }              // 攻撃力
    void SetAttackInterval(const int attack_interval_) { this->attack_interval = attack_interval_; }  // 攻撃間隔
    void SetHealth(const int health_) { this->health = health_; }                                     // HP
    void SetRadius(const float radius_) { this->radius = radius_; }                                   // 半径
    void SetAttackRadius(const float attack_radius_) { this->attack_collision_range = attack_radius_; } // 半径
    void SetSpeedPower(const float speed_power_) { this->speed_power = speed_power_; }                // 速度

private:
    std::shared_ptr<FbxModelManager> model = nullptr;                           // FBXモデル
    std::shared_ptr<GltfModelManager> gltf_model = nullptr;                     // Gltfモデル

protected:
    //----------モデル関連------------------------------------------------
    Microsoft::WRL::ComPtr<ID3D11PixelShader> PS    = nullptr;                     // ピクセルシェーダー
    Animation::keyframe keyframe                    = {};                          // キーフレーム
    bool                is_gltf                     = false;                       // 使用するモデルがGLTFか

    DirectX::XMFLOAT3   velocity                    = { 0, 0, 0 };                 // 速度
    DirectX::XMFLOAT3   position                    = { 0, 0, 0 };                 // 位置
    DirectX::XMFLOAT3   scale                       = { 1.0f, 1.0f, 1.0f };        // スケール
    float               scaleFactor                 = 1.0f;                        // スケールのまとめ
    DirectX::XMFLOAT3   rotation                    = { 0, 0, 0 };                 //　回転
    DirectX::XMFLOAT4   material_color              = { 1, 1, 1, 1 };              // 色
    DirectX::XMFLOAT3   direction                   = { 0,0,1 };                   // 方向

    //----------ゲーム関連------------------------------------------------
    float   radius                 = 1.0f;          // 半径
    float   height                 = 0.0f;          // 高さ 

    int     max_health             = 5;             // 最大健康状態
    int     health                 = max_health;    // 健康状態
    bool    death                  = false;         // 死亡判定

    int     attack_power           = 0;             // 基礎攻撃力
    float   attack_interval        = 0.0f;          // 攻撃間隔
    float   attack_collision_range = 0.3f;          // 攻撃半径
    float   speed_power            = 10.0f;         // スピードパラメータ
    float   defense_power          = 0;             // 基礎防御力

    float   maxMoveSpeed           = 5.0f;          // 最大速度
    float   moveVecX               = 0.0f;          // X方向移動
    float   moveVecZ               = 0.0f;          // Z方向移動
    float   friction               = 0.7f;          // 摩擦力
    float   acceleration           = 1.5f;          // 加速力
    float   gravity                = -1.0f;         // 重力

    float   invincibleTimer        = 1.0f;          // 無敵時間
    float	airControl             = 0.3f;          // 空気抵抗

    bool    isGround               = false;         // 着地フラグ

    //----------アニメーション関連------------------------------------------------
    bool    animation_loop_flag     = true;         // ループフラグ
    bool    end_animation           = false;        // アニメーション終了フラグ
    float   animation_blend_time    = 0.0f;         // ブレンド
    float   animation_blend_seconds = 0.0f;         // ブレンド秒
    float   animation_tick          = 0;            // アニメーション
    int     animation_index         = 0;            // アニメーション番号
    int     frame_index             = 0;            // アニメーションのクリップ番号
    float   anim_calc_rate          = 1.0f;         // アニメーションの再生速度

    
    float   hit_stop_time           =0.0f;          // ヒットストップする時間
    float   hit_stop_rate           = 1.0f;         // ヒットストップ係数
    float   hit_stop_timer          =0.0f;          // ヒットストップ経過時間
   
    bool    is_hit_stop             = false;        // ヒットストップしてるか

};