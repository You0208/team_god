#pragma once
#include "./Lemur/Scene/BaseScene.h"

class ClearScene :public Lemur::Scene::BaseScene
{
public:
    ClearScene() {}
    ~ClearScene() override {}

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
        Next,
        Select,
        Again
    };

    EasingFunction star1_scale = {};
    EasingFunction star2_scale = {};
    EasingFunction star3_scale = {};
    EasingFunction select = {};
    EasingFunction again = {};
    EasingFunction next = {};

    bool is_star_1 = true;
    bool is_star_2 = true;
    bool is_star_3 = true;

    bool once_only = true;                 // 演出用

    bool is_continue=false;

    int  direction_num_1 = 0;             // どの演出か（star1）
    int  direction_num_2 = 0;             // どの演出か（star1）
    int  direction_num_3 = 0;             // どの演出か（star1）

    float timer = 0.0f;
    float interval_1 = 0.5f;
    float interval_2 = 1.5f;
    float interval_3 = 2.0f;

    int select_num = 0;
private:
    std::shared_ptr<Sprite> clear_back = nullptr;              // クリア基本の背景
    std::shared_ptr<Sprite> continue_back = nullptr;              // 選択された時の背景

    std::shared_ptr<Sprite> result_cover = nullptr;              // もう一度
    std::shared_ptr<Sprite> star1 = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> star2 = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> star3 = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> star_frame1 = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> star_frame2 = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> star_frame3 = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> next_text = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> select_text = nullptr;              // 畑に戻る
    std::shared_ptr<Sprite> again_text = nullptr;              // 畑に戻る
};