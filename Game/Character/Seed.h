#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Effekseer/Effect.h"

class Seed :public Character
{
public:
    Seed();
    ~Seed()override;

    void Update(float elapsedTime);

    void throwDirection();
    void DisDirection();

    // 描画処理
    void Render(float scale, ID3D11PixelShader* replaced_pixel_shader);

    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // 飛ばしたての種の位置決定
    void DecisionPos();

    // ランダムなベクトル
    DirectX::XMFLOAT2 GenerateRandomDirection();

    // 指定した距離分だけ移動する関数
    DirectX::XMFLOAT2 MoveInRandomDirection(const DirectX::XMFLOAT2& position, float distance);


    float   GetTimer() { return timer; };
    int     GetNumber() { return number; };
    bool    GetBorn() { return born; };
    bool    GetDesitionPos() { return decision_pos; }

    void    SetDistinationPosition(DirectX::XMFLOAT3 distination_position_) { distination_position = distination_position_; }
    void    SetNumber(int number_) { number= number_; }
    void    SetCategory(int category_) { category = category_; }
    void    SetthrowSpeed(float throw_speed_) { throw_speed = throw_speed_; }
    void    SetBorn(bool born_) { born = born_; }
    void    SetDesitionPos(bool decision_pos_) { decision_pos = decision_pos_; }
    void    SetIsDirection(bool is_direction_) { is_direction = is_direction_; }
    void    SetIsDisDirection(bool is_dis_direction_) { is_dis_direction = is_dis_direction_; }

    void    DecNumber() { number--; }

    Effect* error_effect = nullptr;
    Effekseer::Handle error_effect_handle = 0;
private:
    enum Animation_Index
    {
        Stop,
        Idle,
        Jump,
        Throw
    };

    float               timer                   = 0.0f;            // 種が配置されてからの時間
    int                 number                  = 0;               // 種番号
    int                 category                = 0;               // 種の種類
    bool                born                    = false;           // ユニットが生まれるかどうか
    bool                decision_pos            = false;           // ユニットの位置が決定されているか
    int                 overlap_num             = 0;               // 何回重なったか
    DirectX::XMFLOAT2   outPosition             = {};              // 押し出し位置
    DirectX::XMFLOAT3   distination_position    = {};              // 目的地

    bool                is_direction            = false;           // 種の投げ演出
    bool                is_dis_direction        = false;           // 種の消える演出
    float               throw_speed             = 20.0f;           // 種の速度
    float               effect_size             = 0.0f;            // エフェクトのサイズ

    float               seed_fence_dis          = 0.5f;            // 種と柵の距離の許容範囲
};


