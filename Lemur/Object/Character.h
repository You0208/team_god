#pragma once
#include "../Model/Model.h"
#include "../Graphics/Graphics.h"
#include "../Resource/ResourceManager.h"
#include "./Lemur/Input/Mouse.h"
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <unordered_map>
#include <string>
#include <fbxsdk.h>

class Character
{
public:
    Character() {};
    virtual ~Character() {}

    virtual void DebugImgui() {};
    virtual void DrawDebugPrimitive() {};
    void AnimationUpdate(float elapsedTime);

    // ルートモーション更新
    void RootmationUpdate(float elapsedTime);
    // ルートモーションするノード設定
    void SetupRootMotion(const char* root_motion_nodeName)
    {
        root_motion_node_index = Model->FindMeshNodeIndex(root_motion_nodeName);
    }

    Animation animation_{};

    Animation::keyframe keyframe{};
    DirectX::XMFLOAT4X4 world;

public:
    // 位置取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

    // 位置更新
    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
    // 位置更新
    void SetPosition(const float pos_x, const float pos_y, const float pos_z) { position.x = pos_x, position.y = pos_y, position.z = pos_z; }

    // 回転取得
    const DirectX::XMFLOAT3& GetAngle() const { return rotation; }

    // 回転更新
    void SetAngle(const DirectX::XMFLOAT3& rotation) { this->rotation = rotation; }

    // スケール取得
    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    // スケール更新
    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // スケールファクター取得
    const float& GetScaleFactor() const { return scaleFactor; }

    // スケールファクター更新
    void SetScaleFactor(const float scaleFactor) { this->scaleFactor = scaleFactor; }

    // 半径取得
    float GetRadius() const { return radius; }

    // 高さ取得
    float GetHeight() const { return height; }

    // 地面に設置しているか
    bool IsGround() const { return isGround; }

    // 健康状態を取得
    int GetHealth() const { return health; }

    // 最大健康状態を取得
    int GetMaxHealth() const { return max_health; }

    // ダメージを与える
    bool ApplyDamage(int damage);

    // 攻撃当たり半径取得
    float GetAttackCollisionRange()const { return attack_collision_range; }

    // 色取得
    const DirectX::XMFLOAT4& GetColor() { return material_color; }

    // モデル設定
    void SetModel(std::shared_ptr<SkinnedMesh> Model) { this->Model = Model; }

    // モデル取得(AI側でアニメーションの切り替えしたいから作りました Byトミー)
    SkinnedMesh* GetModel()const { return Model.get(); }

    // アニメーションの切り替え
    void SetAnimationIndex(int index, const bool& loop = false)
    {
        frame_index = 0;
        animation_tick = 0;
        animation_index = index;
        end_animation = false;
        animation_loop_flag = loop;

        animation_blend_time = 0.0f;
        animation_blend_seconds = 1.0f;
    }

    // アニメーション終了フラグ取得
    bool GetEndAnimation()const { return end_animation; }

    // アニメーションフレーム取得
    int GetFrameIndex()const { return frame_index; }

    // アニメーションデータ取得
    std::vector<Animation>* GetAnimation() { return &Model->animation_clips; };

    // 描画設定
    void Render(float elapsedTime, ID3D11PixelShader* replaced_pixel_shader);
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // 移動処理
    void Move(float vx, float vz, float speed);
    // 方向転換
    void Turn(float vx, float vz, float speed);

    void SetPixelShader(ID3D11PixelShader* ps) { pixelShader = ps; }

    // 速力更新
    void UpdateVelocity(float elapsedTime);

    // ヒットストップする(秒)
    void HitStopON(float hit_stop_time_);

    // アニメーションの再生速度倍率設定
    void SetAnimCalcRate(const float calc_rate_) { anim_calc_rate = calc_rate_; }
protected:


    // ジャンプ処理
    void Jump(float speed);


    // 着地した時に呼ばれる
    virtual void OnLanding() {};

    // ダメージを受けたときに呼ばれる
    virtual void OnDamaged() {}

