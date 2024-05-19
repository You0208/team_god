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
    std::shared_ptr<Sprite> title_left = nullptr;
    std::shared_ptr<Sprite> title_right = nullptr;
    std::shared_ptr<Sprite> title_logo = nullptr;

    EasingFunction left_scale = {};
    EasingFunction right_scale = {};
    EasingFunction logo_scale = {};

    bool is_direction = true;
    int direction_num = 0;
};