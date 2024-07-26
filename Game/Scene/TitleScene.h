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
    // �X�e�[�W                                    
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
    EasingFunction  pause_window_scale = {};     // �|�[�Y�Z���N�g��ʃC�[�W���O
    EasingFunction          yes = {};                   // �Z���N�g�T�C�Y�p
    EasingFunction          no = {};                   // ������x�̃T�C�Y�p
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

    bool is_tutorial = false;  // �|�[�Y�t���O
    bool is_tutorial_next = false;  // �|�[�Y�t���O
    int  select_num = 1;                    // �����̃{�^�����I�΂�Ă��邩
};