    // 死亡した時に呼ばれる
    virtual void OnDead() {}

public:
    void UpdateScale() {
        scale.x = scale.y = scale.z = scaleFactor;
    }

    // 垂直速力更新処理
    void UpdataVerticalVelocity(float elapsedFrame);

    // 垂直移動更新処理
    void UpdateVerticalMove(float elapsedTime);

    // 水平速力更新処理
    void UpdataHorizontalVelocity(float elapsedFrame);

    // 水平移動更新処理
    void UpdateHorizontalMove(float elapsedTime);

    // ヒットストップの計算
    void HitStopCalc();

public:

    /* 攻撃、HP関係はスキルとかギャンブルとかで変動する機会多いから
     * 妥協でパブリックな。*/
    int     max_health = 5; // 最大健康状態
    int     health = max_health; // 健康状態
    bool    death = false;

    //基礎攻撃力
    float attack_power = 0;
    // モーション値(○倍)
    float motion_value = 1.0f;

    // 基礎防御力
    float defense_power = 0;

    // スピードパラメータ
    float speed_power = 10.0f;

    // 攻撃当たり判定するか
    bool attack_collision_flag = false;

    bool animStop = false;

    Microsoft::WRL::ComPtr<ID3D11PixelShader> PS;

public:
    void UpdateAnimation(float elapsedTIme);
    void UpdateBlendRate(float blendRate, const float& elapsedTime);
    // アニメーション再生中か[17]
    bool IsPlayAnimation()const;
    // 現在のアニメーション番号
    bool animation_loop_flag = true;
    float animation_blend_time = 0.0f;
    float animation_blend_seconds = 0.0f;

    DirectX::XMFLOAT4X4 World;

    DirectX::XMFLOAT3 velocity = { 0, 0, 0 };  // 速度
    DirectX::XMFLOAT3 position = { 0, 0, 0 };  // 位置
    DirectX::XMFLOAT3 scale = { 3.0f, 3.0f, 3.0f }; // スケール
    DirectX::XMFLOAT3 rotation = { 0, 0, 0 }; //　回転
    DirectX::XMFLOAT4 material_color = { 1, 1, 1, 1 }; // 色

    DirectX::XMFLOAT3 direction = { 0,0,1 };// 方向
    DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };// 行列

    float   invincibleTimer = 1.0f; // 無敵時間
    float   maxMoveSpeed = 5.0f; // 最大速度
    float   moveVecX = 0.0f; // X方向移動
    float   moveVecZ = 0.0f; // Z方向移動
    float   friction = 0.7f; // 摩擦力
    float   acceleration = 1.5f; // 加速力
    float   gravity = -1.0f; // 重力
    float   radius = 1.0f; // 半径
    float   height = 0.0f; // 高さ 

    float   scaleFactor = 1.0f;

    float	airControl = 0.3f; // 空気抵抗

    bool    isGround = false; // 着地フラグ

    // 攻撃当たり判定する半径
    float attack_collision_range = 0.3f;

    // 歩きの速さ(スピードパラメータがあるからベースの速度は激おそ)
    float walk_speed = 0.1f;


    float animation_tick = 0; // アニメーション
    std::shared_ptr<SkinnedMesh> Model = nullptr;
    // アニメーション用(メンバ化しときました。これに相当する変数とかもしあったらごめん)
    //名前をclip_indexから変えました。
    int animation_index = 0;
    int frame_index = 0;

    // ルートモーションで使うから作りました。 by:tomy
    int root_motion_node_index = -1;

    // アニメーション終了フラグ
    bool end_animation = false;


    // ヒットストップする時間
    float hit_stop_time;
    // ヒットストップ経過時間
    float hit_stop_timer;
    // ヒットストップしてるか
    bool is_hit_stop = false;

    // この値をいじればヒットストップできる
    float hit_stop_rate = 1.0f;
    // アニメーションの再生速度を制御していろいろな表現をする奴
    float anim_calc_rate = 1.0f;


    Mouse* mouse;


    ID3D11PixelShader* pixelShader = nullptr; // シェーダー
};