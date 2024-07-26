#pragma once
#include "./Lemur/Scene/BaseScene.h"

class TitleScene :public Lemur::Scene::BaseScene
{
public:
    TitleScene() {}
    ~TitleScene() override {}

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
    void Direction();
    void HandleInput();
private:
    enum Type
    {
        LEFT,
        RIGHT,
        RIGHT_2,
        LOGO,
        LOGO_2
    };
    // ステージ                                    
    std::shared_ptr<Sprite> title_back = nullptr;
    std::shared_ptr<Sprite> title_back2 = nullptr;
    std::shared_ptr<Sprite> title_left = nullptr;
    std::shared_ptr<Sprite> title_right = nullptr;
    std::shared_ptr<Sprite> title_logo = nullptr;
    std::shared_ptr<Sprite> title_credit = nullptr;
    std::shared_ptr<Sprite> title_credit_PC = nullptr;
    std::shared_ptr<Sprite> title_menu = nullptr;
    std::shared_ptr<Sprite> title_start = nullptr;

    std::shared_ptr<Sprite> window;
    std::shared_ptr<Sprite> window_tutorial;
    std::shared_ptr<Sprite> window_yes;
    std::shared_ptr<Sprite> window_no;
    EasingFunction  pause_window_scale = {};     // ポーズセレクト画面イージング
    EasingFunction          yes = {};                   // セレクトサイズ用
    EasingFunction          no = {};                   // もう一度のサイズ用
    enum Button
    {
        YES,
        NO
    };

    EasingFunction left_scale = {};
    EasingFunction right_scale = {};
    EasingFunction logo_scale = {};
    EasingFunction start_scale = {};
    EasingFunction credit_y = {};

    float title_x_1 = 0;
    float title_x_2 = 0;
    bool is_direction = true;
    int direction_num = 0;
    bool is_credit = false;;

    bool is_tutorial = false;  // ポーズフラグ
    bool is_tutorial_next = false;  // ポーズフラグ
    int  select_num = 1;                    // 今何のボタンが選ばれているか
};