#pragma once
#include "./Lemur/Scene/BaseScene.h"

class TitleScene :public Lemur::Scene::BaseScene
{
public:
    TitleScene() {}
    ~TitleScene() override {}

    // ������
    void Initialize()override;

    // �I����
    void Finalize()override;

    // �X�V����
    void Update(HWND hwnd, float elapsedTime)override;

    // �`�揈��
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
    // �X�e�[�W                                    
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