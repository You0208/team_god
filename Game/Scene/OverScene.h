#pragma once
#include "./Lemur/Scene/BaseScene.h"

class OverScene :public Lemur::Scene::BaseScene
{
public:
    OverScene() {}
    ~OverScene() override {}

    // 初期化
    void Initialize()override;

    // 終了化
    void Finalize()override;

    // 更新処理
    void Update(HWND hwnd, float elapsedTime)override;

    // 描画処理
    void Render(float elapsedTime)override;

    void DebugImgui()override;
private:
    enum Button
    {
        Select,
        Again
    };
    std::shared_ptr<Sprite> game_over_back      = nullptr;              //ゲームオーバー基本の背景
    std::shared_ptr<Sprite> continue_back       = nullptr;              // 選択された時の背景
    std::shared_ptr<Sprite> ui_text[8]          = {};                   // ゲームオーバーの文字
    std::shared_ptr<Sprite> again_text          = nullptr;              // もう一度
    std::shared_ptr<Sprite> stageselect_text    = nullptr;              // 畑に戻る

    EasingFunction          text_y[8]           = {};                   // 文字のY
    EasingFunction          text_angle[8]       = {};                   // 文字の角度

    int                     select_num          = 0;                    // 今何のボタンが選ばれているか

    EasingFunction          select              = {};                   // セレクトサイズ用
    EasingFunction          again               = {};                   // もう一度のサイズ用

    bool                    once_only           = true;                 // 演出用

    int                     direction_num       = 0;                    // どの演出か
    int                     random_num[8]       = { 0,7,5,4,3,1,6,2 };  // 角度が何処から進んでいるか

    float                   interval            = 0.0f;                 // インターバル
    float                   timer               = 0.0f;                 // タイマー

    bool                    anim_stop           = true;                 // アニメがストップされたか
    bool                    is_continue         = false;                // ボタンが選ばれたか
    int                     count_num           = 0;                    // 文字カウント
};