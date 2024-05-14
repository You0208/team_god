#pragma once
#include "./Lemur/Scene/BaseScene.h"

class ClearScene :public Lemur::Scene::BaseScene
{
public:
    ClearScene() {}
    ~ClearScene() override {}

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

    bool once_only = true;                 // ���o�p

    bool is_continue=false;

    int  direction_num_1 = 0;             // �ǂ̉��o���istar1�j
    int  direction_num_2 = 0;             // �ǂ̉��o���istar1�j
    int  direction_num_3 = 0;             // �ǂ̉��o���istar1�j

    float timer = 0.0f;
    float interval_1 = 0.5f;
    float interval_2 = 1.5f;
    float interval_3 = 2.0f;

    int select_num = 0;
private:
    std::shared_ptr<Sprite> clear_back = nullptr;              // �N���A��{�̔w�i
    std::shared_ptr<Sprite> continue_back = nullptr;              // �I�����ꂽ���̔w�i

    std::shared_ptr<Sprite> result_cover = nullptr;              // ������x
    std::shared_ptr<Sprite> star1 = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> star2 = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> star3 = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> star_frame1 = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> star_frame2 = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> star_frame3 = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> next_text = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> select_text = nullptr;              // ���ɖ߂�
    std::shared_ptr<Sprite> again_text = nullptr;              // ���ɖ߂�
};