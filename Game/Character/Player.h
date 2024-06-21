#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Effekseer/Effect.h"
#include "Seed.h"

class Player :public Character
{
public:
    Player();
    ~Player()override;

    // 更新処理
    void Update(float elapsedTime);

    // Imgui
    void DrawDebugGUI();

    // インスタンス取得
    static Player& Instance();

    int GetCategory() { return unit_category; }
private:
    // 入力処理
    void InputProcess();

    // はじき処理
    void Flick(float elapsedTime);

    //void FlickRe(float elapsedFrame);

    // カテゴリーの変更
    void ChangeCategory();
private:
    enum Animation
    {
        Pull,
        Throw,
        Right,
        Left,
        Idle
    };

    float               sub_pos_z           = 0.0f;     // 減算されるZ座標
    float               sub_pos_z_puls      = 0.0f;     // 減算されるZ座標の追加分
    float               moveSpeed           = 5.0f;     // 横移動の速度
    DirectX::XMFLOAT2   limit               = {};       // 案山子の左右端
    float               dis_scarecrow       = 0.0f;     // 案山子から種の最短距離
    
    bool                is_direction        = false;    // 種の演出フラグ
    bool                is_dis_direction    = false;    // 種の衝突演出フラグ
    float               seed_throw_speed    = 0.0f;     // 種の射出速度

    int                 unit_category       = 0;        // 選択されているユニットの種類
    
    // コントローラー用
    float               flip_speed          = 0.0f;     // 引っ張りの速度（引きの強さの変更）
    float               right_stick_y       = 0.0f;     // コントローラーの右スティックY
    float               flip_timer          = 0.0f;     // コントローラーがはじかれるのにかかった時間
    float               max_right_stick_y   = 0.0f;     // はじかれた右スティックY座標の最大 
    float               flip_pos_z          = 0.0f;     // 最終的に導き出されたはじき座標
    float               max_charge_time     = 2.0f;     // 溜め攻撃の最大時間
    bool                is_throw            = false;    // 投げられたかどうか

    bool                is_mouse_click = false;
    float               mouse_timer = dis_scarecrow;
    float               mouse_timer_speed = 15.0f;
};


